try:
    from setuptools import setup, Extension
    from setuptools.command.install import install

except ImportError:
    from distutils.core import setup
    from distutils.extension import Extension

import subprocess
import sys

include_dirs = []
# import numpy as np

# creat dummy closures for metadata reading on first parse of setup.py
# that way it picks up the requirements and installs them, then can use them
# for the install.
try:
    import numpy as np
    include_dirs = ['include/', np.get_include(), 'thirdparty/streamvbyte/include']
except ImportError:
    include_dirs = ['include/', 'thirdparty/streamvbyte/include']
    def np(*args, ** kwargs ):
        import numpy as np
        return np(*args, ** kwargs)

try:
    from Cython.Build import build_ext
except ImportError:
    def build_ext(*args, ** kwargs ):
        from Cython.Build import build_ext
        return build_ext(*args, ** kwargs)

# from Cython.Build import build_ext

class CustomInstall(install):
    def run(self):
        command = ["make"]
        print("running make command!!!!")
        if subprocess.call(command) != 0:
            sys.exit(-1)
        install.run(self)


#adapted from https://github.com/lh3/minimap2/blob/master/setup.py

sources=['python/pyslow5.pyx', 'src/slow5.c', 'src/slow5_press.c', 'src/slow5_misc.c', 'src/slow5_idx.c' ]
depends=['python/pyslow5.pxd', 'python/pyslow5.h',
            'slow5/slow5.h', 'slow5/slow5_defs.h', 'slow5/slow5_error.h', 'slow5/slow5_press.h',
            'slow5/klib/khash.h', 'slow5/klib/kvec.h',
            'src/slow5_extra.h', 'src/slow5_idx.h', 'src/slow5_misc.h', 'src/klib/ksort.h' ]
extra_compile_args = ['-g', '-Wall', '-O2', '-std=c99']

# include_dirs = ['include/', np.get_include(), 'thirdparty/streamvbyte/include']
libraries = ['m', 'z', 'streamvbyte']
library_dirs = ['.','thirdparty/streamvbyte/']

extensions = [Extension('pyslow5',
                  sources = sources,
                  depends = depends,
                  extra_compile_args = extra_compile_args,
                  libraries = libraries,
                  include_dirs = include_dirs,
                  library_dirs = library_dirs,
                  language = 'c' )]

def readme():
	with open('docs/pyslow5_api/pyslow5.md') as f:
		return f.read()


setup(
    name = 'pyslow5',
    version='0.3.0a1',
    url = 'https://github.com/hasindu2008/slow5lib',
    description='slow5lib python bindings',
    long_description=readme(),
    author='Hasindu Gamaarachchi, Sasha Jenner, James Ferguson',
    author_email='hasindu2008@gmail.com',
    maintainer='Hasindu Gamaarachchi',
    maintainer_email='hasindu2008@gmail.com',
    license = 'MIT',
    keywords = ['nanopore','slow5','signal'],
    ext_modules=extensions,
    cmdclass= {'install': CustomInstall,
               'build_ext': build_ext},
    classifiers = [
        'Development Status :: 4 - Beta',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: C',
        'Programming Language :: Cython',
        'Programming Language :: Python :: 3',
        'Intended Audience :: Science/Research',
        'Topic :: Scientific/Engineering :: Bio-Informatics'],
    python_requires='>=3.4.3',
    install_requires=["numpy"],
    setup_requires=["cython", "numpy"]
    )
