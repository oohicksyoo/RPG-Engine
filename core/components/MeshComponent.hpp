//
// Created by Alex on 1/1/2021.
//

#pragma once

#include "../IComponent.hpp"
#include "../InternalPointer.hpp"
#include "../AssetInventory.hpp"

namespace RPG {
	struct MeshComponent : public IComponent {
		public:
			MeshComponent(RPG::Assets::StaticMesh mesh, RPG::Assets::Texture texture);
			void Awake() override;
			void Start() override;
			void Update() override;
			std::string Name() { return "MeshComponent";};
			std::vector<std::shared_ptr<RPG::Property>> GetProperties() override;
			RPG::Assets::StaticMesh GetMesh();
			RPG::Assets::Texture GetTexture();

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


