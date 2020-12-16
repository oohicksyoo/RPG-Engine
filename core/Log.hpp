//
// Created by Alex on 12/13/2020.
//

#pragma once

#include <string>
#include <iostream>
//#include <windows.h>

namespace RPG {
	void Log(const std::string& tag, const std::string& message);
	void Log(const std::string& tag, const std::string& message, const std::exception& error);

	//Logging Colours
	/*inline std::ostream& Blue(std::ostream& s) {
		HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE
										 | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		return s;
	}

	inline std::ostream& White(std::ostream& s) {
		HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hStdout,
								FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		return s;
	}*/
}


