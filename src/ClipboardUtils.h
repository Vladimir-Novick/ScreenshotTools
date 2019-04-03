#pragma once
class ClipboardUtils
{
public:
	ClipboardUtils();
	~ClipboardUtils();
	bool BitmapToClipboard(HBITMAP hBM, HWND hWnd);
};

