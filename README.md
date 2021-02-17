## SmolEngine

![SmolEngine](https://i.imgur.com/U4A5J7V.png)
![SmolEngine](https://i.imgur.com/iHBPll5.png)

## Core Features

### Implemented in first iteration

- Renderering: Native rendering API support (Vulkan/OpenGL), Physically Based Rendering (PBR), 
  Multi Sampled Deferred Shading, Multisample Anti-Aliasing (MSAA),
  Screen Space Ambient Occlusion (SSAO), 2D/3D Renderer, 2D Animations
  
- Integrations: 3rd party 2D physics engine (Box2D), 3rd party audio engine (FMOD)

- Systems: Data-Oriented Entity Component System (ECS)

- Scripting: C++

- Tools: Level Editor

- 3D-file-formats: .dae, .fbx, .glFT, .3ds, .ase, .obj

### Features to come

- Renderering: Cascaded Shadow Mapping, Real-Time Ray Tracing (Vulkan), Post-processing,
    3D Animations, Multithreading, Materials (user's shaders), Sprite Atlases

- Systems: Build System, Jobs System, Particles System (2D and 3D), Network Solution, Asset Manager
    
- Integrations: 3rd party 3D physics engine (PhysX)
  
- Tools: Tile Map Editor
  
- Scripting: C# Binding
  
- Platforms: Android Support

## Limitations

- Developed primarily by one person for learning purposes

- At a very early stage

- No documentation yet

## Building
### Windows
1. Install Vulkan SDK (1.2 or higher)
2. Call Win-GenProject.bat or run ```premake5 vs2019```
3. Extract libs.7z in Libraries\vulkan
4. Extract libs.7z in Libraries\assimp
5. Copy all dll's from Libraries\fmod\libs to output (bin\) folder
6. Under configuration properties makes sure that Yojimbo project is set to "build"
