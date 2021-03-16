## Level-Editor
![SmolEngine](https://i.imgur.com/H7PUQVd.png)
![SmolEngine](https://i.imgur.com/vS3DiqW.png)
## Physically Based Rendering
![SmolEngine](https://i.imgur.com/848tYFm.png)

## Core Features

### Implemented in first iteration

- Rendering: Native rendering API support (Vulkan/OpenGL), Physically Based Rendering (PBR), 
  Deferred Rendering, Shadow Mapping, MSAA / FXAA,
  SSAO, 2D/3D Renderer, 2D Animations
  
- Integrations: 3rd party 2D physics engine (Box2D), 3rd party audio engine (FMOD)

- Systems: Data-Oriented Entity Component System (ECS), Jobs System (Multithreading)

- Scripting: C++

- Tools: Level Editor

- 3D-file-formats: .dae, .fbx, .glFT, .3ds, .ase, .obj

### Features to come

- Rendering: Skeletal Animations, Forward Rendering, Real-Time Ray Tracing (Vulkan),
  Post-Processing, User's Shaders, Sprite Atlases

- Systems: Build System, Particles System (2D/3D), Network Solution
    
- Integrations: 3rd party 3D physics engine (PhysX)
  
- Tools: Tile Map Editor
  
- Scripting: C# Binding

## Limitations

- Developed by one person for learning purposes

- At a very early stage

- No documentation yet

## Building
### Windows
1. Install Vulkan SDK (1.2 or higher)
2. Call Win-GenProject.bat or run ```premake5 vs2019```
3. Extract libs.7z in Libraries\vulkan
4. Extract libs.7z in Libraries\assimp
5. Copy all dll's from Libraries\fmod\libs to output (bin\) folder
