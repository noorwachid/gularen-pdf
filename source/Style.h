#pragma once

#include <podofo/podofo.h>

using namespace PoDoFo;

struct FontFamily {
	PdfFont* regular;
	PdfFont* bold;
	PdfFont* boldItalic;
	PdfFont* italic;
};

struct TextStyle {
	FontFamily fontFamily;
	double fontSize;
	double topSpacing;
	double bottomSpacing;
	double lineSpacing;
};

struct Margin {
	double top;
	double bottom;
	double left;
	double right;
};


struct PageStyle {
	Margin margin;
};

struct HeadingStyle {
	Margin margin;
	TextStyle text;
};

struct ParagraphStyle {
	Margin margin;
	TextStyle text;
};


