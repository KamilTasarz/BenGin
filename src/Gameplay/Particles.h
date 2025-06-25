#pragma once

#include "Script.h"

class PrefabInstance;

struct Particle
{
	Node* prefab = nullptr;
	//PrefabInstance* instance = nullptr;
	glm::vec3 position;
	glm::vec3 velocity;
	float size;
	float lifetime;
	float age;
};

class Particles : public Script
{
public:
	using SelfType = Particles;

	VARIABLE(std::string, particlePrefab);
	VARIABLE(float, emissionRate);      // Particles per second
	VARIABLE(bool, emit);               // If true, spawns particles
	VARIABLE(float, lifetime);          // Lifetime of each particle in seconds
	VARIABLE(float, minParticleSize);   // Minimum size of particles
	VARIABLE(float, maxParticleSize);   // Maximum size of particles
	VARIABLE(float, velocityX);		    // Minimum speed of particles	
	VARIABLE(float, velocityY);		    // Minimum speed of particles
	VARIABLE(float, velocityZ);		    // Minimum speed of particles
	VARIABLE(float, velocityVariation); // Maximum speed of particles
	VARIABLE(bool, applyVelocityX);     // If true, applies velocity in X direction
	VARIABLE(bool, applyVelocityY);		// If true, applies velocity in Y direction
	VARIABLE(bool, applyVelocityZ);		// If true, applies velocity in Z direction
	VARIABLE(bool, velocityXInBothDir); // If true, applies velocity in both directions (positive and negative)
	VARIABLE(bool, velocityYInBothDir); // If true, applies velocity in both directions (positive and negative)
	VARIABLE(bool, velocityZInBothDir); // If true, applies velocity in both directions (positive and negative)
	VARIABLE(float, gravity);           // Gravity applied to particles
	VARIABLE(bool, useGravity);			// If true, applies gravity to particles
	VARIABLE(float, drag);
	VARIABLE(bool, useDrag);          // If true, applies drag to particles
	VARIABLE(float, areaX);			    // Area of emission in X direction
	VARIABLE(float, areaY);             // Area of emission in Y direction
	VARIABLE(float, areaZ);			    // Area of emission in Z direction
	VARIABLE(float, distortion);        // Distortion factor for particle movement

	std::vector<Particle> particles;
	float spawnTimer = 0.f;
	float playerVerticalOffset = 0.5f;

	Particles() = default;
	~Particles();
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;
	void onEnd() override;
	void SpawnParticle(float deltaTime);

	void SpawnBurst(int count);

	std::vector<Variable*> getFields() const override {
		static Variable particlePrefabVar = getField_particlePrefab();
		static Variable emissionRateVar = getField_emissionRate();
		static Variable emitVar = getField_emit();
		static Variable lifetimeVar = getField_lifetime();
		static Variable minParticleSizeVar = getField_minParticleSize();
		static Variable maxParticleSizeVar = getField_maxParticleSize();
		static Variable velocityXVar = getField_velocityX();
		static Variable velocityYVar = getField_velocityY();
		static Variable velocityZVar = getField_velocityZ();
		static Variable velocityVariationVar = getField_velocityVariation();
		static Variable applyVelocityXVar = getField_applyVelocityX();
		static Variable applyVelocityYVar = getField_applyVelocityY();
		static Variable applyVelocityZVar = getField_applyVelocityZ();
		static Variable velocityXInBothDirVar = getField_velocityXInBothDir();
		static Variable velocityYInBothDirVar = getField_velocityYInBothDir();
		static Variable velocityZInBothDirVar = getField_velocityZInBothDir();
		static Variable gravityVar = getField_gravity();
		static Variable useGravityVar = getField_useGravity();
		static Variable dragVar = getField_drag();
		static Variable useDragVar = getField_useDrag();
		static Variable areaXVar = getField_areaX();
		static Variable areaYVar = getField_areaY();
		static Variable areaZVar = getField_areaZ();
		static Variable distortionVar = getField_distortion();
		return { &particlePrefabVar, &emissionRateVar, &emitVar, &lifetimeVar, &minParticleSizeVar, &maxParticleSizeVar,
			&velocityXVar, &velocityYVar, &velocityZVar, &velocityVariationVar, &applyVelocityXVar, &applyVelocityYVar, &applyVelocityZVar,
			&velocityXInBothDirVar, &gravityVar, &useGravityVar, &dragVar, &useDragVar, &areaXVar, &areaYVar, &areaZVar, &distortionVar };
	}
};