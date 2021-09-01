//
// Created by Alex on 1/4/2021.
//

#pragma once

#include "../IComponent.hpp"
#include "../InternalPointer.hpp"
#include "../AssetInventory.hpp"
#include "../IRenderable.hpp"

namespace RPG {
	struct SpriteComponent : public IComponent, public IRenderable {
		public:
			SpriteComponent(std::string texture, std::string guid = RPG::Guid::GenerateGuid());
			void Awake() override;
			void Start() override;
			void Update(const float &delta) override;
			std::string Name() { return "SpriteComponent";};
			std::vector<std::shared_ptr<RPG::Property>> GetProperties() override;
			std::string Guid() override;
			bool AllowMultiple() { return false; };
			std::string GetMesh() override;
			std::string GetTexture();

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


