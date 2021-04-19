## Level-Editor
![SmolEngine](https://i.imgur.com/vS3DiqW.png)
## Core Features

- Custom [graphics engine](https://github.com/YellowDummy/Frostium3D)
- Data-Oriented Entity Component System (ECS)
- Jobs System
- Level Editor
- C++ Scripting
- 3rd party 2D physics engine (Box2D)
- 3rd party audio engine (FMOD)

## Building
### Windows
1. Install Vulkan SDK (1.2 or higher)
2. Run gen-project-vs2019.bat or cmd ```premake5 vs2019```
3. Extract libs.7z from vendor\fmod
4. Extract libs.7z from vendor\frostium
5. Compile
6. Copy all dll's from vendor\fmod\libs to output (bin) folder
