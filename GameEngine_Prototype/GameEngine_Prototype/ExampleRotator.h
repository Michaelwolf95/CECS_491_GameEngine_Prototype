#pragma once
#include "Component.h"

class ExampleRotator : public Component
{
public:
	float rotationSpeed = 25.0f;
	ExampleRotator();
	~ExampleRotator();
	void Start() override;
	void Update() override;

};
