//
// Created by Alex on 12/13/2020.
//

#pragma once

#include <string>

namespace RPG::Assets {
	enum class Pipeline {
		Default,
		SceneLines,
		DepthMap
	};

	std::string ResolvePipelinePath(const RPG::Assets::Pipeline& pipeline);
}


