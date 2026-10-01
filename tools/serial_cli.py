"""Protocol v1 bench tool; every command is explicit, no unlock/flight command."""
import argparse,json,math,struct,time
from star_protocol import Frame,Parser,RESPONSE,STATUS,IDENTIFY,CAPABILITIES,PARAM_READ,PARAM_WRITE
def exchange(port,command,payload=b'',timeout=2.0,sequence=1):
    request=Frame(0,sequence,command,payload).encode()
    if port.write(request)!=len(request):raise IOError('partial serial write')
    parser=Parser();end=time.monotonic()+timeout
    while time.monotonic()<end:
        for frame in parser.feed(port.read(min(512,max(1,port.in_waiting))),int(time.monotonic()*1000)):
            if frame.flags==RESPONSE and frame.sequence==sequence and frame.command==command:
                if not frame.payload:raise ValueError('missing status')
                if frame.payload[0]:raise RuntimeError('device status '+str(frame.payload[0]))
                return frame.payload[1:]
    raise TimeoutError('no matching response; do not assume a timed-out save failed')
def main():
    p=argparse.ArgumentParser();p.add_argument('--port',required=True);p.add_argument('--baud',type=int,default=115200)
    sub=p.add_subparsers(dest='operation',required=True)
    for cmd in ['identify','capabilities','status','diagnostics','save']:sub.add_parser(cmd)
    read=sub.add_parser('read');read.add_argument('id',type=lambda v:int(v,0))
    write=sub.add_parser('write');write.add_argument('id',type=lambda v:int(v,0));write.add_argument('value',type=float)
    record=sub.add_parser('record');record.add_argument('output');record.add_argument('--seconds',type=float,default=10)
    a=p.parse_args()
    import serial
    with serial.Serial(a.port,a.baud,timeout=0.05,write_timeout=0.1) as port:
        ident=exchange(port,IDENTIFY);device=struct.unpack('<I',ident)[0]
        car=device==0x4d530001;uav=device==0x58550001
        if a.operation=='identify':print(hex(device));return
        if a.operation=='record':
            if not math.isfinite(a.seconds) or not 0<a.seconds<=3600:p.error('record duration must be 0..3600s')
            parser=Parser();end=time.monotonic()+a.seconds
            with open(a.output,'x',encoding='utf-8') as f:
                f.write(json.dumps({'format':'star-log-v1','device':hex(device),'baud':a.baud})+'\n')
                while time.monotonic()<end:
                    for frame in parser.feed(port.read(512),int(time.monotonic()*1000)):
                        f.write(json.dumps({'time_s':time.monotonic(),'flags':frame.flags,'sequence':frame.sequence,'command':frame.command,'payload_hex':frame.payload.hex()})+'\n')
            return
        command={'capabilities':CAPABILITIES,'status':STATUS,'diagnostics':0x1002 if car else 0x2001,'save':0x1001,'read':PARAM_READ,'write':PARAM_WRITE}[a.operation]
        if a.operation in ['save','read','write'] and not car:p.error('this tool only tunes maoxiu STM32')
        if a.operation=='diagnostics' and not(car or uav):p.error('no diagnostics capability on this device')
        payload=b''
        if a.operation in ['read','write']:
            if not 0<=a.id<=65535:p.error('parameter id out of range')
            payload=struct.pack('<H',a.id)
        if a.operation=='write':
            if not math.isfinite(a.value):p.error('finite value required')
            if a.id==1:
                if not a.value.is_integer() or not 50<=a.value<=1000:p.error('telemetry period must be integer 50..1000ms')
                payload+=struct.pack('<H',int(a.value))
            else:payload+=struct.pack('<f',a.value)
        result=exchange(port,command,payload,timeout=15 if a.operation=='save' else 2,sequence=2)
        print(json.dumps({'device':hex(device),'command':hex(command),'data_hex':result.hex()}))
if __name__=='__main__':main()
