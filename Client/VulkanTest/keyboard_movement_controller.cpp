#include "pch.h"
#include "keyboard_movement_controller.hpp"
#include "Board.hpp"
#include "Global.h"

namespace lve {
	void KeyboardMovementController::moveInPlaneXY(GLFWwindow* window, float dt, Coordinator& coordinator)
	{
		glm::vec2 moveDir {};
		if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDir.x += 1.f;
		if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDir.x -= 1.f;
		if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDir.y -= 1.f;
		if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDir.y += 1.f;

		if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
			moveDir = glm::normalize(moveDir);
			

			for(auto const& entity : mEntities) {
				auto& transform = coordinator.GetComponent<Transform2dComponent>(entity);
				transform.translation += moveSpeed * dt * moveDir;
				
				transform.translation.x = glm::clamp(transform.translation.x, -1.f, 1.f);
				transform.translation.y = glm::clamp(transform.translation.y, -1.f, 1.f);
			}
		}
	}

	void KeyboardMovementController::processPlayerInput(GLFWwindow* window, Coordinator& coordinator, Board* chessBoard)
	{
		bool isLeft = glfwGetKey(window, keys.moveLeft) == GLFW_PRESS;
		bool isRight = glfwGetKey(window, keys.moveRight) == GLFW_PRESS;
		bool isUp = glfwGetKey(window, keys.moveUp) == GLFW_PRESS;
		bool isDown = glfwGetKey(window, keys.moveDown) == GLFW_PRESS;


		uint8_t direction = 0;
		if (isLeft && !wasLeftPressed) { direction |= 1; }
		if (isRight && !wasRightPressed) {  direction |= 2; }
		if (isUp && !wasUpPressed) { direction |= 4; }
		if (isDown && !wasDownPressed) { direction |= 8; }

		// 즉시 업데이트
		wasLeftPressed = isLeft;
		wasRightPressed = isRight;
		wasUpPressed = isUp;
		wasDownPressed = isDown;

		if (direction != 0) {
			g_moveFlag |= direction;
		}

		for(auto const& entity : mEntities) {
			auto& transform = coordinator.GetComponent<Transform2dComponent>(entity);
			auto& gridPos = coordinator.GetComponent<GridPositionComponent>(entity);

			transform.translation = chessBoard->getTilePosition(gridPos.x, gridPos.y);
				
		}

	}
	void KeyboardMovementController::setServerPosition(Coordinator& coordinator, int x, int y)
	{
		for (auto const& entity : mEntities) {
			auto& gridPos = coordinator.GetComponent<GridPositionComponent>(entity);

			gridPos.x = x;
			gridPos.y = y;
		}
	}
}
