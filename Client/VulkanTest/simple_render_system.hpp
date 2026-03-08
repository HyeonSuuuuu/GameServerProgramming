#pragma once

#include "lve_pipeline.hpp"
#include "lve_game_object.hpp"
#include "lve_device.hpp"

#include <memory>
#include <vector>

namespace lve
{
	struct SimplePushConstantData {
		glm::mat2 transform{ 1.f };
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	class SimpleRenderSystem {
	public:

		SimpleRenderSystem(LveDevice& device, VkRenderPass renderPass);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<LveGameObject>& gameObjects);


	private:

		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		LveDevice& lveDevice;

		std::unique_ptr<LvePipeline> lvePipeline;
		VkPipelineLayout pipelineLayout;
	};
}