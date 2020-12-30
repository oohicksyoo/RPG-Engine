//
// Created by Alex on 12/27/2020.
//

#pragma once

#include "../IComponent.hpp"
#include "../InternalPointer.hpp"
#include <string>

namespace RPG {
	struct CameraComponent : public IComponent {
		public:
			CameraComponent();
			virtual void Awake() override;
			virtual void Start() override;
			virtual void Update() override;
			virtual std::string Name() { return "CameraComponent";};

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


