//
// Created by Alex on 12/27/2020.
//

#pragma once

#include "../IComponent.hpp"
#include "../InternalPointer.hpp"
#include <string>

namespace RPG {
	struct TransformComponent : public IComponent {
		public:
			TransformComponent();
			virtual void Awake() override;
			virtual void Start() override;
			virtual void Update() override;
			virtual std::string Name() { return "TransformComponent";};

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


