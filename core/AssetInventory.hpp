//
// Created by Alex on 12/13/2020.
//

#pragma once

#include <string>

namespace RPG::Assets {
	enum class Pipeline {
	    DefaultReleaseFrameBuffer,
		Default,
		SceneLines,
		DepthMap,
		Sprite,
		Default2,
		Unlit,
		Flipbook
	};

	std::string ResolvePipelinePath(const RPG::Assets::Pipeline& pipeline);
	RPG::Assets::Pipeline GetPipelineByName(std::string name);
}


