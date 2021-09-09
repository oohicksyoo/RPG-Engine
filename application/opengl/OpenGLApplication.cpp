//
// Created by Alex on 12/13/2020.
//

#include "OpenGLApplication.hpp"
#include "../../core/GraphicsWrapper.hpp"
#include "../../core/Log.hpp"
#include "../../core/SDLWindow.hpp"
#include "OpenGLAssetManager.hpp"
#include "OpenGLRenderer.hpp"
#include "../../core/SceneManager.hpp"
#include <string>
#include <map>

#include "../ApplicationStats.hpp"

#ifdef RPG_EDITOR
	#include "../../../editor/EditorManager.hpp"
	#include "../../core/FrameBuffer.hpp"
	#include "../../core/Vertex.hpp"
#endif

#include "../../core/Serializer.hpp"
#include "../../core/Scene.hpp"
#include "../../core/components/MeshComponent.hpp"
#include "../../core/GameObjectMaterialGroup.hpp"

using RPG::OpenGLApplication;

namespace {
	void UpdateViewport(SDL_Window* window) {
		static const std::string logTag{ "RPG::OpenGLApplication::UpdateViewport" };

		int viewportWidth;
		int viewportHeight;
		SDL_GL_GetDrawableSize(window, &viewportWidth, &viewportHeight);
		RPG::Log(logTag, "Created OpenGL context with viewport size: " + std::to_string(viewportWidth) + " x " + std::to_string(viewportHeight));
		RPG::ApplicationStats::GetInstance().SetWindowSize(glm::vec2{viewportWidth, viewportHeight});

		glViewport(0, 0, viewportWidth, viewportHeight);
	}

	SDL_GLContext CreateContext(SDL_Window* window) {
		static const std::string logTag{ "RPG::OpenGLApplication::CreateContext" };

		SDL_GLContext context{SDL_GL_CreateContext(window)};

		#ifdef WIN32
			glewInit();
		#endif

		glClearDepthf(1.0f);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_CULL_FACE);

		::UpdateViewport(window);

