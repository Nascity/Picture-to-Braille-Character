#include <Windows.h>
#include <tchar.h>

#include "Resource.h"
#include "Winmain.h"

#pragma warning(disable:28251)

HINSTANCE hInst;

INT WINAPI _tWinMain
(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine, INT nShowCmd)
{
	WNDCLASSEX    wndclass;
	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = _T("PTB");
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&wndclass);

	hInst = hInstance;

	HWND hwnd;
	hwnd = CreateWindow
	(
		_T("PTB"),
		_T("PTB"),
		WS_POPUP,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (INT)msg.wParam;
}

LRESULT CALLBACK WndProc
(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_CREATE:
		SetTimer(hwnd, 1, 1, NULL);
		break;

	case WM_TIMER:
		KillTimer(hwnd, 1);
		DialogBox(hInst, DIALOG_MAIN, hwnd, DlgProc);

	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}