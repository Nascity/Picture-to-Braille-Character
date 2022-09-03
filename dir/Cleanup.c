#include <Windows.h>
#include <tchar.h>

#include "Cleanup.h"



VOID ReleaseBitmap(HBITMAP hb)
{
	if (hb) DeleteObject(hb);
}

VOID Free2DArr(WCHAR** dpwc, DWORD width, DWORD height)
{
	DWORD i;

	for (i = 0; i < width; i++)
	{
		free(*(dpwc + i));
	}

	free(dpwc);
}