		return context;
	}

	std::shared_ptr<RPG::OpenGLAssetManager> CreateAssetManager() {
		return std::make_shared<RPG::OpenGLAssetManager>(RPG::OpenGLAssetManager());
	}

	RPG::OpenGLRenderer CreateRenderer(std::shared_ptr<RPG::OpenGLAssetManager> assetManager) {
		return RPG::OpenGLRenderer(assetManager);
	}

	#if ! defined(USING_GLES)
		void ResizeFrameBuffer(std::shared_ptr<RPG::FrameBuffer> frameBuffer, glm::vec2 size) {
			//Resize
			//Resize Texture
			glBindTexture(GL_TEXTURE_2D, frameBuffer->GetRenderTextureID());
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
			glBindTexture(GL_TEXTURE_2D, 0);

			//Setup depth-stencil buffer
			glBindRenderbuffer(GL_RENDERBUFFER, frameBuffer->GetDepthStencilBufferID());
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_STENCIL, size.x, size.y);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);

			//Attach depth and stencil buffer to the framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->GetBufferID());
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, frameBuffer->GetDepthStencilBufferID());
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, frameBuffer->GetDepthStencilBufferID());
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		std::shared_ptr<RPG::FrameBuffer> CreateFrameBuffer(glm::vec2 size) {
			uint32_t bufferID;
			uint32_t renderTextureID;
			uint32_t depthStencilBufferID;

			//Generate OpenGL Objects
			glGenFramebuffers(1, &bufferID);
			glGenTextures(1, &renderTextureID);
			glGenRenderbuffers(1, &depthStencilBufferID);

			//Setup Textures
			glBindTexture(GL_TEXTURE_2D, renderTextureID);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glBindTexture(GL_TEXTURE_2D, 0);

			//Setup Framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, bufferID);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderTextureID, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			std::shared_ptr<RPG::FrameBuffer> framebuffer = std::make_unique<RPG::FrameBuffer>(RPG::FrameBuffer{bufferID, renderTextureID, depthStencilBufferID});
			ResizeFrameBuffer(framebuffer, size);

			if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
				RPG::Log("Framebuffer", "Framebuffer was created and completed");
			}

			return framebuffer;
		}

        std::shared_ptr<RPG::FrameBuffer> CreateDepthBuffer(glm::vec2 size) {
	        uint32_t depthMapFBO;
	        glGenFramebuffers(1, &depthMapFBO);

	        uint32_t depthMap;
            glGenTextures(1, &depthMap);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            glTexImage2D(GL_TEXTURE_2D, 0 , GL_DEPTH_COMPONENT, size.x, size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            std::shared_ptr<RPG::FrameBuffer> framebuffer = std::make_unique<RPG::FrameBuffer>(RPG::FrameBuffer{depthMapFBO, depthMap});
            if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
                RPG::Log("Framebuffer", "Framebuffer was created and completed");
            }

            return framebuffer;
	    }

        std::shared_ptr<RPG::FrameBuffer> CreateCubemapDepthBuffer(glm::vec2 size) {
            uint32_t depthMapFBO;
            glGenFramebuffers(1, &depthMapFBO);

            uint32_t depthCubemap;
            glGenTextures(1, &depthCubemap);
            glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
            for (unsigned int i = 0; i < 6; i++) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0 , GL_DEPTH_COMPONENT, size.x, size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            }
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

            //Attach depth texture as FBOs depth buffer
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthCubemap, 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            std::shared_ptr<RPG::FrameBuffer> framebuffer = std::make_unique<RPG::FrameBuffer>(RPG::FrameBuffer{depthMapFBO, depthCubemap});
            if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
                RPG::Log("Framebuffer", "Framebuffer was created and completed");
            }

	        return framebuffer;
        }
    #endif

    std::shared_ptr<RPG::IScene> CreateTestScene() {
        glm::vec2 size = RPG::ApplicationStats::GetInstance().GetWindowSize();
	    auto scene = std::make_unique<RPG::Scene>(RPG::Scene({static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y)}));

        auto cube = std::make_shared<RPG::GameObject>(RPG::GameObject("Cube"));
        cube->AddComponent(std::make_shared<RPG::MeshComponent>(RPG::MeshComponent("assets/models/1_Meter_Cube.obj", "assets/materials/default.mat")));

        auto camera = std::make_shared<RPG::GameObject>(RPG::GameObject("Camera"));
        auto transformComponent = camera->GetTransform();
        camera->AddComponent(std::make_shared<RPG::CameraComponent>(RPG::CameraComponent(static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y), transformComponent)));

        auto cameraComponent = camera->GetComponent<std::shared_ptr<RPG::CameraComponent>, RPG::CameraComponent>(std::make_unique<RPG::CameraComponent>(1, 1, transformComponent));
        cameraComponent->SetDistance(2);

	    scene->GetHierarchy()->Add(cube);
        scene->GetHierarchy()->Add(camera);

	    return scene;
	}

    std::shared_ptr<RPG::GameObject> CreateSceneLinesGameObject() {
        std::shared_ptr<RPG::GameObject> go = std::make_unique<RPG::GameObject>("Scene Lines");
        go->AddComponent(std::make_unique<RPG::MeshComponent>("assets/models/1_Meter_Cube.obj", "assets/materials/default.mat"));

        return go;
    }

    void CreateFullscreenQuad(uint32_t &quadVAO, uint32_t &quadVBO) {
        float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
                // positions   // texCoords
                -1.0f,  1.0f,  0.0f, 1.0f,
                -1.0f, -1.0f,  0.0f, 0.0f,
                1.0f, -1.0f,  1.0f, 0.0f,

                -1.0f,  1.0f,  0.0f, 1.0f,
                1.0f, -1.0f,  1.0f, 0.0f,
                1.0f,  1.0f,  1.0f, 1.0f
        };

        //unsigned int quadVAO, quadVBO;
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	}
}

