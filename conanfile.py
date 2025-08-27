import platform
from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, cmake_layout

class CompressorRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps"

    def configure(self):
        self.options["poco"].enable_data_mysql = False

    def requirements(self):
        self.requires("gtest/1.16.0")
        self.requires("poco/1.13.3")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        
        tc.variables["CMAKE_EXPORT_COMPILE_COMMANDS"] = True
        tc.variables["CMAKE_CXX_FLAGS_DEBUG"] = "-DUSERVER_SANITIZE='addr ub'"
        tc.variables["CMAKE_CXX_FLAGS_RELEASE"] = "-O2"
        if platform.system() == "Darwin":
            brew_openssl = os.popen("brew --prefix openssl").read().strip()
            tc.variables["OPENSSL_ROOT_DIR"] = brew_openssl

        tc.generate()
