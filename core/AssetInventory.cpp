//
// Created by Alex on 12/13/2020.
//

#include "AssetInventory.hpp"

std::string RPG::Assets::ResolvePipelinePath(const RPG::Assets::Pipeline &pipeline) {
	switch (pipeline) {
		case RPG::Assets::Pipeline::SceneLines:
			return "lines";
		case RPG::Assets::Pipeline::Default:
		default:
			return "default";
	}
}