struct OpenGLApplication::Internal {
	bool hasRanFirstFrame = false;

	const RPG::SDLWindow window;
	SDL_GLContext context;
	const std::shared_ptr<RPG::OpenGLAssetManager> assetManager;
    std::map<Assets::Pipeline, std::vector<GameObjectMaterialGroup>> mappedRenderPass;
	RPG::OpenGLRenderer renderer;

	#ifdef RPG_EDITOR
		bool hasRanPreviewFrame = false;
		RPG::EditorManager editorManager;
		std::shared_ptr<RPG::FrameBuffer> framebuffer;
		std::shared_ptr<RPG::FrameBuffer> gameFramebuffer;
		std::shared_ptr<RPG::FrameBuffer> depthBuffer;
		std::shared_ptr<RPG::IScene> materialMakerScene;
		std::shared_ptr<RPG::FrameBuffer> materialMakerBuffer;

		//SceneLines
		std::shared_ptr<GameObject> sceneLinesGameObject;
		std::shared_ptr<RPG::Material> sceneLinesMaterial;
    #else
        uint32_t quadVAO, quadVBO;
        std::shared_ptr<RPG::FrameBuffer> framebuffer;
    #endif


	#ifdef RPG_EDITOR
		Internal() : window(RPG::SDLWindow(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI)),
					 context(::CreateContext(window.GetWindow())),
					 assetManager(::CreateAssetManager()),
					 renderer(::CreateRenderer(assetManager)),
					 editorManager(window, context, assetManager),
					 framebuffer(::CreateFrameBuffer(glm::vec2{1280, 720})),
					 gameFramebuffer(::CreateFrameBuffer(glm::vec2{1280, 720})),
					 depthBuffer(::CreateDepthBuffer(glm::vec2{1024, 1024})),
					 materialMakerScene(::CreateTestScene()),
                     materialMakerBuffer(::CreateFrameBuffer(glm::vec2{1280, 720})),
                     sceneLinesGameObject(::CreateSceneLinesGameObject()),
                     sceneLinesMaterial(std::make_unique<RPG::Material>("Scene Lines", 0, "lines")) {}
	#else
		Internal() : window(RPG::SDLWindow(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI)),
					 context(::CreateContext(window.GetWindow())),
					 assetManager(::CreateAssetManager()),
					 renderer(::CreateRenderer(assetManager)),
                     framebuffer(::CreateFrameBuffer(glm::vec2{1280, 720})) {
            CreateFullscreenQuad(quadVAO, quadVBO);
            RPG::Log("Quad", std::to_string(quadVAO));
		}
	#endif

