//
// Created by Alex on 12/13/2020.
//

#include "AssetInventory.hpp"

std::string RPG::Assets::ResolvePipelinePath(const RPG::Assets::Pipeline &pipeline) {
	switch (pipeline) {
	    case RPG::Assets::Pipeline::DefaultReleaseFrameBuffer:
	        return "defaultReleaseFrameBuffer";
		case RPG::Assets::Pipeline::SceneLines:
			return "lines";
	    case RPG::Assets::Pipeline::DepthMap:
	        return "depthMap";
	    case RPG::Assets::Pipeline::Sprite:
	        return "sprite";
        case RPG::Assets::Pipeline::Default2:
            return "default2";
        case RPG::Assets::Pipeline::Unlit:
            return "unlit";
        case RPG::Assets::Pipeline::Flipbook:
            return "flipbook";
		case RPG::Assets::Pipeline::Default:
		default:
			return "default";
	}
}

RPG::Assets::Pipeline RPG::Assets::GetPipelineByName(std::string name) {
    if (name == "defaultReleaseFrameBuffer") {
        return RPG::Assets::Pipeline::DefaultReleaseFrameBuffer;
    } else if (name == "lines") {
        return RPG::Assets::Pipeline::SceneLines;
    } else if (name == "depthMap") {
        return RPG::Assets::Pipeline::DepthMap;
    } else if (name == "sprite") {
        return RPG::Assets::Pipeline::Sprite;
    } else if (name == "default2") {
        return RPG::Assets::Pipeline::Default2;
    } else if (name == "unlit") {
        return RPG::Assets::Pipeline::Unlit;
    } else if (name == "flipbook") {
        return RPG::Assets::Pipeline::Flipbook;
    }

    return RPG::Assets::Pipeline::Default;
}
