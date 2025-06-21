#include "config.h"
#include "src/System/Engine.h"

int main() {

	// --- ENGINE INIT --- //

	Engine engine;

    engine.init();
    engine.run();
    engine.shutdown();

    return 0;

}
