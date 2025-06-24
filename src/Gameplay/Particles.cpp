#include "Particles.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include <random>

REGISTER_SCRIPT(Particles);

void Particles::onAttach(Node* owner) {
    this->owner = owner;
}

void Particles::onDetach() {
	for (auto& particle : particles) {
		if (particle.prefab) {
			owner->scene_graph->deleteChild(particle.prefab);
		}
        delete particle.instance;
	}
	particles.clear();
    this->owner = nullptr;
}

void Particles::onStart() {
}

void Particles::onUpdate(float deltaTime) {
    if (emit) {
        spawnTimer += deltaTime;

        float timePerParticle = 1.0f / emissionRate;
        while (spawnTimer >= timePerParticle) {
            SpawnParticle(deltaTime);
            spawnTimer -= timePerParticle;
        }
    }

    for (auto it = particles.begin(); it != particles.end();) {
        it->age += deltaTime;

        if (distortion != 0.0f) {
			it->velocity.x += distortion * ((rand() / (float)RAND_MAX) - 0.5f);
			it->velocity.y += distortion * ((rand() / (float)RAND_MAX) - 0.5f);
			it->velocity.z += distortion * ((rand() / (float)RAND_MAX) - 0.5f);
        }

        if (useGravity) {
            it->velocity.y += gravity * deltaTime;
        }

		if (useDrag) {
			it->velocity.x *= (1.0f - drag * deltaTime);
			it->velocity.y *= (1.0f - drag * deltaTime);
			it->velocity.z *= (1.0f - drag * deltaTime);
		}

        it->position += it->velocity * deltaTime;
		it->prefab->transform.setLocalPosition(it->position);

		if (it->lifetime - it->age < 0.3f) {
			float newSize = it->size * (it->lifetime - it->age) / 0.3f;
            it->prefab->transform.setLocalScale(glm::vec3(newSize));
		}

        if (it->age >= it->lifetime) {
            owner->scene_graph->deleteChild(it->prefab);
            it = particles.erase(it);
        }
        else {
            ++it;
        }
    }
}

void Particles::onEnd()
{
	for (auto& particle : particles) {
		if (particle.prefab) {
			owner->scene_graph->deleteChild(particle.prefab);
		}
        delete particle.instance;
	}
	particles.clear();
}

void Particles::SpawnParticle(float deltaTime) {

    Particle p;

    p.position.x = owner->transform.getGlobalPosition().x + ((rand() / (float)RAND_MAX) - 0.5f) * areaX;
    p.position.y = owner->transform.getGlobalPosition().y + ((rand() / (float)RAND_MAX) - 0.5f) * areaY;
    p.position.z = owner->transform.getGlobalPosition().z + ((rand() / (float)RAND_MAX) - 0.5f) * areaZ;

    p.lifetime = lifetime;
    p.age = 0.0f;
    p.size = minParticleSize + static_cast<float>(rand()) / RAND_MAX * (maxParticleSize - minParticleSize);

	glm::vec3 speed(0.f);
    speed.x = (velocityX - velocityVariation) + static_cast<float>(rand()) / RAND_MAX * (velocityVariation * 2);
	speed.y = (velocityY - velocityVariation) + static_cast<float>(rand()) / RAND_MAX * (velocityVariation * 2);
	speed.z = (velocityZ - velocityVariation) + static_cast<float>(rand()) / RAND_MAX * (velocityVariation * 2);

    glm::vec3 dir(0.f);
    auto randomSignX = [&]() { return velocityXInBothDir ? (rand() % 2 == 0 ? -1.f : 1.f) : 1.f; };
	auto randomSignY = [&]() { return velocityYInBothDir ? (rand() % 2 == 0 ? -1.f : 1.f) : 1.f; };
	auto randomSignZ = [&]() { return velocityZInBothDir ? (rand() % 2 == 0 ? -1.f : 1.f) : 1.f; };

    if (applyVelocityX) dir.x = randomSignX();
    if (applyVelocityY) dir.y = randomSignY();
    if (applyVelocityZ) dir.z = randomSignZ();

    if (glm::length(dir) > 0.01f) {
        dir = glm::normalize(dir);
    }
    else {
        dir = glm::vec3(0.f, 1.f, 0.f);
    }

    p.velocity.x = dir.x * speed.x;
	p.velocity.y = dir.y * speed.y;
	p.velocity.z = dir.z * speed.z;

    PrefabInstance* pref = new PrefabInstance (
        PrefabRegistry::FindPuzzleByName(particlePrefab),
        owner->scene_graph,
        "particle",
        owner->getTransform().getLocalPosition()
    );

    Node* particle = pref->prefab_root->getChildById(0);

    if (owner->getTagName() == "Box") p.position.y += playerVerticalOffset;

    particle->transform.setLocalPosition(p.position);
	particle->transform.setLocalScale(glm::vec3(p.size));

	p.prefab = particle;
    p.instance = pref;

    particles.push_back(p);
    owner->scene_graph->addChild(particle);
}

void Particles::SpawnBurst(int count) {
	for (int i = 0; i < count; ++i) {
		SpawnParticle(0.0f);
	}
}
