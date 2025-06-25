#include "ButtonFunctions.h"
#include "../System/SceneManager.h"
#include "../Gameplay/MusicManager.h"
#include "../Basic/Node.h"
#include "../Gameplay/KeyboardWrite.h"
#include "../Gameplay/AnimatedMenu.h"
#include "../Gameplay/GameManager.h"
#include "../System/ServiceLocator.h"

void MenuButton()
{
	SceneGraph* s = SceneManager::Instance().getCurrentScene();
	
    //SceneManager::Instance().next();
    //MusicManager::instance().StartGameTransition();
    //std::cout << "You clicked button in Menu." << std::endl;
    if (s) {
        s->alpha_anim = 0.f; // Reset alpha to start moving back
        s->menu_anim = true; // Set menu animation flag to true
        Node* keywriter = s->root->getChildByName("keyboard_controller");
        if (keywriter) {
            AnimatedMenu* script = keywriter->getComponent<AnimatedMenu>();
            if (script) {
                script->endAnim = true;
				script->endAnimTimer = 0.f; // Reset end animation timer
                script->reverse = false;
                script->starting = true;
            }

        }


    }
}

void ReRunButton()
{
    SceneManager::Instance().LateNext();

    //SceneManager::Instance().next();
    //MusicManager::instance().StartGameTransition();
    //std::cout << "You clicked button in Menu." << std::endl;
    //if (s) {
        //s->alpha_anim = 0.f; // Reset alpha to start moving back
        //s->menu_anim = true; // Set menu animation flag to true
		//GameManager::instance().end_screen = false;
		//GameManager::instance().start = true;
    //}
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

void LeaderboardButton()
{
    SceneGraph* s = SceneManager::Instance().getCurrentScene();
    if (s) {
        Node* keywriter = s->root->getChildByName("keyboard_controller");
        if (keywriter) {
            AnimatedMenu* script = keywriter->getComponent<AnimatedMenu>();
            if (script) {
                script->isActive = true;
				script->right = true; // Move to the right
                //script->alfa = 0.f; // Reset alfa to start moving back
            }

        }


    }
}

void ReturnButton()
{
    SceneGraph* s = SceneManager::Instance().getCurrentScene();
    if (s) {
        Node* keywriter = s->root->getChildByName("keyboard_controller");
        if (keywriter) {
            AnimatedMenu* script = keywriter->getComponent<AnimatedMenu>();
            if (script) {
                script->isActive = true;
                script->right = false; // Move to the right
				//script->alfa = 0.f; // Reset alfa to start moving back
            }

        }


    }
}

void ExitButton()
{
	auto* window = ServiceLocator::getWindow();
	if (window) {
		glfwSetWindowShouldClose(window->window, true);
	}
}

void CheckboxButton()
{
	GameManager::instance().tutorialActive = !GameManager::instance().tutorialActive;

}

void ReturnToMenuButton()
{
    SceneGraph* s = SceneManager::Instance().getCurrentScene();

    if (s) {
        s->alpha_anim = 0.f; // Reset alpha to start moving back
        s->menu_anim = true; // Set menu animation flag to true
        Node* keywriter = s->root->getChildByName("keyboard_controller");
        if (keywriter) {
            AnimatedMenu* script = keywriter->getComponent<AnimatedMenu>();
            if (script) {
                script->endAnim = true;
                script->endAnimTimer = 1.f; // Reset end animation timer
                script->ending = true; // Reset end animation timer
                script->starting = false; // Reset end animation timer
                script->reverse = true; // Reset end animation timer
            }

        }


    }

    MusicManager::instance().StopDeathMusic();
    GameManager::instance().end_screen = false; 
}
