//
// Created by Alex on 12/13/2020.
//

#include "OpenGLApplication.hpp"
#include "../../core/GraphicsWrapper.hpp"
#include "../../core/Log.hpp"
#include "../../core/SDLWindow.hpp"
#include "../../../project/SceneMain.hpp"
#include "OpenGLAssetManager.hpp"
#include "OpenGLRenderer.hpp"
#include <string>

#include "../../../editor/EditorManager.hpp"
#include "../../core/FrameBuffer.hpp"

using RPG::OpenGLApplication;

namespace {
	void UpdateViewport(SDL_Window* window) {
		static const std::string logTag{ "RPG::OpenGLApplication::UpdateViewport" };

		int viewportWidth;
		int viewportHeight;
		SDL_GL_GetDrawableSize(window, &viewportWidth, &viewportHeight);
		RPG::Log(logTag, "Created OpenGL context with viewport size: " + std::to_string(viewportWidth) + " x " + std::to_string(viewportHeight));

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

	std::unique_ptr<RPG::IScene> CreateMainScene(const RPG::SDLWindow& window, RPG::OpenGLAssetManager& assetManager) {
		std::unique_ptr<RPG::IScene> scene{std::make_unique<RPG::SceneMain>(RPG::SDL::GetWindowSize(window.GetWindow()))};
		assetManager.LoadAssetManifest(scene->GetAssetManifest());
		scene->Prepare();

		return scene;
	}

	RPG::FrameBuffer CreateFrameBuffer(glm::vec2 size) {
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

		//Resize
		//Resize Texture
		glBindTexture(GL_TEXTURE_2D, renderTextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		//Setup depth-stencil buffer
		glBindRenderbuffer(GL_RENDERBUFFER, depthStencilBufferID);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_STENCIL, size.x, size.y);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		//Attach depth and stencil buffer to the framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, bufferID);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthStencilBufferID);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthStencilBufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
			RPG::Log("Framebuffer", "Framebuffer was created and completed");
		}

		return RPG::FrameBuffer{bufferID, renderTextureID, depthStencilBufferID};
	}
}

struct OpenGLApplication::Internal {
	bool isRunning;
	bool hasRanFirstFrame = false;

	const RPG::SDLWindow window;
	SDL_GLContext context;
	const std::shared_ptr<RPG::OpenGLAssetManager> assetManager;
	RPG::OpenGLRenderer renderer;
	std::unique_ptr<RPG::IScene> scene;
	#ifdef RPG_DEBUG
		RPG::EditorManager editorManager;
		RPG::FrameBuffer framebuffer;
	#endif

	#ifdef RPG_DEBUG
		Internal() : window(RPG::SDLWindow(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI)),
					 context(::CreateContext(window.GetWindow())),
					 assetManager(::CreateAssetManager()),
					 renderer(::CreateRenderer(assetManager)),
					 editorManager(window, context),
					 framebuffer(::CreateFrameBuffer(glm::vec2{1280, 720})),
					 isRunning(false){}
	#else
		Internal() : window(RPG::SDLWindow(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI)),
					 context(::CreateContext(window.GetWindow())),
					 assetManager(::CreateAssetManager()),
					 renderer(::CreateRenderer(assetManager)),
					 isRunning(true) {}
	#endif

	void Render() {
		SDL_GL_MakeCurrent(window.GetWindow(), context);

		#ifdef RPG_DEBUG
			GetScene().RenderToFrameBuffer(renderer, framebuffer);
		#endif

		#ifndef RPG_DEBUG
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		#endif

		#ifdef RPG_DEBUG
			editorManager.NewFrame(window);
		#endif

		#ifndef RPG_DEBUG
			GetScene().Render(renderer);
		#endif

		#ifdef RPG_DEBUG
			//RPG::Log("FrameBuffer", "Framebuffer ID: " + std::to_string(framebuffer.GetRenderTextureID()));
			editorManager.BuildGUI();
			editorManager.Render();
		#endif

		SDL_GL_SwapWindow(window.GetWindow());
	}

	void Update(const float& delta) {
		if (isRunning || !hasRanFirstFrame) {
			hasRanFirstFrame = true;
			GetScene().Update(delta);
		}
	}

	void OnWindowResized() {
		GetScene().OnWindowResized(RPG::SDL::GetWindowSize(window.GetWindow()));
		::UpdateViewport(window.GetWindow());
	}

	RPG::IScene& GetScene() {
		if (!scene) {
			scene = ::CreateMainScene(window, *assetManager);
		}
		return *scene;
	}

	~Internal() {
		SDL_GL_DeleteContext(context);

		#ifdef RPG_DEBUG
			renderer.DeleteFrameBuffer(RPG::Assets::Pipeline::Default, framebuffer);
		#endif
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
