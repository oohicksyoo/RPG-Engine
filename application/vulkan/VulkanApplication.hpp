//
// Created by Alex on 12/14/2020.
//

#pragma once

#include "../../core/InternalPointer.hpp"
#include "../Application.hpp"

namespace RPG {
	struct VulkanApplication : public RPG::Application {
		public:
			VulkanApplication();
			void Update(const float& delta) override;
			void Render() override;
			void OnWindowResized() override;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


