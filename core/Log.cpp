//
// Created by Alex on 12/13/2020.
//

#include "Log.hpp"
#include <iostream>

void RPG::Log(const std::string &tag, const std::string &message) {
	#ifndef NDEBUG
		std::cout << Blue << "[" << tag << "] " << White << message << std::endl;
	#endif
}

void RPG::Log(const std::string &tag, const std::string &message, const std::exception &error) {
	#ifndef NDEBUG
		std::string output = message + " Exception message was " + std::string{error.what()};
		RPG::Log(tag, output);
	#endif
}

