//
// Created by Alex on 12/27/2020.
//

#pragma once

#include "../IComponent.hpp"
#include "../InternalPointer.hpp"
#include "../GLMWrapper.hpp"
#include "../components/TransformComponent.hpp"

namespace RPG {
	enum class CameraType {
		Perspective = 0,
		Orthographic
	};

	struct CameraComponent : public IComponent {
		public:
			CameraComponent(float width, float height, std::shared_ptr<RPG::TransformComponent> transformComponent, std::string guid = RPG::Guid::GenerateGuid());
			void Awake() override;
			void Start() override;
			void Update(const float &delta) override;
			std::string Name() { return "CameraComponent";};
			std::vector<std::shared_ptr<RPG::Property>> GetProperties() override;
			std::string Guid() override;
			bool AllowMultiple() { return false; };
			glm::mat4 GetProjectionMatrix() const;
			glm::mat4 GetViewMatrix() const;
			void Pan(glm::vec2 delta);
			void Rotate(glm::vec2 delta);
			void Zoom(float delta);
			void SetDistance(float value);
			void SetIsMainCamera(bool value);
			void SetYaw(float value);
			void SetPitch(float value);

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


