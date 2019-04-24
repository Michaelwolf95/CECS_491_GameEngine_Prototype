#include <vector>
#include "GameObject.h"
#include "ApplicationManager.h"

GameObject::GameObject(const char* _name)
{
	if (_name == nullptr)
	{
		name = "New GameObject";
	}
	else
	{
		name = _name;
	}

	transform = new Transform();
	transform->gameObject = this;// shared_from_this();
}

GameObject::~GameObject()
{
	HandleDisable();
	std::cout << "\tDeconstructing " << name << "..." << std::endl;
	components.clear();
}

std::vector<GameObject*> GameObject::GetChildren()
{
	std::vector<GameObject*> children;
	for (size_t i = 0; i < this->transform->children.size(); i++)
	{
		children.push_back(this->transform->children[i]->gameObject);
	}
	return children;
}

GameObject* GameObject::GetChild(int index)
{
	if (index >= 0 && index < this->transform->children.size())
	{
		return this->transform->children[index]->gameObject;
	}
	return nullptr;
}

bool GameObject::IsActiveInHierarchy()
{
	if (!isActive) return false;
	if (this->transform->parent != nullptr)
	{
		return this->transform->parent->gameObject->IsActiveInHierarchy();
	}
	return true;
}

void GameObject::SetActive(bool active)
{
	if (isActive != active)
	{
		isActive = active;
		parentHierarchyActive = (this->transform->parent != nullptr)?
			this->transform->parent->gameObject->IsActiveInHierarchy() : true;
		// Enabling/Disabling only matters if the objects activation is not overriden by its parents.
		if (parentHierarchyActive)
		{
			if (isActive)
				HandleEnable();
			else
				HandleDisable();
			// Update children with hierarchy change.
			HandleHierarchyChanged();
		}
	}
}

void GameObject::HandleHierarchyChanged()
{
	bool newParentHierarchyActive = (this->transform->parent != nullptr) ?
		this->transform->parent->gameObject->IsActiveInHierarchy() : true;
	// If previous parent was inactive, and new parent is active -> Update as enable.
	// If previous parent was active, and new parent is inactive -> Update as disable.
	if (isActive)
	{
		if (newParentHierarchyActive && !parentHierarchyActive)
		{
			HandleEnable();
		}
		else if (!newParentHierarchyActive && parentHierarchyActive)
		{
			HandleDisable();
		}
	}
	
	parentHierarchyActive = newParentHierarchyActive;

	// Recursively update children.
	for (size_t i = 0; i < this->transform->children.size(); i++)
	{
		this->transform->children[i]->gameObject->HandleHierarchyChanged();
	}
}

void GameObject::AddComponent(Component_ptr comp)
{
	components.push_back(comp);
	comp->gameObject = this;// shared_from_this();

	if (this->IsActiveInHierarchy())
	{
		comp->OnEnable();
	}
}

void GameObject::RemoveComponent(Component_ptr comp)
{
	// If vector contains it, remove it.
	// https://stackoverflow.com/questions/39912/how-do-i-remove-an-item-from-a-stl-vector-with-a-certain-value
	auto n = std::find(components.begin(), components.end(), comp);
	if (n != components.end())
	{
		// swap the one to be removed with the last element and remove the item at the end of the container
		// to prevent moving all items after it by one
		std::swap(*n, components.back());
		components.pop_back();
	}
}

void GameObject::EmitComponentEvent(void(*eventFunction)(Component_ptr))
{
	for (size_t i = 0; i < components.size(); i++)
	{
		eventFunction(components[i]);
	}
}

void GameObject::StartComponents()
{
	for (size_t i = 0; i < components.size(); i++)
	{
		StartComponent(components[i]);
	}
}

//TODO: Remove dependency on ApplicationManager
void GameObject::StartComponent(Component_ptr comp)
{
	if((ApplicationManager::getInstance().IsEditMode() == false) || comp->executeInEditMode)
		comp->callback_PerformStart();
}

void GameObject::UpdateComponents()
{
	for (size_t i = 0; i < components.size(); i++)
	{
		UpdateComponent(components[i]);
	}
}

void GameObject::UpdateComponent(Component_ptr comp)
{
	if ((ApplicationManager::getInstance().IsEditMode() == false) || comp->executeInEditMode)
		comp->callback_PerformUpdate();
}
void GameObject::FixedUpdateComponents()
{
	for (size_t i = 0; i < components.size(); i++)
	{
		FixedUpdateComponent(components[i]);
	}
}

void GameObject::FixedUpdateComponent(Component_ptr comp)
{
	if ((ApplicationManager::getInstance().IsEditMode() == false) || comp->executeInEditMode)
		comp->FixedUpdate();
}

Component_ptr GameObject::FilterComponent(std::function<bool(Component_ptr)> predicate)
{
	for (Component_ptr c : components)
	{
		if (predicate(c))
		{
			return c;
		}
	}
	return NULL;
}

// Finds an object matching the typeInfo type definition.
bool GameObject::FindComponent(const std::type_info& typeInfo, void** object)
{
	return (this->FilterComponent([&](Component_ptr c)->bool {
		if (typeid(*(c.get())) == typeInfo)
		{
			// Set the value of the pointer-pointer to the value of the pointer that we just found.
			*object = (c.get()); 
			return true;
		}
		return false;
	})!= NULL);
}

// WARNING: This can cause crashes if a shared_ptr to the object does not already exist.
// TODO: Add safety measure to prevent crashes.
GameObject_ptr GameObject::GetSelfPtr()
{
	return shared_from_this();
}

void GameObject::HandleEnable()
{
	for (size_t i = 0; i < components.size(); i++)
	{
		components[i]->OnEnable();
	}
}

void GameObject::HandleDisable()
{
	for (size_t i = 0; i < components.size(); i++)
	{
		components[i]->OnDisable();
	}
}
