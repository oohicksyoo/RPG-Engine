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
			MeshComponent(RPG::Assets::StaticMesh mesh, RPG::Assets::Texture texture, std::string guid = RPG::Guid::GenerateGuid());
			void Awake() override;
			void Start() override;
			void Update(const float &delta) override;
			std::string Name() { return "MeshComponent";};
			std::vector<std::shared_ptr<RPG::Property>> GetProperties() override;
			std::string Guid() override;
			bool AllowMultiple() { return false; };
			RPG::Assets::StaticMesh GetMesh();
			RPG::Assets::Texture GetTexture();

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


