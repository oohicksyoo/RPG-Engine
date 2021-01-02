//
// Created by Alex on 12/13/2020.
//

#include "OpenGLPipeline.hpp"
#include "OpenGLAssetManager.hpp"
#include "../../core/Assets.hpp"
#include "../../core/Log.hpp"
#include <stdexcept>
#include <vector>

#include "../../core/components/MeshComponent.hpp"

using RPG::OpenGLPipeline;

namespace {
	GLuint CompileShader(const GLenum& shaderType, const std::string& shaderSource) {
		const std::string logTag{"RPG::OpenGLPipeline::CompileShader"};
		GLuint shaderId{glCreateShader(shaderType)};

		const char* shaderData{shaderSource.c_str()};
		glShaderSource(shaderId, 1, &shaderData, nullptr);
		glCompileShader(shaderId);

		GLint shaderCompilationResult;
		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &shaderCompilationResult);

		if (!shaderCompilationResult) {
			GLint errorMessageLength;
			glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &errorMessageLength);
			std::vector<char> errorMessage(errorMessageLength + 1);
			glGetShaderInfoLog(shaderId, errorMessageLength, nullptr, &errorMessage[0]);
			RPG::Log(logTag, &errorMessage[0]);
			throw std::runtime_error(logTag + "Shader failed to compile.");
		}

		return shaderId;
	}

	GLuint CreateShaderProgram(const std::string& shaderName) {
		const std::string logTag{"RPG::OpenGLPipeline::CreateShaderProgram"};

		RPG::Log(logTag, "Creating pipeline for '" + shaderName + "'");

		const std::string vertexShaderCode{RPG::Assets::LoadTextFile("assets/shaders/opengl/" + shaderName + ".vert")};
		const std::string fragmentShaderCode{RPG::Assets::LoadTextFile("assets/shaders/opengl/" + shaderName + ".frag")};

		#ifdef USING_GLES
			std::string vertexShaderSource{"#version 100\n" + vertexShaderCode};
			std::string fragmentShaderSource{"#version 100\nprecision mediump float;\n" + fragmentShaderCode};
		#else
			std::string vertexShaderSource{"#version 120\n" + vertexShaderCode};
			std::string fragmentShaderSource{"#version 120\n" + fragmentShaderCode};
		#endif

		GLuint shaderProgramId{glCreateProgram()};
		GLuint vertexShaderId{::CompileShader(GL_VERTEX_SHADER, vertexShaderSource)};
		GLuint fragmentShaderId{::CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource)};

		glAttachShader(shaderProgramId, vertexShaderId);
		glAttachShader(shaderProgramId, fragmentShaderId);
		glLinkProgram(shaderProgramId);

		GLint shaderProgramLinkResult;
		glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &shaderProgramLinkResult);

		if (!shaderProgramLinkResult) {
			GLint errorMessageLength;
			glGetProgramiv(shaderProgramId, GL_INFO_LOG_LENGTH, &errorMessageLength);
			std::vector<char> errorMessage(errorMessageLength + 1);
			glGetProgramInfoLog(shaderProgramId, errorMessageLength, nullptr, &errorMessage[0]);
			RPG::Log(logTag, &errorMessage[0]);
			throw std::runtime_error(logTag + "Shader program failed to compile.");
		}

		glDetachShader(shaderProgramId, vertexShaderId);
		glDetachShader(shaderProgramId, fragmentShaderId);
		glDeleteShader(vertexShaderId);
		glDeleteShader(fragmentShaderId);

		return shaderProgramId;
	}
}

struct OpenGLPipeline::Internal {
	const GLuint shaderProgramId;
	const GLuint uniformLocationMVP;
	const GLuint attributeLocationVertexPosition;
	const GLuint attributeLocationTexCoord;
	const GLsizei stride;
	const GLsizei offsetPosition;
	const GLsizei offsetTexCoord;

	Internal(const std::string& shaderName)
			: shaderProgramId(::CreateShaderProgram(shaderName)),
			  uniformLocationMVP(glGetUniformLocation(shaderProgramId, "u_mvp")),
			  attributeLocationVertexPosition(glGetAttribLocation(shaderProgramId, "a_vertexPosition")),
			  attributeLocationTexCoord(glGetAttribLocation(shaderProgramId, "a_texCoord")),
			  stride(5 * sizeof(float)),
			  offsetPosition(0),
			  offsetTexCoord(3 * sizeof(float)) {}

	void Render(const RPG::OpenGLAssetManager& assetManager, const std::vector<RPG::StaticMeshInstance>& staticMeshInstances) const {
		// Instruct OpenGL to starting using our shader program.
		glUseProgram(shaderProgramId);

		// Enable the 'a_vertexPosition' attribute.
		glEnableVertexAttribArray(attributeLocationVertexPosition);

		// Enable the 'a_texCoord' attribute.
		glEnableVertexAttribArray(attributeLocationTexCoord);

		for (const auto& staticMeshInstance : staticMeshInstances) {
			const RPG::OpenGLMesh& mesh = assetManager.GetStaticMesh(staticMeshInstance.GetMesh());

			// Populate the 'u_mvp' uniform in the shader program.
			glUniformMatrix4fv(uniformLocationMVP, 1, GL_FALSE, &staticMeshInstance.GetTransformMatrix()[0][0]);

			// Apply the texture we want to paint the mesh with.
			assetManager.GetTexture(staticMeshInstance.GetTexture()).Bind();

			// Bind the vertex and index buffers.
			glBindBuffer(GL_ARRAY_BUFFER, mesh.GetVertexBufferId());
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.GetIndexBufferId());

			// Configure the 'a_vertexPosition' attribute.
			glVertexAttribPointer(
					attributeLocationVertexPosition,
					3,
					GL_FLOAT,
					GL_FALSE,
					stride,
					reinterpret_cast<const GLvoid*>(offsetPosition)
			);

			// Configure the 'a_texCoord' attribute.
			glVertexAttribPointer(attributeLocationTexCoord,
								  2,
								  GL_FLOAT,
								  GL_FALSE,
								  stride,
								  reinterpret_cast<const GLvoid*>(offsetTexCoord)
			);

			// Execute the draw command - with how many indices to iterate.
			glDrawElements(
					GL_TRIANGLES,
					mesh.GetNumIndices(),
					GL_UNSIGNED_INT,
					reinterpret_cast<const GLvoid*>(0)
			);
		}

