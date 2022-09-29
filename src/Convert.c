//// 변환의 절차
// 1. DIB를 받아서 이차원 배열로 옮김 (4의 배수 잊지 말 것)
// 2. WCHAR 이차원 배열 생성
// 3. 픽셀로부터 정보를 받아 반영함
// 4. 이차원 배열을 문자열로 변환해 출력

#include <Windows.h>
#include <tchar.h>
#include <math.h>

#include "Convert.h"



WCHAR** Convert(BITMAP bmp, DETECT dt)
{
	// 임시 변수
	DWORD	i;
	DWORD	j;
	DWORD	tmp;

	// 임시 위치
	INDEX	indexBitmap;
	INDEX	indexBuffer;

	// 임시 픽셀
	RGBTRIPLE	tmpPixel;
	BOOL		bPixelComplete = FALSE;

	// 필요한 점자의 수를 계산하기 위한 변수
	DWORD	corrected_width = (2 - (bmp.bmWidth % 2)) % 2 + bmp.bmWidth;
	DWORD	corrected_height = (4 - (bmp.bmHeight % 4)) % 4 + bmp.bmHeight;

	// 비트맵 한 줄당 바이트 수
	DWORD	bytes_per_line = (4 - (bmp.bmWidth * 3) % 4) % 4 + bmp.bmWidth * 3;

	// DIB를 받을 이차원 배열 생성
	WCHAR** pixelarr = malloc(sizeof(WCHAR*) * corrected_height);

	// 오류 처리
	if (!pixelarr) return NULL;

	for (i = 0; i < corrected_height; i++)
	{
		// 이차원 배열 내 공간 할당
		*(pixelarr + i) = malloc(sizeof(WCHAR) * corrected_width);

		// 오류 처리
		if (!*(pixelarr + i)) return NULL;

		// 기본값으로 메모리 초기화
		memset(*(pixelarr + i), 0, sizeof(WCHAR) * corrected_width / 2);
		for (j = 0; j < corrected_width / 2; j++) *(*(pixelarr + i) + j) += 0x2800;
	}

	for (i = 0; i < bytes_per_line * bmp.bmHeight; i++)
	{
		// 오프셋 제거
		if ((i + 1) % bytes_per_line > bmp.bmWidth * 3) continue;
		
		if ((i % bytes_per_line) % 3 == 0) tmpPixel.rgbtRed = ((LPBYTE)bmp.bmBits)[i];
		if ((i % bytes_per_line) % 3 == 1) tmpPixel.rgbtGreen = ((LPBYTE)bmp.bmBits)[i];
		if ((i % bytes_per_line) % 3 == 2)
		{
			tmpPixel.rgbtBlue = ((LPBYTE)bmp.bmBits)[i];
			bPixelComplete = TRUE;
		}

		if (bPixelComplete)
		{
			// 픽셀의 색이 타겟 색과 감도 내에 있을 때 (sRGB 알고리즘)
			if (DetectColor(tmpPixel, dt))
			{
				// 보낼 좌표 제작
				indexBitmap.x = ((i % bytes_per_line) - ((i % bytes_per_line) % 3)) / 3;
				indexBitmap.y = (corrected_height - (i - (i % bytes_per_line)) / bytes_per_line);

				// 위치와 더할 값을 얻음
				tmp = BitmapIndexToBufferIndexAndAdd(indexBitmap, &indexBuffer);

				// 더할 값 더함
				*(*(pixelarr + indexBuffer.y) + indexBuffer.x) += tmp;
			}

			bPixelComplete = FALSE;
		}
	}

	return pixelarr;
}

LPWSTR ConvertToString(WCHAR** dpwc, DWORD width, DWORD height)
{
	// 임시 변수
	DWORD	i;
	DWORD	j;

	// 현재 인덱스 기록용
	DWORD	pointer = 0;

	// 메모리 크기
	SIZE_T	size = (((width + RETURN_LETTER_SIZE) * height) + 1) * sizeof(WCHAR);

	// 할당
	LPWSTR	ret = malloc(size);

	// 오류 처리
	if (!ret)	return NULL;

	memset(ret, 0, size);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			*(ret + (pointer++)) = dpwc[i][j];
		}

		// 개행
		*(ret + (pointer++)) += '\r';
		*(ret + (pointer++)) += '\n';
	}

	return ret;
}

DWORD BitmapIndexToBufferIndexAndAdd(INDEX indexBitmap, LPINDEX indexBuffer)
{
	//		|	0	1
	//	_____________
	//	0	|	1	4
	//	1	|	2	5
	//	2	|	3	6
	//	3	|	7	8

	POINT p;
	POINT remain;

	DWORD shift;

	remain.x = indexBitmap.x % 2;
	remain.y = indexBitmap.y % 4;

	p.x = (indexBitmap.x - remain.x) / 2;
	p.y = (indexBitmap.y - remain.y) / 4;

	indexBuffer->x = p.x;
	indexBuffer->y = p.y;

	if (remain.y == 3)	shift = remain.x + 7;
	else				shift = remain.x ? remain.y + 4 : remain.y + 1;

	return 1 << (shift - 1);
}

BOOL DetectColor(RGBTRIPLE color, DETECT dt)
{
	DOUBLE	dstBlue_Sqred = pow(((DOUBLE)color.rgbtBlue) - ((DOUBLE)dt.target.rgbtBlue), 2);
	DOUBLE	dstGreen_Sqrd = pow(((DOUBLE)color.rgbtGreen) - ((DOUBLE)dt.target.rgbtGreen), 2);
	DOUBLE	dstRed_Sqrd = pow(((DOUBLE)color.rgbtRed) - ((DOUBLE)dt.target.rgbtRed), 2);
	
	DOUBLE	dst_Sqrd = dstBlue_Sqred + dstGreen_Sqrd + dstRed_Sqrd;

	if (sqrt(dst_Sqrd) <= (DOUBLE)dt.sensitivity)	return TRUE;
	else											return FALSE;
}