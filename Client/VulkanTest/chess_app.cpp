#include "pch.h"
#include "chess_app.hpp"
#include "simple_render_system.hpp"
#include "keyboard_movement_controller.hpp"
#include "lve_utils.hpp"
// Vulkan Game Engine Tutorial 참고

namespace lve
{
	
	ChessApp::ChessApp() {
		coordinator.Init();
		coordinator.RegisterComponent<Transform2dComponent>();
		coordinator.RegisterComponent<RenderComponent>();
		coordinator.RegisterComponent<PlayerInputComponent>();
		coordinator.RegisterComponent<GridPositionComponent>();

		simpleRenderSystem = coordinator.RegisterSystem<SimpleRenderSystem>(lveDevice, lveRenderer.getSwapChainRenderPass());
		{
			Signature signature;
			signature.set(coordinator.GetComponentType<Transform2dComponent>());
			signature.set(coordinator.GetComponentType<RenderComponent>());
			coordinator.SetSystemSignature<SimpleRenderSystem>(signature);
		}

		playerController = coordinator.RegisterSystem<KeyboardMovementController>();
		{
			Signature signature;
			signature.set(coordinator.GetComponentType<Transform2dComponent>());
			signature.set(coordinator.GetComponentType<PlayerInputComponent>());
			coordinator.SetSystemSignature<KeyboardMovementController>(signature);
		}

		loadGameObjects();
	}
	
