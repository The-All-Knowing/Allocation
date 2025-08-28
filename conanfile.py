import os
from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, cmake_layout

class CompressorRecipe(ConanFile):
    name = "compressor"
    version = "0.1"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps"

    def requirements(self):
        self.requires("poco/1.14.2")
        self.requires("gtest/1.16.0")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["CMAKE_CXX_STANDARD"] = 23
        tc.variables["CMAKE_EXPORT_COMPILE_COMMANDS"] = True
        tc.variables["CMAKE_CXX_STANDARD_REQUIRED"] = True
        tc.variables["CMAKE_CXX_FLAGS_RELEASE"] = "-O2"
        tc.generate()
