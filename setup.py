#!/usr/bin/env python

from distutils.core import setup, Extension

setup(
	name="tr_level",
	version="0.1",
	author="Florian Schulze",
	author_email="crow@icculus.org",
	url="http://www.icculuy.org/vt/",
	package_dir = {'': 'src'},
	ext_modules=[Extension(
		name="_tr_level",
		library_dirs=["C:\home\dev\libs\lib"],
		libraries=["SDL", "zlib"],
		sources=["src/tr_level_wrap.cpp", "src/glmath.cpp", "src/l_main.cpp", "src/l_common.cpp", "src/l_tr1.cpp", "src/l_tr2.cpp", "src/l_tr3.cpp", "src/l_tr4.cpp", "src/l_tr5.cpp"]
	)],
	py_modules=["tr_level"]
)
