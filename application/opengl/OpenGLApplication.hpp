//
// Created by Alex on 12/13/2020.
//

#pragma once

#include "../../core/InternalPointer.hpp"
#include "../Application.hpp"

namespace RPG {
	struct OpenGLApplication : public RPG::Application {
		public:
			OpenGLApplication();
			void Render() override;
			void Update(const float& delta) override;
			void OnWindowResized() override;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


