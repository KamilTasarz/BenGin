#include "ButtonFunctions.h"
#include "../System/SceneManager.h"
#include "../Gameplay/MusicManager.h"
#include "../Basic/Node.h"
#include "../Gameplay/KeyboardWrite.h"

void MenuButton()
{
    SceneManager::Instance().next();
    MusicManager::instance().StartGameTransition();
    std::cout << "You clicked button in Menu." << std::endl;
}

void NickButton()
{
    SceneGraph* s = SceneManager::Instance().getCurrentScene();
    if (s) {
        Node* keywriter = s->root->getChildByName("keyboard_controller");
        if (keywriter) {
			KeyboardWrite* script = keywriter->getComponent<KeyboardWrite>();
			if (script) {
                script->isActive = true;
			}
			
		}

    
    }
}
