//
// Created by Alex on 12/13/2020.
//

#include "Log.hpp"
#include <iostream>

#ifdef RPG_EDITOR
	#include "../../editor/EditorStats.hpp"
	#include "../../editor/LogFile.hpp"
#endif

void RPG::Log(const std::string &tag, const std::string &message) {
	#ifndef NDEBUG
		//std::cout << Blue << "[" << tag << "] " << White << message << std::endl;
		std::cout << "[" << tag << "] " << message << std::endl;

		RPG::LogFile log = RPG::LogFile();
		log.type = RPG::LogFileType::NormalLog;
		log.caller = tag;
		log.message = message;
		log.isLua = false;
		RPG::EditorStats::GetInstance().AddLog(log);
	#endif
}

void RPG::Log(const std::string &tag, const std::string &message, const std::exception &error) {
	#ifndef NDEBUG
		std::string output = message + " Exception message was " + std::string{error.what()};
		RPG::Log(tag, output);
	#endif
}

