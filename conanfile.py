from conans import ConanFile, CMake, tools
import os

class ConanBuild(ConanFile):
    generators = "cmake", "virtualenv"
    requires = 'boost/1.69.0@conan/stable','eigen/3.3.7@conan/stable'
    build_requires = 'cmake_installer/3.13.0@conan/stable'

    def build(self):
      cmake = CMake(self)
      defs = {}
      defs["Eigen3_DIR"] = os.path.join( self.deps_cpp_info["eigen"].rootpath, "share", "eigen3", "cmake" )
      cmake.configure(defs=defs)

      cmake.build()
