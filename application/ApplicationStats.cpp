//
// Created by Alex on 12/27/2020.
//

#include "ApplicationStats.hpp"

using RPG::ApplicationStats;

ApplicationStats::ApplicationStats() {

}

ApplicationStats::~ApplicationStats() {

}

glm::vec2 ApplicationStats::GetWindowSize() {
	return (windowSize.x != 0 && windowSize.y != 0) ? windowSize : GetRenderingSize();
}

void ApplicationStats::SetWindowSize(glm::vec2 size) {
	windowSize = size;
}

glm::vec2 ApplicationStats::GetRenderingSize() {
	return glm::vec2{1280, 720};
}