	void Render() {
		SDL_GL_MakeCurrent(window.GetWindow(), context);

		#ifdef RPG_EDITOR
		    //Organize Scene for Rendering
		    BuildRenderOrder();
		    GetScene()->ClearFrameBufferToColor(renderer, framebuffer, {0.3f, 0.3f, 0.3f});
            GetScene()->ClearFrameBufferToColor(renderer, gameFramebuffer, {0.0f, 0.0f, 0.0f});

            //Render Scene Lines
            std::vector<RPG::GameObjectMaterialGroup> sceneLines;
            RPG::GameObjectMaterialGroup sceneLinesGroup;
            sceneLinesGroup.gameObject = sceneLinesGameObject;
            sceneLinesGroup.material = sceneLinesMaterial;
            sceneLines.push_back(sceneLinesGroup);
            GetScene()->RenderToFrameBuffer(renderer, RPG::Assets::Pipeline::SceneLines, framebuffer, sceneLines, false);

            //Render through scene once for Scene and Once for Game view
		    for (auto map : mappedRenderPass) {
                GetScene()->RenderToFrameBuffer(renderer, map.first, framebuffer, map.second, false);
                GetScene()->RenderToFrameBuffer(renderer, map.first, gameFramebuffer, map.second, true);
		    }

            //Render Material Editor MiniScene
            materialMakerScene->ClearFrameBufferToColor(renderer, materialMakerBuffer, {0.3f, 0.3f, 0.3f});
		    auto material = editorManager.GetCurrentMaterial();
		    if (material != nullptr) {
		        std::vector<RPG::GameObjectMaterialGroup> materialGroup;
		        for (auto go : materialMakerScene->GetHierarchy()->GetHierarchy()) {
		            if (go->IsRenderable()) {
                        RPG::GameObjectMaterialGroup group;
                        group.gameObject = go;
                        group.material = material;
                        materialGroup.push_back(group);
		            }
		        }
                materialMakerScene->RenderToFrameBuffer(renderer, RPG::Assets::GetPipelineByName(material->GetShader()), materialMakerBuffer, materialGroup, true);
            }



            //Depth Buffer for Lighting
            /*GetScene()->RenderToDepthBuffer(renderer, depthBuffer);
            //Scene Render
		    GetScene()->RenderToFrameBuffer(renderer, framebuffer, {0.3f, 0.3f, 0.3f}, depthBuffer->GetRenderTextureID(), false);*/

            //Rendering and Sending the Editor
            editorManager.NewFrame(window);
			//TODO: Scene and game framebuffer should follow similar process to the material maker submit
			editorManager.SubmitMaterialMakerFrameBuffer(materialMakerBuffer);
			editorManager.BuildGUI(framebuffer, gameFramebuffer, GetScene()->GetHierarchy());
			editorManager.Render();
        #else
			//Rendering for Release
            glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            //Organize Scene for Rendering
            BuildRenderOrder();
            GetScene()->ClearFrameBufferToColor(renderer, framebuffer, {0.0f, 0.0f, 0.0f});

            //Render through scene once for Scene and Once for Game view
            for (auto map : mappedRenderPass) {
                GetScene()->RenderToFrameBuffer(renderer, map.first, framebuffer, map.second, false);
            }

           renderer.DisplayFrameBuffer(RPG::Assets::Pipeline::DefaultReleaseFrameBuffer, framebuffer, quadVAO);
		#endif

		SDL_GL_SwapWindow(window.GetWindow());
	}

	void Update(const float& delta) {
		std::string s = "RPG-Engine | FPS: " + std::to_string(RPG::ApplicationStats::GetInstance().GetFPS());
		SDL_SetWindowTitle(window.GetWindow(), s.c_str());
		#ifdef RPG_EDITOR
			if (editorManager.IsGameRunning() || !hasRanPreviewFrame) {
				if (hasRanPreviewFrame && !hasRanFirstFrame) {
					hasRanFirstFrame = true;
					GetScene()->Awake();
					GetScene()->Start();
				}
				hasRanPreviewFrame = true;
				GetScene()->Update(delta);
			} else {
				hasRanFirstFrame = false;
				GetScene()->UpdateEditorScene(delta);
			}
		#else
			if (!hasRanFirstFrame) {
				hasRanFirstFrame = true;
				GetScene()->Awake();
				GetScene()->Start();
			}
			GetScene()->Update(delta);
		#endif

	}

	void OnWindowResized() {
		GetScene()->OnWindowResized(RPG::SDL::GetWindowSize(window.GetWindow()));
		::UpdateViewport(window.GetWindow());

		#ifdef RPG_EDITOR
			::ResizeFrameBuffer(framebuffer, RPG::ApplicationStats::GetInstance().GetWindowSize());
			::ResizeFrameBuffer(gameFramebuffer, RPG::ApplicationStats::GetInstance().GetWindowSize());
			hasRanPreviewFrame = false;
		#endif
	}

	void OnGeneralEventData(SDL_Event event) {
		#ifdef RPG_EDITOR
			editorManager.OnGeneralEventData(event);
		#endif
	}

