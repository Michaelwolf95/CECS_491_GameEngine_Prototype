#include "AudioEngine.h"
#include "AudioComponent.h"
#include "AudioManager.h"
#include "GameObject.h" 
#include "Component.h"
#include <math.h>
#include "RenderManager.h"

#include "imgui_inspector_extensions.h"
#include "imgui_stdlib.h"
#include "Input.h"
//CAudioEngine test;
//soundTest->Init();
//
////LoadSound(const std::string& strSoundName, bool b3d, bool bLooping, bool bStream)
////parameters about streaming, looping, and whether or not it's a 3D sound
//soundTest->LoadSound(soundPath1, false, true, true);
//soundTest->PlaySounds(soundPath1);

REGISTER_COMPONENT(AudioComponent, "AudioComponent")

void AudioComponent::Start() {

	//string soundPath1 = "../Assets/sounds/foghorn.wav";
	//soundList.push_back(soundPath1);
	if (soundPath.size() > 0)
	{
		if (is3D)
		{
			Load3D(soundPath, true, true, true);
		}
		else
		{
			Load2D(soundPath, false, true);
		}
	}
	//gameObject->transform->getPosition();

	glm::mat4 camView = RenderManager::getInstance().getCurrentCamera()->getView();
	glm::vec3 camLocation(camView[3].x, camView[3].y, camView[3].z);
	AudioManager::getInstance().sound.Set3dListenerAndOrientation(camLocation, camLocation, camLocation);
	
}

void AudioComponent::Update()
{
	// TESTING ONLY - REMOVE LATER
	if (Input::GetKeyDown(GLFW_KEY_SPACE))
	{
		Play();
		std::cout << "PLAY!" << std::endl;
	}
	//// MOVE THIS TO AUDIO LISTENER
	//glm::mat4 camView = RenderManager::getInstance().getCurrentCamera()->getView();
	//glm::vec3 camLocation(camView[3].x, camView[3].y, camView[3].z);
	//AudioManager::getInstance().sound.Set3dListenerAndOrientation(camLocation, camLocation, camLocation);
}

AudioComponent::AudioComponent()
{
	
}
AudioComponent::~AudioComponent()
{
	AudioManager::getInstance().sound.UnLoadSound(soundPath);
	/*for (int i = 0; i < soundList.size(); i++) {
		AudioManager::getInstance().sound.UnLoadSound(soundList.at(i));
	}*/
}

void AudioComponent::Load2D(string path, bool loop, bool stream)
{
	//CAudioEngine sound = AudioManager::getInstance();
	//AudioManager::AudioEngine;
	AudioManager::getInstance().sound.LoadSound(path, false, loop, stream);
	
	//AudioManager::getInstance().LoadSound(path, 3d, loop, stream);
}

void AudioComponent::Load3D(string path, bool location, bool loop, bool stream) {

	if (location == true) {
		AudioManager::getInstance().sound.LoadSound(path, location, loop, stream);
	}
}

void AudioComponent::Play(string path, glm::vec3 location, float fVolumedB) {
	//mChannel = AudioManager::getInstance().sound.PlaySounds(path, location, fVolumedB);
	AudioManager::getInstance().sound.PlaySounds(path, location, fVolumedB);
	//AudioManager::getInstance().sound.SetChannelVolume(mChannel, .1);
}

void AudioComponent::Play()
{
	glm::vec3 objectLocation = gameObject->transform->getPosition();
	Play(soundPath, objectLocation, 1);
}

glm::vec3 AudioComponent::getListener()
{
	glm::mat4 camView = RenderManager::getInstance().getCurrentCamera()->getView();
	glm::vec3 camLocation(camView[3].x, camView[3].y, camView[3].z);
	return camLocation;
}

void AudioComponent::DrawInspector()
{
	// File Path
	std::string path = soundPath;
	ImGui::InputText(soundPath.c_str(), &path);
	if (path != soundPath)
	{
		soundPath = path;
	}
	// Flags
	ImGui::Checkbox("Is 3D", &is3D);
}


//void AudioComponent::Play3DSound(string path, ) {
//	//void PlaySound(const string& strSoundName, const glm::vec3& vPos = glm::vec3{ 0, 0, 0 }, float fVolumedB = 0.0f);
//	//PlaySound
//}