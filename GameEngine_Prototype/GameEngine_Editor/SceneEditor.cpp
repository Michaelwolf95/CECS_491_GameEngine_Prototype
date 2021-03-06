#include "SceneEditor.h"
#include "EditorSettingsManager.h"
#include "XEngine.h"
#include "CoreComponents.h"

// ImGui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_inspector_extensions.h"
#include "imgui_stdlib.h"
#include "imgui_internal.h" // Access to default docking.

//TODO: Put 'libboost_filesystem-vc141-mt-gd-x32-1_68.lib' in Libraries folder so we can use this.
//#include <boost/filesystem.hpp>
#include <direct.h> // Alternative to boost filesystem. This limits us to Windows/Linux
#include <filesystem> // C++ 17 Filesystem

#include "ProjectCompiler.h"
#include "AssetManager.h"
#include "ModelLibrary.h"
#include "MaterialLibrary.h"
#include "Singleton.h"
#include "PhysicsManager.h"
#include "Rigidbody.h"
#include <BulletPhysics/btBulletDynamicsCommon.h>


namespace XEngine::Editor {


#pragma region Initialization

SceneEditor * SceneEditor::CreateManager()
{
	SceneEditor* instance = &SceneEditor::getInstance();
	instance->Init();
	return instance;
}

int SceneEditor::Init()
{
	// Create Editor Camera.
	editorCameraGameObject = new GameObject("EditorCamGo");
	std::shared_ptr<EditorCamera> editCamPtr(new EditorCamera());
	editorCamera = editCamPtr.get();
	editorCameraGameObject->AddComponent(editCamPtr);

	LoadEditorConfig();

	// Set Editor Camera Pos (?)
	// get from editor settings
	//	editorCameraGameObject->transform->setPosition(-5.0f, 4.0f, -7.0f);
	//	editorCameraGameObject->transform->setLocalRotationEuler(15, 35, 0);


	// INITIALIZE ImGUI
	GLFWwindow* window = ApplicationManager::APP_WINDOW;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	const char* glsl_version = "#version 130";

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	isInitialized = true;
	return 0;
}

SceneEditor::SceneEditor() {}

SceneEditor::~SceneEditor()
{
	SaveEditorConfig();
}

void SceneEditor::ShutDown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void SceneEditor::EditorPreRender()
{
	//UpdateGUI();
	ImGui::Render();
}

void SceneEditor::EditorPostRender()
{
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	SceneEditor::getInstance().DrawEditorGizmos();
}

#pragma endregion

#pragma region EDITOR CONFIG

void SceneEditor::LoadEditorConfig()
{
	editorConfig = new SceneEditorConfig();
	std::string configPath = std::string(EDITOR_CONFIG_FILE_PATH);

	JSON configJSON;
	char* tempChar;
	try
	{
		std::ifstream file(configPath);

		file >> configJSON;

		std::string tempStr = configJSON["First_Scene_Filepath"];
		tempChar = new char[tempStr.length() + 1];
		strcpy_s(tempChar, tempStr.length() + 1, tempStr.c_str());

		editorConfig->firstSceneFilepath = tempChar;

		glm::vec3 camPos;
		// if there is an camera position entry
		if (configJSON.find("Editor_Camera_Position") != configJSON.end()) 
		{
			camPos = 
			{
				configJSON["Editor_Camera_Position"]["xPos"],
				configJSON["Editor_Camera_Position"]["yPos"],
				configJSON["Editor_Camera_Position"]["zPos"]
			};
		}
		else  // is there is no camera position, put default
		{
			camPos =
			{
				-5.0f,
				4.0f,
				-7.0f
			};
		}

		glm::vec3 camRot;
		// if there is an camera rotation entry
		if (configJSON.find("Editor_Camera_Rotation") != configJSON.end())
		{
			camRot = 
			{
				configJSON["Editor_Camera_Rotation"]["xRot"],
				configJSON["Editor_Camera_Rotation"]["yRot"],
				configJSON["Editor_Camera_Rotation"]["zRot"]
			};
		}
		else // is there is no camera rotation, put default
		{
			camRot =
			{
				15.0f,
				35.0f,
				0.0f
			};
		}

		editorCameraGameObject->transform->setPosition(camPos);
		editorCameraGameObject->transform->setLocalRotationEuler(camRot);

		file.close();
		delete[] tempChar;
	}
	catch (const std::exception& e)
	{
		std::cout << "ERROR: Problem loading EditorConfig.json." << std::endl;
		std::cout << e.what() << std::endl;
		std::cout << "Creating new EditorConfig.json" << std::endl;
		_mkdir("../Settings/Editor");

		editorConfig->firstSceneFilepath = "";
		SaveEditorConfig();
	}
}

void SceneEditor::SaveEditorConfig()
{
	OutputDebugStringW(L"Saving Editor Config...\n");
	std::string configPath = std::string(EDITOR_CONFIG_FILE_PATH);
	JSON configJSON;
	try
	{
		std::ofstream file(configPath);

		configJSON["First_Scene_Filepath"] = this->editorConfig->firstSceneFilepath.c_str();

		// save camera
		if (editorCameraGameObject != nullptr)
		{
			configJSON["Editor_Camera_Position"]["xPos"] = editorCameraGameObject->transform->getPosition().x;
			configJSON["Editor_Camera_Position"]["yPos"] = editorCameraGameObject->transform->getPosition().y;
			configJSON["Editor_Camera_Position"]["zPos"] = editorCameraGameObject->transform->getPosition().z;

			configJSON["Editor_Camera_Rotation"]["xRot"] = editorCameraGameObject->transform->getLocalRotationEuler().x;
			configJSON["Editor_Camera_Rotation"]["yRot"] = editorCameraGameObject->transform->getLocalRotationEuler().y;
			configJSON["Editor_Camera_Rotation"]["zRot"] = editorCameraGameObject->transform->getLocalRotationEuler().z;
		}
		else //default editor camera positions
		{
			configJSON["Editor_Camera_Position"]["xPos"] = -5.0f;
			configJSON["Editor_Camera_Position"]["yPos"] = 4.0f;
			configJSON["Editor_Camera_Position"]["zPos"] = -7.0f;

			configJSON["Editor_Camera_Rotation"]["xRot"] = 15.0f;
			configJSON["Editor_Camera_Rotation"]["yRot"] = 35.0f;
			configJSON["Editor_Camera_Rotation"]["zRot"] = 0.0f;
		}


		file << configJSON.dump(4) << std::endl;

		file.close();

		OutputDebugStringW(L"Saved Editor Config.\n");
		std::cout << "Saved EditorConfig.json with filePath: " << editorConfig->firstSceneFilepath << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << "ERROR: Problem Saving EditorConfig." << std::endl;
		OutputDebugStringW(L"Failed to Save EditorConfg.\n");
	}
}

void SceneEditor::LoadInitialEditorScene()
{
	bool exists = false;
	Scene_ptr scene(new Scene("TEMP_NAME"));
	if (editorConfig->firstSceneFilepath.empty() == false)
	{
		exists = SceneManager::getInstance().LoadSceneFromFile(*scene, editorConfig->firstSceneFilepath.c_str());
		if (exists)
		{
			std::cout << "Loading First Editor Scene from File!" << std::endl;
			// Activate Scene
			selectedGameObject = nullptr;
			SceneManager::getInstance().SetActiveScene(scene);
			return;
		}
	}
	if(exists == false)
	{
		std::cout << "Creating new Empty Scene" << std::endl;
		scene = SceneManager::getInstance().CreateNewScene();
		editorConfig->firstSceneFilepath = scene->filePath;

		SaveEditorConfig();

		// SAVE SCENE
		SceneManager::getInstance().SaveSceneToFile(*scene);

		// Activate Scene
		selectedGameObject = nullptr;
		SceneManager::getInstance().SetActiveScene(scene);
	}
	
}

#pragma endregion

#pragma region EDIT_MODE_CONTROL

void SceneEditor::StartEditMode()
{
	if (ApplicationManager::getInstance().IsEditMode() == false)
	{
		ApplicationManager::getInstance().SetEditMode(true);
		if (SceneManager::getInstance().GetActiveScene() != nullptr)
		{
			selectedGameObject = nullptr;
			SceneManager::getInstance().ReloadSceneFromFile();
		}
		else
		{
			selectedGameObject = nullptr;
			SceneEditor::getInstance().LoadInitialEditorScene();
			std::cout << "SCENE EDITOR: Press CTRL+SHIFT+E to Toggle Edit Mode." << std::endl;
		}
		
		std::cout << "ENTERING EDIT MODE =============================" << std::endl;
		std::cout << "\tCTRL+S: Save Scene" << std::endl;
		std::cout << "\tCTRL+P: Print Scene" << std::endl;
		std::cout << "\t[Q],[W],[E],[R]: Manipulate Selected Object." << std::endl;

		if (selectedGameObject == nullptr)
		{
			if (SceneManager::getInstance().GetActiveScene()->rootGameObjects.size() >= 1)
			{
				selectedGameObject = SceneManager::getInstance().GetActiveScene()->rootGameObjects[0];
				//std::cout << "Auto-Selected GameObject[0]: " << selectedGameObject->name << std::endl;
			}
		}
		RenderManager::getInstance().setCurrentCamera(editorCamera);

		Input::ShowCursor(true);
		Input::ResetMouseInput();
	}
}

void SceneEditor::ExitEditMode()
{
	if (ApplicationManager::getInstance().IsEditMode())
	{
		std::cout << "EXITING EDIT MODE =========================" << std::endl;
		selectedGameObject = nullptr;
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ApplicationManager::getInstance().SetEditMode(false);
		SceneManager::getInstance().ReloadSceneFromFile();
	}

	Input::ResetMouseInput();
}

void SceneEditor::UpdateEditor()
{
	// Swap edit mode
	if ((Input::GetKey(GLFW_KEY_LEFT_SHIFT) || Input::GetKey(GLFW_KEY_RIGHT_SHIFT))
		&& (Input::GetKey(GLFW_KEY_LEFT_CONTROL) || Input::GetKey(GLFW_KEY_RIGHT_CONTROL))
		&& Input::GetKeyDown(GLFW_KEY_E))
	{
		if (ApplicationManager::getInstance().IsEditMode())
		{
			ExitEditMode();
		}
		else
		{
			StartEditMode();
		}
		return;
	}

	if (ApplicationManager::getInstance().IsEditMode())
	{
		editorCameraGameObject->UpdateComponents();

		// Cursor Selection ======
		UpdateCursorSelection();

		// GUI ======
		UpdateGUI();

		// Keyboard Input ======
		if (Input::GetKey(GLFW_KEY_LEFT_CONTROL) || Input::GetKey(GLFW_KEY_RIGHT_CONTROL))
		{
			if (Input::GetKeyDown(GLFW_KEY_S))
			{
				// Might need to add some sort of "wait until it finished saving" functionality.
				SceneManager::getInstance().SaveActiveScene();
			}
			else if(Input::GetKeyDown(GLFW_KEY_P)) // Print Scene
			{
				SceneManager::getInstance().GetActiveScene()->PrintScene();
			}
		}
		else // Ctrl not pressed.
		{
			SelectManipTool();

			// Update Manip Tool
			if (selectedGameObject != nullptr)
			{
				ManipToolUpdate();
			}
		}
	}
}

void SceneEditor::DrawEditorGizmos()
{
	// TODO: Make it a global option of whether we draw gizmos or not.
	// TODO: Make drawing gizmos an event subsriber based model - not a callback.
	// Draw Gizmos
	if (SceneManager::getInstance().GetActiveScene() != nullptr && SceneManager::getInstance().GetActiveScene()->isLoaded)
	{
		for (GameObject_ptr go : SceneManager::getInstance().GetActiveScene()->allGameObjects)
		{
			for (Component_ptr c : go->components)
			{
				c->OnDrawGizmos();
			}
		}
	}
	// Draw SELECTED gizmos.
	if (selectedGameObject != nullptr)
	{
		selectedGameObject->transform->DrawGizmo();
		for (size_t i = 0; i < selectedGameObject->components.size(); i++)
		{
			selectedGameObject->components[i]->OnDrawGizmosSelected();
		}
	}
}
#pragma endregion

#pragma region MANIPULATION_TOOLS

void SceneEditor::SelectManipTool()
{
	if (Input::GetKeyDown(GLFW_KEY_Q))
	{
		manipTool = ManipToolMode::None;
		std::cout << "ManipTool: None." << std::endl;
	}
	else if (Input::GetKeyDown(GLFW_KEY_W))
	{
		manipTool = ManipToolMode::Move;
		std::cout << "ManipTool: Move." << std::endl;
	}
	else if (Input::GetKeyDown(GLFW_KEY_E))
	{
		manipTool = ManipToolMode::Rotate;
		std::cout << "ManipTool: Rotate." << std::endl;
	}
	else if (Input::GetKeyDown(GLFW_KEY_R))
	{
		manipTool = ManipToolMode::Scale;
		std::cout << "ManipTool: Scale." << std::endl;
	}
}

void SceneEditor::ManipToolUpdate()
{
	switch (manipTool)
	{
	case ManipToolMode::None:

		break;
	case ManipToolMode::Move:
		MoveTool();
		break;
	case ManipToolMode::Rotate:
		RotateTool();
		break;
	case ManipToolMode::Scale:
		ScaleTool();
		break;
	default:
		break;
	}
}

void SceneEditor::MoveTool()
{
	glm::vec3 forward = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	float moveSpeed = 2.5f;

	float deltaMove = moveSpeed * GameTime::deltaTime;

	if (Input::GetKey(GLFW_KEY_LEFT))
		selectedGameObject->transform->Translate(-glm::normalize(glm::cross(forward, up)) * deltaMove);
	if (Input::GetKey(GLFW_KEY_RIGHT))
		selectedGameObject->transform->Translate(glm::normalize(glm::cross(forward, up)) * deltaMove);

	if ((Input::GetKey(GLFW_KEY_LEFT_SHIFT) || Input::GetKey(GLFW_KEY_RIGHT_SHIFT)) == false)
	{
		if (Input::GetKey(GLFW_KEY_UP))
			selectedGameObject->transform->Translate(deltaMove * forward);
		if (Input::GetKey(GLFW_KEY_DOWN))
			selectedGameObject->transform->Translate(-deltaMove * forward);
	}
	else // Pressing Shift
	{
		if (Input::GetKey(GLFW_KEY_UP))
			selectedGameObject->transform->Translate(up * deltaMove);
		if (Input::GetKey(GLFW_KEY_DOWN))
			selectedGameObject->transform->Translate(-up * deltaMove);
	}
}

void SceneEditor::RotateTool()
{
	glm::vec3 forward = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	float rotSpeed = 25.0f;

	float deltaRot = rotSpeed * GameTime::deltaTime;

	if (Input::GetKey(GLFW_KEY_LEFT))
		selectedGameObject->transform->Rotate(glm::vec3(0, deltaRot, 0));
	if (Input::GetKey(GLFW_KEY_RIGHT))
		selectedGameObject->transform->Rotate(glm::vec3(0, -deltaRot, 0));

	if ((Input::GetKey(GLFW_KEY_LEFT_SHIFT) || Input::GetKey(GLFW_KEY_RIGHT_SHIFT)) == false)
	{
		if (Input::GetKey(GLFW_KEY_UP))
			selectedGameObject->transform->Rotate(glm::vec3(deltaRot, 0, 0));
		if (Input::GetKey(GLFW_KEY_DOWN))
			selectedGameObject->transform->Rotate(glm::vec3(-deltaRot, 0, 0));
	}
	else // Pressing Shift
	{
		if (Input::GetKey(GLFW_KEY_UP))
			selectedGameObject->transform->Rotate(glm::vec3(0, 0, -deltaRot));
		if (Input::GetKey(GLFW_KEY_DOWN))
			selectedGameObject->transform->Rotate(glm::vec3(0, 0, deltaRot));
	}
}

void SceneEditor::ScaleTool()
{
	glm::vec3 forward = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	float scaleSpeed = 1.0f;

	float deltaScale = scaleSpeed * GameTime::deltaTime;

	if (Input::GetKey(GLFW_KEY_LEFT))
		selectedGameObject->transform->setLocalScale(selectedGameObject->transform->getLocalScale() + (-glm::normalize(glm::cross(forward, up))*deltaScale));
		//selected->transform->Scale(-glm::normalize(glm::cross(forward, up)) * deltaScale);
	if (Input::GetKey(GLFW_KEY_RIGHT))
		selectedGameObject->transform->setLocalScale(selectedGameObject->transform->getLocalScale() + (glm::cross(forward, up)* deltaScale));

	if ((Input::GetKey(GLFW_KEY_LEFT_SHIFT) || Input::GetKey(GLFW_KEY_RIGHT_SHIFT)) == false)
	{
		if (Input::GetKey(GLFW_KEY_UP))
			selectedGameObject->transform->setLocalScale(selectedGameObject->transform->getLocalScale() + (deltaScale * forward));
		if (Input::GetKey(GLFW_KEY_DOWN))
			selectedGameObject->transform->setLocalScale(selectedGameObject->transform->getLocalScale() + (-deltaScale * forward));
	}
	else // Pressing Shift
	{
		if (Input::GetKey(GLFW_KEY_UP))
			selectedGameObject->transform->setLocalScale(selectedGameObject->transform->getLocalScale() + (up * deltaScale));
		if (Input::GetKey(GLFW_KEY_DOWN))
			selectedGameObject->transform->setLocalScale(selectedGameObject->transform->getLocalScale() + (-up * deltaScale));
	}
}
#pragma endregion

#pragma region Editor_GUI

// ===========================================================================================================================================================================
// Editor GUI
// ===========================================================================================================================================================================
static bool show_demo_menu = false;
static bool inspector_open = true;
static bool hierarchy_open = true;
static bool assetFolder_open = true;

bool layoutInitialized = false;
//bool  currentLayout_m = true;
//ImGuiID dockSpaceId_m;


void SceneEditor::UpdateCursorSelection()
{
	return;

	// PICKING IS DONE HERE
		// (Instead of picking each frame if the mouse button is down, 
		// you should probably only check if the mouse button was just released)
	if (PhysicsManager::getInstance().isInitialized == true) 
	{
		if (Input::GetMouseButtonUp(GLFW_MOUSE_BUTTON_LEFT))
		{
			glm::mat4 view = editorCamera->getView();
			//TODO: Rotate relative to the cameras facing.
			//glm::vec3 camForward = editorCamera->gameObject->transform->getForwardDirection();
			view = glm::rotate(view, 180.0f, glm::vec3(1, 0, 0));
			glm::vec2 mousePos = Input::GetMousePos();
			glm::vec3 out_origin;
			glm::vec3 out_direction;
			ScreenPosToWorldRay(
				mousePos.x, mousePos.y,
				//1024 / 2, 768 / 2,
				SCREEN_WIDTH, SCREEN_HEIGHT,
				//1024, 768,
				view,
				editorCamera->getProjection(),
				out_origin,
				out_direction
			);

			glm::vec3 out_end = out_origin + out_direction * 1000.0f;
			std::cout << "NumObjs: " << PhysicsManager::getInstance().dynamicsWorld->getNumCollisionObjects() << std::endl;
			btCollisionWorld::ClosestRayResultCallback RayCallback(btVector3(out_origin.x, out_origin.y, out_origin.z), btVector3(out_end.x, out_end.y, out_end.z));
			PhysicsManager::getInstance().dynamicsWorld->rayTest(btVector3(out_origin.x, out_origin.y, out_origin.z), btVector3(out_end.x, out_end.y, out_end.z), RayCallback);
			if (RayCallback.hasHit()) {
				//std::ostringstream oss;
				//RayCallback.m_collisionObject->getUserPointer();
				std::cout << "SELECTED: " << ((Rigidbody*)RayCallback.m_collisionObject->getUserPointer())->gameObject->name << std::endl;
				//oss << "mesh " << (size_t)RayCallback.m_collisionObject->getUserPointer();
				//message = oss.str();
			}
			else {
				//message = "background";
				std::cout << "Origin:\t" << out_origin.x << ", " << out_origin.y << ", " << out_origin.z << std::endl;
				std::cout << "End:\t" << out_end.x << ", " << out_end.y << ", " << out_end.z << std::endl;
				std::cout << "Dir:\t" << out_direction.x << ", " << out_direction.y << ", " << out_direction.z << std::endl;
				std::cout << "SELECTED: (background)" << std::endl;
			}
		}
	}
}


//https://github.com/opengl-tutorials/ogl/blob/master/misc05_picking/misc05_picking_BulletPhysics.cpp
void SceneEditor::ScreenPosToWorldRay(
	int mouseX, int mouseY,             // Mouse position, in pixels, from bottom-left corner of the window
	int screenWidth, int screenHeight,  // Window size, in pixels
	glm::mat4 ViewMatrix,               // Camera position and orientation
	glm::mat4 ProjectionMatrix,         // Camera parameters (ratio, field of view, near and far planes)
	glm::vec3& out_origin,              // Ouput : Origin of the ray. /!\ Starts at the near plane, so if you want the ray to start at the camera's position instead, ignore this.
	glm::vec3& out_direction            // Ouput : Direction, in world space, of the ray that goes "through" the mouse.
) {

	// The ray Start and End positions, in Normalized Device Coordinates (Have you read Tutorial 4 ?)
	glm::vec4 lRayStart_NDC(
		((float)mouseX / (float)screenWidth - 0.5f) * 2.0f, // [0,1024] -> [-1,1]
		((float)mouseY / (float)screenHeight - 0.5f) * 2.0f, // [0, 768] -> [-1,1]
		-1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
		1.0f
	);
	glm::vec4 lRayEnd_NDC(
		((float)mouseX / (float)screenWidth - 0.5f) * 2.0f,
		((float)mouseY / (float)screenHeight - 0.5f) * 2.0f,
		0.0,
		1.0f
	);

	//ViewMatrix = glm::rotate(ViewMatrix, 90.0f, glm::vec3(0, 1, 0));

	// The Projection matrix goes from Camera Space to NDC.
	// So inverse(ProjectionMatrix) goes from NDC to Camera Space.
	glm::mat4 InverseProjectionMatrix = glm::inverse(ProjectionMatrix);

	// The View Matrix goes from World Space to Camera Space.
	// So inverse(ViewMatrix) goes from Camera Space to World Space.
	glm::mat4 InverseViewMatrix = glm::inverse(ViewMatrix);

	glm::vec4 lRayStart_camera = InverseProjectionMatrix * lRayStart_NDC;    lRayStart_camera /= lRayStart_camera.w;
	glm::vec4 lRayStart_world = InverseViewMatrix * lRayStart_camera; lRayStart_world /= lRayStart_world.w;
	glm::vec4 lRayEnd_camera = InverseProjectionMatrix * lRayEnd_NDC;      lRayEnd_camera /= lRayEnd_camera.w;
	glm::vec4 lRayEnd_world = InverseViewMatrix * lRayEnd_camera;   lRayEnd_world /= lRayEnd_world.w;


	// Faster way (just one inverse)
	//glm::mat4 M = glm::inverse(ProjectionMatrix * ViewMatrix);
	//glm::vec4 lRayStart_world = M * lRayStart_NDC; lRayStart_world/=lRayStart_world.w;
	//glm::vec4 lRayEnd_world   = M * lRayEnd_NDC  ; lRayEnd_world  /=lRayEnd_world.w;


	glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
	lRayDir_world = glm::normalize(lRayDir_world);


	out_origin = glm::vec3(lRayStart_world);
	//out_origin.x *= -1;
	//out_origin.z *= -1;
	out_direction = glm::normalize(lRayDir_world);
}

void SceneEditor::UpdateGUI()
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//drawTestDock();
	//return;

