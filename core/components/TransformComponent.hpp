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
			TransformComponent();
			void Awake() override;
			void Start() override;
			void Update() override;
			std::string Name() { return "TransformComponent";};
			std::vector<std::shared_ptr<RPG::Property>> GetProperties() override;
			glm::mat4 GetTransformMatrix();
			void SetGetParent(RPG::Action<>::Func<std::shared_ptr<RPG::TransformComponent>> getParentFunc);

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


