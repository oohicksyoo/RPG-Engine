//
// Created by Alex on 12/13/2020.
//

#include "OpenGLPipeline.hpp"
#include "OpenGLAssetManager.hpp"
#include "../../application/ApplicationStats.hpp"
#include "../../core/Assets.hpp"
#include "../../core/Log.hpp"
#include "../../core/Time.hpp"
#include <stdexcept>
#include <vector>

#include "../../core/components/MeshComponent.hpp"
#include "../../core/components/SpriteComponent.hpp"
#include "../../core/components/BoxColliderComponent.hpp"
#include "../../core/components/PhysicsComponent.hpp"

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


		//TODO: Inject Engine functions for lighting
		#ifdef USING_GLES
			std::string vertexShaderSource{"#version 100\n" + vertexShaderCode};
			std::string fragmentShaderSource{"#version 100\nprecision mediump float;\n" + fragmentShaderCode};
        #else
			//Use to be 120 going to  bump to 420  though
			std::string vertexShaderSource{"#version 420\n" + vertexShaderCode};
			std::string fragmentShaderSource{"#version 420\n" + fragmentShaderCode};
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
    const std::string shaderName;
	const GLuint shaderProgramId;
	const GLuint uniformLocationMVP;
	const GLuint uniformLocationM;
	const GLuint attributeLocationVertexPosition;
	const GLuint attributeLocationTexCoord;
	const GLuint attributeLocationNormal;
	const GLsizei stride;
	const GLsizei offsetPosition;
	const GLsizei offsetTexCoord;
	const GLsizei offsetNormal;

	Internal(const std::string& shaderName)
			: shaderName(shaderName),
              shaderProgramId(::CreateShaderProgram(shaderName)),
			  uniformLocationMVP(glGetUniformLocation(shaderProgramId, "u_mvp")),
              uniformLocationM(glGetUniformLocation(shaderProgramId, "u_model")),
			  attributeLocationVertexPosition(glGetAttribLocation(shaderProgramId, "a_vertexPosition")),
			  attributeLocationTexCoord(glGetAttribLocation(shaderProgramId, "a_texCoord")),
              attributeLocationNormal(glGetAttribLocation(shaderProgramId, "a_normal")),
			  stride(8 * sizeof(float)),
			  offsetPosition(0),
			  offsetTexCoord(3 * sizeof(float)),
			  offsetNormal(5 * sizeof(float)) {}

	void Render(const RPG::OpenGLAssetManager& assetManager, const std::shared_ptr<RPG::Hierarchy> hierarchy, const glm::mat4 cameraMatrix, const uint32_t shadowMap, const bool isGameCamera) const {
		// Instruct OpenGL to starting using our shader program.
		glUseProgram(shaderProgramId);

		// Enable the 'a_vertexPosition' attribute.
		glEnableVertexAttribArray(attributeLocationVertexPosition);

		// Enable the 'a_texCoord' attribute.
		glEnableVertexAttribArray(attributeLocationTexCoord);

		//Enable the 'a_normal' attribute
		glEnableVertexAttribArray(attributeLocationNormal);

		//Enable Transparent
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		for (auto gameObject : hierarchy->GetHierarchy()) {
			RenderGameObject(assetManager, gameObject, cameraMatrix, shadowMap, isGameCamera);
		}

		// Tidy up.
		glDisableVertexAttribArray(attributeLocationVertexPosition);
		glDisableVertexAttribArray(attributeLocationTexCoord);
        glDisableVertexAttribArray(attributeLocationNormal);
	}

	void RenderToFrameBuffer(const RPG::OpenGLAssetManager& assetManager, const std::shared_ptr<RPG::Hierarchy> hierarchy, const std::shared_ptr<RPG::FrameBuffer> frameBuffer, const glm::mat4 cameraMatrix, const glm::vec3 clearColor, const uint32_t shadowMap, const bool isGameCamera) const {
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->GetRenderTextureID());
		glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
		glEnable(GL_DEPTH_TEST);

		Render(assetManager, hierarchy, cameraMatrix, shadowMap, isGameCamera);

		//Tidy Up
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void RenderToDepthBuffer(const RPG::OpenGLAssetManager& assetManager, const std::shared_ptr<RPG::Hierarchy> hierarchy, const std::shared_ptr<RPG::FrameBuffer> frameBuffer) const {
        //TODO: Move location and pre calc once
        float near_plane = 1.0f, far_plane = 7.5f;
        glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3( 0.0f), glm::vec3( 0.0f, 1.0f,  0.0f));
        glm::mat4  lightSpaceMatrix = lightProjection * lightView;

        glViewport(0, 0, 1024, 1024);
	    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->GetRenderTextureID());
        glClear(GL_DEPTH_BUFFER_BIT);

        glCullFace(GL_FRONT);
        RenderSceneToDepthBuffer(assetManager, hierarchy, lightSpaceMatrix);
        glCullFace(GL_BACK);

        //Tidy Up
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glm::vec2 size = RPG::ApplicationStats::GetInstance().GetWindowSize();
        glViewport(0, 0, size.x, size.y);
	}

	void RenderSceneToDepthBuffer(const RPG::OpenGLAssetManager& assetManager, const std::shared_ptr<RPG::Hierarchy> hierarchy, const glm::mat4 lightMatrix) const {
        // Instruct OpenGL to starting using our shader program.
        glUseProgram(shaderProgramId);

        // Enable the 'a_vertexPosition' attribute.
        glEnableVertexAttribArray(attributeLocationVertexPosition);

        for (auto gameObject : hierarchy->GetHierarchy()) {
            RenderGameObjectToDepthBuffer(assetManager, gameObject, lightMatrix);
        }

        // Tidy up.
        glDisableVertexAttribArray(attributeLocationVertexPosition);
	}

	void RenderGameObjectToDepthBuffer(const RPG::OpenGLAssetManager& assetManager, std::shared_ptr<RPG::GameObject> gameObject, const glm::mat4 lightMatrix) const {
        //Render Children First
        for (auto childGameObject : gameObject->GetChildren()) {
            RenderGameObjectToDepthBuffer(assetManager, childGameObject, lightMatrix);
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

        if (!canRenderMesh) {
            return;
        }

        //Render Mesh
        std::string meshString = (meshComponent != nullptr) ? meshComponent->GetMesh() : spriteComponent->GetMesh();
        if (meshString == "") return;
        const RPG::OpenGLMesh &mesh = assetManager.GetStaticMesh(meshString);

        // Populate the 'u_mvp' uniform in the shader program.
        glUniformMatrix4fv(uniformLocationMVP, 1, GL_FALSE, &(lightMatrix * transform->GetTransformMatrix())[0][0]);

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

        // Execute the draw command - with how many indices to iterate.
        glDrawElements(
                GL_TRIANGLES,
                mesh.GetNumIndices(),
                GL_UNSIGNED_INT,
                reinterpret_cast<const GLvoid *>(0)
        );
	}

	void RenderLinesToFrameBuffer(const RPG::OpenGLAssetManager &assetManager, const std::shared_ptr<RPG::FrameBuffer> frameBuffer, const glm::mat4 cameraMatrix) const {
        #ifdef RPG_EDITOR
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
        #endif
	}

	void RenderGameObject(const RPG::OpenGLAssetManager& assetManager, std::shared_ptr<RPG::GameObject> gameObject, const glm::mat4 cameraMatrix, const uint32_t shadowMap, const bool isGameCamera) const {
		//Render Children First
		for (auto childGameObject : gameObject->GetChildren()) {
			RenderGameObject(assetManager, childGameObject, cameraMatrix, shadowMap, isGameCamera);
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

					//TODO: We cant guarantee that that obj is there and should store a cube in code for it to reference
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
                    glUniformMatrix4fv(uniformLocationM, 1, GL_FALSE, &(modelMatrix)[0][0]);

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

                    // Configure the 'a_normal' attribute.
                    glVertexAttribPointer(
                            attributeLocationNormal,
                            3,
                            GL_FLOAT,
                            GL_FALSE,
                            stride,
                            reinterpret_cast<const GLvoid*>(offsetNormal)
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

				auto physicsComponent = gameObject->GetComponent<std::shared_ptr<RPG::PhysicsComponent>, RPG::PhysicsComponent>("PhysicsComponent");

				if (physicsComponent != nullptr) {
					//Draw box
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

					//TODO: We cant guarantee that that obj is there and should store a cube in code for it to reference
					const RPG::OpenGLMesh &mesh = assetManager.GetStaticMesh("assets/models/1_Meter_Circle.obj");

					//TODO: We need to refactor and compress the rendering of stuff into a single call
					if (physicsComponent->GetShape() == RPG::PhysicsShape::Circle) {

						// Populate the 'u_mvp' uniform in the shader program.

						glm::vec3 rotation = transform->GetRotation();
						glm::fquat rot{rotation};
						float diameter = physicsComponent->GetDiameter();
						glm::vec3 scale{diameter, 1, diameter};

						glm::mat4 modelMatrix = glm::mat4{1.0f};
						modelMatrix = glm::translate(modelMatrix, transform->GetWorldPosition());
						modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3{1.0f, 0.0f, 0.0f});
						modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3{0.0f, 1.0f, 0.0f});
						modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3{0.0f, 0.0f, 1.0f});
						modelMatrix = glm::scale(modelMatrix, scale);

						glUniformMatrix4fv(uniformLocationMVP, 1, GL_FALSE, &(cameraMatrix * modelMatrix)[0][0]);
                        glUniformMatrix4fv(uniformLocationM, 1, GL_FALSE, &(modelMatrix)[0][0]);

						// Apply the texture we want to paint the mesh with.
						std::string colliderText = (physicsComponent->IsTrigger()) ? "assets/textures/trigger.png"
																				   : "assets/textures/collider.png";
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

                        // Configure the 'a_normal' attribute.
                        glVertexAttribPointer(
                                attributeLocationNormal,
                                3,
                                GL_FLOAT,
                                GL_FALSE,
                                stride,
                                reinterpret_cast<const GLvoid*>(offsetNormal)
                        );

						// Execute the draw command - with how many indices to iterate.
						glDrawElements(
								GL_TRIANGLES,
								mesh.GetNumIndices(),
								GL_UNSIGNED_INT,
								reinterpret_cast<const GLvoid *>(0)
						);
					} else {
						//Circle 1
						{
							// Populate the 'u_mvp' uniform in the shader program.

							glm::vec3 rotation = transform->GetRotation();
							glm::fquat rot{rotation};
							float diameter = physicsComponent->GetDiameter();
							glm::vec3 scale{diameter, 1, diameter};
							auto startPosition = physicsComponent->GetStartPosition();

							glm::mat4 modelMatrix = glm::mat4{1.0f};
							modelMatrix = glm::translate(modelMatrix, transform->GetWorldPosition() + glm::vec3{startPosition.x, 0, startPosition.y});
							modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3{1.0f, 0.0f, 0.0f});
							modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3{0.0f, 1.0f, 0.0f});
							modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3{0.0f, 0.0f, 1.0f});
							modelMatrix = glm::scale(modelMatrix, scale);

							glUniformMatrix4fv(uniformLocationMVP, 1, GL_FALSE, &(cameraMatrix * modelMatrix)[0][0]);
                            glUniformMatrix4fv(uniformLocationM, 1, GL_FALSE, &(modelMatrix)[0][0]);

							// Apply the texture we want to paint the mesh with.
							std::string colliderText = (physicsComponent->IsTrigger()) ? "assets/textures/trigger.png"
																					   : "assets/textures/collider.png";
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

                            // Configure the 'a_normal' attribute.
                            glVertexAttribPointer(
                                    attributeLocationNormal,
                                    3,
                                    GL_FLOAT,
                                    GL_FALSE,
                                    stride,
                                    reinterpret_cast<const GLvoid*>(offsetNormal)
                            );

							// Execute the draw command - with how many indices to iterate.
							glDrawElements(
									GL_TRIANGLES,
									mesh.GetNumIndices(),
									GL_UNSIGNED_INT,
									reinterpret_cast<const GLvoid *>(0)
							);
						}

						//Circle Two
						{
							// Populate the 'u_mvp' uniform in the shader program.

							glm::vec3 rotation = transform->GetRotation();
							glm::fquat rot{rotation};
							float diameter = physicsComponent->GetDiameter();
							glm::vec3 scale{diameter, 1, diameter};
							auto endPosition = physicsComponent->GetEndPosition();

							glm::mat4 modelMatrix = glm::mat4{1.0f};
							modelMatrix = glm::translate(modelMatrix, transform->GetWorldPosition() + glm::vec3{endPosition.x, 0, endPosition.y});
							modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3{1.0f, 0.0f, 0.0f});
							modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3{0.0f, 1.0f, 0.0f});
							modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3{0.0f, 0.0f, 1.0f});
							modelMatrix = glm::scale(modelMatrix, scale);

							glUniformMatrix4fv(uniformLocationMVP, 1, GL_FALSE, &(cameraMatrix * modelMatrix)[0][0]);
                            glUniformMatrix4fv(uniformLocationM, 1, GL_FALSE, &(modelMatrix)[0][0]);

							// Apply the texture we want to paint the mesh with.
							std::string colliderText = (physicsComponent->IsTrigger()) ? "assets/textures/trigger.png"
																					   : "assets/textures/collider.png";
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

                            // Configure the 'a_normal' attribute.
                            glVertexAttribPointer(
                                    attributeLocationNormal,
                                    3,
                                    GL_FLOAT,
                                    GL_FALSE,
                                    stride,
                                    reinterpret_cast<const GLvoid*>(offsetNormal)
                            );

							// Execute the draw command - with how many indices to iterate.
							glDrawElements(
									GL_TRIANGLES,
									mesh.GetNumIndices(),
									GL_UNSIGNED_INT,
									reinterpret_cast<const GLvoid *>(0)
							);
						}

						//Lines
						{
							std::vector<RPG::Vertex> lines;
							auto radius = physicsComponent->GetDiameter() * 0.5f;
							auto worldPosition = transform->GetWorldPosition();
							auto startPosition = physicsComponent->GetStartPosition();
							auto endPosition = physicsComponent->GetEndPosition();
							auto sp = glm::vec3{startPosition.x, 0, startPosition.y};
							auto ep = glm::vec3{endPosition.x, 0, endPosition.y};
							float distance = glm::distance(sp, ep);
							glm::vec3 direction = glm::normalize(ep - sp);
							glm::vec3 right = glm::cross(direction, glm::vec3{0,1,0}) * radius;
							glm::vec3 left = -right;

							lines.push_back(RPG::Vertex{{transform->GetWorldPosition() + sp + right}, {0, 0}});
							lines.push_back(RPG::Vertex{{transform->GetWorldPosition() + sp + right + direction * distance}, {0, 0}});
							lines.push_back(RPG::Vertex{{transform->GetWorldPosition() + sp + left}, {0, 0}});
							lines.push_back(RPG::Vertex{{transform->GetWorldPosition() + sp + left + direction * distance}, {0, 0}});

							//Render Mesh
							const RPG::OpenGLMesh& mesh = RPG::OpenGLMesh(std::make_unique<RPG::Mesh>(RPG::Mesh(lines, {})));

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

                            // Configure the 'a_normal' attribute.
                            glVertexAttribPointer(
                                    attributeLocationNormal,
                                    3,
                                    GL_FLOAT,
                                    GL_FALSE,
                                    stride,
                                    reinterpret_cast<const GLvoid*>(offsetNormal)
                            );

							// Execute the draw command - with how many indices to iterate.
							glDrawArrays(GL_LINES, 0, 4);
						}
					};

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

        //Camera Position
        glm::vec3 cameraPosition = glm::vec3(cameraMatrix[3]);

        //TODO: Move location and pre calc once
        float near_plane = 1.0f, far_plane = 7.5f;
        glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3( 0.0f), glm::vec3( 0.0f, 1.0f,  0.0f));
        glm::mat4  lightSpaceMatrix = lightProjection * lightView;
        glm::vec3 lightDirection = glm::normalize(-lightPos);

        //Lighting
        glUniformMatrix4fv(glGetUniformLocation(shaderProgramId, "u_lightSpace"), 1, GL_FALSE, &lightSpaceMatrix[0][0]);
        //glUniform1i(glGetUniformLocation(shaderProgramId, "shadowMap"), shadowMap);
        glUniform1f(glGetUniformLocation(shaderProgramId, "material.shininess"), 32.0);
        glUniform3f(glGetUniformLocation(shaderProgramId, "directionLight.direction"), lightDirection.x, lightDirection.y, lightDirection.z);
        glUniform3f(glGetUniformLocation(shaderProgramId, "directionLight.ambient"), 0.05, 0.05, 0.05);
        glUniform3f(glGetUniformLocation(shaderProgramId, "directionLight.diffuse"), 0.4, 0.4, 0.4);
        glUniform3f(glGetUniformLocation(shaderProgramId, "directionLight.specular"), 0.5, 0.5, 0.5);

        /*for (int i = 0; i < 4; i++) {
            glUniform3f(glGetUniformLocation(shaderProgramId, ("pointLights[" + std::to_string(i) + "].position").c_str()), 0.0, 10.0, 0.0);
            glUniform3f(glGetUniformLocation(shaderProgramId, ("pointLights[" + std::to_string(i) + "].ambient").c_str()), 0.05, 0.05, 0.05);
            glUniform3f(glGetUniformLocation(shaderProgramId, ("pointLights[" + std::to_string(i) + "].diffuse").c_str()), 0.8, 0.8, 0.8);
            glUniform3f(glGetUniformLocation(shaderProgramId, ("pointLights[" + std::to_string(i) + "].specular").c_str()), 1.0, 1.0, 1.0);
            glUniform1f(glGetUniformLocation(shaderProgramId, ("pointLights[" + std::to_string(i) + "].constant").c_str()), 1.0);
            glUniform1f(glGetUniformLocation(shaderProgramId, ("pointLights[" + std::to_string(i) + "].linear").c_str()), 0.09);
            glUniform1f(glGetUniformLocation(shaderProgramId, ("pointLights[" + std::to_string(i) + "].quadratic").c_str()), 0.032);
        }*/

        glUniform3f(glGetUniformLocation(shaderProgramId, "viewPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z);



		// Populate the 'u_mvp' uniform in the shader program.
		glUniformMatrix4fv(uniformLocationMVP, 1, GL_FALSE, &(cameraMatrix * transform->GetTransformMatrix())[0][0]);
        glUniformMatrix4fv(uniformLocationM, 1, GL_FALSE, &(transform->GetTransformMatrix())[0][0]);

		// Apply the texture we want to paint the mesh with.
		std::string textureString = "assets/textures/default.png";//(meshComponent != nullptr) ? meshComponent->GetTexture() : spriteComponent->GetTexture();
		if (textureString == "") return;
		glActiveTexture(GL_TEXTURE0);
		assetManager.GetTexture(textureString).Bind();

        glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, shadowMap);

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

        // Configure the 'a_normal' attribute.
        glVertexAttribPointer(
                attributeLocationNormal,
                3,
                GL_FLOAT,
                GL_FALSE,
                stride,
                reinterpret_cast<const GLvoid*>(offsetNormal)
        );

		// Execute the draw command - with how many indices to iterate.
		glDrawElements(
				GL_TRIANGLES,
				mesh.GetNumIndices(),
				GL_UNSIGNED_INT,
				reinterpret_cast<const GLvoid *>(0)
		);
	}

	//Clears framebuffer to a solid colour
    void ClearFrameBufferToColor(const std::shared_ptr<RPG::FrameBuffer> framebuffer, const glm::vec3 clearColor) const {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->GetRenderTextureID());
        glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
        glEnable(GL_DEPTH_TEST);

        //Tidy Up
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    //Renders a group of GameObjects with this pipeline/shader type
    void RenderToFrameBuffer(const RPG::OpenGLAssetManager& assetManager, const std::shared_ptr<RPG::FrameBuffer> frameBuffer, const std::vector<RPG::GameObjectMaterialGroup> gameObjects, const glm::mat4 cameraMatrix) const {

	    //Catch to render scene lines
	    if (shaderName == Assets::ResolvePipelinePath(Assets::Pipeline::SceneLines)) {
            RenderLinesToFrameBuffer(assetManager, frameBuffer, cameraMatrix);
	        return;
	    }

	    //Empty vector passed in
	    if (gameObjects.size() == 0) {
	        return;
	    }

        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->GetRenderTextureID());

        //Rendering all objects using the shader
        {
            // Instruct OpenGL to starting using our shader program.
            glUseProgram(shaderProgramId);

            // Enable the 'a_vertexPosition' attribute.
            glEnableVertexAttribArray(attributeLocationVertexPosition);

            // Enable the 'a_texCoord' attribute.
            glEnableVertexAttribArray(attributeLocationTexCoord);

            //Enable the 'a_normal' attribute
            glEnableVertexAttribArray(attributeLocationNormal);

            //Enable Transparent
            //TODO: 4000 should come from some sort of Enum value for Transparent render queue materials
            if (gameObjects[0].material->GetRenderQueue() >= 4000) {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            }

            //TODO: Grab and Set Material Values
            //TODO: Add in a Particles Component so maybe really its an IRenderable
            for (auto gomg : gameObjects) {
                auto gameObject = gomg.gameObject;
                auto material = gomg.material;
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

                if (canRenderMesh) {
                    //Render Mesh
                    std::string meshString = (meshComponent != nullptr) ? meshComponent->GetMesh()
                                                                        : spriteComponent->GetMesh();

                    if (meshString == "") return;
                    const RPG::OpenGLMesh &mesh = assetManager.GetStaticMesh(meshString);

                    // Populate the 'u_mvp' uniform in the shader program.
                    glUniformMatrix4fv(uniformLocationMVP, 1, GL_FALSE, &(cameraMatrix * transform->GetTransformMatrix())[0][0]);
                    glUniformMatrix4fv(uniformLocationM, 1, GL_FALSE, &(transform->GetTransformMatrix())[0][0]);

                    //Default ones shaders can use in general
                    glUniform1f(glGetUniformLocation(shaderProgramId, "time"), RPG::Time::Seconds());

                    //SET PROPERTIES
                    //TEXTURES
                    int textureCount = 0;
                    for (auto property : material->GetProperties()) {
                        auto type = property->GetType();
                        auto name = property->GetName();
                        auto value = property->GetProperty();

                        if (type == "float") {
                            auto v = std::any_cast<float>(value);
                            glUniform1f(glGetUniformLocation(shaderProgramId, name.c_str()), v);
                        }

                        if (type == "glm::vec2") {
                            auto v = std::any_cast<glm::vec2>(value);
                            glUniform2f(glGetUniformLocation(shaderProgramId, name.c_str()), v.x, v.y);
                        }

                        if (type == "glm::vec3") {
                            auto v = std::any_cast<glm::vec3>(value);
                            glUniform3f(glGetUniformLocation(shaderProgramId, name.c_str()), v.x, v.y, v.z);
                        }

                        if (type == "glm::vec4") {
                            auto v = std::any_cast<glm::vec4>(value);
                            glUniform4f(glGetUniformLocation(shaderProgramId, name.c_str()), v.x, v.y, v.z, v.w);
                        }

                        if (type == "RPG::Resource::String") {
                            auto v = std::any_cast<std::string>(value);
                            glActiveTexture(GL_TEXTURE0 + textureCount);
                            assetManager.GetTexture(v).Bind();
                            textureCount++;
                        }
                    }

                    // Bind the vertex and index buffers.
                    glBindBuffer(GL_ARRAY_BUFFER, mesh.GetVertexBufferId());
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.GetIndexBufferId());

                    // Configure the 'a_vertexPosition' attribute.
                    glVertexAttribPointer( attributeLocationVertexPosition, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const GLvoid *>(offsetPosition));

                    // Configure the 'a_texCoord' attribute.
                    glVertexAttribPointer(attributeLocationTexCoord, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const GLvoid *>(offsetTexCoord));

                    // Configure the 'a_normal' attribute.
                    glVertexAttribPointer( attributeLocationNormal, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const GLvoid *>(offsetNormal));

                    // Execute the draw command - with how many indices to iterate.
                    glDrawElements( GL_TRIANGLES, mesh.GetNumIndices(), GL_UNSIGNED_INT, reinterpret_cast<const GLvoid *>(0));
                } else {
                    RPG::Log("Render", "Cant render " + gameObject->GetName());
                };
            }

            glDisable(GL_BLEND);

            // Tidy up.
            glDisableVertexAttribArray(attributeLocationVertexPosition);
            glDisableVertexAttribArray(attributeLocationTexCoord);
            glDisableVertexAttribArray(attributeLocationNormal);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void DisplayFrameBuffer(const RPG::OpenGLAssetManager& assetManager, const std::shared_ptr<RPG::FrameBuffer> frameBuffer) const {
        #ifndef RPG_EDITOR
	    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgramId);

        const RPG::OpenGLMesh &mesh = assetManager.GetFullscreenQuad();
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, frameBuffer->GetRenderTextureID());

        glBindVertexArray(mesh.GetVertexBufferId());
        glDrawElements(GL_TRIANGLES, mesh.GetNumIndices(), GL_UNSIGNED_INT, 0);
        #endif
	}

	~Internal() {
		glDeleteProgram(shaderProgramId);
	}
};

