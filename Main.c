#include <Windows.h>
#include <tchar.h>

#include "Resource.h"


#include "Cleanup.h"
#include "Convert.h"
#include "Ctrl.h"

#include "Winmain.h"

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


#define DEFALUT_SIZE	256



INT_PTR CALLBACK DlgProc
(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static OPENFILENAME ofn;				// OPENFILENAME ����ü
	static TCHAR		filename[MAX_PATH];	// ���� �̸�

	static HBITMAP	hb;		// ��Ʈ�� ������Ʈ
	static HBITMAP	hb_def;	// �⺻ ��Ʈ�� ������Ʈ
	static BITMAP	bmp;	// ��Ʈ��
	static BOOL		bLoaded;// �ε� �Ǿ��°�?

	static BOOL		bUseinput;		// ����Ʈ �ڽ��� ���� ��Ʈ�� ũ�⸦ ���
	static DWORD	dwInputWidth;	// ���� �ʺ�
	static DWORD	dwInputHeight;	// ���� ����

	static WCHAR**	braille;	// ���
	static LPWSTR	string;		// ��� (���ڿ�)
	static DETECT	dt;			// Ÿ�� ���� �� ����

	static DWORD	str_width;	// ���ڿ� �ʺ�
	static DWORD	str_height;	// ���ڿ� ����

	switch (iMsg)
	{
	case WM_INITDIALOG:
		// filename �ʱ�ȭ
		memset(filename, 0, sizeof(TCHAR) * MAX_PATH);

		// OPENFILENAME ����ü �ʱ�ȭ
		memset(&ofn, 0, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = hwnd;
		ofn.lpstrFilter = _T("24-bits bitmap files (*.bmp)\0*.bmp\0\0");
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFile = filename;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_READONLY;
		
		// üũ�ڽ� �ʱ�ȭ
		bUseinput = FALSE;

		// ��Ʈ�� ���� ���� �ʱ�ȭ
		hb = NULL;
		bLoaded = FALSE;

		// �⺻ ��Ʈ�� �ε�
		hb_def = LoadImage
		(
			NULL, _T("def.bmp"), IMAGE_BITMAP,
			0, 0, LR_LOADFROMFILE
		);
		
		break;
		
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case CHECKBOX_USE:
			bUseinput = !bUseinput;
			break;

		case BUTTON_LOAD:
			if (GetOpenFileName(&ofn))
			{
				// �̹� �ε�� ��� ����� �ٽ� �ε�
				if (bLoaded)
				{
					// �⺻ ��Ʈ������ �ٲ�
					DisplayImage(hwnd, hb_def);

					// ��Ʈ�� �ڿ� ����
					ReleaseBitmap(hb);

					// ��ư �ٽ� ��Ȱ��ȭ
					DisableButton(hwnd, BUTTON_ERASE);
					DisableButton(hwnd, BUTTON_CONVERT);
				}

				// üũ�ڽ��κ��� ���� ũ�� ������
				if (bUseinput)
				{
					dwInputWidth = GetDlgItemInt(hwnd, INPUT_WIDTH, NULL, FALSE);
					dwInputHeight = GetDlgItemInt(hwnd, INPUT_HEIGHT, NULL, FALSE);
				}

				// ���� �ϳ��� 0�̸� ���� 0���� ����
				if(!dwInputWidth || !dwInputHeight)
				{
					dwInputWidth = 0;
					dwInputHeight = 0;
				}

				// ��Ʈ�� ������Ʈ ����
				hb = LoadImage
				(
					NULL, ofn.lpstrFile, IMAGE_BITMAP,
					dwInputWidth, dwInputHeight,
					LR_CREATEDIBSECTION | LR_LOADFROMFILE
				);

				// ��Ʈ�� ����
				GetObject(hb, sizeof(BITMAP), &bmp);

				// ��Ʈ�� �ȼ� �� ��Ʈ �� Ȯ�� (24bpp���� ��)
				if (bmp.bmBitsPixel == 24)
				{
					// �����, ��ȯ�ϱ� ��ư Ȱ��ȭ
					EnableButton(hwnd, BUTTON_ERASE);
					EnableButton(hwnd, BUTTON_CONVERT);

					// ������ ��Ʈ�� ��Ʈ�ѿ� ����
					DisplayImage(hwnd, hb);

					bLoaded = TRUE;
				}
			}
			break;

		case BUTTON_ERASE:
			// �⺻ ��Ʈ������ �ٲ�
			DisplayImage(hwnd, hb_def);

			// ��Ʈ�� �ڿ� ����
			ReleaseBitmap(hb);

			// ��ư �ٽ� ��Ȱ��ȭ
			DisableButton(hwnd, BUTTON_ERASE);
			DisableButton(hwnd, BUTTON_CONVERT);

			break;

		case BUTTON_CONVERT:
			// �ӽ÷� �ص� ��
			// ���߿� �ٲ� �� �ֵ��� �ڵ� ¥����
			dt.target.rgbtBlue = 0xff;
			dt.target.rgbtGreen = 0xff;
			dt.target.rgbtRed = 0xff;
			dt.sensitivity = 20;

			// ����-�迭 ��ȯ
			braille = Convert(bmp, dt);

			// ���ڿ� �ʺ� ����
			str_width = ((2 - (bmp.bmWidth % 2)) % 2 + bmp.bmWidth) / 2;
			str_height = ((4 - (bmp.bmHeight % 4)) % 4 + bmp.bmHeight) / 4;

			// �迭-���ڿ� ��ȯ
			string = ConvertToString(braille, str_width, str_height);

			// ���ڿ� ǥ��
			SetDlgItemTextW(hwnd, OUTPUT_EDIT, string);

			break;

		case BUTTON_EXIT:
			goto End;
			break;
		}
		break;

	End:
	case WM_CLOSE:
	case WM_DESTROY:
		ReleaseBitmap(hb);
		ReleaseBitmap(hb_def);

		EndDialog(hwnd, 0);
		break;
	}

	return 0;
}