	std::shared_ptr<RPG::IScene> GetScene() {
		auto scene = RPG::SceneManager::GetInstance().GetCurrentScene();
		if (!scene->HasLoaded()) {
			RPG::Log("Application", "Scene hasnt loaded yet");
			assetManager->LoadAssetManifest(scene->GetAssetManifest());
			#ifdef RPG_EDITOR
				hasRanPreviewFrame = false;
            #else
				hasRanFirstFrame = false;
            #endif
		}
		return scene;
	}

	glm::vec2 GetWindowPosition() {
		int windowX, windowY;
		SDL_GetWindowPosition(window.GetWindow(), &windowX, &windowY);
		return {windowX, windowY};
	}

	void BuildRenderOrder() {
        mappedRenderPass.clear();

        for (auto gameObject : GetScene()->GetHierarchy()->GetHierarchy()) {
            CheckGameObjectForBuildOrder(gameObject);
        }
    }

    void CheckGameObjectForBuildOrder(std::shared_ptr<RPG::GameObject> gameObject) {
        for (auto childGameObject : gameObject->GetChildren()) {
            CheckGameObjectForBuildOrder(childGameObject);
        }

        if (gameObject->IsRenderable()) {
            std::string materialPath = gameObject->GetMaterial();
            std::shared_ptr<RPG::Material> material = assetManager->GetMaterial(materialPath);
            RPG::Assets::Pipeline pipeline = RPG::Assets::GetPipelineByName(material->GetShader());
            RPG::GameObjectMaterialGroup group;
            group.gameObject = gameObject;
            group.material = material;
            //TODO: Add in a dot product to determine distance cal


            int priority = group.material->GetRenderQueue();
            bool foundLocation = false;
            if (mappedRenderPass.count(pipeline) != 0) {
                auto array = mappedRenderPass.at(pipeline);
                for (int i = 0; i < array.size(); i++) {
                    auto mat = array[i].material;
                    //Determine if material falls under which render queue type since sorting is different for opaque and transparent
                    if (mat->GetRenderQueue() > priority || mat->GetName() == group.material->GetName()) {
                        array.insert(array.begin() + i, group);
                        foundLocation = true;
                        break;
                    }
                }
                mappedRenderPass.at(pipeline) = array;
            }

            if (!foundLocation) {
                std::vector<GameObjectMaterialGroup> vec;
                vec.push_back(group);
                mappedRenderPass.insert({pipeline, vec});
            }
        }
    }

	~Internal() {

		#ifdef RPG_EDITOR
		    //TODO: Convert this to use ::DeleteFrameBuffer(frameBuffer) instead of feeding it through a pipeline for no reason
			renderer.DeleteFrameBuffer(RPG::Assets::Pipeline::Default, framebuffer);
            renderer.DeleteFrameBuffer(RPG::Assets::Pipeline::Default, gameFramebuffer);
            renderer.DeleteFrameBuffer(RPG::Assets::Pipeline::Default, depthBuffer);
            renderer.DeleteFrameBuffer(RPG::Assets::Pipeline::Default, materialMakerBuffer);
        #else
            glDeleteVertexArrays(1, &quadVAO);
            glDeleteBuffers(1, &quadVBO);
            renderer.DeleteFrameBuffer(RPG::Assets::Pipeline::Default, framebuffer);
        #endif

        SDL_GL_DeleteContext(context);
	}
};

OpenGLApplication::OpenGLApplication() : internal(RPG::MakeInternalPointer<Internal>()) {}

void OpenGLApplication::Render() {
	internal->Render();
}

void OpenGLApplication::Update(const float& delta) {
	internal->Update(delta);
}

void OpenGLApplication::OnWindowResized() {
	internal->OnWindowResized();
}

void OpenGLApplication::OnGeneralEventData(SDL_Event event) {
	internal->OnGeneralEventData(event);
}

glm::vec2 OpenGLApplication::GetWindowPosition() {
	return internal->GetWindowPosition();
}
