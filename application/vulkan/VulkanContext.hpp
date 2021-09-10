//
// Created by Alex on 12/14/2020.
//

#pragma once

#include "../../core/AssetManifest.hpp"
#include "../../core/InternalPointer.hpp"
#include "../../core/IRenderer.hpp"
#include "../../core/FrameBuffer.hpp"
#include "../../core/WindowSize.hpp"

namespace RPG {
	struct VulkanContext : public RPG::IRenderer {
		public:
			VulkanContext();
			void LoadAssetManifest(const RPG::AssetManifest& assetManifest);
			bool RenderBegin();
			void Render(const RPG::Assets::Pipeline& pipeline, const std::shared_ptr<RPG::Hierarchy> hierarchy, const glm::mat4 cameraMatrix, const uint32_t shadowMap) override;
			void RenderToFrameBuffer(const RPG::Assets::Pipeline& pipeline, const std::shared_ptr<RPG::Hierarchy> hierarchy, const std::shared_ptr<RPG::FrameBuffer> framebuffer, const glm::mat4 cameraMatrix, const glm::vec3 clearColor, const uint32_t shadowMap, const bool isGameCamera) override;
			void RenderLinesToFrameBuffer(const RPG::Assets::Pipeline& pipeline, const std::shared_ptr<RPG::FrameBuffer> framebuffer, const glm::mat4 cameraMatrix) override;
			void RenderToDepthBuffer(const RPG::Assets::Pipeline& pipeline, const std::shared_ptr<RPG::Hierarchy> hierarchy, const std::shared_ptr<RPG::FrameBuffer> framebuffer) override;
			void RenderEnd();
			RPG::WindowSize GetCurrentWindowSize() const;
			glm::vec2 GetWindowPosition();

            void ClearFrameBufferToColor(const RPG::Assets::Pipeline &pipeline, const std::shared_ptr<RPG::FrameBuffer> framebuffer, const glm::vec3 clearColor) override;
            void RenderToFrameBuffer(const RPG::Assets::Pipeline &pipeline, const std::shared_ptr<RPG::FrameBuffer> framebuffer, const std::vector<RPG::GameObjectMaterialGroup> gameObjects, const glm::mat4 cameraMatrix) override;
            void DisplayFrameBuffer(const RPG::Assets::Pipeline &pipeline, const std::shared_ptr<RPG::FrameBuffer> frameBuffer) override;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


