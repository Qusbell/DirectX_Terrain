# DirectX 11 Terrain Tool 기술 설명서

## 1. 이 문서의 목표

이 문서는 DirectX와 게임 엔진 코드를 처음 접하는 학생이 Terrain Tool의 전체 흐름을 이해하도록 돕기 위한 안내서다.

Terrain Tool은 평평한 격자판의 각 점이 가진 높이를 마우스로 바꾸는 프로그램이다. 점의 높이가 바뀌면 산이나 골짜기처럼 보인다. 이때 화면에 보이는 지형은 하나의 그림이 아니라 수많은 삼각형으로 구성된 3차원 메시다.

처음부터 모든 클래스를 이해하려고 하지 말자. 다음 질문에 차례대로 답할 수 있으면 충분하다.

1. 프로그램은 어디에서 시작하는가?
2. 한 프레임 동안 어떤 코드가 호출되는가?
3. 마우스 위치는 어떻게 지형 위의 위치로 바뀌는가?
4. 브러시는 어떤 정점의 높이를 바꾸는가?
5. 변경된 높이는 어떻게 화면에 나타나는가?
6. 저장과 Undo는 어떤 데이터를 기억하는가?

## 2. 먼저 알아야 할 최소 개념

### 정점, 삼각형, 메시

- 정점(Vertex)은 3차원 공간의 점이다. 이 프로젝트의 정점은 위치, 법선, UV를 가진다.
- 인덱스(Index)는 어떤 정점 세 개로 삼각형을 만들지 나타내는 번호다.
- 메시(Mesh)는 정점과 인덱스의 모음이다.
- 법선(Normal)은 표면이 어느 방향을 바라보는지 나타낸다. 조명이 밝거나 어둡게 보이는 방향을 결정한다.
- UV는 2차원 텍스처의 어느 위치를 삼각형에 붙일지 나타내는 좌표다.

이 Terrain Tool의 기본 지형은 129 × 129개의 정점을 가진다. 정점 수는 16,641개다. 인접한 네 정점으로 사각형 하나를 만들고, 사각형은 삼각형 두 개로 나눈다.

### CPU 데이터와 GPU 데이터

CPU는 브러시 계산, 저장, Undo 같은 일반 프로그램 작업을 담당한다. GPU는 계산된 정점과 텍스처를 이용해 화면의 픽셀을 그린다.

따라서 지형 높이를 바꿀 때는 다음 두 데이터가 함께 바뀌어야 한다.

- CPU의 `m_heights`, `m_vertices`: 편집과 저장에 사용하는 원본 데이터
- GPU의 동적 Vertex Buffer: 화면을 그릴 때 사용하는 복사본

CPU 데이터만 바꾸면 저장 값은 변하지만 화면은 그대로다. GPU 데이터만 바꾸면 화면은 변하지만 저장하거나 Undo할 원본이 없다.

### 컴포넌트

`GameObject`는 장면 속 물체이고 `Component`는 그 물체가 하는 일을 담당한다.

- Editor Camera GameObject: `Camera`, `InputComponent`
- Directional Light GameObject: `Light`
- Terrain GameObject: `Terrain`, `MeshRenderer`, `TerrainToolController`

`Terrain`은 지형 데이터와 계산을 담당한다. `MeshRenderer`는 메시를 그린다. `TerrainToolController`는 키보드와 마우스를 해석해 `Terrain`에 작업을 요청한다.

## 3. 가장 먼저 해야 할 코드 리뷰 순서

초보자는 폴더 순서나 파일 이름순으로 읽지 않는 것이 좋다. 실제 호출 흐름을 따라 아래 순서로 읽는다.

### 1단계: 프로그램 시작과 장면 구성

1. `Main.cpp`의 `WinMain`
2. `Core/Game.cpp`의 `Game::Initialize`
3. `Core/Game.cpp`의 `Game::GameLoop`

여기서 카메라, 조명, 지형이 언제 생성되는지 확인한다. 처음 읽을 때는 각 Manager 내부까지 들어가지 않아도 된다.

### 2단계: 한 프레임의 입력과 편집

1. `Input/InputManager.cpp`의 Windows 입력 처리와 `Update`
2. `Engine/TerrainToolController.cpp`의 `Update`
3. 같은 파일의 `HandleShortcuts`, `FinishStroke`, `Undo`, `Redo`

