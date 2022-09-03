#include <Windows.h>
#include <tchar.h>
#include <windowsx.h>

#include "Resource.h"
#include "Ctrl.h"

VOID EnableButton(HWND hwnd, INT identifier)
{
	Button_Enable(GetDlgItem(hwnd, identifier), TRUE);
}

VOID DisableButton(HWND hwnd, INT identifier)
{
	Button_Enable(GetDlgItem(hwnd, identifier), FALSE);
}

VOID DisplayImage(HWND hwnd, HBITMAP hp)
{
	SendDlgItemMessage(hwnd, PIC_IMAGE, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hp);
}