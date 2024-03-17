#pragma once

#include "Style.h"
#include <podofo/podofo.h>

using namespace PoDoFo;

using std::string_view;
using std::runtime_error;

struct FontFamilyLoader {
	static FontFamily create(PdfDocument& document, string_view name) {
		FontFamily fontFamily;
		PdfFontSearchParams fontSearchParams;
		fontSearchParams.Style = PdfFontStyle::Regular;
		fontFamily.regular = document.GetFonts().SearchFont("Queens Park", fontSearchParams);

		if (fontFamily.regular == nullptr) {
			throw runtime_error("Font is not found in this system");
		}

		fontSearchParams.Style = PdfFontStyle::Bold;
		fontFamily.bold = document.GetFonts().SearchFont("Queens Park", fontSearchParams);

		if (fontFamily.bold == nullptr) {
			throw runtime_error("Font does not have bold style in this system");
		}

		fontSearchParams.Style = PdfFontStyle::Bold | PdfFontStyle::Italic;
		fontFamily.boldItalic = document.GetFonts().SearchFont("Queens Park", fontSearchParams);

		if (fontFamily.boldItalic == nullptr) {
			throw runtime_error("Font does not have bold italic style in this system");
		}

		fontSearchParams.Style = PdfFontStyle::Italic;
		fontFamily.italic = document.GetFonts().SearchFont("Queens Park", fontSearchParams);

		if (fontFamily.italic == nullptr) {
			throw runtime_error("Font does not have italic style in this system");
		}

		return fontFamily;
	}
};

