from setuptools import Extension, setup

import numpy as np
from Cython.Build import cythonize


extensions = [
    Extension(
        name='encc',
        sources=[
            'encc.pyx',
            'c_enc.cpp',
        ],
        include_dirs=[
            np.get_include(),
        ],
        define_macros=[
            ('EXTRA_PP', None),
            ('RM_ASSIGNED', None),
        ],
        extra_compile_args=[
            '-std=c++11',
        ],
        language='c++',
    ),
]

setup(
    ext_modules=cythonize(extensions, language_level='3'),
    zip_safe=False,
)
