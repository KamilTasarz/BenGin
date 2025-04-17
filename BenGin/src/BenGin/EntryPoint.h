#pragma once

#ifdef BENGIN

extern Application* createApplication();

int main(int argc, char** argv) {

	auto app = createApplication();

	app->run();

	delete app;

}

#endif // BENGIN
