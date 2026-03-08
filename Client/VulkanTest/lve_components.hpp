#pragma once

#include "lve_model.hpp"
#include "lve_ecs.hpp"

#include <memory>
#include <vector>

namespace lve
{
	struct Transform2dComponent {
		glm::vec2 translation{};
		glm::vec2 scale{ 1.f, 1.f };
		float rotation = 0.f;
	};

	struct RenderComponent {
		std::shared_ptr<LveModel> model{};
		glm::vec3 color{};
	};

	struct PlayerInputComponent {
		bool isPlayer = true;
	};
}