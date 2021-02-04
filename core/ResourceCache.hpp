//
// Created by Alex on 1/24/2021.
//

#pragma once
#include "IResourceLoader.hpp"
#include "resourceLoader/MeshLoader.hpp"
#include "resourceLoader/BitmapLoader.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace RPG {
	template <typename T>
	struct ResourceCache {
		public:
			ResourceCache();
			std::shared_ptr<T> Load(std::string path);

		private:
			std::shared_ptr<IResourceLoader<T>> loader;
			std::unordered_map<std::string, std::shared_ptr<T>> cache;
	};

	template <typename T>
	inline ResourceCache<T>::ResourceCache() : loader (std::make_unique<IResourceLoader<T>>()) {}

	template <typename T>
	inline std::shared_ptr<T> ResourceCache<T>::Load(std::string path) {
		//Determine if this asset is already loaded or not
		if (cache.count(path) != 0) {
			//RPG::Log("Resource Cache", "Asset (" + path + ") was found in the cache.");
			return cache.at(path);
		}

		std::shared_ptr<T> v = loader.get()->Load(path);

		if (v != nullptr) {
			cache.insert({path, v});
			//RPG::Log("Resource Cache", "Asset (" + path + ") was loaded into the cache.");
		} else {
			//RPG::Log("Resource Cache", "Asset (" + path + ") failed to load into the cache");
		}

		return v;
	}
}


