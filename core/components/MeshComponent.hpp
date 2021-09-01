//
// Created by Alex on 1/1/2021.
//

#pragma once

#include "../IComponent.hpp"
#include "../InternalPointer.hpp"
#include "../AssetInventory.hpp"
#include "../IRenderable.hpp"
#include <string>

namespace RPG {
	struct MeshComponent : public IComponent, public IRenderable {
		public:
			MeshComponent(std::string mesh, std::string material, std::string guid = RPG::Guid::GenerateGuid());
			void Awake() override;
			void Start() override;
			void Update(const float &delta) override;
			std::string Name() { return "MeshComponent";};
			std::vector<std::shared_ptr<RPG::Property>> GetProperties() override;
			std::string Guid() override;
			bool AllowMultiple() { return false; };
			std::string GetMesh() override;
			std::string GetMaterial();

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


