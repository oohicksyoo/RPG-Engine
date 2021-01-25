//
// Created by Alex on 12/13/2020.
//

#include "OpenGLAssetManager.hpp"
#include "../../core/Assets.hpp"
#include "../../core/Log.hpp"
#include "../../core/Content.hpp"
#include <unordered_map>

using RPG::OpenGLAssetManager;

namespace {
	RPG::OpenGLMesh CreateSceneGridLines() {
		std::vector<RPG::Vertex> lines;

		float width = 20;
		float offset = -(width * 0.5f);
		for (int x = 0; x <= width; x++) {
			for (int z = 0; z <= width; z++) {
				lines.push_back(RPG::Vertex{{x + offset, 0, -offset}, {0, 0}});
				lines.push_back(RPG::Vertex{{x + offset, 0, offset}, {0, 0}});

				lines.push_back(RPG::Vertex{{-offset, 0, z + offset}, {0, 0}});
				lines.push_back(RPG::Vertex{{offset, 0, z + offset}, {0, 0}});
			}
		}

		RPG::Log("Grid", "Lines: " + std::to_string(lines.size()));

		return RPG::OpenGLMesh(RPG::Mesh(lines, {}));
	}
}

struct OpenGLAssetManager::Internal {
	std::unordered_map<RPG::Assets::Pipeline, RPG::OpenGLPipeline> pipelineCache;
	std::unordered_map<RPG::Assets::StaticMesh, RPG::OpenGLMesh> staticMeshCache;
	std::unordered_map<RPG::Assets::Texture, RPG::OpenGLTexture> textureCache;
	RPG::OpenGLMesh lines;

	Internal() : lines(::CreateSceneGridLines()) {
		RPG::Content::GetInstance().OnLoadedAsset<RPG::Mesh>([this](std::string path, std::shared_ptr<RPG::Mesh> mesh) {
			//TODO: Insert into opengl cache that this asset is loaded
			RPG::Log("AssetManager", "On Asset Loaded Mesh");
		});

		RPG::Content::GetInstance().OnLoadedAsset<RPG::Bitmap>([this](std::string path, std::shared_ptr<RPG::Bitmap> bitmap) {
			//TODO: Insert into opengl cache that this asset is loaded
			RPG::Log("AssetManager", "On Asset Loaded Mesh");
		});
	}

	void LoadPipelines(const std::vector<RPG::Assets::Pipeline>& pipelines) {
		for (const auto& pipeline : pipelines) 	{
			if (pipelineCache.count(pipeline) == 0) {
				pipelineCache.insert(std::make_pair(pipeline, RPG::OpenGLPipeline(RPG::Assets::ResolvePipelinePath(pipeline))));
			}
		}
	}

	void LoadStaticMeshes(const std::vector<RPG::Assets::StaticMesh>& staticMeshes) {
		for (const auto& staticMesh : staticMeshes) {
			if (staticMeshCache.count(staticMesh) == 0) {
				staticMeshCache.insert(std::make_pair(staticMesh, RPG::OpenGLMesh(RPG::Assets::LoadOBJFile(RPG::Assets::ResolveStaticMeshPath(staticMesh)))));
			}
		}
	}

	void LoadTextures(const std::vector<RPG::Assets::Texture>& textures) {
		for (const auto& texture : textures) {
			if (textureCache.count(texture) == 0) {
				textureCache.insert(std::pair(texture, RPG::OpenGLTexture(RPG::Assets::LoadBitmapFile(RPG::Assets::ResolveTexturePath(texture)))));
			}
		}
	}

	void LoadAssetManifest(const RPG::AssetManifest& assetManifest) {
		LoadPipelines(assetManifest.pipelines);
		LoadStaticMeshes(assetManifest.staticMeshes);
		LoadTextures(assetManifest.textures);
	}
};

OpenGLAssetManager::OpenGLAssetManager() : internal(RPG::MakeInternalPointer<Internal>()) {}

void OpenGLAssetManager::LoadAssetManifest(const RPG::AssetManifest& assetManifest) {
	internal->LoadAssetManifest(assetManifest);
}

const RPG::OpenGLPipeline& OpenGLAssetManager::GetPipeline(const RPG::Assets::Pipeline& pipeline) const {
	return internal->pipelineCache.at(pipeline);
}

const RPG::OpenGLMesh& OpenGLAssetManager::GetStaticMesh(const RPG::Assets::StaticMesh& staticMesh) const {
	return internal->staticMeshCache.at(staticMesh);
}

const RPG::OpenGLTexture& OpenGLAssetManager::GetTexture(const RPG::Assets::Texture& texture) const {
	return internal->textureCache.at(texture);
}

const RPG::OpenGLMesh& OpenGLAssetManager::GetSceneLines() const {
	return internal->lines;
}