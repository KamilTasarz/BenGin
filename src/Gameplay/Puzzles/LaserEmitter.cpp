#include "LaserEmitter.h"
#include "../../Basic/Node.h"
#include "../RegisterScript.h"
#include "../../System/PhysicsSystem.h"
#include "../../System/LineManager.h"
#include "LaserObserver.h"

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

    std::vector<RayCastHit> nodes;
    Node* lastNode = nullptr;
    std::vector<glm::vec3> points;
    bool checkNext = true;
    while (checkNext) {
        checkNext = false;
        nodes.clear();

        if (PhysicsSystem::instance().rayCast(ray, nodes)) {
            int i = 1;

            if (nodes.size() > 1) {
                points.push_back(ray.origin);

                if (lastNode && lastNode == nodes[1].node) i = 2;

                if (dynamic_cast<MirrorNode*>(nodes[i].node)) {
                    MirrorNode* m = dynamic_cast<MirrorNode*>(nodes[i].node);
                    checkNext = true;
                    lastNode = m;
                    ray.direction = m->reflectDirection(ray);

                    if (!nodes[i].is_phys)
                        ray.origin = nodes[i].endPoint;
                    else
                        ray.origin = nodes[i + 1].endPoint;

                    ray.length = 100.f;
                }
                else if (nodes[i].node->getLayerName() == "Observer") {
                    nodes[i].node->getComponent<LaserObserver>()->Activate();
                }

                if (!checkNext) points.push_back(nodes[i].endPoint);
            }

            //cout << "Przecina i parametr t: " << endl;
            //cout << direction.x << ", " << direction.y << ", " << direction.z << endl;
            //cout << "size: " << nodes.size() << endl;
        }
    }

    owner->forceUpdateSelfAndChild();

    LineManager::Instance().addVertices(points);
    //owner->scene_graph->mark(getRayWorld(window->window, camera->GetView(), camera->GetProjection()));
}
