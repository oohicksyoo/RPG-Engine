//
// Created by Alex on 12/28/2020.
//

#pragma once

#include <string>
#include <random>
#include <sstream>

namespace RPG {
	struct Guid {
		public:
			static std::string GenerateGuid() {
				std::random_device randomDevice;
				std::mt19937_64 generator(randomDevice());
				std::uniform_int_distribution<> distribution(0, 15);

				std::stringstream stringstream;
				stringstream << std::hex;
				for (int i = 0; i < 8; ++i) {
					stringstream << distribution(generator);
				}
				stringstream << "-";
				for (int i = 0; i < 4; ++i) {
					stringstream << distribution(generator);
				}
				stringstream << "-";
				for (int i = 0; i < 4; ++i) {
					stringstream << distribution(generator);
				}
				stringstream << "-";
				for (int i = 0; i < 4; ++i) {
					stringstream << distribution(generator);
				}
				stringstream << "-";
				for (int i = 0; i < 12; ++i) {
					stringstream << distribution(generator);
				}

				//8-4-4-4-12 //Total 36 characters 32-Hex and 4-Hyphens
				//"11111111-1111-1111-1111-1111-111111111111"
				return stringstream.str();
			}
	};
}
