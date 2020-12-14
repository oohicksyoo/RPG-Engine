//
// Created by Alex on 12/13/2020.
//

#include "OpenGLRenderer.hpp"

using RPG::OpenGLRenderer;

struct OpenGLRenderer::Internal {
	const std::shared_ptr<RPG::OpenGLAssetManager> assetManager;

	Internal(std::shared_ptr<RPG::OpenGLAssetManager> assetManager) : assetManager(assetManager) {}

	void Render(const RPG::Assets::Pipeline& pipeline, const std::vector<RPG::StaticMeshInstance>& staticMeshInstances) {
		assetManager->GetPipeline(pipeline).Render(*assetManager, staticMeshInstances);
	}
};

OpenGLRenderer::OpenGLRenderer(std::shared_ptr<RPG::OpenGLAssetManager> assetManager) : internal(RPG::MakeInternalPointer<Internal>(assetManager)) {}

void OpenGLRenderer::Render(const RPG::Assets::Pipeline& pipeline, const std::vector<RPG::StaticMeshInstance>& staticMeshInstances) {
	internal->Render(pipeline, staticMeshInstances);
}