
#include "star_dispatch.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#ifdef TEST_CHASSIS
#include "chassis_service.h"
#else
#include "attitude.h"
#endif
static unsigned received;
static star_frame_t latest;
static void accept(void *c,const star_frame_t *f) {(void)c;received++;latest=*f;}
static void protocol_tests(void) {
    star_frame_t q={0},r;star_parser_t p;uint8_t bytes[STAR_FRAME_MAX],bad[STAR_FRAME_MAX],noise[5000];
    size_t n,i,cut;uint32_t seed=42;star_dispatch_t d={123,3,100,50,NULL,NULL};
    assert(star_crc16((const uint8_t *)"123456789",9)==0x29b1);
    q.command=STAR_CMD_IDENTIFY;q.sequence=0x1234;
    n=star_encode(&q,bytes,sizeof(bytes));assert(n==12);
    assert(bytes[0]==0xa5 && bytes[1]==0x5a && bytes[4]==0x34 && bytes[5]==0x12);
    assert(star_encode(&q,bytes,n-1)==0);
    for(cut=0;cut<=n;cut++) {
        star_parser_init(&p);received=0;star_parser_feed(&p,bytes,cut,1,accept,NULL);
        star_parser_feed(&p,bytes+cut,n-cut,2,accept,NULL);
        assert(received==1 && latest.sequence==0x1234 && latest.length==0);
    }
    star_parser_init(&p);received=0;
    for(i=0;i<100;i++) star_parser_feed(&p,bytes,n,3,accept,NULL);
    assert(received==100);
    memcpy(bad,bytes,n);bad[6]^=1;star_parser_init(&p);received=0;
    star_parser_feed(&p,bad,n,4,accept,NULL);assert(received==0 && p.rejected==1);
    star_parser_feed(&p,bytes,n,5,accept,NULL);assert(received==1);
    memcpy(bad,bytes,n);bad[8]=255;bad[9]=255;star_parser_init(&p);received=0;
    star_parser_feed(&p,bad,n,4,accept,NULL);star_parser_feed(&p,bytes,n,5,accept,NULL);assert(received==1);
    star_parser_init(&p);star_parser_feed(&p,bytes,5,0xfffffff0u,accept,NULL);
    star_parser_expire(&p,90);assert(!p.used && p.timed_out==1);
    /* Deterministic random streams plus a valid recovery frame, no heap. */
    for(i=0;i<sizeof(noise);i++) {seed=seed*1664525u+1013904223u;noise[i]=(uint8_t)(seed>>24);}
    star_parser_init(&p);received=0;star_parser_feed(&p,noise,sizeof(noise),1,accept,NULL);
    assert(p.used<=STAR_FRAME_MAX);star_parser_expire(&p,101);
    star_parser_feed(&p,bytes,n,102,accept,NULL);assert(received==1);
    q.length=128;memset(q.payload,0xa5,128);n=star_encode(&q,bytes,sizeof(bytes));assert(n==STAR_FRAME_MAX);
    star_parser_init(&p);received=0;for(i=0;i<n;i++) star_parser_feed(&p,bytes+i,1,1,accept,NULL);
    assert(received==1 && latest.length==128);
    q.length=0;q.command=STAR_CMD_IDENTIFY;star_dispatch(&d,&q,&r);
    assert(r.payload[0]==STAR_OK && star_read_u32(r.payload+1)==123);
    q.command=STAR_CMD_PARAM_WRITE;q.length=4;star_write_u16(q.payload,1);star_write_u16(q.payload+2,49);
    star_dispatch(&d,&q,&r);assert(r.payload[0]==STAR_RANGE && d.telemetry_period_ms==100);
    star_write_u16(q.payload+2,50);star_dispatch(&d,&q,&r);assert(r.payload[0]==STAR_OK && d.telemetry_period_ms==50);
    q.length=3;star_dispatch(&d,&q,&r);assert(r.payload[0]==STAR_BAD_LENGTH);
    q.command=65535;star_dispatch(&d,&q,&r);assert(r.payload[0]==STAR_UNSUPPORTED);
    star_write_f32(bytes,-1.25f);assert(bytes[3]==0xbf && star_read_f32(bytes)==-1.25f);
}
#ifdef TEST_CHASSIS
static void project_tests(void) {
    chassis_velocity_t v={1.0f,-0.25f,0.5f},out;float w[4],pwm[4],zero[4]={0};
    chassis_service_t s;const chassis_config_t config={4,.235f,1800,180,16700,6,500};
    chassis_mecanum_inverse(v,.235f,w);out=chassis_mecanum_forward(w,.235f);
    assert(fabsf(out.vx_mps-v.vx_mps)<1e-6f && fabsf(out.vy_mps-v.vy_mps)<1e-6f && fabsf(out.wz_radps-v.wz_radps)<1e-6f);
    v.vy_mps=0;chassis_differential_inverse(v,.25f,w);out=chassis_differential_forward(w,.25f);
    assert(fabsf(out.vx_mps-v.vx_mps)<1e-6f && fabsf(out.wz_radps-v.wz_radps)<1e-6f);
    chassis_init(&s,&config);chassis_step(&s,zero,12,0,pwm);assert(s.snapshot.state==CHASSIS_IDLE && pwm[0]==0);
    chassis_submit(&s,v,100);chassis_step(&s,zero,12,110,pwm);assert(s.snapshot.state==CHASSIS_ACTIVE && pwm[0]>0);
    chassis_step(&s,zero,5.9f,120,pwm);assert(s.snapshot.state==CHASSIS_UNDERVOLTAGE && pwm[0]==0 && s.pi[0].output==0);
    chassis_step(&s,zero,12,600,pwm);assert(s.snapshot.state==CHASSIS_TIMEOUT && pwm[0]==0);
    chassis_submit(&s,v,0xffffff00u);chassis_step(&s,zero,12,0x100u,pwm);assert(s.snapshot.state==CHASSIS_TIMEOUT);
    s.pi[0].kp=1e6f;assert(chassis_pi_step(&s.pi[0],0,100)==16700);
}
#else
static void project_tests(void) {
    uav_attitude_t a,b;const float gravity[3]={0,0,9.8f},zero[3]={0},mag[3]={1,0,0},spin[3]={0,0,1};unsigned i;
    uav_attitude_init(&a);uav_attitude_init(&b);
    for(i=0;i<2000;i++) assert(uav_attitude_step(&a,gravity,zero,mag,.005f)==0);
    assert(fabsf(a.q[0]-1)<1e-6f && fabsf(a.roll_deg)<1e-6f && fabsf(a.pitch_deg)<1e-6f);
    for(i=0;i<200;i++) assert(uav_attitude_step(&b,zero,spin,zero,.005f)==0);
    assert(fabsf(b.yaw_deg+57.29578f)<.01f && a.yaw_deg==0);
    assert(uav_attitude_step(&b,zero,zero,zero,0)==-1);
    assert(isfinite(b.q[0]) && fabsf(b.q[0]*b.q[0]+b.q[3]*b.q[3]-1)<1e-5f);
}
#endif
int main(void) {protocol_tests();project_tests();puts("PASS: fragmentation, concatenation, CRC, bounds, timeout/wrap, dispatch and project algorithms");return 0;}
