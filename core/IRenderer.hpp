//
// Created by Alex on 12/13/2020.
//

#pragma once

#include "AssetInventory.hpp"
#include "StaticMeshInstance.hpp"
#include <vector>

namespace RPG {
	struct IRenderer {
		virtual void Render(const RPG::Assets::Pipeline& pipeline, const std::vector<RPG::StaticMeshInstance>& staticMeshInstances) = 0;
	};
}
