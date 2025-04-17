#pragma once

#include "Log.h"

#ifdef BENGIN

extern Application* createApplication();

int main(int argc, char** argv) {

	Log::initialize();
	int a = 5;
	LOG_BENGIN_TRACE("Trace Var={0}", a);
	LOG_BENGIN_DEBUG("Debug");
	LOG_BENGIN_INFORMATION("Info");
	LOG_BENGIN_WARNING("Warning");
	LOG_BENGIN_ERROR("Error");
	LOG_BENGIN_CRITICAL("Critical");

	auto app = createApplication();

	app->run();

	delete app;

}

#endif // BENGIN
