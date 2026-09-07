# DirectX 11 Game Framework

This is a basic game framework built with DirectX 11. It is inspired by Unity's GameObject-Component architecture.

## Features

- **GameObject-Component System**: Create `GameObject`s and attach `Component`s to define behavior.
- **Default Transform**: Every `GameObject` automatically gets a `Transform` component.
- **Lifecycle Methods**: Components can use `Start()`, `Update()`, `Render()`, and `OnDestroy()`.
- **Time Management**: A `TimeManager` provides delta time and total elapsed time.
- **Input Management**: An `InputManager` handles keyboard and mouse input.
- **Scene Management**: A `SceneManager` manages the active scene and its GameObjects.
- **Sprite Rendering**: The `SpriteRenderer` component can render 2D textured sprites.

## How to Compile and Run

This repository contains only the C++ source code and HLSL shaders. To compile and run this project, you will need to set it up in a C++ IDE like Visual Studio.

### 1. Project Setup (Visual Studio)

1.  Create a new, empty "Windows Desktop Application" project in Visual Studio.
2.  Copy all the files and folders from this repository (`src`, `Shaders`, `Textures`) into your new project's directory.
3.  In the Visual Studio Solution Explorer, right-click your project and select "Add > Existing Item...". Add all the `.h` and `.cpp` files from the `src` directory.

### 2. Compile the Shaders

This project uses HLSL shaders that must be compiled into a format DirectX can use (`.cso` files). You can compile them using `fxc.exe`, which is included with the Windows SDK.

1.  Open the "Developer Command Prompt for VS".
2.  Navigate to the `Shaders` directory in the project.
3.  Run the following commands to compile the vertex and pixel shaders:

    ```sh
    fxc /T vs_5_0 /E main /Fo SpriteVS.cso SpriteVS.hlsl
    fxc /T ps_5_0 /E main /Fo SpritePS.cso SpritePS.hlsl
    ```

4.  This will create `SpriteVS.cso` and `SpritePS.cso` in the `Shaders` directory. Make sure your application can find these files at runtime (you may need to set the "Working Directory" in your project's debugging settings or copy the files to the output directory).

### 3. Provide a Test Texture

The sample code in `Game.cpp` tries to load a texture for a test sprite.

1.  Create a directory named `Textures` in your project's root directory.
2.  Place a `.png` image file inside it and name it `test.png`.

After completing these steps, you should be able to compile and run the project from Visual Studio. You will see a window with a blue background and your `test.png` sprite rendered in the center.