학생이 가장 먼저 집중할 핵심 파일은 `TerrainToolController.cpp`다. 사용자의 행동이 프로그램 명령으로 바뀌는 과정을 가장 직접적으로 보여준다.

### 3단계: 실제 지형 계산

1. `Engine/Terrain.h`에서 공개 함수와 멤버 변수 확인
2. `Terrain::Initialize`
3. `Terrain::BuildMesh`
4. `Terrain::Raycast`
5. `Terrain::ApplyBrush`
6. `Terrain::RecalculateNormals`
7. `Terrain::UploadMesh`

처음에는 수학 공식을 외우지 말고 입력, 변경 대상, 결과만 찾는다. 예를 들어 `ApplyBrush`의 입력은 브러시 중심, 설정, 프레임 시간, 반전 여부이고 결과는 높이 배열의 변경이다.

### 4단계: GPU로 전달하고 화면에 그리기

1. `Engine/Mesh.cpp`의 `Create`, `UpdateVertices`
2. `Engine/MeshRenderer.cpp`의 `Render`
3. `Core/CoreGraphicsManager.cpp`의 `DrawMesh`
4. `Shaders/Basic3D_VS.hlsl`
5. `Shaders/Basic3D_PS.hlsl`

이 단계에서 CPU의 정점이 Vertex Buffer로 복사되고, Vertex Shader와 Pixel Shader를 거쳐 픽셀이 되는 흐름을 확인한다.

### 5단계: 저장과 보조 기능

1. `Terrain::Save`, `Terrain::Load`
2. `TerrainEditCommand::Undo`, `Redo`
3. `TerrainToolController::DrawHudAndBrush`
4. `Graphics/DebugManager.cpp`, `DebugRenderer.cpp`

`PickingManager`, 충돌체, 스프라이트 시스템은 Terrain Tool의 핵심 흐름을 이해한 뒤 읽어도 된다. 현재 지형 브러시는 색상 ID PickingManager가 아니라 `Terrain::Raycast`의 삼각형 교차 검사를 사용한다.

## 4. 전체 파이프라인

### 프로그램 초기화 파이프라인

```text
WinMain
  -> Game::Initialize
      -> Window 생성
      -> DirectX Device / Context / SwapChain 생성
      -> InputManager와 SceneManager 준비
      -> Editor Camera 생성
      -> Directional Light 생성
      -> Terrain GameObject 생성
          -> Terrain::Initialize
              -> 높이 배열 생성
              -> 정점과 인덱스 생성
              -> 동적 Vertex Buffer 생성
              -> MeshRenderer와 Material 연결
          -> TerrainToolController 연결
      -> DebugRenderer와 PickingManager 준비
```

초기화가 끝나면 평평한 지형을 그릴 준비가 된다. 높이 배열의 모든 값은 0이다.

### 프레임 파이프라인

게임 프로그램은 사진을 매우 빠르게 반복해서 그린다. 사진 한 장을 만드는 단위를 프레임이라고 한다.

```text
Window 메시 처리
  -> TimeManager 갱신
  -> Scene Update
      -> Camera 입력 처리
      -> TerrainToolController::Update
  -> BeginFrame
      -> View / Projection 상수 갱신
  -> Scene Render
      -> Terrain의 MeshRenderer::Render
      -> CoreGraphicsManager::DrawMesh
  -> DebugRenderer::Render
      -> HUD와 브러시 원 표시
  -> EndFrame
      -> SwapChain Present
  -> InputManager 상태 전환
```

마지막 입력 상태 전환이 중요하다. 이번 프레임의 `Down`은 다음 프레임에 `Pressed`가 되고, 버튼을 놓으면 `Up`이 된다. 그래서 한 번만 실행할 단축키는 `IsKeyDown`, 누르는 동안 계속 실행할 조형은 `IsKeyPressed`를 사용한다.

### 마우스 조형 파이프라인

```text
화면의 마우스 좌표
  -> Camera::ScreenPointToRay
      -> 화면 좌표를 월드 공간의 Ray로 변환
  -> Terrain::Raycast
      -> Ray와 지형의 모든 삼각형 교차 검사
      -> 가장 가까운 교차점 선택
  -> TerrainToolController
      -> 현재 도구, 반경, 강도, Shift 상태 확인
  -> Terrain::ApplyBrush
      -> 반경 안 정점 검색
      -> 거리 기반 falloff 계산
      -> 높이 변경
  -> Terrain::RecalculateNormals
  -> Mesh::UpdateVertices
      -> Map
      -> CPU 정점을 GPU Vertex Buffer로 복사
      -> Unmap
  -> 다음 Render에서 변경된 지형 표시
```

