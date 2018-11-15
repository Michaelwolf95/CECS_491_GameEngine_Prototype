#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include "RenderableObject.h"
#include "Shader.h"
#include "Material.h"
#include "Singleton.h"
#include "Camera.h"
#include "Light.h"

/* The RenderManager is responsible for:
- Tracking all renderable objects
- Handling switching between shaders
- Drawing all rendered objects optimally
*/
class RenderManager : public Singleton<RenderManager>
{
	friend class Singleton<RenderManager>;
public:
	static Shader* defaultShader;
	static Material* defaultMaterial;
	static Shader* colorDrawShader;

	unsigned int currentShaderID = 0;
	bool isInitialized = false;
	// Current view and projection matrices. These can be set by the camera.
	glm::mat4* defaultView;
	glm::mat4* defaultProjection;

	std::vector<RenderableObject*> currentRenderables;
	std::vector<Light*> lights;
	// Create static instance
	static RenderManager* CreateManager();

	// Init instance
	int Init();

	glm::mat4 getView();
	glm::mat4 getProjection();
	Camera* getCurrentCamera();
	void setCurrentCamera(Camera* cam);

	void Render();
	void RenderObject(RenderableObject* renderable);

	void AddRenderable(RenderableObject * renderable);
	void RemoveRenderable(RenderableObject * renderable);
	void AddLight(Light * light);
	void RemoveLight(Light * light);

	void FreeAllResources();
	void FreeObjectResources(RenderableObject* renderable);

	static void DrawScreenPoint(glm::vec2 point, glm::vec4 color, int size);
	static void DrawWorldPoint(glm::vec3 worldPoint, glm::vec4 color, int size);

private:
	Camera * currentCamera;
	void CompileShaders();
};