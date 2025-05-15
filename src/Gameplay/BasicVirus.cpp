#include "BasicVirus.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"

REGISTER_SCRIPT(BasicVirus);

void BasicVirus::onAttach(Node* owner)
{
	this->owner = owner;
	std::cout << "BasicVirus::onAttach::" << owner->name << std::endl;
}

void BasicVirus::onDetach()
{
	std::cout << "BasicVirus::onDetach::" << owner->name << std::endl;
	owner = nullptr;
}

//void BasicVirus::VirusEffect(Node* target)
//{
//	// Implement the effect logic here
//}
