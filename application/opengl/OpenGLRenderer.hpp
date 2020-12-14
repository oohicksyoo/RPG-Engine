//
// Created by Alex on 12/13/2020.
//

#pragma once

#include "../../core/InternalPointer.hpp"
#include "../../core/Renderer.hpp"
#include "OpenGLAssetManager.hpp"
#include <memory>

namespace RPG {
	struct OpenGLRenderer : public RPG::Renderer {
		public:
			OpenGLRenderer(std::shared_ptr<RPG::OpenGLAssetManager> AssetManager);
			void Render(const RPG::Assets::Pipeline& pipeline, const std::vector<RPG::StaticMeshInstance>& staticMeshInstances) override;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


