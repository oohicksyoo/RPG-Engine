//
// Created by Alex on 12/13/2020.
//

#pragma once

#include "../../core/InternalPointer.hpp"
#include "../../core/IRenderer.hpp"
#include "../../core/FrameBuffer.hpp"
#include "OpenGLAssetManager.hpp"
#include <memory>

namespace RPG {
	struct OpenGLRenderer : public RPG::IRenderer {
		public:
			OpenGLRenderer(std::shared_ptr<RPG::OpenGLAssetManager> AssetManager);
			void Render(const RPG::Assets::Pipeline& pipeline, const std::vector<RPG::StaticMeshInstance>& staticMeshInstances) override;
			void RenderToFrameBuffer(const RPG::Assets::Pipeline& pipeline, const std::vector<RPG::StaticMeshInstance>& staticMeshInstances, const RPG::FrameBuffer& framebuffer) override;
			void DeleteFrameBuffer(const RPG::Assets::Pipeline &pipeline, const RPG::FrameBuffer& framebuffer) override;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


