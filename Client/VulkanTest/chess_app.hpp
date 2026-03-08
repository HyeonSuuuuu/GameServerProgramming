#pragma once

#include "lve_window.hpp"
#include "lve_game_object.hpp"
#include "lve_device.hpp"
#include "lve_model.hpp"
#include "lve_renderer.hpp"

#include <memory>
#include <vector>

namespace lve
{
	class ChessApp {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		ChessApp();
		~ChessApp() = default;

		ChessApp(const ChessApp&) = delete;
		ChessApp& operator=(const ChessApp&) = delete;

		void run();

	private:

		void loadGameObjects();

		LveWindow lveWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
		LveDevice lveDevice{ lveWindow };
		LveRenderer lveRenderer{ lveWindow, lveDevice };

		std::vector<LveGameObject> gameObjects;
	};
}