OpenGLPipeline::OpenGLPipeline(const std::string& shaderName) : internal(RPG::MakeInternalPointer<Internal>(shaderName)) {}

void OpenGLPipeline::Render(const RPG::OpenGLAssetManager& assetManager, const std::shared_ptr<RPG::Hierarchy> hierarchy, const glm::mat4 cameraMatrix, const uint32_t shadowMap) const {
	internal->Render(assetManager, hierarchy, cameraMatrix, shadowMap, true);
}

void OpenGLPipeline::RenderToFrameBuffer(const RPG::OpenGLAssetManager &assetManager,
										 const std::shared_ptr<RPG::Hierarchy> hierarchy,
										 const std::shared_ptr<RPG::FrameBuffer> frameBuffer,
										 const glm::mat4 cameraMatrix,
										 const glm::vec3 clearColor,
                                         const uint32_t shadowMap,
										 const bool isGameCamera) const {
	internal->RenderToFrameBuffer(assetManager, hierarchy, frameBuffer, cameraMatrix, clearColor, shadowMap, isGameCamera);
}

void OpenGLPipeline::RenderLinesToFrameBuffer(const RPG::OpenGLAssetManager &assetManager,
											  const std::shared_ptr<RPG::FrameBuffer> framebuffer,
											  const glm::mat4 cameraMatrix) const {
	internal->RenderLinesToFrameBuffer(assetManager, framebuffer, cameraMatrix);
}

