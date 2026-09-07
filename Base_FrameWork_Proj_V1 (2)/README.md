# DirectX 11 게임 엔진

DirectXProj_Ver2 폴더에 기본 화면 표시용 프로젝트 진행
DirectX 11 기반의 C++ 게임 엔진입니다. 주요 상용 게임 엔진들에서 착안한 현대적인 컴포넌트 기반 아키텍처로 설계되었습니다.

## 빌드 화면 이동처리
wsad, 휠, 오른버턴, 왼버턴 카메라 이동 적용 Shift 이용한 가중치 이동

![샘플사진](./Docs/빌드샘플이미지/샘플이미지01.png)
![샘플사진](./Docs/빌드샘플이미지/샘플이미지02.png)


## 현재 상태

현재 기본적인 3D 씬 렌더링이 가능한 상태입니다.
- **렌더러(Renderer)**: DirectX 11
- **아키텍처(Architecture)**: GameObject-Component 시스템
- **주요 기능(Features)**:
    - 씬 관리 (Scene Management)
    - 3D 원근 투영 카메라 (3D perspective camera)
    - 기본 디렉셔널 라이트 (Basic directional lighting)
    - 메시 렌더링 (Mesh rendering)
    - 텍스처 기반 스프라이트 렌더링 (Sprite rendering with textures)
- **알려진 이슈(Known Issues)**:
    - 3D 오브젝트가 현재 텍스처 없이 렌더링됩니다 (검은색으로 표시되거나 기본 라이팅만 적용됨).

## 이번 작업 목표

현재 개발 목표는 **3D 오브젝트 텍스처링(Texturing) 구현**입니다. 세부 작업 내용은 다음과 같습니다:
1. `Texture`를 포함할 수 있는 `Material` 클래스 생성
2. `MeshRenderer` 컴포넌트에 `Material` 연동
3. 메시를 그리기 전에 머티리얼의 텍스처를 바인딩하도록 그래픽스 파이프라인 업데이트
4. 기본 씬에 배치된 큐브(Box)에 텍스처를 적용하여 정상 동작 검증 및 시연
