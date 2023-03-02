try:
    from setuptools import setup, Extension
    from setuptools.command.install import install

except ImportError:
    from distutils.core import setup
    from distutils.extension import Extension

import sys
import platform
import os

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

#adapted from https://github.com/lh3/minimap2/blob/master/setup.py

sources=['python/pyslow5.pyx', 'src/slow5.c', 'src/slow5_press.c', 'src/slow5_misc.c', 'src/slow5_idx.c',
            'src/slow5_mt.c',
            'thirdparty/streamvbyte/src/streamvbyte_zigzag.c', 'thirdparty/streamvbyte/src/streamvbyte_decode.c', 'thirdparty/streamvbyte/src/streamvbyte_encode.c']
depends=['python/pyslow5.pxd', 'python/pyslow5.h',
            'slow5/slow5.h', 'slow5/slow5_defs.h', 'slow5/slow5_error.h', 'slow5/slow5_press.h', 'slow5/slow5_lazymt.h',
            'slow5/klib/khash.h', 'slow5/klib/kvec.h',
            'src/slow5_extra.h', 'src/slow5_idx.h', 'src/slow5_misc.h', 'src/klib/ksort.h',
            'thirdparty/streamvbyte/include/streamvbyte.h', 'thirdparty/streamvbyte/include/streamvbyte_zigzag.h']
extra_compile_args = ['-g', '-Wall', '-O2', '-std=c99', '-DSLOW5_ENABLE_MT=1' ]
# extra_compile_args = []
# os.environ["CFLAGS"] = '-g -Wall -O2 -std=c99'

arch=platform.machine()
if arch in ["aarch64", "arm64"]:
    extra_compile_args.append('-D__ARM_NEON__')
elif arch in ["aarch64"]:
	extra_compile_args.append('-mfpu=neon')
elif arch in ["x86_64"]:
    extra_compile_args.extend(['-DSTREAMVBYTE_SSSE3=1', '-mssse3'])   # WARNING: ancient x86_64 CPUs don't have SSSE3


# include_dirs = ['include/', np.get_include(), 'thirdparty/streamvbyte/include']
libraries = ['m', 'z']
library_dirs = ['.']

# a nasty hack to provide option to build with zstd
zstd=0
try:
    zstd=os.environ["PYSLOW5_ZSTD"]
except:
    zstd=0

if zstd=="1":
    extra_compile_args.append('-DSLOW5_USE_ZSTD=1')
    libraries.append('zstd')

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
    version='1.0.0',
    url = 'https://github.com/hasindu2008/slow5lib',
    description='slow5lib python bindings',
    long_description=readme(),
    long_description_content_type='text/markdown',
    author='Hasindu Gamaarachchi, Sasha Jenner, James Ferguson',
    author_email='hasindu2008@gmail.com',
    maintainer='Hasindu Gamaarachchi',
    maintainer_email='hasindu2008@gmail.com',
    license = 'MIT',
    keywords = ['nanopore','slow5','signal'],
    ext_modules=extensions,
    cmdclass= {'build_ext': build_ext},
    classifiers = [
        'Development Status :: 5 - Production/Stable',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: C',
        'Programming Language :: Cython',
        'Programming Language :: Python :: 3',
        'Intended Audience :: Science/Research',
        'Topic :: Scientific/Engineering :: Bio-Informatics'],
    python_requires='>=3.4.3',
    install_requires=["numpy"],
    setup_requires=["Cython", "numpy"]
    )
