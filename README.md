## SmolEngine-Editor

![SmolEngine](https://i.imgur.com/ziZbEl0.png)

## SmolEngine-PBRTest
![SmolEngine](https://i.imgur.com/iz1qtff.png)
![SmolEngine](https://i.imgur.com/AAHqxqw.png)

## Core Features

### Implemented in first iteration

- Native rendering API support (Vulkan/OpenGL)

- 2D Layered Batch Renderer

- 2D Animations

- Data-Oriented Entity Component System (ECS)

- 3rd party 2D physics engine (Box2D)

- 3rd party audio engine (FMOD)

- Debug Renderer

- Level Editor

- In-Game UI

- Scripting in C++

### Features to come

- Physically Based Rendering (PBR)

- FBX and glTF formats support

- 3rd party 3D physics engine (Bullet3 / PhysX)

- Asset Manager

- Build System

- Material System

- Particle System (2D/3D)

- Scripting in C# / Java

- Linux and Android support

- Network Solution

- Ray Tracing

## Limitations

- Developed primarily by one person for learning purposes

- At a very early stage

- No documentation yet

## Building
### Windows
1. Install Vulkan SDK (1.2 or higher)
2. Call Win-GenProject.bat or run ```premake5 vs2019```
3. Go to Libraries\vulkan and extract libs.7z
4. Go to Libraries\fmod\libs and copy all dll's to output folder
5. Under configuration properties makes sure that Yojimbo project is set to "build"
