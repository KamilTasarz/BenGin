#pragma once

#include "../config.h"

void MenuButton();
void NickButton();

class FunctionRegister {
public:
	static FunctionRegister& Instance() {
		static FunctionRegister instance;
		return instance;
	}

    std::map<std::string, std::function<void()>> functionMap = {
	{"MenuButton", MenuButton},
	{"NickButton", NickButton}
    };
};




