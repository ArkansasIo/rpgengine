from conan import ConanFile
from conan.tools.cmake import CMakeDeps, CMakeToolchain


class ArcLightGameEngineConan(ConanFile):
    name = "arclight-gameengine"
    version = "0.1.0"
    package_type = "application"
    settings = "os", "compiler", "build_type", "arch"

    options = {
        "modern_stack": [True, False],
    }
    default_options = {
        "modern_stack": True,
    }

    def requirements(self):
        self.requires("zlib/[>=1.3 <2]")
        self.requires("glew/[>=2.2 <3]")
        self.requires("openal-soft/[>=1.23 <2]")
        self.requires("ogg/[>=1.3 <2]")
        self.requires("vorbis/[>=1.3 <2]")

        if self.options.modern_stack:
            self.requires("sdl/[>=3.2 <4]")
            self.requires("vulkan-headers/[>=1.3 <2]")
            self.requires("vulkan-loader/[>=1.3 <2]")
            self.requires("imgui/[>=1.90 <2]")
            self.requires("glm/[>=1.0 <2]")
            self.requires("entt/[>=3.13 <4]")

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()

        toolchain = CMakeToolchain(self)
        toolchain.variables["ARCLIGHT_ENABLE_MODERN_STACK"] = bool(self.options.modern_stack)
        toolchain.generate()
