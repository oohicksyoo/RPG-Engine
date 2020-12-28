//
// Created by Alex on 12/27/2020.
//

#pragma once

#include <string>

namespace RPG {
	struct IComponent {
		public:
			IComponent() {};
			virtual void Awake() = 0;
			virtual void Start() = 0;
			virtual void Update() = 0;
			virtual std::string Name() = 0;
	};
}
