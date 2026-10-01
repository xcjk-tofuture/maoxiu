#include "star_protocol.h"
#include <string.h>
uint16_t star_read_u16(const uint8_t *p) {
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}
uint32_t star_read_u32(const uint8_t *p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}
void star_write_u16(uint8_t *p, uint16_t v) {
    p[0] = (uint8_t)v;
    p[1] = (uint8_t)(v >> 8);
}
void star_write_u32(uint8_t *p, uint32_t v) {
    p[0] = (uint8_t)v;
    p[1] = (uint8_t)(v >> 8);
    p[2] = (uint8_t)(v >> 16);
    p[3] = (uint8_t)(v >> 24);
}
float star_read_f32(const uint8_t *p) {
    float v;
    uint32_t u = star_read_u32(p);
    memcpy(&v, &u, 4);
    return v;
}
void star_write_f32(uint8_t *p, float v) {
    uint32_t u;
    memcpy(&u, &v, 4);
    star_write_u32(p, u);
}
uint16_t star_crc16(const uint8_t *p, size_t n) {
    uint16_t crc = 0xffffu;
    size_t i;
    unsigned bit;
    for (i = 0; i < n; i++) {
        crc ^= (uint16_t)((uint16_t)p[i] << 8);
        for (bit = 0; bit < 8; bit++)
            crc =
                (uint16_t)((crc & 0x8000u) ? ((uint32_t)crc << 1) ^ 0x1021u : ((uint32_t)crc << 1));
    }
    return crc;
}
size_t star_encode(const star_frame_t *f, uint8_t *p, size_t cap) {
    size_t n;
    if (!f || !p || f->length > STAR_PAYLOAD_MAX || f->flags > STAR_EVENT)
        return 0;
    n = (size_t)f->length + 12u;
    if (cap < n)
        return 0;
    p[0] = 0xa5u;
    p[1] = 0x5au;
    p[2] = STAR_VERSION;
    p[3] = f->flags;
    star_write_u16(p + 4, f->sequence);
    star_write_u16(p + 6, f->command);
    star_write_u16(p + 8, f->length);
    memcpy(p + 10, f->payload, f->length);
    star_write_u16(p + n - 2, star_crc16(p + 2, n - 4));
    return n;
}
void star_parser_init(star_parser_t *p) {
    if (p)
        memset(p, 0, sizeof(*p));
}
void star_parser_expire(star_parser_t *p, uint32_t now) {
    if (p && p->used && (uint32_t)(now - p->last_ms) >= STAR_RX_TIMEOUT_MS) {
        p->used = 0;
        p->timed_out++;
    }
}
static void discard(star_parser_t *p, uint16_t n) {
    p->used = (uint16_t)(p->used - n);
    memmove(p->bytes, p->bytes + n, p->used);
}
void star_parser_feed(star_parser_t *p, const uint8_t *data, size_t length, uint32_t now,
                      star_frame_fn callback, void *context) {
    size_t i;
    uint16_t n, payload;
    star_frame_t f;
    if (!p || (!data && length))
        return;
    star_parser_expire(p, now);
    for (i = 0; i < length; i++) {
        if (p->used == STAR_FRAME_MAX) {
            discard(p, 1);
            p->rejected++;
        }
        p->bytes[p->used++] = data[i];
        p->last_ms = now;
        while (p->used) {
            if (p->bytes[0] != 0xa5u) {
                discard(p, 1);
                continue;
            }
            if (p->used < 2)
                break;
            if (p->bytes[1] != 0x5au) {
                discard(p, 1);
                continue;
            }
            if (p->used < 10)
                break;
            payload = star_read_u16(p->bytes + 8);
            if (p->bytes[2] != STAR_VERSION || p->bytes[3] > STAR_EVENT ||
                payload > STAR_PAYLOAD_MAX) {
                discard(p, 1);
                p->rejected++;
                continue;
            }
            n = (uint16_t)(payload + 12u);
            if (p->used < n)
                break;
            if (star_read_u16(p->bytes + n - 2) != star_crc16(p->bytes + 2, n - 4u)) {
                discard(p, 1);
                p->rejected++;
                continue;
            }
            f.flags = p->bytes[3];
            f.sequence = star_read_u16(p->bytes + 4);
            f.command = star_read_u16(p->bytes + 6);
            f.length = payload;
            memcpy(f.payload, p->bytes + 10, payload);
            discard(p, n);
            if (callback)
                callback(context, &f);
        }
    }
}
