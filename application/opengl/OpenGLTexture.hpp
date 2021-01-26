//
// Created by Alex on 12/13/2020.
//

#pragma once

#include "../../core/Texture.hpp"
#include "../../core/InternalPointer.hpp"
#include <memory>

namespace RPG {
	struct OpenGLTexture {
			OpenGLTexture(std::shared_ptr<RPG::Texture> bitmap);
			void Bind() const;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


