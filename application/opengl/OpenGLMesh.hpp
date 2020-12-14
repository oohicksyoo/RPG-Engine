//
// Created by Alex on 12/13/2020.
//

#pragma once

#include "../../core/GraphicsWrapper.hpp"
#include "../../core/InternalPointer.hpp"
#include "../../core/Mesh.hpp"

namespace RPG {
	struct OpenGLMesh {
		public:
			OpenGLMesh(const RPG::Mesh& mesh);
			const GLuint& GetVertexBufferId() const;
			const GLuint& GetIndexBufferId() const;
			const uint32_t& GetNumIndices() const;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


