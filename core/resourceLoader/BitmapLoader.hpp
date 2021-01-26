//
// Created by Alex on 1/24/2021.
//

#pragma once
#include "../Texture.hpp"
#include "../Assets.hpp"
#include <memory>
#include <string>

namespace RPG {
	template <typename T>
	struct IResourceLoader;

	template <>
	struct IResourceLoader<RPG::Texture> {
		public:
			std::shared_ptr<RPG::Texture> Load(std::string path);
	};

	inline std::shared_ptr<RPG::Texture> IResourceLoader<RPG::Texture>::Load(std::string path) {
		return std::make_unique<RPG::Texture>(RPG::Assets::LoadBitmapFile(path));
	}
}
