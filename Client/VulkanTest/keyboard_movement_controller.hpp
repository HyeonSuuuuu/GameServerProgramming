#pragma once
#include "lve_components.hpp"
#include "lve_window.hpp"
#include "Global.h"


class Board;
namespace lve 
{
	class KeyboardMovementController : public System {
	public:
		struct KeyMappings {
			int moveLeft = GLFW_KEY_A;
			int moveRight = GLFW_KEY_D;
			int moveUp = GLFW_KEY_W;
			int moveDown = GLFW_KEY_S;
		};

		void moveInPlaneXY(GLFWwindow* window, float dt, Coordinator& coordinator);
		void processPlayerInput(GLFWwindow* window, Coordinator& coordinator, Board* chessBoard);
		void setServerPosition(Coordinator& coordinator, int x, int y);

		KeyMappings keys{};
		float moveSpeed{ 3.f };

	private:
		bool wasLeftPressed = false;
		bool wasRightPressed = false;
		bool wasUpPressed = false;
		bool wasDownPressed = false;
	};
}