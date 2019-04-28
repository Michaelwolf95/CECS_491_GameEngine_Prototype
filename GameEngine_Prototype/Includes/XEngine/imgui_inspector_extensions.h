#pragma once
#include "imgui.h"
#include <iostream>
#include <string>
#include <type_traits>
#include "Component.h"
#include "GameObject.h"
namespace XEngine
{
	namespace GUI
	{
		//IMGUI_API void InputTextField(std::string& str, char* label = "##edit");
		IMGUI_IMPL_API void InputTextField(std::string& str, const char* label = "##edit");
		// Helper to display a little (?) mark which shows a tooltip when hovered.
		IMGUI_IMPL_API void HelpMarker(const char* desc);


		IMGUI_IMPL_API void GameObjectReference(GameObject*& go, std::string label);

		IMGUI_IMPL_API void ComponentReference(const std::type_info & typeInfo, Component*& compRef, std::string label);

		template <typename T>
		void ComponentReference(T*& compRef, std::string label)
		{
			static_assert(std::is_base_of<Component, T>::value, "Type T must derive from Component");
			ComponentReference(typeid(T), ((Component*&)compRef), label);
		}
	}
}