## SmolEngine-Editor

![SmolEngine](https://i.imgur.com/ziZbEl0.png)

## SmolEngine-PBRTest
![SmolEngine](https://i.imgur.com/OnnZqr6.png)
![SmolEngine](https://i.imgur.com/iz1qtff.png)

## Core Features

### Implemented in first iteration

- Native rendering API support (Vulkan/OpenGL)

- Data-Oriented Entity Component System (ECS)

- 2D Layered Batch Renderer / 2D Debug Renderer

- 3rd party 2D physics engine (Box2D)

- 3D-file-formats: .dae, .fbx, .glFT, .3ds, .ase, .obj

- 2D Animations

- 3rd party audio engine (FMOD)

- In-Game UI

- Level Editor

- Scripting in C++

### Features to come

- Renderer:

    Physically Based Rendering (PBR), Multi Sampled Deferred Shading, Real-Time Ray Tracing (Vulkan), Post-processing,
    3D Animations, Multithreading, Materials (user's shaders), Sprite Atlases

- Systems:

    Build System, Jobs System, Particles System (2D and 3D), Network Solution
    
- Physics:
   
    3rd party 3D physics engine (PhysX)

- Assets:
   
    .sAsset format, Asset Manager
  
- Tools:

    Tile Map Editor
  
- Scripting

    C# Binding
  
- Platforms:

    Android Support

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
