//
// Created by Alex on 12/13/2020.
//

#pragma once

#include "../../core/Bitmap.hpp"
#include "../../core/InternalPointer.hpp"

namespace RPG {
	struct OpenGLTexture {
			OpenGLTexture(const RPG::Bitmap& bitmap);
			void Bind() const;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


