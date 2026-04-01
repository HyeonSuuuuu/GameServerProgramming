#include "pch.h"
#include "chess_app.hpp"

int main()
{
	try {
		lve::ChessApp app{};
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}