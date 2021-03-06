#include "PhysicsManager.h"
#include "GameTime.h"
#include "SceneManager.h"
#include "Rigidbody.h"

namespace XEngine
{
	PhysicsManager::PhysicsManager() {}
	PhysicsManager::~PhysicsManager()
	{
		CleanUp();
	}

	PhysicsManager * PhysicsManager::CreateManager()
	{
		PhysicsManager* instance = &PhysicsManager::getInstance();
		instance->Init();
		return instance;
	}

	int PhysicsManager::Init()
	{
		///-----initialization_start----- (Comments from Demo)

		///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
		collisionConfiguration = new btDefaultCollisionConfiguration();

		///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
		dispatcher = new btCollisionDispatcher(collisionConfiguration);

		///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
		overlappingPairCache = new btDbvtBroadphase();

		///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
		solver = new btSequentialImpulseConstraintSolver;

		dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

		dynamicsWorld->setGravity(btVector3(gravity.x, gravity.y, gravity.z));


		// Callback for physics ticks.
		dynamicsWorld->setInternalTickCallback(&PhysicsManager::PhysicsTickCallback);

		//dynamicsWorld.

		isInitialized = true;
		return 0;
	}

	void PhysicsManager::PhysicsUpdate()
	{
		dynamicsWorld->stepSimulation(GameTime::deltaTime, 10, fixedUpdateTime);
	}

	void PhysicsManager::PhysicsTick()
	{
		// CHECK COLLISIONS
		//https://stackoverflow.com/questions/11175694/bullet-physics-simplest-collision-example
		int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
		for (int i = 0; i < numManifolds; i++)
		{
			//std::cout << "Manifold " << i << std::endl;
			btPersistentManifold* contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
			btCollisionObject* obA = const_cast<btCollisionObject*>(contactManifold->getBody0());
			btCollisionObject* obB = const_cast<btCollisionObject*>(contactManifold->getBody1());

			btRefRigidbody* refRbA = btRefRigidbody::upcast(obA);
			if (refRbA != 0)
			{
				refRbA->HandleCollision(obB, contactManifold);
			}
			btRefRigidbody* refRbB = btRefRigidbody::upcast(obB);
			if (refRbB != 0)
			{
				// Note that the manifold might be different depending on the body being 0 or 1.
				// We might need to create our own Collision object.
				refRbB->HandleCollision(obA, contactManifold);
			}
			
			//int numContacts = contactManifold->getNumContacts();
			//for (int j = 0; j < numContacts; j++)
			//{
			//	btManifoldPoint& pt = contactManifold->getContactPoint(j);
			//	if (pt.getDistance() < 0.f)
			//	{
			//		const btVector3& ptA = pt.getPositionWorldOnA();
			//		const btVector3& ptB = pt.getPositionWorldOnB();
			//		const btVector3& normalOnB = pt.m_normalWorldOnB;
			//	}
			//}
		}

		SceneManager::getInstance().FixedUpdateActiveScene();
	}

	void PhysicsManager::PhysicsTickCallback(btDynamicsWorld * world, btScalar timeStep)
	{
		PhysicsManager::getInstance().PhysicsTick();
	}

	// Cleanup and unload
	void PhysicsManager::CleanUp()
	{
		if (!isInitialized) return;

		//remove the rigidbodies from the dynamics world and delete them
		for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
		{
			btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
			btRigidBody* body = btRigidBody::upcast(obj);
			if (body && body->getMotionState())
			{
				delete body->getMotionState();
			}
			dynamicsWorld->removeCollisionObject(obj);
			delete obj;
		}

		//delete collision shapes
		for (int j = 0; j < collisionShapes.size(); j++)
		{
			btCollisionShape* shape = collisionShapes[j];
			collisionShapes[j] = 0;
			delete shape;
		}

		//delete dynamics world
		delete dynamicsWorld;
		dynamicsWorld = nullptr;

		//delete solver
		delete solver;

		//delete broadphase
		delete overlappingPairCache;

		//delete dispatcher
		delete dispatcher;

		delete collisionConfiguration;

		//next line is optional: it will be cleared by the destructor when the array goes out of scope
		collisionShapes.clear();
	}

	void PhysicsManager::AddCollisionShape(btCollisionShape* colShape)
	{
		collisionShapes.push_back(colShape);
		//auto n = std::find(collisionShapes..begin(), collisionShapes.end(), colShape);
		//if (n == collisionShapes.end())
		//{
		//	collisionShapes.push_back(colShape);
		//}
	}
	void PhysicsManager::RemoveCollisionShape(btCollisionShape* colShape)
	{
		collisionShapes.remove(colShape);
		//std::cout << "Removed colShape" << std::endl;
		//auto n = std::find(collisionShapes.begin(), collisionShapes.end(), colShape);
		//if (n != collisionShapes.end())
		//{
		//	std::swap(*n, collisionShapes.back());
		//	collisionShapes.pop_back();
		//}
	}
}