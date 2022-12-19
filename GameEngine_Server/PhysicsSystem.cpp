#include "PhysicsSystem.h"

#include <iostream>
#include <ostream>

#include "PhysicsUtils.h"

PhysicsSystem::PhysicsSystem() {
}

PhysicsSystem::~PhysicsSystem() {
	for (int i = 0; i < m_PhysicsObjects.size(); i++) {
		delete m_PhysicsObjects[i];
	}
	m_PhysicsObjects.clear();

	// delete[] m_Particles[0];		Can check if this works (Not sure)
	//  -> edit: Nope, use this for array of pointers
}

PhysicsObject* PhysicsSystem::CreatePhysicsObject(const glm::vec3& position, iShape* shape, std::string name) {
	PhysicsObject* physicsObject = new PhysicsObject(position);
	physicsObject->pShape = shape;
	physicsObject->name = name;
	m_PhysicsObjects.push_back(physicsObject);
	return physicsObject;
}
void PhysicsSystem::UpdateStep(float duration) {
	for (PhysicsObject* obj : m_PhysicsObjects)
	{
		obj->Integrate(duration);
	}
}

void PhysicsSystem::AddTriangleToAABBCollisionCheck(int hash, Triangle* triangle)
{
	m_AABBStructure[hash].push_back(triangle);
}

bool PhysicsSystem::RayCastFirstFound(Ray ray, PhysicsObject** hitObject)
{
	for (int i = 0; i < m_PhysicsObjects.size(); i++) {
		PhysicsObject* physicsObject = m_PhysicsObjects[i];
		if (physicsObject->pShape->GetType() == SHAPE_TYPE_SPHERE)
		{
			Sphere* pSphere = dynamic_cast<Sphere*>(physicsObject->pShape);
			if (TestRaySphere(ray.origin, ray.direction, pSphere->Center + physicsObject->position, pSphere->Radius))
			{
				*hitObject = physicsObject;
				return true;
			}
		}
	}
	return false;
}

std::vector<PhysicsObject*> PhysicsSystem::RayCastAll(Ray ray)
{
	std::vector<PhysicsObject*> hitList;
	for (int i = 0; i < m_PhysicsObjects.size(); i++) {
		PhysicsObject* physicsObject = m_PhysicsObjects[i];
		if (physicsObject->pShape->GetType() == SHAPE_TYPE_SPHERE)
		{
			Sphere* pSphere = dynamic_cast<Sphere*>(physicsObject->pShape);
			if (TestRaySphere(ray.origin, ray.direction, pSphere->Center + physicsObject->position, pSphere->Radius))
			{
				hitList.push_back(physicsObject);
			}
		}
	}
	return hitList;
}

bool PhysicsSystem::CollisionTest(const glm::vec3& posA, iShape* shapeA, const glm::vec3& posB, iShape* shapeB)
{
	bool detectedCollision = false;

	// ShapeA AABB Collision test
	if (shapeA->GetType() == SHAPE_TYPE_AABB)
	{
		if (shapeB->GetType() == SHAPE_TYPE_AABB) {}

		else if (shapeB->GetType() == SHAPE_TYPE_SPHERE) {
			detectedCollision = CollisionTest(posB, dynamic_cast<Sphere*>(shapeB), posA, dynamic_cast<AABB*>(shapeA));
		}

		else if (shapeB->GetType() == SHAPE_TYPE_TRIANGLE) {}
	}

	// ShapeA Sphere collision tests
	else if (shapeA->GetType() == SHAPE_TYPE_SPHERE)
	{
		if (shapeB->GetType() == SHAPE_TYPE_AABB) {
			detectedCollision = CollisionTest(posA, dynamic_cast<Sphere*>(shapeA), posB, dynamic_cast<AABB*>(shapeB));
		}

		else if (shapeB->GetType() == SHAPE_TYPE_SPHERE) {
			detectedCollision = CollisionTest(posA, dynamic_cast<Sphere*>(shapeA), posB, dynamic_cast<Sphere*>(shapeB));
		}

		else if (shapeB->GetType() == SHAPE_TYPE_TRIANGLE) {
			detectedCollision = CollisionTest(posA, dynamic_cast<Sphere*>(shapeA), posB, dynamic_cast<Triangle*>(shapeB));
		}
	}

	// ShapeA Triangle collision tests
	else if (shapeA->GetType() == SHAPE_TYPE_TRIANGLE)
	{
		if (shapeB->GetType() == SHAPE_TYPE_AABB) {}

		else if (shapeB->GetType() == SHAPE_TYPE_SPHERE) {
			detectedCollision = CollisionTest(posB, dynamic_cast<Sphere*>(shapeB), posA, dynamic_cast<Triangle*>(shapeA));
		}

		else if (shapeB->GetType() == SHAPE_TYPE_TRIANGLE) {}
	}

	// ShapeA is unknown shape...
	else
	{
		// what is this!?!?!
	}

	return detectedCollision;
}

bool PhysicsSystem::CollisionTest(const glm::vec3& posA, Sphere* a, const glm::vec3& posB, Sphere* b)
{
	return TestSphereSphere(posA + a->Center, a->Radius, posB + b->Center, b->Radius);
}

bool PhysicsSystem::CollisionTest(const glm::vec3& posA, Sphere* a, const glm::vec3& posB, AABB* b)
{
	return TestSphereAABB(posA + a->Center, a->Radius, *b);
}

bool PhysicsSystem::CollisionTest(const glm::vec3& posA, Sphere* a, const glm::vec3& posB, Triangle* b)
{
	Point unused;
	return TestSphereTriangle(posA + a->Center, a->Radius, posB + (*b).A, posB + (*b).B, posB + (*b).C, unused);
}