//
// Created by Alex on 12/13/2020.
//

#include "Mesh.hpp"

using RPG::Mesh;

struct Mesh::Internal {
	const std::vector<RPG::Vertex> vertices;
	const uint32_t numVertices;
	const std::vector<uint32_t> indices;
	const uint32_t numIndices;

	Internal(const std::vector<RPG::Vertex>& vertices, const std::vector<uint32_t>& indices) : vertices(vertices),
																							   numVertices(static_cast<uint32_t>(vertices.size())),
																							   indices(indices),
																							   numIndices(static_cast<uint32_t>(indices.size())) {}
};

Mesh::Mesh(const std::vector<RPG::Vertex>& vertices, const std::vector<uint32_t>& indices) : internal(RPG::MakeInternalPointer<Internal>(vertices, indices)) {}

const std::vector<RPG::Vertex>& Mesh::GetVertices() const {
	return internal->vertices;
}

const std::vector<uint32_t>& Mesh::GetIndices() const {
	return internal->indices;
}

const uint32_t& Mesh::GetNumVertices() const {
	return internal->numVertices;
}

const uint32_t& Mesh::GetNumIndices() const {
	return internal->numIndices;
}
