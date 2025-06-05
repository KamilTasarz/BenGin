#include "LaserEmitter.h"
#include "../../Basic/Node.h"
#include "../RegisterScript.h"
#include "../../System/PhysicsSystem.h"
#include "../../System/LineManager.h"
#include "LaserObserver.h"
#include "../../Component/BoundingBox.h"

REGISTER_SCRIPT(LaserEmitter);

void LaserEmitter::onAttach(Node* owner)
{
	this->owner = owner;
}

void LaserEmitter::onDetach()
{
	owner = nullptr;
}

void LaserEmitter::onStart()
{
}

void LaserEmitter::onUpdate(float deltaTime)
{
    Ray ray;

    glm::vec3 upVector(0.f, 1.f, 0.f);
    glm::quat rotation = owner->transform.getLocalRotation();
    glm::vec3 direction = rotation * upVector;

    ray.direction = direction;
    ray.origin = owner->transform.getGlobalPosition();
    ray.length = 100.f;

    std::vector<glm::vec3> points;
    std::vector<RayCastHit> nodes;
    Node* lastNode = nullptr;

    bool continueReflection = true;
    while (continueReflection) {
        continueReflection = false;
        nodes.clear();

        if (PhysicsSystem::instance().rayCast(ray, nodes, 50.f, owner)) {
            // Dodaj punkt pocz¹tkowy
            points.push_back(ray.origin);

            for (size_t i = 0; i < nodes.size(); ++i) {
                Node* hitNode = nodes[i].node;


				std::cout << "obiekt: " << owner->getName() << " trafi³ w: " << hitNode->getName() << std::endl;

                // Pomijamy powtórne trafienia tego samego obiektu, np. w pêtli odbiæ
                if (hitNode == lastNode) continue;

                glm::vec3 hitPoint = nodes[i].endPoint;
				Collider* collider = nodes[i].collider;

                if (auto mirror = dynamic_cast<MirrorNode*>(hitNode)) {
                    // Lustro: refleksja
                    if (dynamic_cast<BoundingBox*>(collider)) {
                        std::cout << "LaserEmitter::onUpdate: Trafiono w lustro: " << hitNode->getName() << std::endl;
                        //continue;
                    }

                    ray.origin = hitPoint;
                    ray.direction = mirror->reflectDirection(ray);
                    ray.length = 100.f;
                    lastNode = hitNode;

                    points.push_back(hitPoint); // dodaj miejsce odbicia
                    continueReflection = true;
                    break; // przerywamy tê iteracjê, bo mamy nowy kierunek
                }
                else {
                    // Trafienie w obiekt niebêd¹cy lustrem
                    if (hitNode->getLayerName() == "Observer") {
                        if (auto obs = hitNode->getComponent<LaserObserver>())
                            obs->Activate();
                    }

                    points.push_back(hitPoint);
                    continueReflection = false;
                    break; // zakoñcz odbicia
                }
            }
        }
        else {
            // Promieñ nie trafi³ w nic — opcjonalnie mo¿na dodaæ koñcowy punkt
            points.push_back(ray.origin + ray.direction * ray.length);
        }
    }

    owner->forceUpdateSelfAndChild();

    LineManager::Instance().addVertices(points);
}

