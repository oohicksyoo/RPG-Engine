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
#include "../../core/components/SpriteComponent.hpp"
#include "../../core/components/BoxColliderComponent.hpp"

using RPG::OpenGLPipeline;

namespace {
	GLuint CompileShader(const GLenum& shaderType, const std::string& shaderSource) {
		const std::string logTag{"RPG::OpenGLPipeline::CompileShader"};
		RPG::Log(logTag, "Compiling pipeline");
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

		RPG::Log(logTag, "Creating shader for pipeline '" + shaderName + "'");
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

	void Render(const RPG::OpenGLAssetManager& assetManager, const std::shared_ptr<RPG::Hierarchy> hierarchy, const glm::mat4 cameraMatrix, const bool isGameCamera) const {
		// Instruct OpenGL to starting using our shader program.
		glUseProgram(shaderProgramId);

		// Enable the 'a_vertexPosition' attribute.
		glEnableVertexAttribArray(attributeLocationVertexPosition);

		// Enable the 'a_texCoord' attribute.
		glEnableVertexAttribArray(attributeLocationTexCoord);

		//Enable Transparent
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		for (auto gameObject : hierarchy->GetHierarchy()) {
			RenderGameObject(assetManager, gameObject, cameraMatrix, isGameCamera);
		}

		// Tidy up.
		glDisableVertexAttribArray(attributeLocationVertexPosition);
		glDisableVertexAttribArray(attributeLocationTexCoord);
	}

	void RenderToFrameBuffer(const RPG::OpenGLAssetManager& assetManager, const std::shared_ptr<RPG::Hierarchy> hierarchy, const std::shared_ptr<RPG::FrameBuffer> frameBuffer, const glm::mat4 cameraMatrix, const glm::vec3 clearColor, const bool isGameCamera) const {
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->GetRenderTextureID());
		glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
		glEnable(GL_DEPTH_TEST);

		Render(assetManager, hierarchy, cameraMatrix, isGameCamera);

		//Tidy Up
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void RenderLinesToFrameBuffer(const RPG::OpenGLAssetManager &assetManager, const std::shared_ptr<RPG::FrameBuffer> frameBuffer, const glm::mat4 cameraMatrix) const {
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->GetRenderTextureID());
		glEnable(GL_DEPTH_TEST);
		glLineWidth(3);

		// Instruct OpenGL to starting using our shader program.
		glUseProgram(shaderProgramId);

		// Enable the 'a_vertexPosition' attribute.
		glEnableVertexAttribArray(attributeLocationVertexPosition);

		//Render Mesh
		const RPG::OpenGLMesh& mesh = assetManager.GetSceneLines();

		// Populate the 'u_mvp' uniform in the shader program.

		glUniformMatrix4fv(uniformLocationMVP, 1, GL_FALSE, &(cameraMatrix)[0][0]);

		// Apply the texture we want to paint the mesh with.
		//assetManager.GetTexture((meshComponent != nullptr) ? meshComponent->GetTexture() : spriteComponent->GetTexture()).Bind();

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
		glDrawArrays(GL_LINES, 0, 1764);

		// Tidy up.
		glDisableVertexAttribArray(attributeLocationVertexPosition);

		//Tidy Up
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void RenderGameObject(const RPG::OpenGLAssetManager& assetManager, std::shared_ptr<RPG::GameObject> gameObject, const glm::mat4 cameraMatrix, const bool isGameCamera) const {
		//Render Children First
		for (auto childGameObject : gameObject->GetChildren()) {
			RenderGameObject(assetManager, childGameObject, cameraMatrix, isGameCamera);
		}

		bool canRenderMesh = true;
		auto transform = gameObject->GetTransform();
		auto meshComponent = gameObject->GetComponent<std::shared_ptr<RPG::MeshComponent>, RPG::MeshComponent>(std::make_unique<RPG::MeshComponent>("", ""));
		std::shared_ptr<RPG::SpriteComponent> spriteComponent;
		if (meshComponent == nullptr) {
			spriteComponent = gameObject->GetComponent<std::shared_ptr<RPG::SpriteComponent>, RPG::SpriteComponent>(std::make_unique<RPG::SpriteComponent>(""));
			if (spriteComponent == nullptr)  {
				canRenderMesh = false;
			}
		}

		#ifdef RPG_EDITOR
			if (!isGameCamera) {
				std::shared_ptr<RPG::BoxColliderComponent> boxColliderComponent = gameObject->GetComponent<std::shared_ptr<RPG::BoxColliderComponent>, RPG::BoxColliderComponent>(
						std::make_unique<RPG::BoxColliderComponent>(glm::vec3{1, 1, 1}, false));
				if (boxColliderComponent != nullptr) {
					//Draw box
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

					//TODO: We cant garentee that that obj is there and should store a cube in code for it to reference
					const RPG::OpenGLMesh &mesh = assetManager.GetStaticMesh("assets/models/1_Meter_Cube.obj");

					// Populate the 'u_mvp' uniform in the shader program.

					glm::vec3 rotation = transform->GetRotation();
					glm::fquat rot{rotation};

					glm::mat4 modelMatrix = glm::mat4{1.0f};
					modelMatrix = glm::translate(modelMatrix, transform->GetWorldPosition());
					modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3{1.0f, 0.0f, 0.0f});
					modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3{0.0f, 1.0f, 0.0f});
					modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3{0.0f, 0.0f, 1.0f});
					modelMatrix = glm::scale(modelMatrix, std::any_cast<glm::vec3>(boxColliderComponent->GetSize()));

					glUniformMatrix4fv(uniformLocationMVP, 1, GL_FALSE, &(cameraMatrix * modelMatrix)[0][0]);

					// Apply the texture we want to paint the mesh with.
					std::string colliderText = (boxColliderComponent->IsTrigger()) ? "assets/textures/trigger.png" : "assets/textures/collider.png";
					assetManager.GetTexture(colliderText).Bind();

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
							reinterpret_cast<const GLvoid *>(offsetPosition)
					);

