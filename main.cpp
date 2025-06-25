//#define _CRTDBG_MAP_ALLOC
//#include <crtdbg.h>
//#include <vld.h>

#include "config.h"
#include "src/System/Engine.h"

int main() {

	Engine engine;

    engine.init();
    engine.run();
    engine.shutdown();

    return 0;

}
