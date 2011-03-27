#include "Common.h"

const int MAX_BUFF = 4096;

Font::Font() : textParam(DT_LEFT), textColor(0xFFFFFFFF) {
	addResource();
}

Font::~Font() {
	removeResource();
}

void Font::create( const string& fontName, 
	int fontHeight, int fontWidth, 
	bool isItalic, RECT* rect )
{
	fontPos = *rect;
	D3DXCreateFont( getDevice(), fontHeight, fontWidth, 0, 0, 
					isItalic, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, 
					0, 0, fontName.c_str(), &d3dFont);
}

void Font::release()
{
	if(d3dFont)
		d3dFont->Release();		
}

void Font::write( const char* str, ... )
{
	char temp[MAX_BUFF];
	char* pTemp = temp;

	va_list argv;
	if(!str) return;

	va_start(argv, str);
		vsprintf(temp, str, argv);
	va_end(argv);

	if(d3dFont)
		d3dFont->DrawText(NULL, temp, -1, &fontPos, textParam, textColor);
}

void Font::write( std::stringstream& str )
{
	string str_( str.str() );
	if(d3dFont)
		d3dFont->DrawText(NULL, str_.c_str(), -1, &fontPos, textParam, textColor);
}

void Font::onLostDevice()
{
	stl_foreach(std::list< Font* >, i, resourceList) 
	{
		if ((*i)->d3dFont != NULL) {
			(*i)->d3dFont->OnLostDevice();
		}
	}
}

void Font::onResetDevice()
{
	stl_foreach(std::list< Font* >, i, resourceList) 
	{
		if ((*i)->d3dFont != NULL)  {
			(*i)->d3dFont->OnResetDevice();
		}
	}
}