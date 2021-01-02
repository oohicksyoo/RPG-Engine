//
// Created by Alex on 12/13/2020.
//

#pragma once

#include "../../core/InternalPointer.hpp"
#include "../../core/StaticMeshInstance.hpp"
#include "../../core/FrameBuffer.hpp"
#include "../../core/Hierarchy.hpp"
#include <string>
#include <vector>

namespace RPG {
	struct OpenGLAssetManager;

	struct OpenGLPipeline {
		public:
			OpenGLPipeline(const std::string& shaderName);
			void Render(const RPG::OpenGLAssetManager& assetManager, const std::shared_ptr<RPG::Hierarchy> hierarchy, const glm::mat4 cameraMatrix) const;
			void RenderToFrameBuffer(const RPG::OpenGLAssetManager& assetManager, const std::shared_ptr<RPG::Hierarchy> hierarchy, const std::shared_ptr<RPG::FrameBuffer> frameBuffer, const glm::mat4 cameraMatrix) const;
			void DeleteFrameBuffer(const std::shared_ptr<RPG::FrameBuffer> framebuffer) const;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


