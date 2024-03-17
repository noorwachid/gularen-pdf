#pragma once

#include <iostream>
#include "Style.h"
#include "FontFamilyLoader.h"
#include "WordIterator.h"
#include "Gularen/Frontend/Parser.h"

using std::cout;
using std::endl;

class Renderer {
public:
	void generate(const string_view& filename) {
		_activeTextStyle = nullptr;

		try {
			FontFamily fontFamily = FontFamilyLoader::create(_document, "Queens Park");;

			_pageStyle.margin.top = 56;
			_pageStyle.margin.bottom = 56;
			_pageStyle.margin.left = 56;
			_pageStyle.margin.right = 56;

			_paragraphStyle.text.fontFamily = fontFamily;
			_paragraphStyle.text.fontSize = 12;
			_paragraphStyle.text.topSpacing = 0;
			_paragraphStyle.text.bottomSpacing = 0;
			_paragraphStyle.text.lineSpacing = 4;
			_paragraphStyle.margin.top = 0;
			_paragraphStyle.margin.bottom = 8;
			_paragraphStyle.margin.left = 0;
			_paragraphStyle.margin.right = 0;

			_chapterStyle.text.fontFamily = fontFamily;
			_chapterStyle.text.fontSize = 24;
			_chapterStyle.text.topSpacing = 0;
			_chapterStyle.text.bottomSpacing = 4;
			_chapterStyle.text.lineSpacing = 0;
			_chapterStyle.margin.top = 0;
			_chapterStyle.margin.bottom = 18;
			_chapterStyle.margin.left = 0;
			_chapterStyle.margin.right = 0;

			try {
				_createPage();

				Gularen::Parser parser;
				Gularen::Document* document = parser.parseFile("cache/in.gr");

				if (document == nullptr) {
					throw runtime_error("cannot parse cache/in.gr");
				}

				_compose(document);

			} catch (PdfError& err) {
				if (err.GetCode() == PdfErrorCode::InvalidFontData) {
					cout << "WARNING: The font doesn't support charset in the document" << endl;
				}
			}

			_painter.FinishDrawing();

			_document.GetMetadata().SetCreator(PdfString("examplahelloworld - A PoDoFo test application"));
			_document.GetMetadata().SetAuthor(PdfString("Dominik Seichter"));
			_document.GetMetadata().SetTitle(PdfString("Hello World"));
			_document.GetMetadata().SetSubject(PdfString("Testing the PoDoFo PDF Library"));

			_document.Save(filename);
		} catch (PdfError& e) {
			try {
				_painter.FinishDrawing();
			} catch (...) {
			}

			throw e;
		}
	}

	void _compose(Gularen::Node* node) {
		switch (node->kind) {
			case Gularen::NodeKind::document: {
				for (unsigned int i = 0; i < node->children.size(); i += 1) {
					_compose(node->children.get(i));
				}
				break;
			}
			case Gularen::NodeKind::paragraph: {
				_composeParagraph(node);
				break;
			}
			case Gularen::NodeKind::heading: {
				_composeHeading(node);
				break;
			}
			default: break;
		}
	}

	void _composeHeading(Gularen::Node* node) {
		_setTextStyle(&_chapterStyle.text);

		for (unsigned int i = 0; i < node->children.size(); i += 1) {
			_composeInline(node->children.get(i));
		}

		const PdfFontMetrics& metrics = _activeFont->GetMetrics();

		_cursor.y += ((metrics.GetAscent() + abs(metrics.GetDescent())) * _activeTextStyle->fontSize);
		_cursor.y += _chapterStyle.margin.bottom;
		_cursor.x = _pageStyle.margin.left;
	}

	void _composeParagraph(Gularen::Node* node) {
		_setTextStyle(&_paragraphStyle.text);

		for (unsigned int i = 0; i < node->children.size(); i += 1) {
			_composeInline(node->children.get(i));
		}

		const PdfFontMetrics& metrics = _activeFont->GetMetrics();

		_cursor.y += ((metrics.GetAscent() + abs(metrics.GetDescent())) * _activeTextStyle->fontSize);
		_cursor.y += _paragraphStyle.margin.bottom;
		_cursor.x = _pageStyle.margin.left;
	}

