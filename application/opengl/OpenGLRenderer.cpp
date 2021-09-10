//
// Created by Alex on 12/13/2020.
//

#include "OpenGLRenderer.hpp"

using RPG::OpenGLRenderer;

struct OpenGLRenderer::Internal {
	const std::shared_ptr<RPG::OpenGLAssetManager> assetManager;

	Internal(std::shared_ptr<RPG::OpenGLAssetManager> assetManager) : assetManager(assetManager) {}

	void RenderToFrameBuffer(const RPG::Assets::Pipeline& pipeline, const std::shared_ptr<RPG::Hierarchy> hierarchy, const std::shared_ptr<RPG::FrameBuffer> framebuffer, const glm::mat4 cameraMatrix, const glm::vec3 clearColor, const uint32_t shadowMap, const bool isGameCamera) {
		assetManager->GetPipeline(pipeline).RenderToFrameBuffer(*assetManager, hierarchy, framebuffer, cameraMatrix, clearColor, shadowMap, isGameCamera);
	}

	void RenderLinesToFrameBuffer(const RPG::Assets::Pipeline& pipeline, const std::shared_ptr<RPG::FrameBuffer> framebuffer, const glm::mat4 cameraMatrix) {
		assetManager->GetPipeline(pipeline).RenderLinesToFrameBuffer(*assetManager, framebuffer, cameraMatrix);
	}

    void RenderToDepthBuffer(const RPG::Assets::Pipeline& pipeline, const std::shared_ptr<RPG::Hierarchy> hierarchy, const std::shared_ptr<RPG::FrameBuffer> framebuffer) {
        assetManager->GetPipeline(pipeline).RenderToDepthBuffer(*assetManager, hierarchy, framebuffer);
	}

    void ClearFrameBufferToColor(const RPG::Assets::Pipeline &pipeline,
                                 const std::shared_ptr<RPG::FrameBuffer> framebuffer, const glm::vec3 clearColor) {
        assetManager->GetPipeline(pipeline).ClearFrameBufferToColor(framebuffer, clearColor);
    }

    void RenderToFrameBuffer(const RPG::Assets::Pipeline &pipeline, const std::shared_ptr<RPG::FrameBuffer> framebuffer,
                             const std::vector<RPG::GameObjectMaterialGroup> gameObjects, const glm::mat4 cameraMatrix) {
        assetManager->GetPipeline(pipeline).RenderToFrameBuffer(*assetManager, framebuffer, gameObjects, cameraMatrix);
    }

    void DisplayFrameBuffer(const RPG::Assets::Pipeline &pipeline, const std::shared_ptr<RPG::FrameBuffer> frameBuffer) {
	    assetManager->GetPipeline(pipeline).DisplayFrameBuffer(*assetManager, frameBuffer);
	}
};

OpenGLRenderer::OpenGLRenderer(std::shared_ptr<RPG::OpenGLAssetManager> assetManager) : internal(RPG::MakeInternalPointer<Internal>(assetManager)) {}

void OpenGLRenderer::RenderToFrameBuffer(const RPG::Assets::Pipeline &pipeline,
										 const std::shared_ptr<RPG::Hierarchy> hierarchy,
										 const std::shared_ptr<RPG::FrameBuffer> framebuffer,
										 const glm::mat4 cameraMatrix,
										 const glm::vec3 clearColor,
                                         const uint32_t shadowMap,
										 const bool isGameCamera) {
	internal->RenderToFrameBuffer(pipeline, hierarchy, framebuffer, cameraMatrix, clearColor, shadowMap, isGameCamera);
}

void OpenGLRenderer::RenderLinesToFrameBuffer(const RPG::Assets::Pipeline& pipeline, const std::shared_ptr<RPG::FrameBuffer> framebuffer, const glm::mat4 cameraMatrix) {
	internal->RenderLinesToFrameBuffer(pipeline, framebuffer, cameraMatrix);
}

void OpenGLRenderer::RenderToDepthBuffer(const RPG::Assets::Pipeline &pipeline, const std::shared_ptr<RPG::Hierarchy> hierarchy, const std::shared_ptr<RPG::FrameBuffer> framebuffer) {
    internal->RenderToDepthBuffer(pipeline, hierarchy, framebuffer);
}

void OpenGLRenderer::ClearFrameBufferToColor(const RPG::Assets::Pipeline &pipeline,
                                             const std::shared_ptr<RPG::FrameBuffer> framebuffer, const glm::vec3 clearColor) {
    internal->ClearFrameBufferToColor(pipeline, framebuffer, clearColor);
}

void OpenGLRenderer::RenderToFrameBuffer(const RPG::Assets::Pipeline &pipeline, const std::shared_ptr<RPG::FrameBuffer> frameBuffer,
                                         const std::vector<RPG::GameObjectMaterialGroup> gameObjects, const glm::mat4 cameraMatrix) {
    internal->RenderToFrameBuffer(pipeline, frameBuffer, gameObjects, cameraMatrix);
}

void OpenGLRenderer::DisplayFrameBuffer(const RPG::Assets::Pipeline &pipeline, const std::shared_ptr<RPG::FrameBuffer> frameBuffer) {
    internal->DisplayFrameBuffer(pipeline, frameBuffer);
}