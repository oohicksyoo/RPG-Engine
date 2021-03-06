//
// Created by Alex on 12/13/2020.
//

#pragma once

#include "AssetInventory.hpp"
#include "FrameBuffer.hpp"
#include "Hierarchy.hpp"
#include <vector>

namespace RPG {
	struct IRenderer {
		virtual void Render(const RPG::Assets::Pipeline& pipeline, const std::shared_ptr<RPG::Hierarchy> hierarchy, const glm::mat4 cameraMatrix) = 0;
		virtual void RenderToFrameBuffer(const RPG::Assets::Pipeline& pipeline, const std::shared_ptr<RPG::Hierarchy> hierarchy, const std::shared_ptr<RPG::FrameBuffer> framebuffer, const glm::mat4 cameraMatrix, const glm::vec3 clearColor, const bool isGameCamera) = 0;
		virtual void RenderLinesToFrameBuffer(const RPG::Assets::Pipeline& pipeline, const std::shared_ptr<RPG::FrameBuffer> framebuffer, const glm::mat4 cameraMatrix) = 0;
		virtual void DeleteFrameBuffer(const RPG::Assets::Pipeline &pipeline, const std::shared_ptr<RPG::FrameBuffer> framebuffer) = 0;
	};
}
