//
// Created by Alex on 12/13/2020.
//

#include "OpenGLRenderer.hpp"

using RPG::OpenGLRenderer;

struct OpenGLRenderer::Internal {
	const std::shared_ptr<RPG::OpenGLAssetManager> assetManager;

	Internal(std::shared_ptr<RPG::OpenGLAssetManager> assetManager) : assetManager(assetManager) {}

	void Render(const RPG::Assets::Pipeline& pipeline, const std::shared_ptr<RPG::Hierarchy> hierarchy, const glm::mat4 cameraMatrix) {
		assetManager->GetPipeline(pipeline).Render(*assetManager, hierarchy, cameraMatrix);
	}

	void RenderToFrameBuffer(const RPG::Assets::Pipeline& pipeline, const std::shared_ptr<RPG::Hierarchy> hierarchy, const std::shared_ptr<RPG::FrameBuffer> framebuffer, const glm::mat4 cameraMatrix) {
		assetManager->GetPipeline(pipeline).RenderToFrameBuffer(*assetManager, hierarchy, framebuffer, cameraMatrix);
	}

	void DeleteFrameBuffer(const RPG::Assets::Pipeline& pipeline, const std::shared_ptr<RPG::FrameBuffer> framebuffer) {
		assetManager->GetPipeline(pipeline).DeleteFrameBuffer(framebuffer);
	}
};

OpenGLRenderer::OpenGLRenderer(std::shared_ptr<RPG::OpenGLAssetManager> assetManager) : internal(RPG::MakeInternalPointer<Internal>(assetManager)) {}

void OpenGLRenderer::Render(const RPG::Assets::Pipeline& pipeline, const std::shared_ptr<RPG::Hierarchy> hierarchy, const glm::mat4 cameraMatrix) {
	internal->Render(pipeline, hierarchy, cameraMatrix);
}

void OpenGLRenderer::RenderToFrameBuffer(const RPG::Assets::Pipeline &pipeline,
										 const std::shared_ptr<RPG::Hierarchy> hierarchy,
										 const std::shared_ptr<RPG::FrameBuffer> framebuffer,
										 const glm::mat4 cameraMatrix) {
	internal->RenderToFrameBuffer(pipeline, hierarchy, framebuffer, cameraMatrix);
}

void OpenGLRenderer::DeleteFrameBuffer(const RPG::Assets::Pipeline &pipeline, const std::shared_ptr<RPG::FrameBuffer> framebuffer) {
	internal->DeleteFrameBuffer(pipeline, framebuffer);
}