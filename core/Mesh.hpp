//
// Created by Alex on 12/13/2020.
//

#pragma once

#include "InternalPointer.hpp"
#include "Loadable.hpp"
#include "Vertex.hpp"
#include <vector>

namespace RPG {
	struct Mesh : public Loadable {
		public:
			Mesh(const std::vector<RPG::Vertex>& vertices, const std::vector<uint32_t>& indices);
			const std::vector<RPG::Vertex>& GetVertices() const;
			const std::vector<uint32_t>& GetIndices() const;
			const uint32_t& GetNumVertices() const;
			const uint32_t& GetNumIndices() const;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


