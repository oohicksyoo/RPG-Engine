//
// Created by Alex on 12/13/2020.
//

#include "OpenGLRenderer.hpp"

using RPG::OpenGLRenderer;

struct OpenGLRenderer::Internal {
	const std::shared_ptr<RPG::OpenGLAssetManager> assetManager;

	Internal(std::shared_ptr<RPG::OpenGLAssetManager> assetManager) : assetManager(assetManager) {}

	void Render(const RPG::Assets::Pipeline& pipeline, const std::shared_ptr<RPG::Hierarchy> hierarchy, const glm::mat4 cameraMatrix, const uint32_t shadowMap) {
		assetManager->GetPipeline(pipeline).Render(*assetManager, hierarchy, cameraMatrix, shadowMap);
	}

	void RenderToFrameBuffer(const RPG::Assets::Pipeline& pipeline, const std::shared_ptr<RPG::Hierarchy> hierarchy, const std::shared_ptr<RPG::FrameBuffer> framebuffer, const glm::mat4 cameraMatrix, const glm::vec3 clearColor, const uint32_t shadowMap, const bool isGameCamera) {
		assetManager->GetPipeline(pipeline).RenderToFrameBuffer(*assetManager, hierarchy, framebuffer, cameraMatrix, clearColor, shadowMap, isGameCamera);
	}

	void RenderLinesToFrameBuffer(const RPG::Assets::Pipeline& pipeline, const std::shared_ptr<RPG::FrameBuffer> framebuffer, const glm::mat4 cameraMatrix) {
		assetManager->GetPipeline(pipeline).RenderLinesToFrameBuffer(*assetManager, framebuffer, cameraMatrix);
	}

    void RenderToDepthBuffer(const RPG::Assets::Pipeline& pipeline, const std::shared_ptr<RPG::Hierarchy> hierarchy, const std::shared_ptr<RPG::FrameBuffer> framebuffer) {
        assetManager->GetPipeline(pipeline).RenderToDepthBuffer(*assetManager, hierarchy, framebuffer);
	}

	void DeleteFrameBuffer(const RPG::Assets::Pipeline& pipeline, const std::shared_ptr<RPG::FrameBuffer> framebuffer) {
		assetManager->GetPipeline(pipeline).DeleteFrameBuffer(framebuffer);
	}

    void ClearFrameBufferToColor(const RPG::Assets::Pipeline &pipeline,
                                 const std::shared_ptr<RPG::FrameBuffer> framebuffer, const glm::vec3 clearColor) {
        assetManager->GetPipeline(pipeline).ClearFrameBufferToColor(framebuffer, clearColor);
    }

    void RenderToFrameBuffer(const RPG::Assets::Pipeline &pipeline, const std::shared_ptr<RPG::FrameBuffer> framebuffer,
                             const std::vector<RPG::GameObjectMaterialGroup> gameObjects, const glm::mat4 cameraMatrix) {
        assetManager->GetPipeline(pipeline).RenderToFrameBuffer(*assetManager, framebuffer, gameObjects, cameraMatrix);
    }
};

OpenGLRenderer::OpenGLRenderer(std::shared_ptr<RPG::OpenGLAssetManager> assetManager) : internal(RPG::MakeInternalPointer<Internal>(assetManager)) {}

void OpenGLRenderer::Render(const RPG::Assets::Pipeline& pipeline, const std::shared_ptr<RPG::Hierarchy> hierarchy, const glm::mat4 cameraMatrix, const uint32_t shadowMap) {
	internal->Render(pipeline, hierarchy, cameraMatrix, shadowMap);
}

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

void OpenGLRenderer::DeleteFrameBuffer(const RPG::Assets::Pipeline &pipeline, const std::shared_ptr<RPG::FrameBuffer> framebuffer) {
	internal->DeleteFrameBuffer(pipeline, framebuffer);
}

void OpenGLRenderer::ClearFrameBufferToColor(const RPG::Assets::Pipeline &pipeline,
                                             const std::shared_ptr<RPG::FrameBuffer> framebuffer, const glm::vec3 clearColor) {
    internal->ClearFrameBufferToColor(pipeline, framebuffer, clearColor);
}

void OpenGLRenderer::RenderToFrameBuffer(const RPG::Assets::Pipeline &pipeline, const std::shared_ptr<RPG::FrameBuffer> frameBuffer,
                                         const std::vector<RPG::GameObjectMaterialGroup> gameObjects, const glm::mat4 cameraMatrix) {
    internal->RenderToFrameBuffer(pipeline, frameBuffer, gameObjects, cameraMatrix);
}