# Sunlight
Sunlight is an open-source MMORPG server software for a game whose official game server was shut down 20 years ago. It is being developed solely for educational purposes, such as learning about programming, understanding MMORPG mechanics, maintaining large-scale projects, and improving skills.

This repository does not infringe on any copyright of the original game and implements only the server logic. Commercial use of this code is not allowed, and the user is solely responsible for the consequences of using this software.

# Requirements
- C++ 20
- CMake
- Windows
- MySQL
- The following libraries should be installed via **vcpkg**
  - boost 1.86
  - libprotobuf 5.29
  - openssl
  - spdlog
  - eigen3
  - argon2
  - pugixml
  - gtest
  - tbb
  - lua
  - fmt

System Environment Variables
- set ``VCPKG_ROOT`` to your vcpkg directory

# License
This project is licensed under the AGPLv3 License with the following restriction:
- Commercial use is strictly prohibited
- The user is solely responsible for the consequences of using this software
