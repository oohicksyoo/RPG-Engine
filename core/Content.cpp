//
// Created by Alex on 1/24/2021.
//

#include "Content.hpp"

using RPG::Content;

Content::Content() {
	meshResourceCache = RPG::ResourceCache<RPG::Mesh>();
	bitmapResourceCache = RPG::ResourceCache<RPG::Texture>();
}