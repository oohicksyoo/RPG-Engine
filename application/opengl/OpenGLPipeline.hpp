//
// Created by Alex on 12/13/2020.
//

#pragma once

#include "../../core/InternalPointer.hpp"
#include "../../core/FrameBuffer.hpp"
#include "../../core/Hierarchy.hpp"
#include "../../core/GameObjectMaterialGroup.hpp"
#include <string>
#include <vector>

namespace RPG {
	struct OpenGLAssetManager;

	struct OpenGLPipeline {
		public:
			OpenGLPipeline(const std::string& shaderName);
			void Render(const RPG::OpenGLAssetManager& assetManager, const std::shared_ptr<RPG::Hierarchy> hierarchy, const glm::mat4 cameraMatrix, const uint32_t shadowMap) const;
			void RenderToFrameBuffer(const RPG::OpenGLAssetManager& assetManager, const std::shared_ptr<RPG::Hierarchy> hierarchy, const std::shared_ptr<RPG::FrameBuffer> frameBuffer, const glm::mat4 cameraMatrix, const glm::vec3 clearColor, const uint32_t shadowMap, const bool isGameCamera) const;
			void RenderLinesToFrameBuffer(const RPG::OpenGLAssetManager& assetManager, const std::shared_ptr<RPG::FrameBuffer> framebuffer, const glm::mat4 cameraMatrix) const;
            void RenderToDepthBuffer(const RPG::OpenGLAssetManager& assetManager, const std::shared_ptr<RPG::Hierarchy> hierarchy, const std::shared_ptr<RPG::FrameBuffer> frameBuffer) const;

			void ClearFrameBufferToColor(const std::shared_ptr<RPG::FrameBuffer> framebuffer, const glm::vec3 clearColor) const;
			void RenderToFrameBuffer(const RPG::OpenGLAssetManager& assetManager, const std::shared_ptr<RPG::FrameBuffer> framebuffer, const std::vector<RPG::GameObjectMaterialGroup> gameObjects, const glm::mat4 cameraMatrix) const;
            void DisplayFrameBuffer(const RPG::OpenGLAssetManager& assetManager, const std::shared_ptr<RPG::FrameBuffer> frameBuffer) const;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


