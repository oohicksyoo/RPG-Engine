//
// Created by Alex on 12/13/2020.
//

#include "OpenGLMesh.hpp"
#include <vector>

using RPG::OpenGLMesh;

namespace {
	GLuint CreateVertexBuffer(const RPG::Mesh& mesh) {
		std::vector<float> bufferData;

		for (const auto& vertex : mesh.GetVertices()) {
			// Position
			bufferData.push_back(vertex.position.x);
			bufferData.push_back(vertex.position.y);
			bufferData.push_back(vertex.position.z);

			// Texture coordinate
			bufferData.push_back(vertex.texCoord.x);
			bufferData.push_back(vertex.texCoord.y);
		}

		GLuint bufferId;
		glGenBuffers(1, &bufferId);
		glBindBuffer(GL_ARRAY_BUFFER, bufferId);
		glBufferData(GL_ARRAY_BUFFER, bufferData.size() * sizeof(float), bufferData.data(), GL_STATIC_DRAW);

		return bufferId;
	}

	GLuint CreateIndexBuffer(const RPG::Mesh& mesh) {
		GLuint bufferId;
		glGenBuffers(1, &bufferId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.GetIndices().size() * sizeof(uint32_t), mesh.GetIndices().data(), GL_STATIC_DRAW);

		return bufferId;
	}
}

struct OpenGLMesh::Internal {
	const GLuint bufferIdVertices;
	const GLuint bufferIdIndices;
	const uint32_t numIndices;

	Internal(const RPG::Mesh& mesh) : bufferIdVertices(::CreateVertexBuffer(mesh)),
									  bufferIdIndices(::CreateIndexBuffer(mesh)),
									  numIndices(static_cast<uint32_t>(mesh.GetIndices().size())) {}

	~Internal() {
		glDeleteBuffers(1, &bufferIdVertices);
		glDeleteBuffers(1, &bufferIdIndices);
	}
};

OpenGLMesh::OpenGLMesh(const RPG::Mesh& mesh) : internal(RPG::MakeInternalPointer<Internal>(mesh)) {}

const GLuint& OpenGLMesh::GetVertexBufferId() const {
	return internal->bufferIdVertices;
}

const GLuint& OpenGLMesh::GetIndexBufferId() const {
	return internal->bufferIdIndices;
}

const uint32_t& OpenGLMesh::GetNumIndices() const {
	return internal->numIndices;
}
