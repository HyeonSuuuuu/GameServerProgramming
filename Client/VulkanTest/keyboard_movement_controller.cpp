#include "keyboard_movement_controller.hpp"

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

	void KeyboardMovementController::moveDiscreteXY(GLFWwindow* window, Coordinator& coordinator, float stepX, float stepY) {
		bool isLeftPressed = glfwGetKey(window, keys.moveLeft) == GLFW_PRESS;
		bool isRightPressed = glfwGetKey(window, keys.moveRight) == GLFW_PRESS;
		bool isUpPressed = glfwGetKey(window, keys.moveUp) == GLFW_PRESS;
		bool isDownPressed = glfwGetKey(window, keys.moveDown) == GLFW_PRESS;

		glm::vec2 moveDir {0.f};

		if (isLeftPressed && !wasLeftPressed) moveDir.x -= 1.f;
		if (isRightPressed && !wasRightPressed) moveDir.x += 1.f;
		if (isUpPressed && !wasUpPressed) moveDir.y -= 1.f;
		if (isDownPressed && !wasDownPressed) moveDir.y += 1.f;

		wasLeftPressed = isLeftPressed;
		wasRightPressed = isRightPressed;
		wasUpPressed = isUpPressed;
		wasDownPressed = isDownPressed;

		if (moveDir.x != 0.f || moveDir.y != 0.f) {
			for(auto const& entity : mEntities) {
				auto& transform = coordinator.GetComponent<Transform2dComponent>(entity);

				transform.translation.x += moveDir.x * stepX;
				transform.translation.y += moveDir.y * stepY;

				// Limit boundary to the exact 8x8 chessboard
				float maxBoundX = 3.5f * stepX;
				float maxBoundY = 3.5f * stepY;

				transform.translation.x = glm::clamp(transform.translation.x, -maxBoundX, maxBoundX);
				transform.translation.y = glm::clamp(transform.translation.y, -maxBoundY, maxBoundY);
			}
		}
	}
}
