#adapted from https://github.com/lh3/minimap2/blob/master/setup.py

try:
	from setuptools import setup, Extension
except ImportError:
	from distutils.core import setup
	from distutils.extension import Extension

import numpy as np

cmdclass={}

from Cython.Build import build_ext
module_src = 'python/pyslow5.pyx'
cmdclass['build_ext'] = build_ext


#TODO add all
sources=[module_src, 'src/slow5.c', 'src/slow5_press.c', 'src/slow5_misc.c', 'src/slow5_idx.c' ]
depends=['python/pyslow5.pxd', 'python/pyslow5.h',  'slow5/slow5.h',
            'slow5/klib/khash.h', 'slow5/klib/kvec.h',
            'slow5/slow5_defs.h', 'slow5/slow5_error.h', 'slow5/slow5_press.h',
            'src/slow5_extra.h', 'src/slow5_idx.h', 'src/slow5_misc.h',
            'src/klib/ksort.h' ]
extra_compile_args = ['-g', '-Wall', '-O2', '-std=c99']
libraries = ['z']
include_dirs = ['include/', np.get_include()]
library_dirs = ['.']

#py_inc = [get_python_inc()]

# np_lib = os.path.dirname(numpy.__file__)
# np_inc = [os.path.join(np_lib, 'core/include')]
#cmdclass = {'build_py': build_py}


#cmdclass.update({'build_ext': build_ext})
#packages=['test']


extensions = [Extension('pyslow5',
                  sources = sources,
                  depends = depends,
                  extra_compile_args = extra_compile_args,
                  libraries = libraries,
                  include_dirs = include_dirs,
                  library_dirs = library_dirs,
                  language = 'c' )]

#TODO ad all
setup(name = 'pyslow5',
      version='0.0',
	  python_requires='>=3.5.2',
      url = 'https://github.com/hasindu2008/slow5lib',
      requires=['numpy (>=1.3.0)'],
      description='slow5 python library',
      author='Hasindu Gamaarachchi, Sasha Jenner, James Ferguson ...',
      author_email='hasindu@garvan.org.au, ..., j.ferguson@garvan.org.au',
      maintainer='Hasindu Gamaarachchi, Sasha Jenner, James Ferguson ...',
      maintainer_email='hasindu@garvan.org.au',
      license = 'MIT',
      keywords = ['slow5','nanopore'],
      #packages=packages,
      cmdclass=cmdclass,
      ext_modules=extensions
      #ext_modules=cythonize(extensions),
      )
