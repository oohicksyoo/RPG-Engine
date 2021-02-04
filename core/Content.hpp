//
// Created by Alex on 1/24/2021.
//

#pragma once

#include "Singleton.hpp"
#include "ResourceCache.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"
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
			void Load(std::string path);

			template <typename T>
			void OnLoadedAsset(std::function<void(std::string path, std::shared_ptr<T>)> callback);

			void OnGetTextureID(std::function<uint32_t(std::string path)> getTextureIDFunc);
			uint32_t GetTextureID(std::string path);

		private:
			ResourceCache<RPG::Mesh> meshResourceCache;
			std::function<void(std::string path, std::shared_ptr<RPG::Mesh>)> meshOnLoadedCallback;
			ResourceCache<RPG::Texture> bitmapResourceCache;
			std::function<void(std::string path, std::shared_ptr<RPG::Texture>)> bitmapOnLoadedCallback;
			std::function<int(std::string path)> textureIDFunc;

	};

	template<>
	inline void Content::Load<RPG::Mesh>(std::string path) {
		//RPG::Log("Content", "Loading Mesh Asset " + path);
		auto mesh = meshResourceCache.Load(path);
		if (meshOnLoadedCallback != nullptr) {
			meshOnLoadedCallback(path, mesh);
		}
	}

	template<>
	inline void Content::Load<RPG::Texture>(std::string path) {
		//RPG::Log("Content", "Loading Texture Asset " + path);
		auto bitmap = bitmapResourceCache.Load(path);
		if (bitmapOnLoadedCallback != nullptr) {
			bitmapOnLoadedCallback(path, bitmap);
		}
	}

	template<>
	inline void Content::OnLoadedAsset<RPG::Mesh>(std::function<void(std::string path, std::shared_ptr<RPG::Mesh>)> callback) {
		meshOnLoadedCallback = callback;
	}

	template<>
	inline void Content::OnLoadedAsset<RPG::Texture>(std::function<void(std::string path, std::shared_ptr<RPG::Texture>)> callback) {
		bitmapOnLoadedCallback = callback;
	}
}


