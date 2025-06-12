
#include "config.h"
#include "src/System/Engine.h"


// Audio paths
std::string track1 = "res/audios/music/kill-v-maim.ogg";
std::string track2 = "res/audios/music/burning-bright.ogg";

std::string sound_effect = "res/audios/sounds/bonk.ogg";

float pauseTimer = 0.0f;
float pauseCooldown = 0.5f; // Half second cooldown


int main() {

	// --- ENGINE INIT --- //

	Engine engine;

    engine.init();
    engine.run();
    engine.shutdown();

    return 0;
}
