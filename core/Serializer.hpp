//
// Created by Alex on 1/5/2021.
//

#pragma once

#include "IScene.hpp"

namespace RPG::Serializer {
	void SaveScene(std::unique_ptr<RPG::IScene> scene);
	std::unique_ptr<RPG::IScene> LoadScene(const RPG::WindowSize& frameSize);
}


