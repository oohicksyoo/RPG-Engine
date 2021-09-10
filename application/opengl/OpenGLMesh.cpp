//
// Created by Alex on 12/13/2020.
//

#include "OpenGLMesh.hpp"
#include <vector>

using RPG::OpenGLMesh;

namespace {
    std::vector<float> CreateBufferData(std::shared_ptr<RPG::Mesh> mesh) {
        std::vector<float> bufferData;

        for (const auto& vertex : mesh->GetVertices()) {
            // Position
            bufferData.push_back(vertex.position.x);
            bufferData.push_back(vertex.position.y);
            bufferData.push_back(vertex.position.z);

            // Texture coordinate
            bufferData.push_back(vertex.texCoord.x);
            bufferData.push_back(vertex.texCoord.y);

            //Normal
            bufferData.push_back(vertex.normal.x);
            bufferData.push_back(vertex.normal.y);
            bufferData.push_back(vertex.normal.z);
        }

        return bufferData;
	}
}

struct OpenGLMesh::Internal {
    GLuint vao, vbo, ebo;
    //TODO: In future make dynamic stride based on provide data to the RPG::Mesh
    GLsizei stride = 8 * sizeof(float);
    GLuint indiceSize;

	Internal(std::shared_ptr<RPG::Mesh> mesh) {
        std::vector<float> bufferData = ::CreateBufferData(mesh);
        indiceSize = mesh->GetIndices().size();
	    glGenVertexArrays(1, &vao);
	    glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, bufferData.size() * sizeof(float), bufferData.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indiceSize * sizeof(uint32_t), mesh->GetIndices().data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const GLvoid*>(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const GLvoid*>(5 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
	}

	~Internal() {
	    glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
	}
};

OpenGLMesh::OpenGLMesh(std::shared_ptr<RPG::Mesh> mesh) : internal(RPG::MakeInternalPointer<Internal>(mesh)) {}

const GLuint& OpenGLMesh::GetVertexArrayObject() const {
	return internal->vao;
}

const uint32_t& OpenGLMesh::GetNumIndices() const {
	return internal->indiceSize;
}
