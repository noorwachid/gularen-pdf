#pragma once

#include <podofo/podofo.h>

using namespace PoDoFo;

struct FontFamily {
	PdfFont* regular;
	PdfFont* bold;
	PdfFont* boldItalic;
	PdfFont* italic;
};

enum class TextAlignment {
	left,
	center,
	right,
};

struct TextStyle {
	FontFamily fontFamily;
	double fontSize;
	double topSpacing;
	double bottomSpacing;
	double lineSpacing;
	TextAlignment alignment;
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

struct ParagraphStyle {
	Margin margin;
	TextStyle text;
};

struct HeadingStyle {
	Margin margin;
	TextStyle text;
};

struct IndentStyle {
	Margin margin;
};

