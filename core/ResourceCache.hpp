//
// Created by Alex on 1/24/2021.
//

#pragma once
#include "IResourceLoader.hpp"
#include "resourceLoader/MeshLoader.hpp"
#include <memory>
#include <string>

namespace RPG {
	template <typename T>
	struct ResourceCache {
		public:
			ResourceCache();
			std::shared_ptr<T> Load(std::string path);

		private:
			std::shared_ptr<IResourceLoader<T>> loader;
	};

	template <typename T>
	inline ResourceCache<T>::ResourceCache() : loader (std::make_unique<IResourceLoader<T>>()) {}

	template <typename T>
	inline std::shared_ptr<T> ResourceCache<T>::Load(std::string path) {
		std::shared_ptr<T> v = loader.get()->Load(path);
		return v;
	}
}


