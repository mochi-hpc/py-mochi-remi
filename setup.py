from distutils.core import setup
from distutils.extension import Extension
from distutils.sysconfig import get_config_vars
import pkgconfig
import os
import os.path
import sys

(opt,) = get_config_vars('OPT')
os.environ['OPT'] = " ".join(
		    flag for flag in opt.split() if flag != '-Wstrict-prototypes'
		)

# Find out the dependencies using pkgconfig 
# For client...
pk = pkgconfig.parse('remi')
client_libraries    = pk['libraries']
client_library_dirs = pk['library_dirs']
client_include_dirs = pk['include_dirs']
client_include_dirs.append(".")
# For server...
server_libraries    = pk['libraries']
server_library_dirs = pk['library_dirs']
server_include_dirs = pk['include_dirs']
server_include_dirs.append(".")
# For filesets...
fileset_libraries    = pk['libraries']
fileset_library_dirs = pk['library_dirs']
fileset_include_dirs = pk['include_dirs']
fileset_include_dirs.append('.')

pyremi_server_module = Extension('_pyremiserver', ["pyremi/src/server.cpp"],
		           libraries=server_libraries,
                   library_dirs=server_library_dirs,
                   include_dirs=server_include_dirs,
                   extra_compile_args=['-std=c++11'],
                   depends=["pyremi/src/server.cpp"])

pyremi_client_module = Extension('_pyremiclient', ["pyremi/src/client.cpp"],
		           libraries=client_libraries,
                   library_dirs=client_library_dirs,
                   include_dirs=client_include_dirs,
                   extra_compile_args=['-std=c++11'],
                   depends=["pyremi/src/client.cpp"])

pyremi_fileset_module = Extension('_pyremifileset', ["pyremi/src/fileset.cpp"],
                    libraries=fileset_libraries,
                    library_dirs=fileset_library_dirs,
                    include_dirs=fileset_include_dirs,
                    extra_compile_args=['-std=c++11'],
                    depends=["pyremi/src/fileset.cpp"])

setup(name='pyremi',
      version='0.1',
      author='Matthieu Dorier',
      description="""Python binding for REMI""",
      ext_modules=[ pyremi_server_module, pyremi_client_module, pyremi_fileset_module ],
      packages=['pyremi']
     )
