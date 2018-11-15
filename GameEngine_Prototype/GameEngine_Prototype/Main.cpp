#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_DEFINE  
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include "AssetManager.h"
#include "ApplicationManager.h"
#include "RenderManager.h"
#include "SceneManager.h"
#include "Time.h"

#include "TestScenes.h"

unsigned int __stdcall mythread(void* data);
long __stdcall WindowProcedure(HWND window, unsigned int msg, WPARAM wp, LPARAM lp);

// ENTRY POINT
int main()
{
	std::cout << "===== LAUNCHING CECS_491 GAME ENGINE =====" << std::endl;
	// Init Managers
	ApplicationManager::CreateManager();
	RenderManager::CreateManager();
	SceneManager::CreateManager();
	Time::CreateManager();

	// Create & Load Scene
	RunTestScene();

	// FRAME LOOP
	while (!ApplicationManager::getInstance().CheckIfAppShouldClose())
	{
		ApplicationManager::getInstance().ApplicationStartUpdate();
		Time::getInstance().UpdateTime();

		// Do Game Logic here
		SceneManager::getInstance().UpdateActiveScene();

		RenderManager::getInstance().Render();

		ApplicationManager::getInstance().ApplicationEndUpdate();
	}

	ApplicationManager::getInstance().CloseApplication();
	return 0;
}