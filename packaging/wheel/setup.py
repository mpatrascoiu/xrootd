from setuptools import setup, Extension
from setuptools.command.install import install
from setuptools.command.sdist import sdist
from distutils.command.bdist import bdist

import subprocess
import sys
import getpass

def get_version():
    version = subprocess.check_output(['./genversion.sh', '--print-only'])
    version = version.decode()
    if version.startswith('v'):
        version = version[1:]
    version = version.split('-')[0]
    return version

def get_version_from_file():
    try:
        f = open('./bindings/python/VERSION')
        version = f.read().split('/n')[0]
        f.close()
        return version
    except:
        print('Failed to get version from file. Using unknown')
        return 'unknown'


class CustomInstall(install):
    def run(self): 
        command = ['./install.sh']
        if self.user:
            username = getpass.getuser()
            path = [path for path in sys.path if username in path and '.local' in path]
            if not path: raise RuntimeError( 'No user specific directory in sys.path!' )
            path = path[0][:path[0].index( '.local' ) + len( '.local' )]
            prefix = path
        else:
            prefix = sys.prefix
        command.append(prefix)
        command.append( self.config_vars['py_version_short'] )
        subprocess.call(command)


class CustomDist(sdist):
    def write_version_to_file(self):
        version = get_version()
        with open('bindings/python/VERSION', 'w') as vi:
            vi.write(version)
    
    def run(self):
        self.write_version_to_file()
        sdist.run(self)


class CustomWheelGen(bdist):
    # Do not generate wheel
    def run(self):
        return


version = get_version()
if version.startswith('unknown'):
    version = get_version_from_file()

setup( 
    name             = 'xrootd',
    version          = version,
    author           = 'XRootD Developers',
    author_email     = 'xrootd-dev@slac.stanford.edu',
    url              = 'http://xrootd.org',
    license          = 'LGPLv3+',
    description      = "XRootD with Python bindings",
    long_description = "XRootD with Python bindings",
    cmdclass        = {
        'install': CustomInstall,
        'sdist': CustomDist,
        'bdist_wheel': CustomWheelGen
    }
)
