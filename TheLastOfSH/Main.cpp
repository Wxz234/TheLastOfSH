#ifdef _WIN64
#include <Windows.h>
#include <WinBase.h>

#include "Graphics/Renderer.h"
#include "Runtime/Scene.h"

UINT Width = 1024, Height = 768;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
		break;

	case WM_GETMINMAXINFO:
		if (lParam) {
			auto info = reinterpret_cast<MINMAXINFO*>(lParam);
			info->ptMinTrackSize.x = 320;
			info->ptMinTrackSize.y = 200;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPreInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
	WNDCLASSEXW wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEXW);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = GetModuleHandle(nullptr);
	wcex.hIcon = LoadIconW(wcex.hInstance, L"IDI_ICON");
	wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wcex.lpszClassName = L"TheLastOfSH";
	wcex.hIconSm = LoadIconW(wcex.hInstance, L"IDI_ICON");
	RegisterClassExW(&wcex);
	RECT rc = { 0, 0, static_cast<LONG>(Width), static_cast<LONG>(Height) };
	auto stype = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX;
	AdjustWindowRect(&rc, stype, FALSE);
	auto Hwnd = CreateWindowExW(0, L"TheLastOfSH", L"TheLastOfSH", stype, 10, 10, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, GetModuleHandle(nullptr), nullptr);

	auto renderer = TheLastOfSH::CreateRenderer(Hwnd, Width, Height);
	auto scene = TheLastOfSH::CreateScene(renderer, Width, Height);

	ShowWindow(Hwnd, SW_SHOW);

	MSG msg{};
	while (msg.message != WM_QUIT) {
		if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		else {
			scene->Draw();
		}
	}
	TheLastOfSH::RemoveScene(scene);
	TheLastOfSH::RemoveRenderer(renderer);
	return 0;
}
#else
#include <iostream>
int main() {
	std::cout << "Error : Please use x64 platform." << std::endl;
	return 0;
}
#endif
