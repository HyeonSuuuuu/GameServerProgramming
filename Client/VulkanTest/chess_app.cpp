#include "pch.h"
#include "chess_app.hpp"
#include "simple_render_system.hpp"
#include "keyboard_movement_controller.hpp"
#include "lve_utils.hpp"
#include "../../Common/Protocol.h"
// Vulkan Game Engine Tutorial 참고

namespace lve
{
	
	ChessApp::ChessApp() {
		std::string serverIP;
		if (!networker.Init())
		{
			std::println("network 초기화 실패");
			exit(1);
		}

		std::print("접속할 서버 IP를 입력해주세요: ");
		std::getline(std::cin, serverIP);
		if (serverIP.empty()) serverIP = "127.0.0.1";

		if (!networker.Connect(serverIP.c_str(), SERVER_PORT))
		{
			std::println("connect 실패");
			exit(1);
		}
		std::println("네트워크 연결 성공");


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
		std::shared_ptr<lve::LveModel> pawnModel = createpawnModel(lveDevice);
		while (!lveWindow.shouldClose()) 
		{
			networker.Run(coordinator);

			// 입장 및 퇴장 처리

			while (networker.m_enterQueue.empty() == false)
			{
				uint8_t id = networker.m_enterQueue.front();
				networker.m_enterQueue.pop();
				coordinator.AddComponent(g_idToEntity[id], RenderComponent{pawnModel, lve::hexToColor(0x000000)});
			}

			while (networker.m_leaveQueue.empty() == false)
			{
				uint8_t id = networker.m_leaveQueue.front();
				networker.m_leaveQueue.pop();
				coordinator.RemoveComponent<RenderComponent>(g_idToEntity[id]);
			}

			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			// 플레이어 엔티티(마커가 있는 엔티티) 이동
			float tileStepY = 2.0f / 8.0f;
			float tileStepX = tileStepY * (static_cast<float>(HEIGHT) / WIDTH);
			playerController->processPlayerInput(lveWindow.getGLFWwindow(), coordinator, chessBoard.get());
			playerController->setServerPosition(coordinator, g_clients[g_myId].x, g_clients[g_myId].y);

			// 위치 이동
			for (auto& [id, client] : g_clients)
			{
				auto& transform = coordinator.GetComponent<Transform2dComponent>(g_idToEntity[id]);

				transform.translation = chessBoard->getTilePosition(client.x, client.y);
			}


			if (auto commandBuffer = lveRenderer.beginFrame())
			{
				lveRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem->renderEntities(commandBuffer, coordinator);
				lveRenderer.endSwapChainRenderPass(commandBuffer);
				lveRenderer.endFrame();
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		vkDeviceWaitIdle(lveDevice.device());
	}

	namespace 
	{
		std::shared_ptr<LveModel> createSquareModel(LveDevice& device, glm::vec2 offset) 
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

		std::shared_ptr<LveModel> createpawnModel(LveDevice& device, glm::vec2 offset) 
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
		std::shared_ptr<lve::LveModel> pawnModel = createpawnModel(lveDevice);

		// Board
		chessBoard = std::make_unique<Board>(coordinator, squareModel, WIDTH, HEIGHT);

		// Players
		for (int i = 0; i < MAX_PLAYERS; ++i)
		{
			Entity player = coordinator.CreateEntity();
			coordinator.AddComponent(player, GridPositionComponent{ 0, 0 });
			coordinator.AddComponent(player, Transform2dComponent{
				chessBoard->getTilePosition(0, 0),
				chessBoard->GetTileScale() * 0.8f,
				0.f
				});
			if (i == g_myId)
			{
				coordinator.AddComponent(player, PlayerInputComponent{});
				coordinator.AddComponent(player, RenderComponent{pawnModel, hexToColor(0x800000)});

			}
			g_idToEntity.try_emplace(i, player);
		}
	}
}