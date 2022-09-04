#pragma once

#ifndef _CTRL_H_
#define _CTRL_H_

#define MAX_SENSITIVITY		256

VOID EnableButton(HWND hwnd, INT identifier);

VOID DisableButton(HWND hwnd, INT identifier);

VOID DisplayImage(HWND hwnd, HBITMAP hp);

VOID EnableTrackbar(HWND hwnd, INT identifier);

VOID DisableTrackbar(HWND hwnd, INT identifier);

#endif