//#define _CRTDBG_MAP_ALLOC
//#include <crtdbg.h>
//#include <vld.h>

#include "config.h"
#include "src/System/Engine.h"

#define TRACY_ENABLE

#include "../src/Profiler/tracy/public/tracy/Tracy.hpp"
#include "../src/Profiler/tracy/public/tracy/TracyOpenGL.hpp"



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {

	Engine engine;

    engine.init();
    engine.run();
    engine.shutdown();

    return 0;

}
