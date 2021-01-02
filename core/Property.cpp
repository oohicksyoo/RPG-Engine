//
// Created by Alex on 1/1/2021.
//

#include "Property.hpp"

using RPG::Property;

void Property::SetProperty(std::any property) {
	prop = property;
}

std::any Property::GetProperty() {
	return prop;
}

std::string Property::GetName() {
	return name;
}

std::string Property::GetType() {
	return type;
}