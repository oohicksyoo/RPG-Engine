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
}

struct OpenGLApplication::Internal {
	const RPG::SDLWindow window;
	SDL_GLContext context;
	const std::shared_ptr<RPG::OpenGLAssetManager> assetManager;
	RPG::OpenGLRenderer renderer;
	std::unique_ptr<RPG::IScene> scene;

	Internal() : window(RPG::SDLWindow(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI)),
				 context(::CreateContext(window.GetWindow())),
				 assetManager(::CreateAssetManager()),
				 renderer(::CreateRenderer(assetManager)) {}

	void Render() {
		SDL_GL_MakeCurrent(window.GetWindow(), context);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GetScene().Render(renderer);

		SDL_GL_SwapWindow(window.GetWindow());
	}

	void Update(const float& delta) {
		GetScene().Update(delta);
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
