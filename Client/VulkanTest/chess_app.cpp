
#include "chess_app.hpp"
#include "simple_render_system.hpp"
#include "keyboard_movement_controller.hpp"
#include "lve_utils.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <GLM/gtc/constants.hpp>

// std
#include <array>
#include <stdexcept>
#include <iostream>
#include <chrono>
#include <cassert>

namespace lve
{
	
	ChessApp::ChessApp() {
		coordinator.Init();
		coordinator.RegisterComponent<Transform2dComponent>();
		coordinator.RegisterComponent<RenderComponent>();
		coordinator.RegisterComponent<PlayerInputComponent>();

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
			playerController->moveDiscreteXY(lveWindow.getGLFWwindow(), coordinator, tileStepX, tileStepY);

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

		assert(WIDTH > HEIGHT && " WIDTH 가 더 커야 타일이 안찌그러짐");
		float tileScaleY = 2.0f / 8.0f; // -1 ~ 1 
		float tileScaleX = tileScaleY * (static_cast<float>(HEIGHT) / WIDTH);

		float startY = -1.0f + (tileScaleY / 2.0f);
		float startX = -(tileScaleX * 8.0f / 2.0f) + (tileScaleX / 2.0f);

		for (int row = 0; row < 8; row++) {
			for (int col = 0; col < 8; col++) {
				Entity tile = coordinator.CreateEntity();

				coordinator.AddComponent(tile, Transform2dComponent{
					{ startX + col * tileScaleX, startY + row * tileScaleY },
					{ tileScaleX, tileScaleY },
					0.f
				});
				
				glm::vec3 color = ((row + col) % 2 == 0) ? hexToColor(0xE6E6E6) : hexToColor(0xB6876B);
				coordinator.AddComponent(tile, RenderComponent{ squareModel, color });
			}
		}

		Entity piece = coordinator.CreateEntity();

		coordinator.AddComponent(piece, Transform2dComponent{
			{ startX + 0 * tileScaleX, startY + 0 * tileScaleY },
			{ tileScaleX * 0.8f, tileScaleY * 0.8f },
			0.f
		});
		coordinator.AddComponent(piece, RenderComponent{ pawnModel, hexToColor(0x000000)});
		
		// 플레이어라는 의미의 마커(marker) 역할을 하는 빈 컴포넌트 추가
		coordinator.AddComponent(piece, PlayerInputComponent{});
	}
}