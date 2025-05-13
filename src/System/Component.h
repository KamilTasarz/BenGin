#pragma once

#include <iostream>

class Node; // Forward declaration

class Component
{
protected:
	Node* owner = nullptr; // Pointer to the owner Node
public:
    std::string name;

    virtual void onAttach(Node* owner) {
        this->owner = owner;
    }

    // Wywoływane, gdy skrypt zostaje odczepiony lub Node niszczony
    virtual void onDetach() {
        owner = nullptr;
    }

    // Wywoływane raz na początku (po onAttach)
    virtual void onStart() {}

    // Wywoływane co klatkę z delta-time
    virtual void onUpdate(float deltaTime) {}

    // Wywoływane na końcu, przed usunięciem
    virtual void onEnd() {}

	virtual ~Component() {
		onDetach();
	}


	// Wywoływane, gdy następuje kolizja z innym obiektem
	virtual void onCollision(Node* other) {
		
	}

	virtual void onCollisionLogic(Node* other) {
		
	}

	virtual void onStayCollision(Node* other) {

	}

	virtual void onStayCollisionLogic(Node* other) {

	}

	virtual void onExitCollision(Node* other) {

	}

	virtual void onExitCollisionLogic(Node* other) {

	}
};

