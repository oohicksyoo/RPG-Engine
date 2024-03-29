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
			void RenderToFrameBuffer(const RPG::Assets::Pipeline& pipeline, const std::shared_ptr<RPG::Hierarchy> hierarchy, const std::shared_ptr<RPG::FrameBuffer> framebuffer, const glm::mat4 cameraMatrix, const glm::vec3 clearColor, const uint32_t shadowMap, const bool isGameCamera) override;
			void RenderLinesToFrameBuffer(const RPG::Assets::Pipeline& pipeline, const std::shared_ptr<RPG::FrameBuffer> framebuffer, const glm::mat4 cameraMatrix) override;
			void RenderToDepthBuffer(const RPG::Assets::Pipeline& pipeline, const std::shared_ptr<RPG::Hierarchy> hierarchy, const std::shared_ptr<RPG::FrameBuffer> framebuffer) override;

            void ClearFrameBufferToColor(const RPG::Assets::Pipeline &pipeline, const std::shared_ptr<RPG::FrameBuffer> framebuffer, const glm::vec3 clearColor) override;
            void RenderToFrameBuffer(const RPG::Assets::Pipeline &pipeline, const std::shared_ptr<RPG::FrameBuffer> framebuffer, const std::vector<RPG::GameObjectMaterialGroup> gameObjects, const glm::mat4 cameraMatrix) override;
            void DisplayFrameBuffer(const RPG::Assets::Pipeline &pipeline, const std::shared_ptr<RPG::FrameBuffer> frameBuffer) override;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


