//
// Created by Alex on 12/25/2020.
//

#include "FrameBuffer.hpp"

using RPG::FrameBuffer;

struct FrameBuffer::Internal {
	uint32_t bufferID;
	uint32_t renderTextureID;
	uint32_t depthStencilBufferID;

	Internal(uint32_t bID, uint32_t rtID, uint32_t dsbID) : bufferID(bID),
															renderTextureID(rtID),
															depthStencilBufferID(dsbID) {}

	~Internal() {
		//TODO: Need to delete the buffer stuff above but dont know the renderer to delete it
	}
};

FrameBuffer::FrameBuffer(uint32_t bufferID, uint32_t renderTextureID, uint32_t depthStencilBufferID) : internal(RPG::MakeInternalPointer<Internal>(bufferID, renderTextureID, depthStencilBufferID)) {}

const glm::vec2& FrameBuffer::GetSize() const{
	return glm::vec2{0, 0};
}

const uint32_t& FrameBuffer::GetBufferID() const{
	return internal->bufferID;
}

const uint32_t& FrameBuffer::GetRenderTextureID() const{
	return internal->renderTextureID;
}

const uint32_t& FrameBuffer::GetDepthStencilBufferID() const{
	return internal->depthStencilBufferID;
}