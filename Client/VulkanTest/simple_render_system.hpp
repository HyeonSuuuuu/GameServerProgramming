#pragma once

#include "lve_pipeline.hpp"
#include "lve_components.hpp"
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

	class SimpleRenderSystem : public System {
	public:

		SimpleRenderSystem(LveDevice& device, VkRenderPass renderPass);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void renderEntities(VkCommandBuffer commandBuffer, Coordinator& coordinator);


	private:

		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		LveDevice& lveDevice;

		std::unique_ptr<LvePipeline> lvePipeline;
		VkPipelineLayout pipelineLayout;
	};
}