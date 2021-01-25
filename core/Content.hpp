//
// Created by Alex on 1/24/2021.
//

#pragma once

#include "Singleton.hpp"
#include "ResourceCache.hpp"
#include "Mesh.hpp"
#include "Bitmap.hpp"
#include "Log.hpp"
#include <string>
#include <memory>
#include <typeinfo>
#include <functional>

namespace RPG {
	struct Content : public Singleton<Content> {
		friend struct Singleton<Content>;

		public:
			Content();

			template <typename T>
			std::shared_ptr<T> Load(std::string path);

			template <typename T>
			void OnLoadedAsset(std::function<void(std::string path, std::shared_ptr<T>)> callback);

		private:
			ResourceCache<RPG::Mesh> meshResourceCache;
			std::function<void(std::string path, std::shared_ptr<RPG::Mesh>)> meshOnLoadedCallback;
			ResourceCache<RPG::Bitmap> bitmapResourceCache;
			std::function<void(std::string path, std::shared_ptr<RPG::Bitmap>)> bitmapOnLoadedCallback;

	};

	template<>
	inline std::shared_ptr<RPG::Mesh> Content::Load<RPG::Mesh>(std::string path) {
		RPG::Log("Content", "Loading Mesh Asset " + path);
		auto mesh = meshResourceCache.Load(path);
		if (meshOnLoadedCallback != nullptr) {
			meshOnLoadedCallback(path, mesh);
		}
		return mesh;
	}

	template<>
	inline std::shared_ptr<RPG::Bitmap> Content::Load<RPG::Bitmap>(std::string path) {
		RPG::Log("Content", "Loading Bitmap Asset " + path);
		auto bitmap = bitmapResourceCache.Load(path);
		if (bitmapOnLoadedCallback != nullptr) {
			bitmapOnLoadedCallback(path, bitmap);
		}
		return bitmap;
	}

	template<>
	inline void Content::OnLoadedAsset<RPG::Mesh>(std::function<void(std::string path, std::shared_ptr<RPG::Mesh>)> callback) {
		meshOnLoadedCallback = callback;
	}

	template<>
	inline void Content::OnLoadedAsset<RPG::Bitmap>(std::function<void(std::string path, std::shared_ptr<RPG::Bitmap>)> callback) {
		bitmapOnLoadedCallback = callback;
	}
}


