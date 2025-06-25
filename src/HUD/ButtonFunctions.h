#pragma once

#include "../config.h"

void MenuButton();
void NickButton();
void LeaderboardButton();
void ReturnButton();
void ExitButton();
void CheckboxButton();
void ReturnToMenuButton();
void ReRunButton();

class FunctionRegister {
public:
	static FunctionRegister& Instance() {
		static FunctionRegister instance;
		return instance;
	}

    std::map<std::string, std::function<void()>> functionMap = {
	{"MenuButton", MenuButton},
	{"NickButton", NickButton},
	{"LeaderboardButton", LeaderboardButton},
	{"ReturnButton", ReturnButton},
	{"ExitButton", ExitButton},
	{"CheckboxButton", CheckboxButton},
	{"ReturnToMenuButton", ReturnToMenuButton},
	{"ReRunButton", ReRunButton}
    };
};




