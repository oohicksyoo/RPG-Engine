//
// Created by Alex on 12/13/2020.
//

#pragma once

#include "AssetInventory.hpp"
#include "StaticMeshInstance.hpp"
#include "FrameBuffer.hpp"
#include "Hierarchy.hpp"
#include <vector>

namespace RPG {
	struct IRenderer {
		virtual void Render(const RPG::Assets::Pipeline& pipeline, const std::vector<RPG::StaticMeshInstance>& staticMeshInstances) = 0;
		virtual void RenderToFrameBuffer(const RPG::Assets::Pipeline& pipeline, const std::shared_ptr<RPG::Hierarchy> hierarchy, const std::shared_ptr<RPG::FrameBuffer> framebuffer, const glm::mat4 cameraMatrix) = 0;
		virtual void DeleteFrameBuffer(const RPG::Assets::Pipeline &pipeline, const std::shared_ptr<RPG::FrameBuffer> framebuffer) = 0;
	};
}
