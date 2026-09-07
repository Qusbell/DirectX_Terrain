# DirectX 11 게임 프레임워크

DirectX 11 기반으로 제작된 기본 게임 프레임워크입니다. Unity의 GameObject-Component 아키텍처에서 영감을 받아 제작되었습니다.

## 주요 기능 (Features)

- **GameObject-Component 시스템**: `GameObject`를 생성하고 `Component`를 부착하여 동작을 정의합니다.
- **기본 Transform 제공**: 모든 `GameObject`는 자동으로 `Transform` 컴포넌트를 가집니다.
- **생명주기 메서드 (Lifecycle Methods)**: 컴포넌트는 `Start()`, `Update()`, `Render()`, `OnDestroy()`를 사용할 수 있습니다.
- **시간 관리 (Time Management)**: `TimeManager`가 델타 타임(delta time)과 총 경과 시간을 제공합니다.
- **입력 관리 (Input Management)**: `InputManager`가 키보드 및 마우스 입력을 처리합니다.
- **씬 관리 (Scene Management)**: `SceneManager`가 활성 씬과 씬에 속한 GameObject들을 관리합니다.
- **스프라이트 렌더링 (Sprite Rendering)**: `SpriteRenderer` 컴포넌트를 통해 텍스처가 적용된 2D 스프라이트를 렌더링할 수 있습니다.

## 컴파일 및 실행 방법 (How to Compile and Run)

이 저장소는 C++ 소스 코드와 HLSL 셰이더만 포함하고 있습니다. 프로젝트를 컴파일하고 실행하려면 Visual Studio와 같은 C++ IDE에서 환경을 설정해야 합니다.

### 1. 프로젝트 설정 (Visual Studio)

1. Visual Studio에서 비어 있는 "Windows 데스크톱 애플리케이션(Windows Desktop Application)" 프로젝트를 새로 생성합니다.
2. 이 저장소의 모든 파일과 폴더(`src`, `Shaders`, `Textures`)를 새로 생성한 프로젝트 디렉터리로 복사합니다.
3. Visual Studio 솔루션 탐색기에서 프로젝트를 마우스 오른쪽 버튼으로 클릭한 뒤 **추가 > 기존 항목...** 을 선택합니다. `src` 디렉터리에 있는 모든 `.h` 및 `.cpp` 파일을 추가합니다.

### 2. 셰이더 컴파일 (Compile the Shaders)

이 프로젝트는 DirectX에서 사용할 수 있는 형식(`.cso` 파일)으로 컴파일해야 하는 HLSL 셰이더를 사용합니다. Windows SDK에 포함된 `fxc.exe`를 사용하여 컴파일할 수 있습니다.

1. "VS용 개발자 명령 프롬프트(Developer Command Prompt for VS)"를 엽니다.
2. 프로젝트의 `Shaders` 디렉터리로 이동합니다.
3. 다음 명령어를 실행하여 버텍스(정점) 및 픽셀 셰이더를 컴파일합니다:

    ```sh
    fxc /T vs_5_0 /E main /Fo SpriteVS.cso SpriteVS.hlsl
    fxc /T ps_5_0 /E main /Fo SpritePS.cso SpritePS.hlsl
    ```

4. 컴파일이 완료되면 `Shaders` 디렉터리에 `SpriteVS.cso` 및 `SpritePS.cso` 파일이 생성됩니다. 런타임에 애플리케이션이 이 파일들을 찾을 수 있도록 설정되었는지 확인합니다 (프로젝트 디버깅 설정에서 "작업 디렉터리(Working Directory)"를 지정하거나 출력 디렉터리로 파일을 복사해야 할 수 있습니다).

### 3. 테스트 텍스처 준비 (Provide a Test Texture)

`Game.cpp`의 샘플 코드는 테스트 스프라이트를 위한 텍스처를 로드하려고 시도합니다.

1. 프로젝트의 루트 디렉터리에 `Textures` 폴더를 생성합니다.
2. 폴더 안에 `.png` 이미지 파일을 넣고 이름을 `test.png`로 지정합니다.

위 단계를 모두 완료하면 Visual Studio에서 프로젝트를 컴파일하고 실행할 수 있습니다. 파란색 배경의 창 중앙에 `test.png` 스프라이트가 렌더링되는 것을 확인할 수 있습니다.


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