		// Tidy up.
		glDisableVertexAttribArray(attributeLocationVertexPosition);
		glDisableVertexAttribArray(attributeLocationTexCoord);
	}

	void RenderToFrameBuffer(const RPG::OpenGLAssetManager& assetManager, const std::shared_ptr<RPG::Hierarchy> hierarchy, const std::shared_ptr<RPG::FrameBuffer> frameBuffer, const glm::mat4 cameraMatrix) const {
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->GetRenderTextureID());
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
		glEnable(GL_DEPTH_TEST);

		// Instruct OpenGL to starting using our shader program.
		glUseProgram(shaderProgramId);

		// Enable the 'a_vertexPosition' attribute.
		glEnableVertexAttribArray(attributeLocationVertexPosition);

		// Enable the 'a_texCoord' attribute.
		glEnableVertexAttribArray(attributeLocationTexCoord);

		for (auto gameObject : hierarchy->GetHierarchy()) {
			RenderGameObject(assetManager, gameObject, cameraMatrix);
		}

		// Tidy up.
		glDisableVertexAttribArray(attributeLocationVertexPosition);
		glDisableVertexAttribArray(attributeLocationTexCoord);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void RenderGameObject(const RPG::OpenGLAssetManager& assetManager, std::shared_ptr<RPG::GameObject> gameObject, const glm::mat4 cameraMatrix) const {
		//Render Children First
		for (auto childGameObject : gameObject->GetChildren()) {
			RenderGameObject(assetManager, childGameObject, cameraMatrix);
		}

		auto transform = gameObject->GetTransform();
		auto meshComponent = gameObject->GetComponent<std::shared_ptr<RPG::MeshComponent>, RPG::MeshComponent>(std::make_unique<RPG::MeshComponent>(RPG::Assets::StaticMesh::Crate, RPG::Assets::Texture::Crate));
		if (meshComponent == nullptr) {
			return;
		}

		//Render Mesh
		const RPG::OpenGLMesh& mesh = assetManager.GetStaticMesh(meshComponent->GetMesh());

		// Populate the 'u_mvp' uniform in the shader program.
		//TODO: Get reference to the current rendering camera and its CameraMatrix to do the final multiplication
		glUniformMatrix4fv(uniformLocationMVP, 1, GL_FALSE, &(cameraMatrix * transform->GetTransformMatrix())[0][0]);

		// Apply the texture we want to paint the mesh with.
		assetManager.GetTexture(meshComponent->GetTexture()).Bind();

		// Bind the vertex and index buffers.
		glBindBuffer(GL_ARRAY_BUFFER, mesh.GetVertexBufferId());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.GetIndexBufferId());

		// Configure the 'a_vertexPosition' attribute.
		glVertexAttribPointer(
				attributeLocationVertexPosition,
				3,
				GL_FLOAT,
				GL_FALSE,
				stride,
				reinterpret_cast<const GLvoid*>(offsetPosition)
		);

		// Configure the 'a_texCoord' attribute.
		glVertexAttribPointer(attributeLocationTexCoord,
							  2,
							  GL_FLOAT,
							  GL_FALSE,
							  stride,
							  reinterpret_cast<const GLvoid*>(offsetTexCoord)
		);

		// Execute the draw command - with how many indices to iterate.
		glDrawElements(
				GL_TRIANGLES,
				mesh.GetNumIndices(),
				GL_UNSIGNED_INT,
				reinterpret_cast<const GLvoid*>(0)
		);
	}

	void DeleteFrameBuffer(const std::shared_ptr<RPG::FrameBuffer> framebuffer) const {
		glDeleteBuffers(1, &framebuffer->GetBufferID());
		glDeleteTextures(1, &framebuffer->GetRenderTextureID());
		glDeleteRenderbuffers(1, &framebuffer->GetDepthStencilBufferID());
	}

	~Internal() {
		glDeleteProgram(shaderProgramId);
	}
};

OpenGLPipeline::OpenGLPipeline(const std::string& shaderName) : internal(RPG::MakeInternalPointer<Internal>(shaderName)) {}

void OpenGLPipeline::Render(const RPG::OpenGLAssetManager& assetManager, const std::vector<RPG::StaticMeshInstance>& staticMeshInstances) const {
	internal->Render(assetManager, staticMeshInstances);
}

void OpenGLPipeline::RenderToFrameBuffer(const RPG::OpenGLAssetManager &assetManager,
										 const std::shared_ptr<RPG::Hierarchy> hierarchy,
										 const std::shared_ptr<RPG::FrameBuffer> frameBuffer,
										 const glm::mat4 cameraMatrix) const {
	internal->RenderToFrameBuffer(assetManager, hierarchy, frameBuffer, cameraMatrix);
}

void OpenGLPipeline::DeleteFrameBuffer(const std::shared_ptr<RPG::FrameBuffer> framebuffer) const {
	internal->DeleteFrameBuffer(framebuffer);
}