//
// Created by Alex on 12/13/2020.
//

#include "OpenGLTexture.hpp"
#include "../../core/GraphicsWrapper.hpp"

using RPG::OpenGLTexture;

namespace {
	GLuint CreateTexture(std::shared_ptr<RPG::Texture> bitmap) {
		GLuint textureId;

		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_CLAMP_TO_EDGE); //GL_LINEAR
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_CLAMP_TO_EDGE);
		glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RGBA,
				bitmap->GetWidth(),
				bitmap->GetHeight(),
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				bitmap->GetPixelData());
		glGenerateMipmap(GL_TEXTURE_2D);

		return textureId;
	}
}

struct OpenGLTexture::Internal {
	const GLuint textureId;

	Internal(std::shared_ptr<RPG::Texture> bitmap) : textureId(::CreateTexture(bitmap)) {}

	~Internal() {
		glDeleteTextures(1, &textureId);
	}
};

OpenGLTexture::OpenGLTexture(std::shared_ptr<RPG::Texture> bitmap) : internal(RPG::MakeInternalPointer<Internal>(bitmap)) {}

void OpenGLTexture::Bind() const {
	glBindTexture(GL_TEXTURE_2D, internal->textureId);
}

uint32_t OpenGLTexture::TextureID() const {
	return internal->textureId;
}