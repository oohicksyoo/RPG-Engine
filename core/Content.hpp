//
// Created by Alex on 12/17/2020.
//

#pragma once

#include "Assets.hpp"
#include "Loadable.hpp"
#include "Mesh.hpp"
#include "Log.hpp"
#include <string>
#include <unordered_map>

namespace RPG {
	struct Content {
		public:
			static Content& Instance() {
				static Content instance;
				return instance;
			}

			template<typename T>
			inline T Load(std::string path) {
				LogSize();

				//Path not loaded before
				if (cachedLoadables.find(path) == cachedLoadables.end()) {
					Mesh mesh = RPG::Assets::LoadOBJFile(path);
					cachedLoadables.emplace(path, mesh);
					return mesh;
				} else {
					RPG::Log("Content", "Path (" + path + ") already exists and loaded.");
					Mesh mesh = RPG::Assets::LoadOBJFile(path);
					cachedLoadables.emplace(path, mesh);
					return mesh;
					//return cachedLoadables.at(path);
				}
			}

			void LogSize() {
				RPG::Log("Content", std::to_string(cachedLoadables.size()));
			}

		private:
			std::unordered_map<std::string, Loadable> cachedLoadables;
	};
}


