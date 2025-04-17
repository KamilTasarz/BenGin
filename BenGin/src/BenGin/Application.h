#pragma once

#include "Core.h"

class BENGIN_API Application {

public:

	Application();
	virtual ~Application();

	void run();

};

// To be defined in client (BenDitor defines itself)
Application* createApplication();
