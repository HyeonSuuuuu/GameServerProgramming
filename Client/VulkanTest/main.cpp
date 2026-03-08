#include "chess_app.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

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