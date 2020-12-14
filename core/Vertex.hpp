//
// Created by Alex on 12/13/2020.
//

#pragma once

#include "GLMWrapper.hpp"

namespace RPG {
	struct Vertex {
		glm::vec3 position;
		glm::vec2 texCoord;

		bool operator==(const RPG::Vertex& other) const;
	};
}

namespace std {
	template<>
	struct hash<RPG::Vertex> {
		size_t operator()(const RPG::Vertex& vertex) const {
			return ((hash<glm::vec3>()(vertex.position) ^ (hash<glm::vec2>()(vertex.texCoord) << 1)) >> 1);
		}
	};
}


