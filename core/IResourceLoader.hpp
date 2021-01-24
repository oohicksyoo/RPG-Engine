//
// Created by Alex on 1/24/2021.
//

#pragma once

#include "Log.hpp"
#include <memory>
#include <string>

namespace RPG {
	template <typename T>
	struct IResourceLoader {
		public:
			std::shared_ptr<T> Load(std::string path);
	};

	template <typename T>
	inline std::shared_ptr<T> IResourceLoader<T>::Load(std::string path) {
		RPG::Log("IResourceLoader", "Can not load resource at path: " + path);
		return nullptr;
	}
}
