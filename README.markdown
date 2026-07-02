# ArcLight Engine
[![Build Status](https://api.travis-ci.com/ArkansasIo/rpgengine.svg?branch=develop)](https://app.travis-ci.com/github/ArkansasIo/rpgengine)

## README

ArcLight is an Open Source Real Time Strategy game engine built for performance,
scalability, and deep gameplay. Developed by Stephen, ArcLight provides a
powerful foundation for creating RTS games with large-scale battles, advanced
AI systems, and rich Lua scripting support.

Visit the [project repository](https://github.com/ArkansasIo/rpgengine) for
issues, suggestions, and community discussion.

### Features

- Large-scale real-time strategy gameplay with thousands of units
- Built-in Lua scripting for game logic, UI, and AI
- Multiplayer support with lobby integration
- Headless server mode for dedicated hosting
- Cross-platform (Windows, Linux)
- Extensible AI interface (C++, Java, Python)
- Powerful map and terrain rendering

### Compiling

**Prerequisites:**
- CMake 3.10+
- C++17 compatible compiler (MSVC, GCC, Clang)
- SDL2
- OpenGL / GLEW
- Lua 5.x
- zlib

**Build with CMake:**

	mkdir build
	cd build
	cmake ..
	cmake --build .

**Or on Linux/macOS:**

	cmake .
	make

For detailed build instructions, see [doc/HowToBuild.txt](doc/HowToBuild.txt).

### Installation

**Windows:**
Download the latest release from the
[releases page](https://github.com/ArkansasIo/rpgengine/releases) or build
from source.

**Linux:**

	sudo make install

See [doc/HowToBuild.txt](doc/HowToBuild.txt) for platform-specific details.

### Using ArcLight

Use a compatible lobby client for single- or multi-player matches.

Once in-game, ArcLight games work like any other RTS, with deep
customizability through Lua scripting and a wide range of control options.

### Project Structure

	spring/
	├── rts/            Core engine (simulation, rendering, networking, Lua)
	├── AI/             AI interfaces and skirmish AIs
	├── tools/          Utility tools (map compiler, demo tool, unitsync)
	├── cont/           Content and assets (cursors, bitmaps, Lua gadgets)
	├── doc/            Documentation and build guides
	├── test/           Test suite
	└── buildbot/       Continuous integration configuration

### Contributing

Contributions are welcome! Fork the repository, create a feature branch,
and submit a pull request.

1. Fork the repo
2. Create your feature branch (`git checkout -b feature/my-feature`)
3. Commit your changes
4. Push to the branch (`git push origin feature/my-feature`)
5. Open a Pull Request

### License

ArcLight Engine is licensed under the MIT License.
See [LICENSE](LICENSE) for the full license text.

### Credits

ArcLight Engine is developed by **Stephen** and built upon the legacy of the
original Spring community project.
