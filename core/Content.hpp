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

namespace RPG {
	struct Content : public Singleton<Content> {
		friend struct Singleton<Content>;

		public:
			Content();

			template <typename T>
			std::shared_ptr<T> Load(std::string path);

		private:
			ResourceCache<RPG::Mesh> meshResourceCache;
			ResourceCache<RPG::Bitmap> bitmapResourceCache;
	};

	template<>
	inline std::shared_ptr<RPG::Mesh> Content::Load<RPG::Mesh>(std::string path) {
		RPG::Log("Content", "Loading Mesh Asset " + path);
		return meshResourceCache.Load(path);
	}

	template<>
	inline std::shared_ptr<RPG::Bitmap> Content::Load<RPG::Bitmap>(std::string path) {
		RPG::Log("Content", "Loading Bitmap Asset " + path);
		return bitmapResourceCache.Load(path);
	}
}


