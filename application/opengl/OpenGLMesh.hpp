//
// Created by Alex on 12/13/2020.
//

#pragma once

#include "../../core/GraphicsWrapper.hpp"
#include "../../core/InternalPointer.hpp"
#include "../../core/Mesh.hpp"
#include <memory>

namespace RPG {
	struct OpenGLMesh {
		public:
			OpenGLMesh(std::shared_ptr<RPG::Mesh> mesh);
			const GLuint& GetVertexBufferId() const;
			const GLuint& GetIndexBufferId() const;
			const uint32_t& GetNumIndices() const;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


