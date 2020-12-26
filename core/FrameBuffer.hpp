//
// Created by Alex on 12/25/2020.
//

#pragma once

#include "GLMWrapper.hpp"
#include "InternalPointer.hpp"

namespace RPG {
	struct FrameBuffer {
		public:
			FrameBuffer(uint32_t bufferID, uint32_t renderTextureID, uint32_t depthStencilBufferID);
			const glm::vec2& GetSize() const;
			const uint32_t& GetBufferID() const;
			const uint32_t& GetRenderTextureID() const;
			const uint32_t& GetDepthStencilBufferID() const;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


