//
// Created by Alex on 12/27/2020.
//

#pragma once

#include "../IComponent.hpp"
#include "../InternalPointer.hpp"
#include "../GLMWrapper.hpp"
#include "../Action.hpp"

namespace RPG {
	struct TransformComponent : public IComponent {
		public:
			TransformComponent(std::string guid = RPG::Guid::GenerateGuid());
			void Awake() override;
			void Start() override;
			void Update(const float &delta) override;
			std::string Name() { return "TransformComponent";};
			std::vector<std::shared_ptr<RPG::Property>> GetProperties() override;
			std::string Guid() override;
			bool AllowMultiple() { return false; };
			glm::mat4 GetTransformMatrix();
			void SetGetParent(RPG::Action<>::Func<std::shared_ptr<RPG::TransformComponent>> getParentFunc);
			void SetPosition(glm::vec3 position);
			glm::vec3 GetPosition();
			glm::vec3 GetWorldPosition();
			void SetWorldPosition(glm::vec3 value);
			void SetRotation(glm::vec3 rotation);
			glm::vec3 GetRotation();
			void SetScale(glm::vec3 scale);
			glm::vec3 GetScale();

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