Ray는 시작점과 방향을 가진 보이지 않는 직선이다. 카메라에서 마우스가 가리키는 방향으로 Ray를 쏜 뒤 지형 삼각형과 만나는 가장 가까운 위치를 브러시 중심으로 사용한다.

현재 구현은 이해하기 쉬운 대신 매 프레임 모든 지형 삼각형을 검사한다. 129 × 129 MVP에서는 동작하지만 더 큰 지형에는 공간 분할 또는 높이 필드 전용 교차 알고리즘이 필요하다.

## 5. 브러시가 높이를 바꾸는 방법

모든 브러시는 먼저 각 정점과 브러시 중심의 수평 거리를 계산한다. 거리가 반경보다 크면 건드리지 않는다.

```text
falloff = 1 - (중심으로부터의 거리 / 브러시 반경)
변화량 = 브러시 강도 × deltaTime × falloff
```

중심에서는 falloff가 1에 가깝고 가장자리에서는 0에 가까워진다. 따라서 경계가 갑자기 꺾이지 않고 부드럽게 변한다. `deltaTime`을 곱하는 이유는 컴퓨터의 프레임 속도가 달라도 초당 변화량을 비슷하게 유지하기 위해서다.

### Raise/Lower

- LMB: 현재 높이에 변화량을 더한다.
- Shift+LMB: 변화량을 뺀다.

### Flatten

스트로크를 시작한 지점의 높이를 목표 높이로 기억한다. 반경 안의 정점은 그 목표 높이에 조금씩 가까워진다.

### Smooth

각 정점 주변 3 × 3 정점의 평균 높이를 계산하고 현재 높이를 평균에 가깝게 이동시킨다. 계산 도중 이미 바뀐 값이 다음 정점 계산에 영향을 주지 않도록 변경 전 높이 배열의 복사본을 사용한다.

## 6. 법선과 렌더링 파이프라인

높이만 바꾸고 법선을 바꾸지 않으면 지형 모양은 변해도 빛은 여전히 평평한 바닥처럼 보인다. `RecalculateNormals`는 한 정점의 왼쪽·오른쪽·위·아래 높이 차이로 경사 방향을 구한다.

GPU 렌더링 흐름은 다음과 같다.

```text
CPU Vertex 배열
  -> D3D11 동적 Vertex Buffer
  -> Input Assembler: 정점 3개씩 삼각형 구성
  -> Basic3D_VS: World -> View -> Projection 좌표 변환
  -> Rasterizer: 삼각형 내부 픽셀 후보 생성
  -> Basic3D_PS: UV로 텍스처 샘플링
  -> Render Target
  -> SwapChain Present
  -> 모니터
```

`Basic3D_PS.hlsl`은 `MaterialConstants`의 tiling과 offset을 사용한다. 이전 테스트 코드에 있던 시간 기반 `uv.x` 이동은 제거되었으므로 지형 텍스처는 흐르지 않는다.

## 7. Undo와 Redo 파이프라인

LMB를 누른 순간 전체 높이 배열을 임시로 복사한다. 드래그하는 동안에는 계속 지형을 편집한다. LMB를 놓으면 편집 전과 편집 후를 비교해 실제로 변한 정점만 `TerrainEditCommand`에 저장한다.

```text
LMB Down -> 편집 전 높이 임시 보관
LMB Pressed -> 여러 프레임 동안 브러시 적용
LMB Up -> 달라진 정점의 index / before / after 저장
Ctrl+Z -> before 적용
Ctrl+Y -> after 적용
```

새로운 편집이 시작되면 Redo 기록은 제거된다. Undo 기록은 최대 64개 스트로크만 보관한다.

## 8. 저장과 불러오기 파이프라인

기본 파일은 실행 작업 폴더 기준 `Assets/Terrain/default.terrain.json`이다.

저장 데이터에는 다음 값이 포함된다.

- 파일 포맷 버전
- 지형의 가로와 세로 크기
- 행과 열의 정점 수
- 모든 정점의 높이 배열
- 현재 브러시 모드, 반경, 강도, Flatten 높이

