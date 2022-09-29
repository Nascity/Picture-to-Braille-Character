#include <Windows.h>
#include <tchar.h>

#include <windowsx.h>
#include <commctrl.h>

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

VOID EnableTrackbar(HWND hwnd, INT identifier)
{
	HWND trackbar = GetDlgItem(hwnd, identifier);

	EnableWindow(trackbar, TRUE);

	SendMessage(trackbar, TBM_SETRANGEMIN, TRUE, 0);
	SendMessage(trackbar, TBM_SETRANGEMAX, TRUE, MAX_SENSITIVITY);
}

VOID DisableTrackbar(HWND hwnd, INT identifier)
{
	EnableWindow(GetDlgItem(hwnd, identifier), FALSE);
}