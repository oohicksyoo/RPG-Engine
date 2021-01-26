//
// Created by Alex on 12/13/2020.
//

#pragma once

#include "Mesh.hpp"
#include "Texture.hpp"
#include <string>
#include <vector>

namespace RPG::Assets {
	std::string LoadTextFile(const std::string& path);
	void SaveTextFile(const std::string& content, const std::string& path);
	void RemoveFile(const std::string &path);
	RPG::Mesh LoadOBJFile(const std::string& path);
	RPG::Texture LoadBitmapFile(const std::string& path);
	std::vector<char> LoadBinaryFile(const std::string& path);
}


