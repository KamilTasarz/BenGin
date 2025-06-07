/*
* 
*  KAMIL PILNUJ TEGO
* 
snake_case - zmienne
camelCase - metody
PascalCase - klasy/struktury

*/
#include "config.h"
#include "src/System/Engine.h"


// Audio paths
std::string track1 = "res/audios/music/kill-v-maim.ogg";
std::string track2 = "res/audios/music/burning-bright.ogg";

std::string sound_effect = "res/audios/sounds/bonk.ogg";

float pauseTimer = 0.0f;
float pauseCooldown = 0.5f; // Half second cooldown

//Text* text;
//Background* background;
Sprite *sprite, *sprite2, *sprite3, *icon, *eye_icon, *eye_slashed_icon;



int main() {

    /*text = new Text("res/fonts/arial.ttf");
    background = new Background(1920.f, 1080.f, "res/textures/sky.png", 200.f);

    const char* sprites[] = {"res/sprites/ghostFlying1.png", "res/sprites/ghostFlying2.png",
                             "res/sprites/ghostFlying3.png", "res/sprites/ghostFlying4.png",
                             "res/sprites/ghostFlying5.png", "res/sprites/ghostFlying6.png" };

    
    sprite = new AnimatedSprite(1920.f, 1080.f, 1.f, sprites, 6, 100.f, 300.f);
    sprite3 = new AnimatedSprite(1920.f, 1080.f, 1.f, "res/sprites/piratWalking.png", 1, 9, 9, 300.f, 300.f);
    sprite2 = new Sprite(1920.f, 1080.f, "res/sprites/heart2.png", 700.f, 100.f, 0.1f);
	icon = new Sprite(1920.f, 1080.f, "res/sprites/icon.png", 0.f, 0.f, 1.f);
	eye_icon = new Sprite(1920.f, 1080.f, "res/sprites/eye.png", 0.f, 0.f, 1.f);
	eye_slashed_icon = new Sprite(1920.f, 1080.f, "res/sprites/eye_slashed.png", 0.f, 0.f, 1.f);

    // --- AUDIO INIT --- //
    // Initialize our audio engine
    CAudioEngine audioEngine;
    audioEngine.Init();

    // Load two music tracks and one sound effect
    audioEngine.LoadSound(track1, true, true, true);
    audioEngine.LoadSound(track2, true, true, true);
    audioEngine.LoadSound(sound_effect, false, false, true);

    int current_track_id = 0;//audioEngine.PlayMusic(track1, Vector3{ 0.0f }, -10.0);
    bool paused = false;

    // When we want to call PlayMusic and don't care about the channel number
    int useless_garbage;*/

    // --- //

	// --- ENGINE INIT --- //

	Engine engine;

    engine.init();
    engine.run();
    engine.shutdown();

    return 0;
}
