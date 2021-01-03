//
// Created by Alex on 12/27/2020.
//

#pragma once

#include "../IComponent.hpp"
#include "../InternalPointer.hpp"

namespace RPG {
	enum class CameraType {
		Perspective = 0,
		Orthographic
	};

	struct CameraComponent : public IComponent {
		public:
			CameraComponent();
			void Awake() override;
			void Start() override;
			void Update(const float &delta) override;
			std::string Name() { return "CameraComponent";};
			std::vector<std::shared_ptr<RPG::Property>> GetProperties() override;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


