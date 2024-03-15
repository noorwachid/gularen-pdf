#include <iostream>
#include <podofo/podofo.h>

using namespace std;
using namespace PoDoFo;

void generate(const string_view& filename) {
	PdfMemDocument document;
	PdfPainter painter;
	PdfFont* font;

	try {
		auto& page = document.GetPages().CreatePage(PdfPage::CreateStandardPageSize(PdfPageSize::A4));

		painter.SetCanvas(page);

		font = document.GetFonts().SearchFont("Arial");

		if (font == nullptr)
			throw runtime_error("Font is not found in this system");

		auto& metrics = font->GetMetrics();
		cout << "The font name is " << metrics.GetFontName() << endl;
		cout << "The family font name is " << metrics.GetFontFamilyName() << endl;
		cout << "The font file path is " << metrics.GetFilePath() << endl;
		cout << "The font face index is " << metrics.GetFaceIndex() << endl;

		painter.TextState.SetFont(*font, 18);

		painter.DrawText("ABCDEFGHIKLMNOPQRSTVXYZ", 56.69, page.GetRect().Height - 56.69);

		try {
			// Add also some non-ASCII characters (Cyrillic alphabet)
			painter.DrawText("АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЬЫЭЮЯ", 56.69, page.GetRect().Height - 80);
		} catch (PdfError& err) {
			if (err.GetCode() == PdfErrorCode::InvalidFontData)
				cout << "WARNING: The matched font \"" << metrics.GetFontName() << "\" doesn't support cyrillic"
					 << endl;
		}

		painter.FinishDrawing();

		document.GetMetadata().SetCreator(PdfString("examplahelloworld - A PoDoFo test application"));
		document.GetMetadata().SetAuthor(PdfString("Dominik Seichter"));
		document.GetMetadata().SetTitle(PdfString("Hello World"));
		document.GetMetadata().SetSubject(PdfString("Testing the PoDoFo PDF Library"));
		document.GetMetadata().SetKeywords(vector<string>({"Test", "PDF", "Hello World"}));

		document.Save(filename);
	} catch (PdfError& e) {
		try {
			painter.FinishDrawing();
		} catch (...) {
		}

		throw e;
	}
}

int main(int argc, char* argv[]) {
	try {
		generate("out.pdf");
	} catch (PdfError& err) {
		err.PrintErrorMsg();
		return (int) err.GetCode();
	}

	return 0;
}
