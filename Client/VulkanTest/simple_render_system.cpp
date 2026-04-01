#include "pch.h"
#include "simple_render_system.hpp"
#include <filesystem>

namespace lve
{
	SimpleRenderSystem::SimpleRenderSystem(LveDevice& device, VkRenderPass renderPass) : lveDevice{ device } {
		createPipelineLayout();
		createPipeline(renderPass);
	}

	SimpleRenderSystem::~SimpleRenderSystem() {
		vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
	}

	void SimpleRenderSystem::createPipelineLayout()
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}
	void SimpleRenderSystem::createPipeline(VkRenderPass renderPass)
	{
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		LvePipeline::defaultPipelineConfigInfo(
			pipelineConfig);

		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;

		// 파일 찾기
		std::vector<std::string> vertPaths = {
			"shaders/simple_shader.vert.spv",          // MSVC 빌드시
			"../../VulkanTest/shaders/simple_shader.vert.spv",       // 바이너리 실행시
			"simple_shader.vert.spv"     // 바이너리 실행시
		};

		std::vector<std::string> fragPaths = {
			"shaders/simple_shader.frag.spv",
			"../../VulkanTest/shaders/simple_shader.frag.spv",
			"simple_shader.frag.spv" 
		};
		std::string finalVertPath = "";
		std::string finalFragPath = "";
		for (const auto& path : vertPaths) {
			if (std::filesystem::exists(path)) {
				finalVertPath = path;
				break;
			}
		}

		for (const auto& path : fragPaths) {
			if (std::filesystem::exists(path)) {
				finalFragPath = path;
				break;
			}
		}
		if (finalVertPath.empty() || finalFragPath.empty()) {
			throw std::runtime_error("쉐이더 파일을 찾을 수 없습니다! 경로를 확인하세요.");
		}

		lvePipeline = std::make_unique<LvePipeline>(
			lveDevice,
			finalVertPath,
			finalFragPath,
			pipelineConfig);
	}

	void SimpleRenderSystem::renderEntities(VkCommandBuffer commandBuffer, Coordinator& coordinator)
	{
		lvePipeline->bind(commandBuffer);

		for (Entity entity : mEntities) {
			auto& renderComp = coordinator.GetComponent<RenderComponent>(entity);
			auto& transformComp = coordinator.GetComponent<Transform2dComponent>(entity);

			SimplePushConstantData push{};
			push.offset = transformComp.translation;
			push.color = renderComp.color;

			const float s = glm::sin(transformComp.rotation);
			const float c = glm::cos(transformComp.rotation);
			glm::mat2 rotMatrix{ {c, s}, {-s, c} };
			glm::mat2 scaleMat{ {transformComp.scale.x, 0.f}, {0.f, transformComp.scale.y} };
			
			push.transform = rotMatrix * scaleMat;

			vkCmdPushConstants(
				commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push);
				
			renderComp.model->bind(commandBuffer);
			renderComp.model->draw(commandBuffer);
		}
	}
}