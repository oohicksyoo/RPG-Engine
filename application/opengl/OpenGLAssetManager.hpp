//
// Created by Alex on 12/13/2020.
//

#pragma once

#include "../../core/AssetManifest.hpp"
#include "../../core/InternalPointer.hpp"
#include "OpenGLMesh.hpp"
#include "OpenGLPipeline.hpp"
#include "OpenGLTexture.hpp"
#include <string>
#include <memory>

namespace RPG {
	struct OpenGLAssetManager {
		public:
			OpenGLAssetManager();
			void LoadAssetManifest(const RPG::AssetManifest& assetManifest);
			const RPG::OpenGLPipeline& GetPipeline(const RPG::Assets::Pipeline& pipeline) const;
			const RPG::OpenGLMesh& GetStaticMesh(std::string path) const;
			const RPG::OpenGLTexture& GetTexture(std::string path) const;
            #ifdef RPG_EDITOR
			    const RPG::OpenGLMesh& GetSceneLines() const;
            #endif
			const std::shared_ptr<RPG::Material> GetMaterial(std::string path) const;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


