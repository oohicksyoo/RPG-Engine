//
// Created by Alex on 12/27/2020.
//

#pragma once

#include "../core/Singleton.hpp"
#include "../core/GLMWrapper.hpp"

namespace RPG {
	struct ApplicationStats : public Singleton<ApplicationStats> {
		friend struct Singleton<ApplicationStats>;

		public:
			ApplicationStats();
			~ApplicationStats();
			glm::vec2 GetWindowSize();
			void SetWindowSize(glm::vec2 size);
			glm::vec2 GetRenderingSize();

		private:
			glm::vec2 windowSize;
	};
}


