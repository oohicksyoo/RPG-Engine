//
// Created by Alex on 12/13/2020.
//

#pragma once

#include "../../core/AssetManifest.hpp"
#include "../../core/InternalPointer.hpp"
#include "OpenGLMesh.hpp"
#include "OpenGLPipeline.hpp"
#include "OpenGLTexture.hpp"

namespace RPG {
	struct OpenGLAssetManager {
		public:
			OpenGLAssetManager();
			void LoadAssetManifest(const RPG::AssetManifest& assetManifest);
			const RPG::OpenGLPipeline& GetPipeline(const RPG::Assets::Pipeline& pipeline) const;
			const RPG::OpenGLMesh& GetStaticMesh(const RPG::Assets::StaticMesh& staticMesh) const;
			const RPG::OpenGLTexture& GetTexture(const RPG::Assets::Texture& texture) const;
			const RPG::OpenGLMesh& GetSceneLines() const;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


