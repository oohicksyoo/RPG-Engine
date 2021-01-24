//
// Created by Alex on 1/23/2021.
//

#pragma once
#include "../IComponent.hpp"
#include "../InternalPointer.hpp"
#include "../Guid.hpp"
#include "../GLMWrapper.hpp"

namespace RPG {
	struct BoxColliderComponent : public IComponent {
		public:
			BoxColliderComponent(glm::vec3 size, bool isTrigger, std::string guid = RPG::Guid::GenerateGuid());
			void Awake() override;
			void Start() override;
			void Update(const float &delta) override;
			std::string Name() { return "BoxColliderComponent";};
			std::vector<std::shared_ptr<RPG::Property>> GetProperties() override;
			std::string Guid() override;
			bool AllowMultiple() { return false; };
			bool IsTrigger();
			glm::vec3 GetSize();

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