					// Configure the 'a_texCoord' attribute.
					glVertexAttribPointer(attributeLocationTexCoord,
										2,
										GL_FLOAT,
										GL_FALSE,
										stride,
										reinterpret_cast<const GLvoid *>(offsetTexCoord)
					);

					// Execute the draw command - with how many indices to iterate.
					glDrawElements(
							GL_TRIANGLES,
							mesh.GetNumIndices(),
							GL_UNSIGNED_INT,
							reinterpret_cast<const GLvoid *>(0)
					);

					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				}
			}
		#endif

		if (!canRenderMesh) {
			return;
		}

		//Render Mesh
		std::string meshString = (meshComponent != nullptr) ? meshComponent->GetMesh() : spriteComponent->GetMesh();
		if (meshString == "") return;
		const RPG::OpenGLMesh &mesh = assetManager.GetStaticMesh(meshString);

		// Populate the 'u_mvp' uniform in the shader program.

		glUniformMatrix4fv(uniformLocationMVP, 1, GL_FALSE,
						   &(cameraMatrix * transform->GetTransformMatrix())[0][0]);

		// Apply the texture we want to paint the mesh with.
		std::string textureString = (meshComponent != nullptr) ? meshComponent->GetTexture() : spriteComponent->GetTexture();
		if (textureString == "") return;
		assetManager.GetTexture(textureString).Bind();

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
				reinterpret_cast<const GLvoid *>(offsetPosition)
		);

		// Configure the 'a_texCoord' attribute.
		glVertexAttribPointer(attributeLocationTexCoord,
							  2,
							  GL_FLOAT,
							  GL_FALSE,
							  stride,
							  reinterpret_cast<const GLvoid *>(offsetTexCoord)
		);

		// Execute the draw command - with how many indices to iterate.
		glDrawElements(
				GL_TRIANGLES,
				mesh.GetNumIndices(),
				GL_UNSIGNED_INT,
				reinterpret_cast<const GLvoid *>(0)
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

void OpenGLPipeline::Render(const RPG::OpenGLAssetManager& assetManager, const std::shared_ptr<RPG::Hierarchy> hierarchy, const glm::mat4 cameraMatrix) const {
	internal->Render(assetManager, hierarchy, cameraMatrix, true);
}

void OpenGLPipeline::RenderToFrameBuffer(const RPG::OpenGLAssetManager &assetManager,
										 const std::shared_ptr<RPG::Hierarchy> hierarchy,
										 const std::shared_ptr<RPG::FrameBuffer> frameBuffer,
										 const glm::mat4 cameraMatrix,
										 const glm::vec3 clearColor,
										 const bool isGameCamera) const {
	internal->RenderToFrameBuffer(assetManager, hierarchy, frameBuffer, cameraMatrix, clearColor, isGameCamera);
}

void OpenGLPipeline::RenderLinesToFrameBuffer(const RPG::OpenGLAssetManager &assetManager,
											  const std::shared_ptr<RPG::FrameBuffer> framebuffer,
											  const glm::mat4 cameraMatrix) const {
	internal->RenderLinesToFrameBuffer(assetManager, framebuffer, cameraMatrix);
}

void OpenGLPipeline::DeleteFrameBuffer(const std::shared_ptr<RPG::FrameBuffer> framebuffer) const {
	internal->DeleteFrameBuffer(framebuffer);
}