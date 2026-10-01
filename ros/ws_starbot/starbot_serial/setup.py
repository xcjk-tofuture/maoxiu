from setuptools import find_packages, setup
from glob import glob
import os

package_name = 'starbot_serial'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test', 'starbot_serial/starbot_serial_tools']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
         (os.path.join('share', package_name, 'launch'), glob('launch/*.launch.py')),
         (os.path.join('share', package_name, 'config'), glob('config/*')),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='star',
    maintainer_email='star@todo.todo',
    description='maoxiu chassis protocol v1 serial bridge',
    license='TODO: License declaration',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            "starbot_serial = starbot_serial.starbot_serial:main"
        ],
    },
)