	void _composeInline(Gularen::Node* node) {
		switch (node->kind) {
			case Gularen::NodeKind::text: {
				auto text = static_cast<Gularen::Text*>(node);

				_drawText(string_view(text->content.pointer(), text->content.size()));
				break;
			}

			case Gularen::NodeKind::style: {
				auto style = static_cast<Gularen::Style*>(node);
				switch (style->type) {
					case Gularen::Style::Type::bold:
						_setFontStyle(PdfFontStyle::Bold);
						break;

					case Gularen::Style::Type::italic:
						_setFontStyle(PdfFontStyle::Italic);
						break;
				}
				for (unsigned int i = 0; i < node->children.size(); i += 1) {
					_composeInline(node->children.get(i));
				}
				_setFontStyle(PdfFontStyle::Regular);
				break;
			}

			default: break;
		}
	}

	// PDF 0.0 is at bottom left
	// |
	// |
	// +-----

	void _setTextStyle(TextStyle* style) {
		_activeTextStyle = style;
		_activeFont = _activeTextStyle->fontFamily.regular;
		_painter.TextState.SetFont(*_activeFont, _activeTextStyle->fontSize);
	}

	void _setFontStyle(PdfFontStyle style) {
		if (style == PdfFontStyle::Regular) {
			_activeFont = _activeTextStyle->fontFamily.regular;
			_painter.TextState.SetFont(*_activeFont, _activeTextStyle->fontSize);
		}
		if (style == PdfFontStyle::Bold) {
			_activeFont = _activeTextStyle->fontFamily.bold;
			_painter.TextState.SetFont(*_activeFont, _activeTextStyle->fontSize);
		}
		if (style == PdfFontStyle::Italic) {
			_activeFont = _activeTextStyle->fontFamily.italic;
			_painter.TextState.SetFont(*_activeFont, _activeTextStyle->fontSize);
		}
	}

	void _drawText(string_view view, int style = 0) {
		const PdfFontMetrics& fontMetrics = _activeFont->GetMetrics();
		double ascent = fontMetrics.GetAscent() * _activeTextStyle->fontSize;
		double descent = abs(fontMetrics.GetDescent()) * _activeTextStyle->fontSize;
		double spaceLength = _activeFont->GetStringLength(" ", _painter.TextState);
		double pageHeight = _activePage->GetRect().Height;
		double pageWidth = _activePage->GetRect().Width;

		WordIterator wordIterator(view);

		for (; wordIterator.hasNext(); wordIterator.next()) {
			string_view word = wordIterator.get();

			double length = _activeFont->GetStringLength(word, _painter.TextState);

			if (_cursor.x + length > pageWidth - _pageStyle.margin.right) {
				_cursor.x = _pageStyle.margin.left;
				_cursor.y += ascent + descent + _activeTextStyle->lineSpacing;

				if (_cursor.y + ascent + descent > pageHeight - _pageStyle.margin.bottom) {
					_createPage();
				}
			}

			_painter.DrawText(word, _cursor.x, pageHeight - _cursor.y - ascent);

			_cursor.x += length + spaceLength;
		}
	}

	void _drawMarginLines() {
		_painter.DrawLine(
			0, 
			_activePage->GetRect().Height - _pageStyle.margin.top, 
			_activePage->GetRect().Width, 
			_activePage->GetRect().Height - _pageStyle.margin.top
		);
		_painter.DrawLine(
			0, 
			_pageStyle.margin.bottom, 
			_activePage->GetRect().Width, 
			_pageStyle.margin.bottom
		);

		_painter.DrawLine(
			_pageStyle.margin.left, 
			0,
			_pageStyle.margin.left,
			_activePage->GetRect().Height
		);

		_painter.DrawLine(
			_activePage->GetRect().Width - _pageStyle.margin.right, 
			0,
			_activePage->GetRect().Width - _pageStyle.margin.right,
			_activePage->GetRect().Height
		);
	}

	void _createPage() {
		PdfPage& page = _document.GetPages().CreatePage(PdfPage::CreateStandardPageSize(PdfPageSize::A5));
		_activePage = &page;
		_painter.SetCanvas(*_activePage);

		if (_activeTextStyle) {
			_painter.TextState.SetFont(*_activeFont, _activeTextStyle->fontSize);
		}

		_cursor.x = _pageStyle.margin.left;
		_cursor.y = _pageStyle.margin.top;

		_drawMarginLines();
	}

private:
	PdfMemDocument _document;
	PdfPainter _painter;

	PdfPage* _activePage;
	PdfFont* _activeFont;
	TextStyle* _activeTextStyle;

	PageStyle _pageStyle;
	ParagraphStyle _paragraphStyle;
	HeadingStyle _chapterStyle;

private:
	struct Cursor {
		double x;
		double y;
	};

	Cursor _cursor;
};

