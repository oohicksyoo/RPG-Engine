//
// Created by Alex on 12/13/2020.
//

#pragma once

#include "../../core/InternalPointer.hpp"
#include "../../core/StaticMeshInstance.hpp"
#include <string>
#include <vector>

namespace RPG {
	struct OpenGLAssetManager;

	struct OpenGLPipeline {
		public:
			OpenGLPipeline(const std::string& shaderName);
			void Render(const RPG::OpenGLAssetManager& assetManager, const std::vector<RPG::StaticMeshInstance>& staticMeshInstances) const;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