	bool open_dockspace = true;
	UpdateDockSpace(&open_dockspace);
	InspectorUpdate();
	HierarchyUpdate();
	AssetFolderMenuUpdate();

	if (show_demo_menu)
	{
		ImGui::ShowDemoWindow();
	}
}

//-/////////////////////////////////////////////////////////////////////////////////////
// Defines and Updates Dockspace used for GUI. Other windows will dock to this.
// Most of this is copied from imgui_demo.cpp.
//
void SceneEditor::UpdateDockSpace(bool* p_open)
{
	//drawTestDock();
	//return;
	
	bool open = true;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	window_flags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DOCK_ROOT", &open, window_flags); // Start Root
	//ImGui::PopStyleVar();
	ImGui::PopStyleVar(3);
	ImGuiID dockSpaceId_m = ImGui::GetID("DOCK_SPACE");
	//if(false)
	if (layoutInitialized)
	{
		std::cout << "Creating Dock" << std::endl;
		ImGui::DockBuilderRemoveNode(dockSpaceId_m);
		ImGui::DockBuilderAddNode(dockSpaceId_m, ImGuiDockNodeFlags_None);// , ImGuiDockNodeFlags_Dockspace);
		ImGui::DockBuilderSetNodeSize(dockSpaceId_m, viewport->Size);

		ImGuiID dock_main_id = dockSpaceId_m;
		ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.20f, nullptr, &dock_main_id);
		ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.20f, nullptr, &dock_main_id);
		ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.20f, nullptr, &dock_main_id);

		//ImGui::DockBuilderDockWindow("Inspector", dock_id_right);

		ImGui::DockBuilderFinish(dockSpaceId_m);
		layoutInitialized = true;
	}

	//ImGuiID dockspace_id = ImGui::GetID(DOCKSPACE_ID.c_str());
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui::DockSpace(dockSpaceId_m, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruDockspace);
	//ImGui::End();
	ImGui::PopStyleVar();
	/*
	static bool opt_fullscreen_persistant = true;
	static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_None;
	bool opt_fullscreen = opt_fullscreen_persistant;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	// When using ImGuiDockNodeFlags_PassthruDockspace, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (opt_flags & ImGuiDockNodeFlags_PassthruDockspace)
		window_flags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("X-Engine Editor", p_open, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// Dockspace
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		//ImGuiID dockspace_id = ImGui::GetID("EditorDockSpace");
		//ImGui::DockSpace(dockspace_id, ImVec2(10.0f, 10.0f), opt_flags);

		ImGuiID dock_id = ImGui::DockBuilderAddNode(0, ImGuiDockNodeFlags_None);
		ImVec2 viewport_pos = ImGui::GetMainViewport()->Pos;
		ImGui::DockBuilderSetNodePos(dock_id, ImVec2(viewport_pos.x + 100, viewport_pos.y + 100));
		ImGui::DockBuilderSetNodeSize(dock_id, ImVec2(200, 200));
		ImGui::DockBuilderDockWindow("Inspector", dock_id);
		ImGui::DockBuilderFinish(dock_id);
	}
	else
	{
		//ShowDockingDisabledMessage();
	}
	opt_flags |= ImGuiDockNodeFlags_PassthruDockspace;
	*/


	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Scene"))
		{
			ImGui::Button("Open Scene...");
			if (ImGui::BeginPopupContextItem("Open Scene", 0))
			{
				static char sceneName[32];
				ImGui::Text("Scene Name:");
				ImGui::InputText("##edit", sceneName, 32);
				if (ImGui::Button("Open"))
				{
					Scene_ptr scene(new Scene("TEMP"));
					bool exists = SceneManager::getInstance().LoadSceneFromFileByName(*scene, sceneName);
					if (exists)
					{
						//std::cout << "Scene Found!" << std::endl;
						selectedGameObject = nullptr;
						// Activate Scene
						SceneManager::getInstance().SetActiveScene(scene);
						//std::cout << "LOADING NEW SCENE" << std::endl;
						editorConfig->firstSceneFilepath = scene->filePath;
					}
					else
					{
						std::cout << "Scene not found." << std::endl;
					}
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
			ImGui::Button("New Scene");
			if (ImGui::BeginPopupContextItem("New Scene", 0))
			{
				static char sceneName[32];
				ImGui::Text("Scene Name:");
				ImGui::InputText("##edit", sceneName, 32);
				if (ImGui::Button("Create"))
				{
					Scene_ptr scene(new Scene("TEMP"));
					bool exists = SceneManager::getInstance().LoadSceneFromFileByName(*scene, sceneName);
					if (!exists)
					{
						scene = SceneManager::getInstance().CreateNewScene(sceneName);
						editorConfig->firstSceneFilepath = scene->filePath;
						SaveEditorConfig();

						// SAVE SCENE
						SceneManager::getInstance().SaveSceneToFile(*scene);

						selectedGameObject = nullptr;
						// Activate Scene
						SceneManager::getInstance().SetActiveScene(scene);
					}
					else
					{
						std::cout << "Scene already exists." << std::endl;
					}
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
			ImGui::EndMenu();
		}
		// CREATE MENU =============================================================
		if (ImGui::BeginMenu("Create"))
		{
			// TODO: Create a way to add to this menu from another file.
			if (ImGui::MenuItem("Empty GameObject"))
			{
				Scene_ptr scene = SceneManager::getInstance().GetActiveScene();
				GameObject_ptr go = scene->CreateGameObject("New GameObject");
				selectedGameObject = go;
			}
			if (ImGui::MenuItem("New First-Person Camera Example"))
			{
				Scene_ptr scene = SceneManager::getInstance().GetActiveScene();
				GameObject_ptr go = scene->CreateGameObject("New First-Person Camera Example");
				selectedGameObject = go;
				std::shared_ptr<CameraComponent> cameraComponent(new CameraComponent());
				go->AddComponent(cameraComponent);
				std::shared_ptr<CameraControllerComponent> cameraControllerComponent(new CameraControllerComponent());
				go->AddComponent(cameraControllerComponent);
			}
			if (ImGui::MenuItem("Point Light"))
			{
				Scene_ptr scene = SceneManager::getInstance().GetActiveScene();
				GameObject_ptr go = scene->CreateGameObject("New Point Light");
				selectedGameObject = go;
				std::shared_ptr<LightComponent> pLight(new PointLightComponent());
				go->AddComponent(pLight);
			}
			if (ImGui::MenuItem("Global Light"))
			{
				Scene_ptr scene = SceneManager::getInstance().GetActiveScene();
				GameObject_ptr go = scene->CreateGameObject("New Global Light");
				selectedGameObject = go;
				std::shared_ptr<LightComponent> gLight(new GlobalLightComponent());
				go->AddComponent(gLight);
			}
			if (ImGui::MenuItem("Spot Light"))
			{
				Scene_ptr scene = SceneManager::getInstance().GetActiveScene();
				GameObject_ptr go = scene->CreateGameObject("New Spot Light");
				selectedGameObject = go;
				std::shared_ptr<LightComponent> sLight(new SpotLightComponent());
				go->AddComponent(sLight);
			}
			if (ImGui::MenuItem("Camera"))
			{
				Scene_ptr scene = SceneManager::getInstance().GetActiveScene();
				GameObject_ptr go = scene->CreateGameObject("Camera");
				selectedGameObject = go;
				
				std::shared_ptr<CameraComponent> cam(new CameraComponent());
				go->AddComponent(cam);
				std::shared_ptr<AudioListener> listener(new AudioListener());
				go->AddComponent(listener);
			}
			if (ImGui::MenuItem("New Model Metal Crate"))
			{
				Scene_ptr scene = SceneManager::getInstance().GetActiveScene();
				GameObject_ptr go = scene->CreateGameObject("New Model Metal Crate");
				selectedGameObject = go;
				std::shared_ptr<MeshRenderer> crate(new MeshRenderer("3Dmodel/MetalCrate/cube.obj|Cube"));
				go->AddComponent(crate);
				
			}
			if (ImGui::MenuItem("New Nanosuit"))
			{
				Scene_ptr scene = SceneManager::getInstance().GetActiveScene();
				std::string objPath = "3Dmodel/nanosuit/nanosuit.obj";
				GameObject_ptr go = AssetManager::getInstance().modelLib.GenerateModelGameObject("3Dmodel/nanosuit/nanosuit.obj");
				selectedGameObject = go;
			}			

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Code"))
		{
			if (ImGui::Button("Reload"))
			{
				// TODO: Finish this...?
				std::cout << "Reload" << std::endl;
				//std::string buildCommand;
				ProjectCompiler::getInstance().Recompile();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Build"))
		{
			if (ImGui::Button("Build Project"))
			{
				// TODO: Finish this...?
				std::cout << "'Build Project' command currently doesn't do anything." << std::endl;
				//std::string buildCommand;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Misc"))
		{
			ImGui::Checkbox("Show Demo Menu", &show_demo_menu);
			ImGui::EndMenu();
		}


		ImGui::EndMenuBar();
	}

	ImGui::End(); // End Root
}

//-/////////////////////////////////////////////////////////////////////////////////////
// Defines & Updates the Inspector Window. 
// This window allows us to edit the selected gameObject.
//
void SceneEditor::InspectorUpdate()
{
	// We specify a default position/size in case there's no data in the .ini file. Typically this isn't required! 
	// We only do it to make the Demo applications a little more welcoming.
	//ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_Once);
	//ImGui::SetNextWindowSize(ImVec2(250, 680), ImGuiCond_Once);// , ImGuiCond_FirstUseEver);
	//ImGui::SetNextWindowDockID(dockSpaceId_m, ImGuiCond_Once);

	ImGuiID dockspace_id = ImGui::GetID("DOCK_SPACE");
	ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_Always);

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_MenuBar;
	// Main body of the Demo window starts here.
	if (!ImGui::Begin("Inspector", &inspector_open, window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}
/*
	ImGuiID dockspace_id = ImGui::GetID("EditorDockSpace");

	ImGui::DockBuilderRemoveNode(dockspace_id);
	ImGuiDockNodeFlags dockSpaceFlags = 0;
	dockSpaceFlags |= ImGuiDockNodeFlags_PassthruDockspace;
	ImGui::DockBuilderAddNode(dockspace_id, dockSpaceFlags);

	ImGuiID dockMain = dockspace_id;
	ImGuiID dockLeft = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Left, 0.40f, NULL, &dockMain);*/


	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			if (ImGui::MenuItem("Delete GameObject", "CTRL+D", false, (selectedGameObject != nullptr)))
			{
				if (selectedGameObject != nullptr)
				{
					SceneManager::getInstance().GetActiveScene()->DeleteGameObject(selectedGameObject);
					selectedGameObject = nullptr;
				}
			}
			if (ImGui::MenuItem("Create Prefab", "CTRL+D", false, (selectedGameObject != nullptr)))
			{
				if (selectedGameObject != nullptr)
				{
					GameObject::CreatePrefab(selectedGameObject);
					//SceneManager::getInstance().GetActiveScene()->DeleteGameObject(selectedGameObject);
					//selectedGameObject = nullptr;
				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			ImGui::Text("Nobody can help you now.");
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	// Inspect Selected GameObject
	if (selectedGameObject != nullptr)
	{
		bool active = selectedGameObject->IsActiveInHierarchy();
		ImGui::Checkbox("Active", &active);
		if (active != selectedGameObject->IsActiveInHierarchy())
		{
			selectedGameObject->SetActive(active);
		}
		// Edit Name of Selected GameObject
		char buf[64];
		sprintf(buf, "GameObject: %s###Button", selectedGameObject->name.c_str()); // ### operator override ID ignoring the preceding label
		ImGui::Button(buf);
		if (ImGui::BeginPopupContextItem("gameObject_edit_name", 0))
		{
			ImGui::Text("Edit name:");
			//SceneEditor::InputTextField(selectedGameObject->name, "##edit");
			ImGui::InputText("##edit", &selectedGameObject->name);
			ImGui::EndPopup();
		}
		ImGui::Spacing();

		// Draw Transform Inspector
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
		{
			selectedGameObject->transform->DrawInspector();
		}

		// Draw Component Inspectors
		for (size_t i = 0; i < selectedGameObject->components.size(); i++)
		{
			try
			{
				typeid(*selectedGameObject->components[i]);
			}
			catch (const std::exception&)
			{
				continue;
			}

			std::string componentTypeName = Component::registry()[typeid(*selectedGameObject->components[i])].name;
			bool isOpen = ImGui::CollapsingHeader(componentTypeName.c_str(), ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen);
			
			if (ImGui::IsItemClicked(1))
			{
				std::cout << "Right clicked component." << std::endl;

				ImGui::OpenPopup(componentTypeName.c_str());

			}
			if (ImGui::BeginPopup(componentTypeName.c_str()))
			{
				if (ImGui::Button("Delete"))
				{
					selectedGameObject->RemoveComponent(selectedGameObject->components[i]);
					//break;
					isOpen = false;
				}
				ImGui::EndPopup();
			}

			if(isOpen)
			{
				selectedGameObject->components[i]->DrawInspector();
			}
		}
		ImGui::Spacing();

		// Add Component Dropdown Menu
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::SameLine(50.0f);
		if (ImGui::Button("Add Component..."))
			ImGui::OpenPopup("add_component_menu");
		if (ImGui::BeginPopup("add_component_menu"))
		{
			std::vector<ComponentTypeInfo> componentTypes;
			for (std::pair<std::type_index, ComponentTypeInfo> element : Component::registry())
			{
				//std::cout << element.first.name() << ": " << element.second.name << std::endl;
				componentTypes.push_back(element.second); // TODO: fix bug that causes engine to crash when simple model component is chosen
			}
			ImGui::Text("Add Component:");
			ImGui::Separator();
			int menuSelectIndex = -1;
			for (int i = 0; i < componentTypes.size(); i++)
			{
				if (ImGui::Selectable(componentTypes[i].name.c_str()))
				{
					menuSelectIndex = i;
					break;
				}
			}
			if (menuSelectIndex > -1)
			{
				selectedGameObject->AddComponent(componentTypes[menuSelectIndex].Constructor());
				std::cout << "Added new '" << componentTypes[menuSelectIndex].name << "' to " << selectedGameObject->name << std::endl;
			}
			ImGui::EndPopup();
		}
	}
	else
	{
		ImGui::Text("No GameObject Selected.");
	}

	ImGui::End();
}

//-/////////////////////////////////////////////////////////////////////////////////////
// Defines & Updates the Hierarchy Window. 
// This window allows us to quickly select GameObjects.
//
void SceneEditor::HierarchyUpdate()
{
	// We specify a default position/size in case there's no data in the .ini file. 
	//Typically this isn't required! We only do it to make the Demo applications a little more welcoming.
	ImGui::SetNextWindowPos(ImVec2(0, 50), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(250, 680), ImGuiCond_Once);
	ImGui::SetNextWindowDockID(ImGui::GetWindowDockID(), ImGuiCond_Once);

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_MenuBar;

	// Main body of the Demo window starts here.
	if (!ImGui::Begin("Heirarchy", &hierarchy_open, window_flags))//, p_open, window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	// Scene Hierarchy.
	Scene_ptr scene = SceneManager::getInstance().GetActiveScene();
	if (scene != nullptr)
	{
		char buf[64]; 
		sprintf(buf, "Scene: %s###Button", scene->name.c_str()); // ### operator override ID ignoring the preceding label
		ImGui::Button(buf);
		if (ImGui::BeginPopupContextItem("scene_edit_name", 0))
		{
			ImGui::Text("Edit name:");
			ImGui::InputText("##edit", &scene->name);
			ImGui::EndPopup();
		}
		// Draw Tree structure.
		ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() * 3); // Increase spacing to differentiate leaves from expanded contents.
		for (int i = 0; i < scene->rootGameObjects.size(); i++)
		{
			DrawGameObjectTreeNode(scene->rootGameObjects[i].get(), "[" + std::to_string(i) + "]");
		}

		ImGui::PopStyleVar();
	}
	else
	{
		ImGui::Text("No Scene Loaded.");
	}

	// Hovering over empty part of hierarchy.
	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
	{
		const ImGuiPayload* payload = ImGui::GetDragDropPayload();
		// Dragging GameObject
		if (payload != nullptr && payload->IsDataType("GAMEOBJECT_DRAG"))
		{
			ImGui::Spacing();
			ImGui::Indent();
			ImGui::Text("<----- UNPARENT ----->");
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT_DRAG"))
				{
					IM_ASSERT(payload->DataSize == sizeof(GameObject_ptr));
					GameObject_ptr payload_n = *(const GameObject_ptr*)payload->Data;
					std::cout << "Dropping " << payload_n->name << " on empty." << std::endl;

					payload_n->transform->SetParent(nullptr);
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::Unindent();
		}

		// Dragging File.
		if (payload != nullptr && payload->IsDataType("FILE_DRAG"))
		{
			ImGui::Spacing();
			ImGui::Indent();
			ImGui::Text("<----- CREATE ----->");
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_DRAG"))
				{
					IM_ASSERT(payload->DataSize == 128);		// Note: This data size might be too small.
					const char* payload_n = (const char*)payload->Data;
					std::string filePath(payload_n);

					// Get file extension.
					std::string ext = filePath.substr(filePath.find_last_of("."));

					// Drop .prefab.
					if (ext == ".prefab")
					{
						auto instance = GameObject::InstantiatePrefab(filePath);
						selectedGameObject = instance;
					}

					// Drop .obj or .fbx
					if (ext == ".obj" || ext == ".fbx" || ext == ".FBX")
					{
						std::cout << "Drag and Drop: " +  filePath << std::endl;

						std::replace(filePath.begin(), filePath.end(), '\\', '/');

						Scene_ptr scene = SceneManager::getInstance().GetActiveScene();
						GameObject_ptr go = AssetManager::getInstance().modelLib.GenerateModelGameObject(filePath);
						selectedGameObject = go;
					}

				}
				ImGui::EndDragDropTarget();
			}
			ImGui::Unindent();
		}
	}

	ImGui::End();
}

void SceneEditor::DrawGameObjectTreeNode(GameObject * go, std::string label)
{
	//Node Flags:
	// If no children, display as leaf. Otherwise, an openable tree.
	// If selected, display as selected.
	ImGuiTreeNodeFlags node_flags = ((go->transform->GetChildCount() > 0)?
		(ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_DefaultOpen)
		: ImGuiTreeNodeFlags_Leaf)
		| ((go == selectedGameObject.get()) ? ImGuiTreeNodeFlags_Selected : 0);
		//| ((selection_mask & (1 << i)) ? ImGuiTreeNodeFlags_Selected : 0);

	// Gray out the object if its inactive.
	if (!go->IsActiveInHierarchy())
	{
		ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::ImColor(0.5f, 0.5f, 0.5f));
	}

	bool node_open = ImGui::TreeNodeEx((label + ": " + go->name).c_str(), node_flags); // THE BUTTON


	ImGuiContext& g = *ImGui::GetCurrentContext();
	if(g.IO.MouseReleased[0] && ImGui::IsItemHovered(ImGuiHoveredFlags_None)) // Equivalent to IsItemClicked with release.
	//if (ImGui::IsItemClicked())
	{
		selectedGameObject = go->GetSelfPtr();
	}

	// Right Click Selected Object
	if (ImGui::BeginPopupContextItem("Game Object Context", 1))
	{
		selectedGameObject = go->GetSelfPtr();

		// Duplicate Object
		if (ImGui::Button("Duplicate")) 
		{
			// add game object to scene
			// change name to differentiate
			GameObject_ptr dupli = GameObject::Duplicate(go->GetSelfPtr());
			std::cout << "EXITED DUPLICATE" << std::endl;
			selectedGameObject = dupli;

			ImGui::CloseCurrentPopup();
		}

		// issues with right click delete
		// 1) if u delete anything that is being used in GameObjectReference/ComponentReference,
		//	  app will break
		// 2) after deletion, gizmo (and sometimes collider) is there until another object is selected
		if (ImGui::Button("Delete"))
		{
			// delete game object from scene
			SceneManager::getInstance().GetActiveScene()->DeleteGameObject(selectedGameObject);
			//selectedGameObject = nullptr;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
	if (!go->IsActiveInHierarchy())
	{
		ImGui::PopStyleColor(1);
	}
	// Our buttons are both drag sources and drag targets here!
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		//if (go->GetSelfPtr() == selectedGameObject)
		{
			GameObject_ptr goPtr = go->GetSelfPtr();
			ImGui::SetDragDropPayload("GAMEOBJECT_DRAG", &goPtr, sizeof(GameObject_ptr));        // Set payload to carry our item 
			ImGui::Text("Moving %s", goPtr->name.c_str());
		}
		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT_DRAG"))
		{
			IM_ASSERT(payload->DataSize == sizeof(GameObject_ptr));
			GameObject_ptr payload_n = *(const GameObject_ptr*)payload->Data;
			std::cout << "Dropping " << payload_n->name << " on " << go->name << "." << std::endl;

			payload_n->transform->SetParent(go->transform);
		}
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_DRAG"))
		{

			IM_ASSERT(payload->DataSize == 128);
			const char* payload_n = (const char*)payload->Data;
			std::string filePath(payload_n);

			if (filePath.substr(filePath.find_last_of(".")) == ".prefab")
			{
				auto instance = GameObject::InstantiatePrefab(filePath);
				instance->transform->SetParent(go->transform);
				selectedGameObject = instance;
			}
		}
		ImGui::EndDragDropTarget();
	}

	if (node_open)
	{
		std::vector<GameObject*> children = go->GetChildren();
		for (size_t i = 0; i < children.size(); i++)
		{
			this->DrawGameObjectTreeNode(children[i], label + "["+ std::to_string(i)+"]");
		}
		ImGui::TreePop();
	}
}

void SceneEditor::AssetFolderMenuUpdate()
{
	ImGui::SetNextWindowPos(ImVec2(250, 680), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(680, 250), ImGuiCond_Once);
	ImGui::SetNextWindowDockID(ImGui::GetWindowDockID(), ImGuiCond_Once);

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_MenuBar;

	// Main body of the Demo window starts here.
	if (!ImGui::Begin("AssetFolderMenu", &assetFolder_open, window_flags))//, p_open, window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	//std::cout << ASSET_FILE_PATH << std::endl;
	for (const auto & entry : std::filesystem::directory_iterator(ASSET_FILE_PATH.c_str()))
	{
		//std::cout << entry.path() << std::endl;
		if (entry.is_directory())
		{
			DrawDirectoryTreeNode(entry.path().string().c_str());
		}
		else if(entry.is_regular_file())
		{
			DrawFileTreeNode(entry.path().string().c_str());
		}
	}
		


	ImGui::End();
}

// Temp static utility - move to a seperate header later.
static std::string GetFileNameFromPath(const char* path)
{
	char sep = '/';
	std::string s(path);
	std::replace(s.begin(), s.end(), '\\', '/');
	size_t i = s.rfind(sep, s.length());
	if (i != std::string::npos) {
		return s.substr(i + 1, s.length() - i);
	}
	return ("");
}

void SceneEditor::DrawDirectoryTreeNode(const char * directory)
{
	bool isEmpty = std::filesystem::is_empty(directory);
	//Node Flags:
	// If no children, display as leaf. Otherwise, an openable tree.
	// If selected, display as selected.
	ImGuiTreeNodeFlags node_flags = ((!isEmpty) ?
		(ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick)
		: ImGuiTreeNodeFlags_Leaf);
		//| ((go == selectedGameObject.get()) ? ImGuiTreeNodeFlags_Selected : 0);
	//| ((selection_mask & (1 << i)) ? ImGuiTreeNodeFlags_Selected : 0);

	std::string fileName = GetFileNameFromPath(directory);
	
	std::string createMenuPopupName("DIRECTORY_CREATE_");
	createMenuPopupName += directory;

	// Node
	bool node_open = ImGui::TreeNodeEx(directory, node_flags, "%s", fileName.c_str());

	// Create Menu Popup.
	if (ImGui::IsItemClicked(1))
	{
		//std::cout << "Right clicked " << directory << std::endl;
		
		ImGui::OpenPopup(createMenuPopupName.c_str());
		
	}
	if (ImGui::BeginPopup(createMenuPopupName.c_str()))
	{
		static std::string materialName = "New Material";
		ImGui::InputText("Name", &materialName);
		if (ImGui::Button("Create Material"))
		{
			//std::cout << materialName << std::endl;
			Material* mat = AssetManager::getInstance().materialLib.GetAsset(ASSET_FILE_PATH + fileName + "/" + materialName + ".material");
			ImGui::CloseCurrentPopup();
			//ImGui::OpenPopup((createMenuPopupName + "MATERIAL").c_str());
		}
		ImGui::EndPopup();
	}

	if (node_open)
	{
		for (const auto & entry : std::filesystem::directory_iterator(directory))
		{
			//std::cout << entry.path() << std::endl;
			if (entry.is_directory())
			{
				//entry.path().
				DrawDirectoryTreeNode(entry.path().string().c_str());
			}
			else if (entry.is_regular_file())
			{
				DrawFileTreeNode(entry.path().string().c_str());
			}
		}
		ImGui::TreePop();
	}
	
}

void SceneEditor::DrawFileTreeNode(const char * directory)
{
	//TODO: Add Drag-and-Drop behavior, and unique behavior for files like scenes and materials.
	std::string fileName = GetFileNameFromPath(directory);
	bool node_open = ImGui::TreeNodeEx((fileName).c_str(), ImGuiTreeNodeFlags_Leaf);
	if (ImGui::IsItemClicked())
	{
		// Do Something on click.
		if (ImGui::IsMouseDoubleClicked(0))
		{
			if (fileName.substr(fileName.find_last_of(".")) == ".scene") 
			{
				// Load Scene.
				if (SceneManager::getInstance().LoadAndActivateSceneFromFile(directory))
				{
					selectedGameObject = nullptr;
					editorConfig->firstSceneFilepath = SceneManager::getInstance().GetActiveScene()->filePath;
				}
			}
			if (fileName.substr(fileName.find_last_of(".")) == ".material")
			{
				// File Inspector Popup?
			}
		}
	}
	// Drag file.
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))//ImGuiDragDropFlags_None))
	{
		//TODO: Remove ASSET_FILE_PATH from directory.
		//std::string assetPath = std::string(directory).substr();
		//std::cout << "Creating Payload: " << directory << std::endl;
		ImGui::SetDragDropPayload("FILE_DRAG", directory, 128);        // Set payload to carry our item 
		ImGui::Text("%s", fileName.c_str());
		ImGui::EndDragDropSource();
	}

	if (node_open)
	{
		ImGui::TreePop();
	}
}

// NOT IMPLEMENTED YET
// TODO: Change the window layout depending on being in X_EDIT_MODE
void SceneEditor::ConfigureWindowLayout()
{
	//// Set position and size of console.
	//int border_thickness = GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CYCAPTION);
	//RECT screenDimensions; //Without Taskbar
	//BOOL fResult = SystemParametersInfo(SPI_GETWORKAREA, 0, &screenDimensions, 0);
	//HWND console = GetConsoleWindow();
	//int consoleWidth = 600;
	//int consoleHeight = screenDimensions.bottom - border_thickness;
	//int consoleX = 0;
	//int consoleY = 0;// border_thickness;
	//MoveWindow(console, consoleX, consoleY, consoleWidth, consoleHeight, TRUE);


	//// Set position of window.
	//int appPosX = consoleWidth;
	//int appPosY = border_thickness; // border_thickness
	//glfwSetWindowPos(ApplicationManager::APP_WINDOW, appPosX, appPosY);
}

#pragma endregion



}
