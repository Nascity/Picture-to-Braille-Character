#pragma once

#ifndef _CONVERT_H_
#define _CONVERT_H_

#define RETURN_LETTER_SIZE	2

typedef struct _DETECT
{
	RGBTRIPLE	target;
	INT			sensitivity;
} DETECT;

typedef struct _INDEX
{
	DWORD	x;
	DWORD	y;
} INDEX, *LPINDEX;

WCHAR** Convert(BITMAP bmp, DETECT dt);

LPWSTR ConvertToString(WCHAR** dpwc, DWORD width, DWORD height);

DWORD BitmapIndexToBufferIndexAndAdd(INDEX indexBitmap, LPINDEX indexBuffer);

#endif