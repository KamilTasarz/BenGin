#include "MaterialColor.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"

REGISTER_SCRIPT(MaterialColor);

void MaterialColor::onAttach(Node* owner)
{
	this->owner = owner;
}

void MaterialColor::onDetach()
{
	owner = nullptr;
}

void MaterialColor::onStart()
{
	owner->changeColor(glm::vec4(red, green, blue, alpha));
}
