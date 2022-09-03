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
	static OPENFILENAME ofn;				// OPENFILENAME 구조체
	static TCHAR		filename[MAX_PATH];	// 파일 이름

	static HBITMAP	hb;		// 비트맵 오브젝트
	static HBITMAP	hb_def;	// 기본 비트맵 오브젝트
	static BITMAP	bmp;	// 비트맵
	static BOOL		bLoaded;// 로드 되었는가?

	static BOOL		bUseinput;		// 에디트 박스에 적힌 비트맵 크기를 사용
	static DWORD	dwInputWidth;	// 적힌 너비
	static DWORD	dwInputHeight;	// 적힌 높이

	static WCHAR**	braille;	// 결과
	static LPWSTR	string;		// 결과 (문자열)
	static DETECT	dt;			// 타겟 색깔 및 감도

	static DWORD	str_width;	// 문자열 너비
	static DWORD	str_height;	// 문자열 높이

	switch (iMsg)
	{
	case WM_INITDIALOG:
		// filename 초기화
		memset(filename, 0, sizeof(TCHAR) * MAX_PATH);

		// OPENFILENAME 구조체 초기화
		memset(&ofn, 0, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = hwnd;
		ofn.lpstrFilter = _T("24-bits bitmap files (*.bmp)\0*.bmp\0\0");
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFile = filename;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_READONLY;
		
		// 체크박스 초기화
		bUseinput = FALSE;

		// 비트맵 관련 변수 초기화
		hb = NULL;
		bLoaded = FALSE;

		// 기본 비트맵 로딩
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
				// 이미 로드된 경우 지우고 다시 로드
				if (bLoaded)
				{
					// 기본 비트맵으로 바꿈
					DisplayImage(hwnd, hb_def);

					// 비트맵 자원 해제
					ReleaseBitmap(hb);

					// 버튼 다시 비활성화
					DisableButton(hwnd, BUTTON_ERASE);
					DisableButton(hwnd, BUTTON_CONVERT);
				}

				// 체크박스로부터 사진 크기 얻어오기
				if (bUseinput)
				{
					dwInputWidth = GetDlgItemInt(hwnd, INPUT_WIDTH, NULL, FALSE);
					dwInputHeight = GetDlgItemInt(hwnd, INPUT_HEIGHT, NULL, FALSE);
				}

				// 만약 하나라도 0이면 전부 0으로 설정
				if(!dwInputWidth || !dwInputHeight)
				{
					dwInputWidth = 0;
					dwInputHeight = 0;
				}

				// 비트맵 오브젝트 생성
				hb = LoadImage
				(
					NULL, ofn.lpstrFile, IMAGE_BITMAP,
					dwInputWidth, dwInputHeight,
					LR_CREATEDIBSECTION | LR_LOADFROMFILE
				);

				// 비트맵 추출
				GetObject(hb, sizeof(BITMAP), &bmp);

				// 비트맵 픽셀 당 비트 수 확인 (24bpp여야 함)
				if (bmp.bmBitsPixel == 24)
				{
					// 지우기, 변환하기 버튼 활성화
					EnableButton(hwnd, BUTTON_ERASE);
					EnableButton(hwnd, BUTTON_CONVERT);

					// 사진을 비트맵 컨트롤에 띄우기
					DisplayImage(hwnd, hb);

					bLoaded = TRUE;
				}
			}
			break;

		case BUTTON_ERASE:
			// 기본 비트맵으로 바꿈
			DisplayImage(hwnd, hb_def);

			// 비트맵 자원 해제
			ReleaseBitmap(hb);

			// 버튼 다시 비활성화
			DisableButton(hwnd, BUTTON_ERASE);
			DisableButton(hwnd, BUTTON_CONVERT);

			break;

		case BUTTON_CONVERT:
			// 임시로 해둔 거
			// 나중에 바꿀 수 있도록 코드 짜야함
			dt.target.rgbtBlue = 0xff;
			dt.target.rgbtGreen = 0xff;
			dt.target.rgbtRed = 0xff;
			dt.sensitivity = 20;

			// 사진-배열 변환
			braille = Convert(bmp, dt);

			// 문자열 너비 구함
			str_width = ((2 - (bmp.bmWidth % 2)) % 2 + bmp.bmWidth) / 2;
			str_height = ((4 - (bmp.bmHeight % 4)) % 4 + bmp.bmHeight) / 4;

			// 배열-문자열 변환
			string = ConvertToString(braille, str_width, str_height);

			// 문자열 표시
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