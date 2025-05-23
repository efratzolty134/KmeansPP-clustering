from setuptools import Extension, setup

module = Extension("mykmeanssp", sources=["kmeans.c", "kmeansmodule.c"])
setup(
    name="mykmeanssp",
    version="1.0",
    description="Python wrapper for C extension",
    ext_modules=[module]
)