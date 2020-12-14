//
// Created by Alex on 12/13/2020.
//

#include "Vertex.hpp"

using RPG::Vertex;

bool Vertex::operator==(const Vertex& other) const {
	return position == other.position && texCoord == other.texCoord;
}
