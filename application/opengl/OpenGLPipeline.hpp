//
// Created by Alex on 12/13/2020.
//

#pragma once

#include "../../core/InternalPointer.hpp"
#include "../../core/StaticMeshInstance.hpp"
#include "../../core/FrameBuffer.hpp"
#include <string>
#include <vector>

namespace RPG {
	struct OpenGLAssetManager;

	struct OpenGLPipeline {
		public:
			OpenGLPipeline(const std::string& shaderName);
			void Render(const RPG::OpenGLAssetManager& assetManager, const std::vector<RPG::StaticMeshInstance>& staticMeshInstances) const;
			void RenderToFrameBuffer(const RPG::OpenGLAssetManager& assetManager, const std::vector<RPG::StaticMeshInstance>& staticMeshInstances, const RPG::FrameBuffer& frameBuffer) const;
			void DeleteFrameBuffer(const RPG::FrameBuffer& framebuffer) const;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


