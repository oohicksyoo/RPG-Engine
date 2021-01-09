//
// Created by Alex on 1/5/2021.
//

#pragma once
#include "IScene.hpp"
#include "InternalPointer.hpp"
#include "WindowSize.hpp"
#include "FrameBuffer.hpp"
#include "Hierarchy.hpp"

namespace RPG {
	struct Scene : public IScene {
		public:
			Scene(const RPG::WindowSize& frameSize, std::string guid = RPG::Guid::GenerateGuid());
			RPG::AssetManifest GetAssetManifest() override;
			void Prepare() override;
			void Awake() override;
			void Start() override;
			void Update(const float& delta) override;
			void Render(RPG::IRenderer& renderer) override;
			void RenderToFrameBuffer(RPG::IRenderer& renderer, std::shared_ptr<RPG::FrameBuffer> frameBuffer, glm::vec3 clearColor) override;
			void OnWindowResized(const RPG::WindowSize& size) override;
			std::shared_ptr<RPG::Hierarchy> GetHierarchy() override;
			std::string GetGuid() override;
			bool HasLoaded() override;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


