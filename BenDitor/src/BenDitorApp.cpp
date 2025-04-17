#include <BenGin.h>

class BenDitorApp : public Application {

public:

	BenDitorApp() {
	
	}
	~BenDitorApp() {
	
	}

};

Application* createApplication() {
	return new BenDitorApp();
}
