#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"
#include <string>
#include "BufferStructs.h"
#include "Material.h"

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// This code assumes files are in "ImGui" subfolder!
// Adjust as necessary for your own folder structure and project setup
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Called once per program, after the window and graphics API
// are initialized but before the game loop begins
// --------------------------------------------------------
void Game::Initialize()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	LoadAssets();

	// Set initial graphics API state
	//  - These settings persist until we change them
	//  - Some of these, like the primitive topology & input layout, probably won't change
	//  - Others, like setting shaders, will need to be moved elsewhere later
	{
		// Tell the input assembler (IA) stage of the pipeline what kind of
		// geometric primitives (points, lines or triangles) we want to draw.  
		// Essentially: "What kind of shape should the GPU draw with our vertices?"
		Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	// Initialize ImGui itself & platform/renderer backends
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(Window::Handle());
	ImGui_ImplDX11_Init(Graphics::Device.Get(), Graphics::Context.Get());
	// Pick a style (uncomment one of these 3)
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();
	//ImGui::StyleColorsClassic();

	// Set initial background color
	color[0] = 0.4f;
	color[1] = 0.6f;
	color[2] = 0.75f;
	color[3] = 0.0f;

	// Create cameras
	cameras.push_back(std::make_shared<Camera>(XMFLOAT3(0.0f, 0.0f, -1.0f), 1.0f, 1.0f, XM_PIDIV2, Window::AspectRatio()));
	cameras.push_back(std::make_shared<Camera>(XMFLOAT3(1.0f, 0.0f, -1.0f), 1.0f, 1.0f, XM_PIDIV2, Window::AspectRatio()));
	cameras.push_back(std::make_shared<Camera>(XMFLOAT3(-1.0f, 0.0f, -1.0f), 1.0f, 1.0f, XM_PIDIV2, Window::AspectRatio()));

	activeCam = cameras[0];

	activeCamIndex = 0;
}


// --------------------------------------------------------
// Clean up memory or objects created by this class
// 
// Note: Using smart pointers means there probably won't
//       be much to manually clean up here!
// --------------------------------------------------------
Game::~Game()
{
	// ImGui clean up
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}


// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{

}


// --------------------------------------------------------
// Loads assets: shaders, entities, etc
// --------------------------------------------------------
void Game::LoadAssets()
{
	// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	XMFLOAT4 black = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 white = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4 purple = XMFLOAT4(0.5f, 0.0f, 1.0f, 1.0f);
	XMFLOAT4 yellow = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);

	// Load Shaders
	std::shared_ptr<SimpleVertexShader> vs = std::make_shared<SimpleVertexShader>(
		Graphics::Device, Graphics::Context, FixPath(L"VertexShader.cso").c_str());
	std::shared_ptr<SimplePixelShader> tintPS = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"PixelShader.cso").c_str());
	std::shared_ptr<SimplePixelShader> uvPS = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"DebugUVsPS.cso").c_str());
	std::shared_ptr<SimplePixelShader> normalPS = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"DebugNormalsPS.cso").c_str());

	// Create materials
	std::shared_ptr<Material> mat1 = std::make_shared<Material>(XMFLOAT4(0.5f, 0.2f, 1.0f, 1.0f), vs, tintPS);
	std::shared_ptr<Material> mat2 = std::make_shared<Material>(XMFLOAT4(1.0f, 0.3f, 0.0f, 1.0f), vs, tintPS);
	std::shared_ptr<Material> mat3 = std::make_shared<Material>(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), vs, uvPS);
	std::shared_ptr<Material> mat4 = std::make_shared<Material>(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), vs, normalPS);

	std::shared_ptr<Mesh> cube = std::make_shared<Mesh>(FixPath("../../Assets/Meshes/cube.obj").c_str(), "Cube");
	std::shared_ptr<Mesh> sphere = std::make_shared<Mesh>(FixPath("../../Assets/Meshes/sphere.obj").c_str(), "Sphere");
	std::shared_ptr<Mesh> cylinder = std::make_shared<Mesh>(FixPath("../../Assets/Meshes/cylinder.obj").c_str(), "Cylinder");
	std::shared_ptr<Mesh> torus = std::make_shared<Mesh>(FixPath("../../Assets/Meshes/torus.obj").c_str(), "Torus");
	std::shared_ptr<Mesh> helix = std::make_shared<Mesh>(FixPath("../../Assets/Meshes/helix.obj").c_str(), "Helix");

	meshes.push_back(cube);
	meshes.push_back(sphere);
	meshes.push_back(cylinder);
	meshes.push_back(torus);
	meshes.push_back(helix);

	entities.push_back(GameEntity(cube, mat1));
	entities.push_back(GameEntity(sphere, mat2));
	entities.push_back(GameEntity(helix, mat3));
	entities.push_back(GameEntity(torus, mat3));
	entities.push_back(GameEntity(cylinder, mat4));
	entities.push_back(GameEntity(helix, mat4));

	// Move entities into starting positions
	entities[0].GetTransform()->MoveAbsolute(-2, 3, 5);
	entities[1].GetTransform()->MoveAbsolute(2, 3, 5);
	entities[2].GetTransform()->MoveAbsolute(-2, 0, 5);
	entities[3].GetTransform()->MoveAbsolute(2, 0, 5);
	entities[4].GetTransform()->MoveAbsolute(-2, -3, 5);
	entities[5].GetTransform()->MoveAbsolute(2, -3, 5);
}


// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	// Only update after cameras have been made
	if (!cameras.empty())
	{
		// Update all cameras
		for (std::shared_ptr<Camera> cam : cameras) 
		{
			cam->UpdateProjectionMatrix(Window::AspectRatio());
		}
	}
}


void Game::UpdateImGui(float deltaTime) {
	// Feed fresh data to ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;
	io.DisplaySize.x = (float)Window::Width();
	io.DisplaySize.y = (float)Window::Height();
	// Reset the frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// Determine new input capture
	Input::SetKeyboardCapture(io.WantCaptureKeyboard);
	Input::SetMouseCapture(io.WantCaptureMouse);
	// Show the demo window
	if (imGuiDemoVisible) {
		ImGui::ShowDemoWindow();
	}
}

// Custom inspector window
// Contains debug info and some general settings
void Game::UpdateInspector(float deltaTime, float totalTime) {

	// Create a new window
	ImGui::Begin("Inspector");

	// General info
	if (ImGui::CollapsingHeader("Program Details")) 
	{
		// Display framerate
		ImGui::Text("Framerate: %f fps", ImGui::GetIO().Framerate);

		// Display resolution
		ImGui::Text("Window Size: %dx%d", Window::Width(), Window::Height());

		// Display elapsed time
		ImGui::Text("Elapsed time: %f", totalTime);

		// Button to display demo window
		if (ImGui::Button("Toggle Demo Window")) {
			imGuiDemoVisible = !imGuiDemoVisible;
		}

		// Button for closing program
		if (ImGui::Button("Exit Program")) {
			Window::Quit();
		}
	}

	// Camera details
	if (ImGui::CollapsingHeader("Camera info")) 
	{
		// Position of the camera
		ImGui::Text("Cam Position: %f,%f,%f", activeCam->GetTransform()->GetPosition().x,
			activeCam->GetTransform()->GetPosition().y,
			activeCam->GetTransform()->GetPosition().z);

		// Camera field of view in degrees
		ImGui::Text("Cam FOV: %f", activeCam->GetFOV() * 180/XM_PI);

		// Change camera
		if (ImGui::Button("Prev")) 
		{
			if (activeCamIndex == 0) activeCamIndex = (int)(cameras.size() - 1);
			else activeCamIndex--;

			activeCam = cameras[activeCamIndex];
		}

		if (ImGui::Button("Next"))
		{
			if (activeCamIndex == (cameras.size() - 1)) activeCamIndex = 0;
			else activeCamIndex++;

			activeCam = cameras[activeCamIndex];
		}
	}

	// Mesh details
	if (ImGui::CollapsingHeader("Mesh info")) 
	{
		for (int i = 0; i < meshes.size(); i++)
		{

			// I had trouble with getting the names to display because of the use of std::string, this conversion method worked but there might be a better way
			std::string tabName = meshes[i].get()->GetName();
			const char* charName = tabName.c_str();

			// Get verts and indices
			int vertexCount = meshes[i].get()->GetVertexCount();
			int indexCount = meshes[i].get()->GetIndexCount();

			// Display info
			if (ImGui::TreeNode(charName)) 
			{
				ImGui::Text("Triangles: %d", indexCount/3);
				ImGui::Text("Verticies: %d", vertexCount);
				ImGui::Text("Indices: %d", indexCount);

				ImGui::TreePop();
			}
		}
	}

	if (ImGui::CollapsingHeader("Entities")) 
	{
		for (int i = 0; i < entities.size(); i++) 
		{
			// Make entity name
			std::string nameStr = "Entity " + std::to_string(i);
			const char* name = nameStr.c_str();

			// Get position, rotation, and scale, and store them as float arrays since thats what ImGui uses
			// temporary XMFLOAT3s made first for readability
			XMFLOAT3 pos = entities[i].GetTransform().get()->GetPosition();
			XMFLOAT3 rot = entities[i].GetTransform().get()->GetPitchYawRoll();
			XMFLOAT3 scale = entities[i].GetTransform().get()->GetScale();
			float posArray[3] = { pos.x, pos.y, pos.z };
			float rotArray[3] = { rot.x, rot.y, rot.z };
			float scaleArray[3] = { scale.x, scale.y, scale.z };

			// Display info and change if value changes
			if (ImGui::TreeNode(name)) 
			{
				if ( ImGui::SliderFloat3("Position", posArray, -2.0f, 2.0f) )
					entities[i].GetTransform().get()->SetPosition(posArray[0], posArray[1], posArray[2]);
				if ( ImGui::SliderFloat3("Rotation", rotArray, -4.0f, 4.0f) )
					entities[i].GetTransform().get()->SetRotation(rotArray[0], rotArray[1], rotArray[2]);
				if ( ImGui::SliderFloat3("Scale", scaleArray, 0.0f, 2.0f) )
					entities[i].GetTransform().get()->SetScale(scaleArray[0], scaleArray[1], scaleArray[2]);
				ImGui::TreePop();
			}
		}
	}

	// Color and offset editors
	if (ImGui::CollapsingHeader("Editors"))
	{
		// Background color editor
		ImGui::ColorEdit4("Background color", color);
	}

	ImGui::End();
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	UpdateImGui(deltaTime);

	UpdateInspector(deltaTime, totalTime);

	// Example input checking: Quit if the escape key is pressed
	if (Input::KeyDown(VK_ESCAPE))
		Window::Quit();

	activeCam->Update(deltaTime);
}


// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*
	{
		Graphics::Context->ClearRenderTargetView(Graphics::BackBufferRTV.Get(),	color);
		Graphics::Context->ClearDepthStencilView(Graphics::DepthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	// DRAW geometry
	// - These steps are generally repeated for EACH object you draw
	// - Other Direct3D calls will also be necessary to do more complex things
	{
		for (int i = 0; i < entities.size(); i++) {
			entities[i].Draw(activeCam);
		}
	}

	ImGui::Render(); // Turns this frame�s UI into renderable triangles
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // Draws it to the screen

	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
		// Present at the end of the frame
		bool vsync = Graphics::VsyncState();
		Graphics::SwapChain->Present(
			vsync ? 1 : 0,
			vsync ? 0 : DXGI_PRESENT_ALLOW_TEARING);

		// Re-bind back buffer and depth buffer after presenting
		Graphics::Context->OMSetRenderTargets(
			1,
			Graphics::BackBufferRTV.GetAddressOf(),
			Graphics::DepthBufferDSV.Get());
	}
}



