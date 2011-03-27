#pragma once
#include "Common.h"

class Font : public resource<Font>
{
public:
	Font();
	virtual ~Font();	

	void create(const string& fontName, 
		int fontHeight, 
		int fontWidth, 
		bool isItalic, RECT* rect);
	void release();

	void write(const char* str, ...);
	void write(std::stringstream& str);

	void setTextFormat(long textParam_)	{ textParam = textParam_;	}
	void setTextColor(long color)		{ textColor = color;		}

	long getTextFormat() { return textParam; }
	long getTextColor()  { return textColor; }

	static void onLostDevice();
	static void onResetDevice();

private:
	LPD3DXFONT	d3dFont;
	long		textParam;
	long		textColor;
	RECT		fontPos;
};
