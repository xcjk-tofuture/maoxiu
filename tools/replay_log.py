"""Offline protocol-log validation and state timeline, no serial writes."""
import argparse,json,math,struct
def replay(path):
    result={'frames':0,'events':0,'states':[],'last_time_s':None};device=None
    with open(path,encoding='utf-8') as f:
        for line_no,line in enumerate(f,1):
            if len(line)>4096:raise ValueError('line too large')
            r=json.loads(line)
            if line_no==1:
                if r.get('format')!='star-log-v1':raise ValueError('unsupported log version')
                device=r['device'];continue
            t=r['time_s'];flags=r['flags'];payload=bytes.fromhex(r['payload_hex'])
            if not isinstance(t,(float,int)) or not math.isfinite(t) or len(payload)>128 or flags not in (0,1,2):raise ValueError('invalid record')
            if result['last_time_s'] is not None and t<result['last_time_s']:raise ValueError('timestamps out of order')
            if not isinstance(r['sequence'],int) or not 0<=r['sequence']<=65535 or not isinstance(r['command'],int) or not 0<=r['command']<=65535:raise ValueError('invalid frame identifier')
            result['frames']+=1;result['events']+=flags==2;result['last_time_s']=t
            if r['command']==4 and flags in (1,2) and payload and payload[0]==0:
                if device=='0x4d530001':
                    if len(payload)!=31:raise ValueError('invalid chassis status length')
                    values=struct.unpack_from('<fff',payload,2)+(struct.unpack_from('<f',payload,15)[0],)
                elif device=='0x58550001':
                    if len(payload)!=14:raise ValueError('invalid UAV status length')
                    values=struct.unpack_from('<fff',payload,2)
                else:raise ValueError('unsupported status device')
                if not all(math.isfinite(v) for v in values):raise ValueError('nonfinite telemetry')
                if not result['states'] or result['states'][-1]['state']!=payload[1]:
                    result['states'].append({'time_s':t,'state':payload[1]})
    if device is None:raise ValueError('empty log')
    result['device']=device;return result
def main():
    p=argparse.ArgumentParser();p.add_argument('log');a=p.parse_args();print(json.dumps(replay(a.log),indent=2))
if __name__=='__main__':main()
