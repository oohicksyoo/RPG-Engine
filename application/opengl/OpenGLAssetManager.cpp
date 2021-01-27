//
// Created by Alex on 12/13/2020.
//

#include "OpenGLAssetManager.hpp"
#include "../../core/Assets.hpp"
#include "../../core/Log.hpp"
#include "../../core/Content.hpp"
#include <unordered_map>
#include <memory>

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

		return RPG::OpenGLMesh(std::make_unique<RPG::Mesh>(RPG::Mesh(lines, {})));
	}
}

struct OpenGLAssetManager::Internal {
	std::unordered_map<RPG::Assets::Pipeline, RPG::OpenGLPipeline> pipelineCache;

	std::unordered_map<std::string, RPG::OpenGLMesh> meshCache;
	std::unordered_map<std::string, RPG::OpenGLTexture> textureCache;

	RPG::OpenGLMesh lines;

	Internal() : lines(::CreateSceneGridLines()) {
		RPG::Content::GetInstance().OnLoadedAsset<RPG::Mesh>([this](std::string path, std::shared_ptr<RPG::Mesh> mesh) {
			if (meshCache.count(path) == 0) {
				RPG::Log("AssetManager", "Adding new mesh to cache (" + path + ")");
				meshCache.insert(std::make_pair(path, RPG::OpenGLMesh(mesh)));
			}
		});

		RPG::Content::GetInstance().OnLoadedAsset<RPG::Texture>([this](std::string path, std::shared_ptr<RPG::Texture> bitmap) {
			if (textureCache.count(path) == 0) {
				RPG::Log("AssetManager", "Adding new bitmap to cache (" + path + ")");
				textureCache.insert(std::make_pair(path, RPG::OpenGLTexture(bitmap)));
			}
		});

		RPG::Content::GetInstance().OnGetTextureID([this](std::string path) -> uint32_t {
			if (textureCache.count(path) != 0) {
				return textureCache.at(path).TextureID();
			}
			return -1;
		});
	}

	void LoadPipelines(const std::vector<RPG::Assets::Pipeline>& pipelines) {
		for (const auto& pipeline : pipelines) 	{
			if (pipelineCache.count(pipeline) == 0) {
				pipelineCache.insert(std::make_pair(pipeline, RPG::OpenGLPipeline(RPG::Assets::ResolvePipelinePath(pipeline))));
			}
		}
	}

	void LoadAssetManifest(const RPG::AssetManifest& assetManifest) {
		LoadPipelines(assetManifest.pipelines);
	}
};

OpenGLAssetManager::OpenGLAssetManager() : internal(RPG::MakeInternalPointer<Internal>()) {}

void OpenGLAssetManager::LoadAssetManifest(const RPG::AssetManifest& assetManifest) {
	internal->LoadAssetManifest(assetManifest);
}

const RPG::OpenGLPipeline& OpenGLAssetManager::GetPipeline(const RPG::Assets::Pipeline& pipeline) const {
	return internal->pipelineCache.at(pipeline);
}

const RPG::OpenGLMesh& OpenGLAssetManager::GetStaticMesh(std::string path) const {
	return internal->meshCache.at(path);
}

const RPG::OpenGLTexture& OpenGLAssetManager::GetTexture(std::string path) const {
	return internal->textureCache.at(path);
}

const RPG::OpenGLMesh& OpenGLAssetManager::GetSceneLines() const {
	return internal->lines;
}