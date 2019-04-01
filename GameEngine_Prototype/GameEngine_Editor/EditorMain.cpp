
#include "../GameEngine_Prototype/XEngine.h"

#include <iostream>
#include <stdio.h>

#ifndef STB_DEFINE
#define STB_DEFINE  
#define STB_IMAGE_IMPLEMENTATION
// Disabled to prevent duplicate.
//#include <stb/stb_image.h> 
#endif

#include <stdlib.h>
#include <Windows.h>
typedef int(WINAPI *MainFunc)(void);

static void Editor_Init();
static void Editor_Update();
static void Editor_PreRender();
static void Editor_PostRender();
static void Editor_ApplicationClose();

// Editor entry point.
int main()
{
	std::cout << "===== LAUNCHING X-ENGINE EDITOR =====" << std::endl;

	XEngine::OnEngineInit = &Editor_Init;
	XEngine::OnEngineUpdate = &Editor_Update;
	XEngine::OnEnginePreRender = &Editor_PreRender;
	XEngine::OnEnginePostRender = &Editor_PostRender;
	XEngine::OnApplicationClose = &Editor_ApplicationClose;

	XEngine::useDefaultSceneInitialization = false;

	return XEngine::main();
}

void Editor_Init()
{
	std::cout << "Initializing Editor. =======================================" << std::endl;
}
void Editor_Update()
{

}
void Editor_PreRender()
{

}
void Editor_PostRender()
{

}
void Editor_ApplicationClose()
{

}
