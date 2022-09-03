//// ��ȯ�� ����
// 1. DIB�� �޾Ƽ� ������ �迭�� �ű� (4�� ��� ���� �� ��)
// 2. WCHAR ������ �迭 ����
// 3. �ȼ��κ��� ������ �޾� �ݿ���
// 4. ������ �迭�� ���ڿ��� ��ȯ�� ���

#include <Windows.h>
#include <tchar.h>
#include <math.h>

#include "Convert.h"



WCHAR** Convert(BITMAP bmp, DETECT dt)
{
	// �ӽ� ����
	DWORD	i;
	DWORD	j;
	DWORD	tmp;

	// �ӽ� ��ġ
	INDEX	indexBitmap;
	INDEX	indexBuffer;

	// �ӽ� �ȼ�
	RGBTRIPLE	tmpPixel;
	BOOL		bPixelComplete = FALSE;

	// �ʿ��� ������ ���� ����ϱ� ���� ����
	DWORD	corrected_width = (2 - (bmp.bmWidth % 2)) % 2 + bmp.bmWidth;
	DWORD	corrected_height = (4 - (bmp.bmHeight % 4)) % 4 + bmp.bmHeight;

	// ��Ʈ�� �� �ٴ� ����Ʈ ��
	DWORD	bytes_per_line = (4 - (bmp.bmWidth * 3) % 4) % 4 + bmp.bmWidth * 3;

	// DIB�� ���� ������ �迭 ����
	WCHAR** pixelarr = malloc(sizeof(WCHAR*) * corrected_height);

	// ���� ó��
	if (!pixelarr) return NULL;

	for (i = 0; i < corrected_height; i++)
	{
		// ������ �迭 �� ���� �Ҵ�
		*(pixelarr + i) = malloc(sizeof(WCHAR) * corrected_width);

		// ���� ó��
		if (!*(pixelarr + i)) return NULL;

		// �⺻������ �޸� �ʱ�ȭ
		memset(*(pixelarr + i), 0, sizeof(WCHAR) * corrected_width / 2);
		for (j = 0; j < corrected_width / 2; j++) *(*(pixelarr + i) + j) += 0x2800;
	}

	for (i = 0; i < bytes_per_line * bmp.bmHeight; i++)
	{
		// ������ ����
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
			// �ȼ��� ���� Ÿ�� ���� ���� ���� ���� ��
			if (abs(tmpPixel.rgbtBlue - dt.target.rgbtBlue) <= dt.sensitivity &&
				abs(tmpPixel.rgbtGreen - dt.target.rgbtGreen) <= dt.sensitivity &&
				abs(tmpPixel.rgbtRed - dt.target.rgbtRed) <= dt.sensitivity)
			{
				// ���� ��ǥ ����
				indexBitmap.x = ((i % bytes_per_line) - ((i % bytes_per_line) % 3)) / 3;
				indexBitmap.y = (i - (i % bytes_per_line)) / bytes_per_line;

				// ��ġ�� ���� ���� ����
				tmp = BitmapIndexToBufferIndexAndAdd(indexBitmap, &indexBuffer);

				// ���� �� ����
				*(*(pixelarr + indexBuffer.y) + indexBuffer.x) += tmp;
			}

			bPixelComplete = FALSE;
		}
	}

	return pixelarr;
}

LPWSTR ConvertToString(WCHAR** dpwc, DWORD width, DWORD height)
{
	// �ӽ� ����
	DWORD	i;
	DWORD	j;

	// ���� �ε��� ��Ͽ�
	DWORD	pointer = 0;

	// �޸� ũ��
	SIZE_T	size = (((width + RETURN_LETTER_SIZE) * height) + 1) * sizeof(WCHAR);

	// �Ҵ�
	LPWSTR	ret = malloc(size);

	// ���� ó��
	if (!ret)	return NULL;

	memset(ret, 0, size);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			*(ret + (pointer++)) = dpwc[i][j];
		}

		// ����
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