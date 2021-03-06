#pragma once
#include "LibraryExport.h"
#include "Component.h"
#include "Light.h"
#include <glm/glm.hpp>
#include "GizmoSpriteDrawer.h"
#define VAR_NAME(Var) (#Var)

class ENGINE_API LightComponent : public Component, public Light
{
public:
	static Registrar<LightComponent> registrar;
	
	glm::vec4 color;
	float intensity;
	float ambience;

	LightComponent(glm::vec4 _color = glm::vec4(1.0f), float _intensity = 1.0f, float _ambience = 0.1f);
	~LightComponent();

	virtual void Start() override;
	virtual void Update() override;

	glm::vec3 getLightPos() override;
	glm::vec3 getDirection() override;
	int getTypeID() override;
	const std::string* getUniformName() override;
	virtual void DrawInspector() override;
	virtual void OnDrawGizmos() override;
	virtual void Draw(Shader* shader, int &counter) override;

private:
	std::shared_ptr<GizmoSpriteDrawer> gizmoDrawer;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		// save/load base class information
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Component);
		ar & BOOST_SERIALIZATION_NVP(intensity);
		ar & BOOST_SERIALIZATION_NVP(color);
		ar & BOOST_SERIALIZATION_NVP(ambience);
		ar & BOOST_SERIALIZATION_NVP(UNIFORM_NAME);
	}
};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(LightComponent)