	void ChessApp::run()
	{
		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!lveWindow.shouldClose()) 
		{
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			// 플레이어 엔티티(마커가 있는 엔티티) 이동
			float tileStepY = 2.0f / 8.0f;
			float tileStepX = tileStepY * (static_cast<float>(HEIGHT) / WIDTH);
			playerController->processPlayerInput(lveWindow.getGLFWwindow(), coordinator, chessBoard.get());
			playerController->setServerPosition(coordinator, g_x.load(), g_y.load());

			if (auto commandBuffer = lveRenderer.beginFrame())
			{
				lveRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem->renderEntities(commandBuffer, coordinator);
				lveRenderer.endSwapChainRenderPass(commandBuffer);
				lveRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(lveDevice.device());
	}

	namespace 
	{
		std::shared_ptr<LveModel> createSquareModel(LveDevice& device, glm::vec2 offset = { 0.f, 0.f }) 
		{
			std::vector<LveModel::Vertex> squareVertices{
				{{-0.5f + offset.x, -0.5f + offset.y}, {1.f, 1.f, 1.f}},
				{{0.5f + offset.x, 0.5f + offset.y}, {1.f, 1.f, 1.f}},
				{{-0.5f + offset.x, 0.5f + offset.y}, {1.f, 1.f, 1.f}},
				{{-0.5f + offset.x, -0.5f + offset.y}, {1.f, 1.f, 1.f}},
				{{0.5f + offset.x, -0.5f + offset.y}, {1.f, 1.f, 1.f}},
				{{0.5f + offset.x, 0.5f + offset.y}, {1.f, 1.f, 1.f}},
			};
			return std::make_shared<LveModel>(device, squareVertices);
		}

		std::shared_ptr<LveModel> createPawnModel(LveDevice& device, glm::vec2 offset = { 0.f, 0.f }) 
		{
			std::vector<LveModel::Vertex> pawnVertices{
				// Base (바닥)
				{{-0.4f + offset.x, 0.4f + offset.y}, {1.f, 1.f, 1.f}}, {{0.4f + offset.x, 0.4f + offset.y}, {1.f, 1.f, 1.f}}, {{-0.3f + offset.x, 0.2f + offset.y}, {1.f, 1.f, 1.f}},
				{{0.4f + offset.x, 0.4f + offset.y}, {1.f, 1.f, 1.f}}, {{0.3f + offset.x, 0.2f + offset.y}, {1.f, 1.f, 1.f}}, {{-0.3f + offset.x, 0.2f + offset.y}, {1.f, 1.f, 1.f}},

				// Stem (몸통 중간)
				{{-0.3f + offset.x, 0.2f + offset.y}, {1.f, 1.f, 1.f}}, {{0.3f + offset.x, 0.2f + offset.y}, {1.f, 1.f, 1.f}}, {{-0.15f + offset.x, 0.0f + offset.y}, {1.f, 1.f, 1.f}},
				{{0.3f + offset.x, 0.2f + offset.y}, {1.f, 1.f, 1.f}}, {{0.15f + offset.x, 0.0f + offset.y}, {1.f, 1.f, 1.f}}, {{-0.15f + offset.x, 0.0f + offset.y}, {1.f, 1.f, 1.f}},

				{{-0.15f + offset.x, 0.0f + offset.y}, {1.f, 1.f, 1.f}}, {{0.15f + offset.x, 0.0f + offset.y}, {1.f, 1.f, 1.f}}, {{-0.2f + offset.x, -0.2f + offset.y}, {1.f, 1.f, 1.f}},
				{{0.15f + offset.x, 0.0f + offset.y}, {1.f, 1.f, 1.f}}, {{0.2f + offset.x, -0.2f + offset.y}, {1.f, 1.f, 1.f}}, {{-0.2f + offset.x, -0.2f + offset.y}, {1.f, 1.f, 1.f}},

				// Collar (목걸이 부분)
				{{-0.25f + offset.x, -0.2f + offset.y}, {1.f, 1.f, 1.f}}, {{0.25f + offset.x, -0.2f + offset.y}, {1.f, 1.f, 1.f}}, {{-0.25f + offset.x, -0.3f + offset.y}, {1.f, 1.f, 1.f}},
				{{0.25f + offset.x, -0.2f + offset.y}, {1.f, 1.f, 1.f}}, {{0.25f + offset.x, -0.3f + offset.y}, {1.f, 1.f, 1.f}}, {{-0.25f + offset.x, -0.3f + offset.y}, {1.f, 1.f, 1.f}},

				// Head (머리)
				{{-0.1f + offset.x, -0.3f + offset.y}, {1.f, 1.f, 1.f}}, {{0.1f + offset.x, -0.3f + offset.y}, {1.f, 1.f, 1.f}}, {{-0.2f + offset.x, -0.4f + offset.y}, {1.f, 1.f, 1.f}},
				{{0.1f + offset.x, -0.3f + offset.y}, {1.f, 1.f, 1.f}}, {{0.2f + offset.x, -0.4f + offset.y}, {1.f, 1.f, 1.f}}, {{-0.2f + offset.x, -0.4f + offset.y}, {1.f, 1.f, 1.f}},
				
				{{-0.2f + offset.x, -0.4f + offset.y}, {1.f, 1.f, 1.f}}, {{0.2f + offset.x, -0.4f + offset.y}, {1.f, 1.f, 1.f}}, {{-0.2f + offset.x, -0.5f + offset.y}, {1.f, 1.f, 1.f}},
				{{0.2f + offset.x, -0.4f + offset.y}, {1.f, 1.f, 1.f}}, {{0.2f + offset.x, -0.5f + offset.y}, {1.f, 1.f, 1.f}}, {{-0.2f + offset.x, -0.5f + offset.y}, {1.f, 1.f, 1.f}},
				
				{{-0.2f + offset.x, -0.5f + offset.y}, {1.f, 1.f, 1.f}}, {{0.2f + offset.x, -0.5f + offset.y}, {1.f, 1.f, 1.f}}, {{-0.1f + offset.x, -0.6f + offset.y}, {1.f, 1.f, 1.f}},
				{{0.2f + offset.x, -0.5f + offset.y}, {1.f, 1.f, 1.f}}, {{0.1f + offset.x, -0.6f + offset.y}, {1.f, 1.f, 1.f}}, {{-0.1f + offset.x, -0.6f + offset.y}, {1.f, 1.f, 1.f}},
			};
			return std::make_shared<LveModel>(device, pawnVertices);
		}
	}

	void ChessApp::loadGameObjects()
	{
		std::shared_ptr<LveModel> squareModel = createSquareModel(lveDevice);
		std::shared_ptr<LveModel> pawnModel = createPawnModel(lveDevice);

		// Board
		chessBoard = std::make_unique<Board>(coordinator, squareModel, WIDTH, HEIGHT);

		// piece - player
		Entity player = coordinator.CreateEntity();
		coordinator.AddComponent(player, GridPositionComponent{ 4, 0 });
		coordinator.AddComponent(player, Transform2dComponent{
			chessBoard->getTilePosition(4, 0),
			chessBoard->GetTileScale() * 0.8f,
			0.f
		});
		coordinator.AddComponent(player , RenderComponent{ pawnModel, hexToColor(0x000000) });
		
		coordinator.AddComponent(player, PlayerInputComponent{});
	}
}