#include "Renderer.h"

int main(int argc, char* argv[]) {

	try {
		Renderer renderer;
		renderer.generate("cache/out.pdf");
	} catch (PdfError& err) {
		err.PrintErrorMsg();
		return (int) err.GetCode();
	}

	return 0;
}
