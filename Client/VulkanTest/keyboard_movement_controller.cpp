#include "pch.h"
#include "keyboard_movement_controller.hpp"
#include "Board.hpp"

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

	void KeyboardMovementController::moveDiscreteXY(GLFWwindow* window, Coordinator& coordinator, Board* chessBoard)
	{
		bool isLeft = glfwGetKey(window, keys.moveLeft) == GLFW_PRESS;
		bool isRight = glfwGetKey(window, keys.moveRight) == GLFW_PRESS;
		bool isUp = glfwGetKey(window, keys.moveUp) == GLFW_PRESS;
		bool isDown = glfwGetKey(window, keys.moveDown) == GLFW_PRESS;

		uint8_t moveFlag{};
		int dx = 0, dy = 0;

		if (isLeft && !wasLeftPressed) { dx = -1; moveFlag |= 1; }
		if (isRight && !wasRightPressed) { dx = 1;  moveFlag |= 2; }
		if (isUp && !wasUpPressed) { dy = -1; moveFlag |= 4; }
		if (isDown && !wasDownPressed) { dy = 1;  moveFlag |= 8; }

		// 즉시 업데이트
		wasLeftPressed = isLeft;
		wasRightPressed = isRight;
		wasUpPressed = isUp;
		wasDownPressed = isDown;

		if (dx != 0.f || dy != 0.f) {
			for(auto const& entity : mEntities) {
				auto& gridPos = coordinator.GetComponent<GridPositionComponent>(entity);
				auto& transform = coordinator.GetComponent<Transform2dComponent>(entity);
				
				int nextX = gridPos.x + dx;
				int nextY = gridPos.y + dy;

				if (nextX >= 0 && nextX < chessBoard->GetBoardSize() && nextY >= 0 && nextY < chessBoard->GetBoardSize()) {
					gridPos.x = nextX;
					gridPos.y = nextY;

					// 7. 시각적 위치 업데이트
					transform.translation = chessBoard->getTilePosition(gridPos.x, gridPos.y);

					// TODO: 네트워크 서버가 있다면 여기서 moveFlag 전송
				}
			}
		}
	}
}
