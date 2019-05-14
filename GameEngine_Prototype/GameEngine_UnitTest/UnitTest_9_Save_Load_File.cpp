#include "stdafx.h"
#include "CppUnitTest.h"
//
#include "XEngine.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include <glad/glad.h>
using namespace XEngine;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XEngine_UnitTest
{
	// each test method needs to close application to complete test
	TEST_CLASS(Asset_Save_Load_Assets)
	{
	public:
		TEST_METHOD(Save_Load_GameObject_File)
		{
			SceneManager::CreateManager();
			Scene_ptr scene = SceneManager::getInstance().CreateNewScene("TestingSaving");
			SceneManager::getInstance().SetActiveScene(scene);


			// Arrange
			GameObject_ptr go(new GameObject("testCrate"));
			std::shared_ptr<TestMoverComponent> move(new TestMoverComponent());
			go->AddComponent(move);

			// save into file
			GameObject::CreatePrefab(go);

			// load from file
			GameObject_ptr loadedGo = GameObject::InstantiatePrefab("../Assets/Prefabs/testCrate.prefab");

			// Assert: same gameobject with same amount of components
			Assert::AreEqual(go->name, loadedGo->name);
			Assert::IsTrue(go->components.size() == loadedGo->components.size());
		}

		//materia
	};
}