void OpenGLPipeline::RenderToDepthBuffer(const RPG::OpenGLAssetManager &assetManager, const std::shared_ptr<RPG::Hierarchy> hierarchy, const std::shared_ptr<RPG::FrameBuffer> frameBuffer) const {
    internal->RenderToDepthBuffer(assetManager, hierarchy, frameBuffer);
}

void OpenGLPipeline::ClearFrameBufferToColor(const std::shared_ptr<RPG::FrameBuffer> framebuffer, const glm::vec3 clearColor) const {
    internal->ClearFrameBufferToColor(framebuffer, clearColor);
}

void OpenGLPipeline::RenderToFrameBuffer(const RPG::OpenGLAssetManager& assetManager, const std::shared_ptr<RPG::FrameBuffer> framebuffer,
                                         const std::vector<RPG::GameObjectMaterialGroup> gameObjects, const glm::mat4 cameraMatrix) const {
    internal->RenderToFrameBuffer(assetManager, framebuffer, gameObjects, cameraMatrix);
}

void OpenGLPipeline::DisplayFrameBuffer(const RPG::OpenGLAssetManager& assetManager, const std::shared_ptr<RPG::FrameBuffer> frameBuffer) const {
    internal->DisplayFrameBuffer(assetManager, frameBuffer);
}