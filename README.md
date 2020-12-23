## SmolEngine

![SmolEngine](https://i.imgur.com/xoUx4Lh.png)

## Core Features

### Ready

- Native rendering API support (Vulkan/OpenGL)

- 2D Workflow (batch renderer, physics, sprites, animations, UI, etc)

- Data-Oriented Entity Component System (ECS)

- Scripting in C++

- Audio Engine

### Features to come

- Physically Based Rendering (PBR)

- Linux and Android support

- 3D Workflow

- Ray Tracing

## Limitations

- Developed primarily by one person for learning purposes

- At the moment only Windows is supported

- At a very early stage

- No documentation yet

## Building
### Windows
1. Call Win-GenProject.bat or run ```premake5 vs2019```
2. Go to Libraries\vulkan and extract libs.7z
3. Go to Libraries\fmod\libs and copy all dll's to output folder
4. Under configuration properties makes sure that Yojimbo project is set to "build"