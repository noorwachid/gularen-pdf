#include <iostream>
#include <podofo/podofo.h>
#include "WordIterator.h"

using namespace std;
using namespace PoDoFo;

class Renderer {
public:
	PdfMemDocument _document;
	PdfPainter _painter;
	PdfFont* _font;

public:
	void generate(const string_view& filename) {
		try {
			auto& page = _document.GetPages().CreatePage(PdfPage::CreateStandardPageSize(PdfPageSize::A5));

			_painter.SetCanvas(page);

			_font = _document.GetFonts().SearchFont("Arial");

			if (_font == nullptr) {
				throw runtime_error("Font is not found in this system");
			}

			auto& metrics = _font->GetMetrics();

			try {
				drawText(page, "To Sherlock Holmes she is always the woman. I have seldom heard him mention her under any other name.");
				// drawText(page, "To Sherlock Holmes");
			} catch (PdfError& err) {
				if (err.GetCode() == PdfErrorCode::InvalidFontData)
					cout << "WARNING: The matched font \"" << metrics.GetFontName() << "\" doesn't support character in the document" << endl;
			}

			_painter.FinishDrawing();

			_document.GetMetadata().SetCreator(PdfString("examplahelloworld - A PoDoFo test application"));
			_document.GetMetadata().SetAuthor(PdfString("Dominik Seichter"));
			_document.GetMetadata().SetTitle(PdfString("Hello World"));
			_document.GetMetadata().SetSubject(PdfString("Testing the PoDoFo PDF Library"));
			_document.GetMetadata().SetKeywords(vector<string>({"Test", "PDF", "Hello World"}));

			_document.Save(filename);
		} catch (PdfError& e) {
			try {
				_painter.FinishDrawing();
			} catch (...) {
			}

			throw e;
		}
	}

	// PDF 0.0 is at bottom left
	// |
	// |
	// +-----

	void drawText(PdfPage& page, string_view view) {
		double margin = 56.69;
		double fontSize = 12;
		double lineHeight = 1.2;
		double x = margin;
		double y = page.GetRect().Height - (lineHeight * fontSize) - margin;

		_painter.TextState.SetFont(*_font, fontSize);

		double spaceLength = _font->GetStringLength(" ", _painter.TextState);

		WordIterator wordIterator(view);

		for (; wordIterator.hasNext(); wordIterator.next()) {
			string_view word = wordIterator.get();

			double length = _font->GetStringLength(word, _painter.TextState);

			cout << "word: <" << word << ">\n";

			if (x + length > page.GetRect().Width - margin) {
				x = margin;
				y -= (lineHeight * fontSize);
			}

			_painter.DrawText(word, x, y);

			x += length + spaceLength;
		}
	}
};

int main(int argc, char* argv[]) {

	try {
		Renderer renderer;
		renderer.generate("out.pdf");
	} catch (PdfError& err) {
		err.PrintErrorMsg();
		return (int) err.GetCode();
	}

	return 0;
}
