//
// Created by Alex on 1/24/2021.
//

#include "Content.hpp"

using RPG::Content;

Content::Content() {
	meshResourceCache = RPG::ResourceCache<RPG::Mesh>();
	bitmapResourceCache = RPG::ResourceCache<RPG::Texture>();
}

void Content::OnGetTextureID(std::function<uint32_t(std::string)> getTextureIDFunc) {
	textureIDFunc = getTextureIDFunc;
}

uint32_t Content::GetTextureID(std::string path) {
	if (textureIDFunc == nullptr) {
		return -1;
	}
	return textureIDFunc(path);
}