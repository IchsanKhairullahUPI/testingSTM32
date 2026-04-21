from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup

setup(
    name="robot_kinematics",
    ext_modules=[Pybind11Extension("robot_kinematics", ["bindings.cpp"], cxx_std=17)],
    cmdclass={"build_ext": build_ext},
)