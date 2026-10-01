
from pathlib import Path
import argparse, subprocess, shutil
p=argparse.ArgumentParser();p.add_argument('--cc',default=shutil.which('gcc') or 'gcc');a=p.parse_args()
root=Path(__file__).resolve().parents[1];build=root/'.host-build';build.mkdir(exist_ok=True)
is_car=(root/'services/chassis_service.c').exists()
sources=list((root/'services/protocol').glob('*.c'))+[root/'tests/host_tests.c']
includes=[root/'services/protocol']
if is_car:
    sources+=list((root/'algorithms/chassis').glob('*.c'))+[root/'services/chassis_service.c']
    includes += [root/'algorithms/chassis',root/'services']
else:
    sources += [root/'algorithms/attitude/attitude.c'];includes += [root/'algorithms/attitude']
exe=build/('host_tests.exe' if __import__('os').name=='nt' else 'host_tests')
cmd=[a.cc,'-std=c99','-Wall','-Wextra','-Werror','-O2']
if is_car:cmd+=['-DTEST_CHASSIS']
cmd += ['-I'+str(x) for x in includes]+[str(x) for x in sources]+['-lm','-o',str(exe)]
subprocess.run(cmd,check=True);subprocess.run([str(exe)],check=True)
