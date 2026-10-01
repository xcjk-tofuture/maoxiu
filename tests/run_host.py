
from pathlib import Path
import argparse, subprocess, shutil
p=argparse.ArgumentParser();p.add_argument('--cc',default=shutil.which('gcc') or 'gcc');a=p.parse_args()
root=Path(__file__).resolve().parents[1];build=root/'.host-build';build.mkdir(exist_ok=True)
is_car=(root/'firmware/services/chassis_service.c').exists()
sources=list((root/'firmware/services/protocol').glob('*.c'))+[root/'tests/host_tests.c']
includes=[root/'firmware/services/protocol']
if is_car:
    sources+=list((root/'firmware/algorithms/chassis').glob('*.c'))+[root/'firmware/services/chassis_service.c']
    includes += [root/'firmware/algorithms/chassis',root/'firmware/services']
else:
    sources += [root/'firmware/algorithms/attitude/attitude.c'];includes += [root/'firmware/algorithms/attitude']
exe=build/('host_tests.exe' if __import__('os').name=='nt' else 'host_tests')
cmd=[a.cc,'-std=c99','-Wall','-Wextra','-Werror','-O2']
if is_car:cmd+=['-DTEST_CHASSIS']
cmd += ['-I'+str(x) for x in includes]+[str(x) for x in sources]+['-lm','-o',str(exe)]
subprocess.run(cmd,check=True);subprocess.run([str(exe)],check=True)
