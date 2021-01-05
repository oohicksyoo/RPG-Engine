//
// Created by Alex on 12/13/2020.
//

#include "AssetInventory.hpp"

std::string RPG::Assets::ResolvePipelinePath(const RPG::Assets::Pipeline &pipeline) {
	switch (pipeline) {
		case RPG::Assets::Pipeline::Default:
		default:
			return "default";
	}
}

std::string RPG::Assets::ResolveStaticMeshPath(const RPG::Assets::StaticMesh &staticMesh) {
	switch (staticMesh) {
		case RPG::Assets::StaticMesh::Quad:
			return "assets/models/Quad.obj";
		case RPG::Assets::StaticMesh::Crate:
			return "assets/models/crate.obj";
		default:
			return "";
	}
}

std::string RPG::Assets::ResolveTexturePath(const RPG::Assets::Texture &texture) {
	switch (texture) {
		case RPG::Assets::Texture::Crate:
			return "assets/textures/crate.png";
		case RPG::Assets::Texture::Sprite:
			return "assets/textures/Sprite.png";
		default:
			return "";
	}
}
