from setuptools import setup

CLASSIFIERS = [
    "Development Status :: 4 - Beta",
    "Intended Audience :: Science/Research",
    "License :: OSI Approved :: MIT License",
    "Programming Language :: Python",
    "Programming Language :: Python :: 3",
    "Topic :: Scientific/Engineering :: Artificial Intelligence",
    "Operating System :: Microsoft :: Windows",
    "Operating System :: POSIX",
    "Operating System :: Unix",
    "Operating System :: MacOS"
]

setup(
    name = 'smurfference',
    packages = [ 'smurfference' ],
    version = '0.1',
    url = "https://gitlab.itwm.fraunhofer.de/EPEEC/smurff/tree/master/smurfference",
    license = "None",
    description = 'SMURFF predictions',
    author = "Tom Vander Aa",
    author_email = "Tom.VanderAa@imec.be",
    classifiers = CLASSIFIERS,
    keywords = "bayesian factorization machine-learning high-dimensional side-information",
    install_requires = ['numpy', 'scipy', 'pandas', 'sklearn', 'matrix_io', 'tensorflow', 'smurff']
)
