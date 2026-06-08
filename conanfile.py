
from conan import ConanFile
from conan.tools.cmake import cmake_layout

class LeEngine(ConanFile):
    generators = {"CMakeToolchain", "CMakeDeps"}
    settings = {"os", "build_type", "arch", "compiler"}
    
    def requirements(self):
        self.requires("libjpeg-turbo/3.1.4.1")
        self.requires("glm/1.0.1")
        self.requires("vulkan-memory-allocator/3.3.0")
        self.requires("cgltf/1.15")
        self.requires("glfw/3.4")
        #self.requires("imgui/1.92.8")
    
     
    def build_requirements(self):
        self.tool_requires("cmake/[>=3.19.2]")
        
    def layout(self):
        cmake_layout(self)