#pragma once

#include "lve_window.hpp"
#include "lve_components.hpp"
#include "lve_device.hpp"
#include "lve_model.hpp"
#include "lve_renderer.hpp"
#include "Board.hpp"
#include "Network.hpp"
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

		std::shared_ptr<class SimpleRenderSystem> simpleRenderSystem;
		std::shared_ptr<class KeyboardMovementController> playerController;
		std::unique_ptr<Board> chessBoard;
		Networker networker;
		Coordinator coordinator;

	};

	namespace
	{
		std::shared_ptr<LveModel> createSquareModel(LveDevice& device, glm::vec2 offset = { 0.f, 0.f });
		std::shared_ptr<LveModel> createpawnModel(LveDevice& device, glm::vec2 offset = { 0.f, 0.f });
	}
}