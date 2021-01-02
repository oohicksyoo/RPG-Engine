//
// Created by Alex on 12/27/2020.
//

#pragma once

#include "InternalPointer.hpp"
#include "IComponent.hpp"
#include "GLMWrapper.hpp"
#include "components/TransformComponent.hpp"
#include <string>
#include <memory>
#include <vector>
#include <typeinfo>

namespace RPG {
	struct GameObject {
		public:
			GameObject(std::string name = "GameObject");
			std::shared_ptr<RPG::IComponent> AddComponent(std::shared_ptr<RPG::IComponent> component);
			std::vector<std::shared_ptr<RPG::IComponent>> GetComponents();
			std::string GetGuid();
			std::string GetName();
			void SetName(std::string name);
			std::vector<std::shared_ptr<RPG::GameObject>> GetChildren();
			bool HasChildren();
			bool AddChild(std::shared_ptr<RPG::GameObject> gameObject);
			bool RemoveChild(std::shared_ptr<RPG::GameObject> gameObject);
			void SetParent(std::shared_ptr<RPG::GameObject> gameObject, std::shared_ptr<RPG::GameObject> parent);
			bool HasParent();
			std::shared_ptr<RPG::TransformComponent> GetTransform();

			template<typename T, typename U>
			T GetComponent(std::shared_ptr<RPG::IComponent> component);

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};

	template<typename T, typename U>
	T GameObject::GetComponent(std::shared_ptr<RPG::IComponent> component) {
		//TODO: Garbage way of grabbing this, replace with a better method of comparing based on generics
		for (std::shared_ptr<RPG::IComponent> c : GetComponents()) {
			if (component->Name() == c->Name()) {
				return std::dynamic_pointer_cast<U>(c);
			}
		}

		return nullptr;
	}
}


