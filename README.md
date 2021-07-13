## Level-Editor
![SmolEngine](https://i.imgur.com/f09k9sF.png)
![SmolEngine](https://i.imgur.com/Eso9Lk3.png)

The engine is still at a very early stage and is not ready for production.

## Core Features

- Custom [graphics engine](https://github.com/YellowDummy/Frostium3D)
- Data-Oriented Entity Component System
- Jobs System (Multithreading)
- Level Editor
- C++ Scripting
- 2D and 3D physics engines (Box2D/Bullet3)
- 3rd party audio engine (FMOD)

## Short-term goals
- Documentation (doxygen)
- Tutorial
- Lua or C# scripting
- Release a game on itch.io

## Building
### Windows
1. Run gen-project-vs2019.bat or cmd ```premake5 vs2019```
2. Extract libs.7z from vendor\frostium
3  Install Mono x64 (https://www.mono-project.com/download/stable/) and copy lib folder to vendor/mono
4. Copy all dll's from vendor\fmod\libs and vendor\mono to output (bin) folder
