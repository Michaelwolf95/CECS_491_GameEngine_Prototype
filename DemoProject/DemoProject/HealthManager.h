#pragma once
#include "Component.h"
#include "XEngineProject.h"
#include "XEngine.h"
using namespace XEngine;
#include "boost/signals2.hpp"
#include "boost/function.hpp"
#include <boost/bind.hpp>

class PROJECT_API HealthManager : public Component
{
public:
	static Registrar<HealthManager> registrar;
	HealthManager();
	~HealthManager();
	void Start() override;
	void Update() override;
	void DrawInspector() override;
	int currentHealth, maxHealth = 100;
	void applyDamage(int);

	AudioComponent* audioComponent = nullptr;

	boost::signals2::signal<void()> OnDeathEvent;
	boost::signals2::signal<void(float)> OnTakeDamageEvent;

private:
	bool isDead = false;
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER()
	template<class Archive>
	void save(Archive & ar, const unsigned int version) const
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Component);
		// Add custom parameters here.
		//ar & BOOST_SERIALIZATION_NVP(currentHealth);
		if (version > 1)
		{
			ar & BOOST_SERIALIZATION_NVP(maxHealth);
		}

	}
	template<class Archive>
	void load(Archive & ar, const unsigned int version)
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Component);
		// Add custom parameters here.
		//ar & BOOST_SERIALIZATION_NVP(currentHealth);
		if (version > 1)
		{
			ar & BOOST_SERIALIZATION_NVP(maxHealth);
		}

	}

};
