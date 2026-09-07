#include <Windows.h>
#include "Core/Game.h"

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd)
{
    Game game(hInstance);

    if (game.Initialize(L"DirectX 11 Terrain Tool", 1280, 720))
    {
        game.Run();
    }

    game.Shutdown();

    return 0;
}
