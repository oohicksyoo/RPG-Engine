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
			Property(std::any property, std::string propertyName, std::string propertyType, bool propertyAllowsDragAndDrop = false, std::string propertyDragAndDropTag = "") {
				name = propertyName;
				type = propertyType;
				guid = RPG::Guid::GenerateGuid();
				allowsDragAndDrop = propertyAllowsDragAndDrop;
				dragAndDropTag = propertyDragAndDropTag;
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
			bool AllowsDragAndDrop() {
				return allowsDragAndDrop;
			}
			std::string DragAndDropTag() {
				return dragAndDropTag;
			}

		private:
			std::any prop;
			std::string name;
			std::string type;
			std::string guid;
			bool allowsDragAndDrop;
			std::string dragAndDropTag;
	};
}


