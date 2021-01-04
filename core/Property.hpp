//
// Created by Alex on 12/31/2020.
//

#pragma once

#include "InternalPointer.hpp"
#include "Guid.hpp"
#include <string>
#include <any>

namespace RPG {
	struct Property {
		public:
			Property(std::any property, std::string propertyName, std::string propertyType) {
				name = propertyName;
				type = propertyType;
				guid = RPG::Guid::GenerateGuid();
				SetProperty(property);
			};
			void SetProperty(std::any property);
			std::any GetProperty();
			std::string GetName();
			std::string GetType();
			std::string GetGuid() {
				return guid;
			}
			std::string GetEditorName() {
				return GetName() + "##" + GetGuid();
			}

		private:
			std::any prop;
			std::string name;
			std::string type;
			std::string guid;
	};
}


