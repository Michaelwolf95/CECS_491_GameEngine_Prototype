#include "SimpleModelComponent.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ApplicationManager.h"
#include "RenderManager.h"
#include "GameObject.h"
#include "CameraComponent.h"
#include "LightComponent.h"

SimpleModelComponent::SimpleModelComponent(float * verts, unsigned int numV, unsigned int vertDataSize, unsigned int * ind, unsigned int numInd, Material * _material)
	: RenderableObject(verts, numV, vertDataSize, ind, numInd, _material)
{
	Setup();
}

SimpleModelComponent::~SimpleModelComponent(){}

void SimpleModelComponent::Setup()
{
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

	// normals attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexDataSize * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertexDataSize * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
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

	material->shader->setFloat("constant", 1.0f);
	material->shader->setFloat("linear", 0.1f);
	material->shader->setFloat("quadratic", 0.3f);

	if (material->useLight)
	{
		int i = 0;
		material->shader->setInt("numLights", RenderManager::getInstance().lights.size());
		material->shader->setVec3("viewPos", RenderManager::getInstance().getCurrentCamera()->getPosition()); // TODO: not working properly
		for (Light* light : RenderManager::getInstance().lights)
		{
			std::string strLights = "lights[]";
			std::string strLightsPos = "lightsPos[]";
			std::string strLightInt =  "lightIntensities[]";
			strLights.insert(7, std::to_string(i));
			strLightsPos.insert(10, std::to_string(i));
			strLightInt.insert(17, std::to_string(i));
			std::cout << "strLightInt == " << strLightInt << std::endl;
			std::cout << "intensity == " << ((LightComponent *)light)->getIntensity() << std::endl;
			material->shader->setVec3(strLights, light->getLightColor());
			material->shader->setVec3(strLightsPos, light->getLightPos());
			material->shader->setFloat(strLightInt, ((LightComponent *)light)->getIntensity());
			i++;
		}
		//material->shader->setMat3
	}

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, numVerts);

	//gameObject->transform->DrawGizmo();
}

void SimpleModelComponent::Start()
{

}

void SimpleModelComponent::Update()
{

}

void SimpleModelComponent::OnDrawGizmos()
{
	gameObject->transform->DrawGizmo();
}
