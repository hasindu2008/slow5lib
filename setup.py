try:
    from setuptools import setup, Extension
except ImportError:
    from distutils.core import setup
    from distutils.extension import Extension

import numpy as np

cmdclass={}
from Cython.Build import build_ext
cmdclass['build_ext'] = build_ext

#adapted from https://github.com/lh3/minimap2/blob/master/setup.py

sources=['python/pyslow5.pyx', 'src/slow5.c', 'src/slow5_press.c', 'src/slow5_misc.c', 'src/slow5_idx.c' ]
depends=['python/pyslow5.pxd', 'python/pyslow5.h',
            'slow5/slow5.h', 'slow5/slow5_defs.h', 'slow5/slow5_error.h', 'slow5/slow5_press.h',
            'slow5/klib/khash.h', 'slow5/klib/kvec.h',
            'src/slow5_extra.h', 'src/slow5_idx.h', 'src/slow5_misc.h', 'src/klib/ksort.h' ]
extra_compile_args = ['-g', '-Wall', '-O2', '-std=c99']

include_dirs = ['include/', np.get_include()]
libraries = ['m', 'z']
library_dirs = ['.']

extensions = [Extension('pyslow5',
                  sources = sources,
                  depends = depends,
                  extra_compile_args = extra_compile_args,
                  libraries = libraries,
                  include_dirs = include_dirs,
                  library_dirs = library_dirs,
                  language = 'c' )]

setup(
    name = 'pyslow5',
    version='0.2.0a1',
    url = 'https://github.com/hasindu2008/slow5lib',
    description='slow5lib python bindings',
    long_description='The slow5 python library (pyslow5) allows a user to read slow5 and blow5 files. Visit https://hasindu2008.github.io/slow5lib/pyslow5_api/pyslow5.html',
    author='Hasindu Gamaarachchi, Sasha Jenner, James Ferguson',
    author_email='hasindu2008@gmail.com',
    maintainer='Hasindu Gamaarachchi',
    maintainer_email='hasindu2008@gmail.com',
    license = 'MIT',
    keywords = ['nanopore','slow5','signal'],
    ext_modules=extensions,
    # requires=['numpy (>=1.3.0)'],
    #packages=packages,
    cmdclass=cmdclass,
    python_requires='>=3.4.3',
    install_requires=["numpy"],
    setup_requires=["cython"]
    )
