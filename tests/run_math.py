from pathlib import Path
import argparse,subprocess,shutil,os
p=argparse.ArgumentParser();p.add_argument('--cc',default=shutil.which('gcc') or 'gcc');a=p.parse_args()
root=Path(__file__).resolve().parents[1];build=root/'.host-build';build.mkdir(exist_ok=True)
exe=build/('math_tests.exe' if os.name=='nt' else 'math_tests')
subprocess.run([a.cc,'-std=c99','-Wall','-Wextra','-Werror','-O2','-I'+str(root/'firmware/algorithms/math/inc'),str(root/'tests/math_tests.c'),str(root/'firmware/algorithms/math/src/mathTool.c'),'-lm','-o',str(exe)],check=True)
subprocess.run([str(exe)],check=True)
print('PASS: inverse-square-root finite range, invalid inputs and host data-model independence')
