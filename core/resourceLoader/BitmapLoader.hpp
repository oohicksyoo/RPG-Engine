//
// Created by Alex on 1/24/2021.
//

#pragma once
#include "../Bitmap.hpp"
#include "../Assets.hpp"
#include <memory>
#include <string>

namespace RPG {
	template <typename T>
	struct IResourceLoader;

	template <>
	struct IResourceLoader<RPG::Bitmap> {
		public:
			std::shared_ptr<RPG::Bitmap> Load(std::string path);
	};

	inline std::shared_ptr<RPG::Bitmap> IResourceLoader<RPG::Bitmap>::Load(std::string path) {
		return std::make_unique<RPG::Bitmap>(RPG::Assets::LoadBitmapFile(path));
	}
}
