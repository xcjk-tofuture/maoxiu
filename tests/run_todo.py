from pathlib import Path
import argparse,subprocess,shutil,os
p=argparse.ArgumentParser();p.add_argument('--cc',default=shutil.which('gcc') or 'gcc');a=p.parse_args()
root=Path(__file__).resolve().parents[1];fw=root/'firmware';build=root/'.host-build';build.mkdir(exist_ok=True)
cases=[('journal',[fw/'services/parameters/param_journal.c',root/'tests/journal_tests.c'])]
if (fw/'services/chassis_service.c').exists():
    cases.append(('parameters',[fw/'services/parameters/chassis_parameters.c',fw/'services/protocol/star_protocol.c',fw/'services/protocol/star_dispatch.c',root/'tests/parameter_tests.c']))
else:
    cases.append(('flight',[fw/'app/flight_machine.c',fw/'services/parameters/calibration_record.c',fw/'services/protocol/star_protocol.c',root/'tests/flight_tests.c']))
includes=[fw/'services/parameters',fw/'services/protocol',fw/'services',fw/'algorithms/chassis',fw/'boards/stm32',fw/'app']
for name,sources in cases:
    exe=build/(name+('_tests.exe' if os.name=='nt' else '_tests'))
    subprocess.run([a.cc,'-std=c11','-Wall','-Wextra','-Werror','-O2',*['-I'+str(x) for x in includes],*[str(x) for x in sources],'-lm','-o',str(exe)],check=True)
    subprocess.run([str(exe)],check=True)
