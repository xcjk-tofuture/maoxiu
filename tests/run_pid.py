from pathlib import Path
import argparse, subprocess, shutil, os
p=argparse.ArgumentParser();p.add_argument('--cc',default=shutil.which('gcc') or 'gcc');args=p.parse_args()
root=Path(__file__).resolve().parents[1];dest=root/'.host-build';dest.mkdir(exist_ok=True)
exe=dest/('pid_tests.exe' if os.name=='nt' else 'pid_tests')
subprocess.run([args.cc,'-std=c99','-Wall','-Wextra','-Werror','-O2','-I'+str(root/'firmware/algorithms/math/inc'),str(root/'firmware/algorithms/math/src/pid.c'),str(root/'tests/pid_tests.c'),'-lm','-o',str(exe)],check=True)
subprocess.run([str(exe)],check=True)
