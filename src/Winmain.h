#pragma once

#ifndef _WINMAIN_H_
#define _WINMAIN_H_

INT WINAPI _tWinMain
(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPTSTR lpCmdLine, INT nShowCmd);

LRESULT CALLBACK WndProc
(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

INT_PTR CALLBACK DlgProc
(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

#endif
