//
// Created by Alex on 12/13/2020.
//

#pragma once

#include "AssetInventory.hpp"
#include <vector>

namespace RPG {
	struct AssetManifest {
		const std::vector<RPG::Assets::Pipeline> pipelines;
	};
}
