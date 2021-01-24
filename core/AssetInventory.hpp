//
// Created by Alex on 12/13/2020.
//

#pragma once

#include <string>

namespace RPG::Assets {
	enum class Pipeline {
		Default,
		SceneLines
	};

	enum class StaticMesh {
		Quad,
		Crate,
		Cube
	};

	enum class Texture {
		Crate,
		Sprite,
		Default,
		Collider,
		Trigger
	};

	std::string ResolvePipelinePath(const RPG::Assets::Pipeline& pipeline);
	std::string ResolveStaticMeshPath(const RPG::Assets::StaticMesh& staticMesh);
	std::string ResolveTexturePath(const RPG::Assets::Texture& texture);
}


