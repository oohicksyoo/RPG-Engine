//
// Created by Alex on 12/13/2020.
//

#include "OpenGLTexture.hpp"
#include "../../core/GraphicsWrapper.hpp"

using RPG::OpenGLTexture;

namespace {
	GLuint CreateTexture(const RPG::Bitmap& bitmap) {
		GLuint textureId;

		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RGBA,
				bitmap.GetWidth(),
				bitmap.GetHeight(),
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				bitmap.GetPixelData());
		glGenerateMipmap(GL_TEXTURE_2D);

		return textureId;
	}
}

struct OpenGLTexture::Internal {
	const GLuint textureId;

	Internal(const RPG::Bitmap& bitmap) : textureId(::CreateTexture(bitmap)) {}

	~Internal() {
		glDeleteTextures(1, &textureId);
	}
};

OpenGLTexture::OpenGLTexture(const RPG::Bitmap& bitmap) : internal(RPG::MakeInternalPointer<Internal>(bitmap)) {}

void OpenGLTexture::Bind() const {
	glBindTexture(GL_TEXTURE_2D, internal->textureId);
}