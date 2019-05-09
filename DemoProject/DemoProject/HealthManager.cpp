#include "HealthManager.h"
#include "XEngine.h"
#include "GameObject.h" 
#include "Component.h"
using namespace XEngine;
#include "AudioComponent.h"

// Register to be created and serialized.
REGISTER_COMPONENT(HealthManager, "HealthManager")

HealthManager::HealthManager() 
{
	this->currentHealth = 100;
	this->maxHealth = 100;
}
HealthManager::~HealthManager() {}

// Start is called on the objects first update.
void HealthManager::Start()
{
	if (audioComponent == nullptr)
	{
		this->gameObject->FindComponent(typeid(AudioComponent), (void**)&audioComponent);
	}
}

// Update is called once per frame.
void HealthManager::Update()
{
	if (this->currentHealth <= 0)
	{
		if (!this->isDead)
		{
			this->isDead = true;

			if (audioComponent != nullptr)
				audioComponent->Play();
		}
	}
}

// Draw the inspector for your custom component.
void HealthManager::DrawInspector()
{
	int size = this->maxHealth;
	ImGui::InputInt("Max Health", (int*)&size);
	if (size != this->maxHealth)
	{
		this->maxHealth = size;
	}
}

void HealthManager::applyDamage(int damage)
{
	this->currentHealth -= damage;
}