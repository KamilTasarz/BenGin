#pragma once

#include "../config.h"

void MenuButton();


class FunctionRegister {
public:
	static FunctionRegister& Instance() {
		static FunctionRegister instance;
		return instance;
	}

    std::map<std::string, std::function<void()>> functionMap = {
    {"MenuButton", MenuButton}
    };
};




