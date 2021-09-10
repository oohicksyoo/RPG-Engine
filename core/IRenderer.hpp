//
// Created by Alex on 12/13/2020.
//

#pragma once

#include "AssetInventory.hpp"
#include "FrameBuffer.hpp"
#include "Hierarchy.hpp"
#include "GameObjectMaterialGroup.hpp"
#include <vector>

namespace RPG {
	struct IRenderer {
		virtual void RenderToFrameBuffer(const RPG::Assets::Pipeline& pipeline, const std::shared_ptr<RPG::Hierarchy> hierarchy, const std::shared_ptr<RPG::FrameBuffer> framebuffer, const glm::mat4 cameraMatrix, const glm::vec3 clearColor, const uint32_t shadowMap, const bool isGameCamera) = 0;
		virtual void RenderLinesToFrameBuffer(const RPG::Assets::Pipeline& pipeline, const std::shared_ptr<RPG::FrameBuffer> framebuffer, const glm::mat4 cameraMatrix) = 0;
		virtual void RenderToDepthBuffer(const RPG::Assets::Pipeline& pipeline, const std::shared_ptr<RPG::Hierarchy> hierarchy, const std::shared_ptr<RPG::FrameBuffer> framebuffer) = 0;

        virtual void ClearFrameBufferToColor(const RPG::Assets::Pipeline &pipeline, const std::shared_ptr<RPG::FrameBuffer> framebuffer, const glm::vec3 clearColor) = 0;
        virtual void RenderToFrameBuffer(const RPG::Assets::Pipeline &pipeline, const std::shared_ptr<RPG::FrameBuffer> framebuffer, const std::vector<RPG::GameObjectMaterialGroup> gameObjects, const glm::mat4 cameraMatrix) = 0;
		virtual void DisplayFrameBuffer(const RPG::Assets::Pipeline &pipeline, const std::shared_ptr<RPG::FrameBuffer> frameBuffer) = 0;
	};
}
