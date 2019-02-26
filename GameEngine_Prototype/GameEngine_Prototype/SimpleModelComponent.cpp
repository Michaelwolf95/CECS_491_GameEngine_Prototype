#include "SimpleModelComponent.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ApplicationManager.h"
#include "RenderManager.h"
#include "GameObject.h"
#include "CameraComponent.h"
#include "Input.h"

BOOST_CLASS_EXPORT_GUID(SimpleModelComponent, "SimpleModelComponent")

SimpleModelComponent::SimpleModelComponent()
{

}

SimpleModelComponent::SimpleModelComponent(float * verts, unsigned int numV, unsigned int vertDataSize, unsigned int * ind, unsigned int numInd, Material * _material)
	: RenderableObject(verts, numV, vertDataSize, ind, numInd, _material)
{
	Setup();
}

SimpleModelComponent::~SimpleModelComponent(){}

void SimpleModelComponent::Setup()
{
	if (isSetup)
	{
		return;
	}
	std::cout << "Setting up SimpleModelComponent." << std::endl;
	render_enabled = true;
	RenderManager::getInstance().AddRenderable((RenderableObject*)this);

	if (material == nullptr)
	{
		material = RenderManager::defaultMaterial;
		std::cout << "Material set to default." << std::endl;
	}
	else
	{
		std::cout << "Material already set." << std::endl;
		std::cout << this->material << std::endl;
		std::cout << this->material->shader->ID << std::endl;
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexDataSize * numVerts * sizeof(float), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexDataSize * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertexDataSize * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	isSetup = true;
}

void SimpleModelComponent::Draw()
{
	//if (Component::enabled == false) return;
	if (gameObject == nullptr) return;

	// create transformations
	// View & projection from RenderManager, which uses active camera.
	glm::mat4 view = RenderManager::getInstance().getView();
	glm::mat4 projection = RenderManager::getInstance().getProjection();

	// Model uses GameObject transform.
	glm::mat4* model = &gameObject->transform->getMatrix4x4();

	// retrieve the matrix uniform locations
	unsigned int modelLoc = glGetUniformLocation(material->shader->ID, "model");
	unsigned int viewLoc = glGetUniformLocation(material->shader->ID, "view");
	// pass them to the shaders (3 different ways)
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(*model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
	// note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
	material->shader->setMat4("projection", projection);

	if (material->useLight)
	{
		// Get ONE light for now.
		for (Light* light : RenderManager::getInstance().lights)
		{
			material->shader->setVec3("lightColor", light->getLightColor());
			material->shader->setVec3("lightPos", light->getLightPos());
		}
		//material->shader->setMat3
	}

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, numVerts);

	//gameObject->transform->DrawGizmo();
}

void SimpleModelComponent::Start()
{
	if (isSetup == false)
	{
		Setup();
	}
}

void SimpleModelComponent::Update()
{
	if (Input::GetKeyDown(GLFW_KEY_I) && (Input::GetKey(GLFW_KEY_LEFT_CONTROL) || Input::GetKey(GLFW_KEY_RIGHT_CONTROL)))
	{
		// Info
		std::cout << "isSetup=" << isSetup << std::endl;
		std::cout << "# renderables=" << RenderManager::getInstance().currentRenderables.size() << std::endl;
		for (RenderableObject* ro : RenderManager::getInstance().currentRenderables)
		{
			std::cout << "\t" << ((typeid(*ro)).name()) << std::endl;
			//std::cout << ro. << std::endl;
		}
	}
}

void SimpleModelComponent::OnDrawGizmos()
{
	gameObject->transform->DrawGizmo();
}