로드할 때는 파일 버전, 크기, 높이 개수, `NaN` 또는 무한대 여부를 먼저 검사한다. 검사가 실패하면 현재 지형을 바꾸지 않는다. 성공하면 새 메시를 만들고 `MeshRenderer`에 교체한다.

## 9. 입력표

| 입력 | 동작 |
|---|---|
| `1` | Raise/Lower 선택 |
| `2` | Flatten 선택 |
| `3` | Smooth 선택 |
| LMB 드래그 | 브러시 적용 |
| Shift + LMB | Raise/Lower 반전 |
| `[` / `]` | 반경 감소 / 증가 |
| `-` / `=` | 강도 감소 / 증가 |
| RMB 드래그 | 카메라 회전 |
| MMB 드래그 | 카메라 이동 |
| 마우스 휠 | 줌 |
| Alt + LMB | 카메라 Orbit |
| Ctrl + S / O / N | 저장 / 불러오기 / 새 평면 |
| Ctrl + Z / Y | Undo / Redo |

Alt+LMB일 때는 카메라 Orbit이 우선되며 지형 브러시는 적용되지 않는다. 카메라와 조형 도구가 같은 입력을 동시에 사용하지 않도록 컨트롤러에서 차단한다.

## 10. 디버깅할 때 확인할 위치

### 마우스가 지형을 찾지 못한다

1. `Camera::ScreenPointToRay`의 화면 크기와 마우스 좌표 확인
2. `TerrainToolController::m_hasHover` 확인
3. `Terrain::Raycast`의 `found`, `nearest` 확인
4. 카메라 View/Projection과 지형 Transform 확인

### 클릭해도 높이가 변하지 않는다

1. `IsKeyDown(VK_LBUTTON)`에서 `m_stroking`이 true가 되는지 확인
2. `ApplyBrush`에 들어오는 반경과 deltaTime 확인
3. `changed`가 true인지 확인
4. `m_heights`와 `m_vertices[i].pos.y`가 함께 변하는지 확인

### CPU 값은 변하지만 화면은 그대로다

1. `UploadMesh`가 호출되는지 확인
2. `Mesh::UpdateVertices`의 `Map` 성공 여부 확인
3. 메시가 `D3D11_USAGE_DYNAMIC`으로 생성됐는지 확인
4. `MeshRenderer`가 최신 `m_mesh`를 가리키는지 확인

### 텍스처가 이상하게 보인다

1. `TerrainGrid.png` 경로 확인
2. `Material::Apply`에서 b3 상수 버퍼와 t0 텍스처 연결 확인
3. `Basic3D_PS.hlsl`의 `input.Tex * tiling + offset` 확인
4. `CoreGraphicsManager::DrawMesh`가 b3를 다른 버퍼로 덮어쓰지 않는지 확인

## 11. 현재 MVP의 한계와 다음 학습 과제

- Raycast가 모든 삼각형을 검사하므로 고해상도 지형에서는 느려질 수 있다.
- 정점이 조금만 변해도 전체 Vertex Buffer를 다시 복사한다.
- 법선도 전체 지형을 다시 계산한다.
- Undo 시작 시 전체 높이 배열을 임시 복사한다.
- 단일 지형만 편집한다.
- Texture Painting, LOD, Chunk, 물리 높이 필드는 포함하지 않는다.

이 구현은 최종 상용 구조가 아니라 파이프라인을 명확히 학습하기 위한 MVP다. 다음 단계에서는 변경된 격자 범위만 갱신하기, 지형 Chunk 분할, 공간 가속 Raycast, Heightmap PNG 입출력 순으로 확장하는 것이 좋다.

## 12. 빌드 및 실행

1. Visual Studio 2022에서 `TerrainTool/DirectXProj.sln`을 연다.
2. 구성을 `Debug`, 플랫폼을 `x64`로 선택한다.
3. 프로젝트를 빌드한다.
4. 실행 작업 폴더는 `TerrainTool/DirectXProj`로 설정되어 있다.
5. 실행 후 좌측 상단 HUD의 단축키를 따라 테스트한다.

코드 리뷰를 시작할 때는 반드시 `Main.cpp -> Game.cpp -> TerrainToolController.cpp -> Terrain.cpp -> Mesh.cpp -> CoreGraphicsManager.cpp -> HLSL` 순서를 먼저 한 바퀴 돈다. 이후 이해되지 않는 Manager나 수학 함수로 범위를 넓히는 편이 훨씬 쉽다.
