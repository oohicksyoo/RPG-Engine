//
// Created by Alex on 1/24/2021.
//

#pragma once
#include "../Mesh.hpp"
#include <memory>
#include <string>

namespace RPG {
	template <typename T>
	struct IResourceLoader;

	template <>
	struct IResourceLoader<RPG::Mesh> {
		public:
			std::shared_ptr<RPG::Mesh> Load(std::string path);
	};

	inline std::shared_ptr<RPG::Mesh> IResourceLoader<RPG::Mesh>::Load(std::string path) {
		RPG::Log("MeshLoader", "Attempting to load a mesh at: " + path);
		return nullptr;
	}
}


