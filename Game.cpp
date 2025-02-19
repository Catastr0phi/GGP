#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"
#include <memory>
#include <vector>
#include <string>
#include "BufferStructs.h"

#include <DirectXMath.h>

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
	CreateGeometry();

	// Set initial graphics API state
	//  - These settings persist until we change them
	//  - Some of these, like the primitive topology & input layout, probably won't change
	//  - Others, like setting shaders, will need to be moved elsewhere later
	{
		// Tell the input assembler (IA) stage of the pipeline what kind of
		// geometric primitives (points, lines or triangles) we want to draw.  
		// Essentially: "What kind of shape should the GPU draw with our vertices?"
		Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Ensure the pipeline knows how to interpret all the numbers stored in
		// the vertex buffer. For this course, all of your vertices will probably
		// have the same layout, so we can just set this once at startup.
		Graphics::Context->IASetInputLayout(inputLayout.Get());

		// Set the active vertex and pixel shaders
		//  - Once you start applying different shaders to different objects,
		//    these calls will need to happen multiple times per frame
		Graphics::Context->VSSetShader(vertexShader.Get(), 0, 0);
		Graphics::Context->PSSetShader(pixelShader.Get(), 0, 0);

		// Create constant buffer

		// Make buffer size a multiple of 16
		unsigned int bufferSize = sizeof(VertexShaderData);
		bufferSize = (bufferSize + 15) / 16 * 16;

		D3D11_BUFFER_DESC cbDesc{};
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.ByteWidth = bufferSize;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;
		cbDesc.StructureByteStride = 0;
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;

		Graphics::Device->CreateBuffer(&cbDesc, 0, constantBuffer.GetAddressOf());

		// Bind constant buffer
		Graphics::Context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
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

	// Initial tint, all set to 1
	for (int i = 0; i < 4; i++) {
		tint[i] = 1.0f;
	}

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
	// BLOBs (or Binary Large OBjects) for reading raw data from external files
	// - This is a simplified way of handling big chunks of external data
	// - Literally just a big array of bytes read from a file
	ID3DBlob* pixelShaderBlob;
	ID3DBlob* vertexShaderBlob;

	// Loading shaders
	//  - Visual Studio will compile our shaders at build time
	//  - They are saved as .cso (Compiled Shader Object) files
	//  - We need to load them when the application starts
	{
		// Read our compiled shader code files into blobs
		// - Essentially just "open the file and plop its contents here"
		// - Uses the custom FixPath() helper from Helpers.h to ensure relative paths
		// - Note the "L" before the string - this tells the compiler the string uses wide characters
		D3DReadFileToBlob(FixPath(L"PixelShader.cso").c_str(), &pixelShaderBlob);
		D3DReadFileToBlob(FixPath(L"VertexShader.cso").c_str(), &vertexShaderBlob);

		// Create the actual Direct3D shaders on the GPU
		Graphics::Device->CreatePixelShader(
			pixelShaderBlob->GetBufferPointer(),	// Pointer to blob's contents
			pixelShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			pixelShader.GetAddressOf());			// Address of the ID3D11PixelShader pointer

		Graphics::Device->CreateVertexShader(
			vertexShaderBlob->GetBufferPointer(),	// Get a pointer to the blob's contents
			vertexShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			vertexShader.GetAddressOf());			// The address of the ID3D11VertexShader pointer
	}

	// Create an input layout 
	//  - This describes the layout of data sent to a vertex shader
	//  - In other words, it describes how to interpret data (numbers) in a vertex buffer
	//  - Doing this NOW because it requires a vertex shader's byte code to verify against!
	//  - Luckily, we already have that loaded (the vertex shader blob above)
	{
		D3D11_INPUT_ELEMENT_DESC inputElements[2] = {};

		// Set up the first element - a position, which is 3 float values
		inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;				// Most formats are described as color channels; really it just means "Three 32-bit floats"
		inputElements[0].SemanticName = "POSITION";							// This is "POSITION" - needs to match the semantics in our vertex shader input!
		inputElements[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// How far into the vertex is this?  Assume it's after the previous element

		// Set up the second element - a color, which is 4 more float values
		inputElements[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			// 4x 32-bit floats
		inputElements[1].SemanticName = "COLOR";							// Match our vertex shader input!
		inputElements[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// After the previous element

		// Create the input layout, verifying our description against actual shader code
		Graphics::Device->CreateInputLayout(
			inputElements,							// An array of descriptions
			2,										// How many elements in that array?
			vertexShaderBlob->GetBufferPointer(),	// Pointer to the code of a shader that uses this layout
			vertexShaderBlob->GetBufferSize(),		// Size of the shader code that uses this layout
			inputLayout.GetAddressOf());			// Address of the resulting ID3D11InputLayout pointer
	}
}


// --------------------------------------------------------
// Creates the geometry we're going to draw
// --------------------------------------------------------
void Game::CreateGeometry()
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

	// Declare meshes and names
	std::shared_ptr<Mesh> triangle;
	std::shared_ptr<Mesh> rectangle;
	std::shared_ptr<Mesh> star;

	// Set up the vertices of the triangle 
	Vertex triangleVertices[] =
	{
		{ XMFLOAT3(+0.0f, +0.5f, +0.0f), red },
		{ XMFLOAT3(+0.5f, -0.5f, +0.0f), blue },
		{ XMFLOAT3(-0.5f, -0.5f, +0.0f), green },
	};

	// Set up indices for the triangle
	unsigned int triangleIndices[] = { 0, 1, 2 };

	// Set up vertices for a rectangle
	Vertex rectangleVertices[]
	{
		{ XMFLOAT3(-0.2f, -0.2f, +0.0f), black },
		{ XMFLOAT3(-0.2f, +0.2f, +0.0f), white },
		{ XMFLOAT3(+0.2f, +0.2f, +0.0f), black },
		{ XMFLOAT3(+0.2f, -0.2f, +0.0f), white },
	};

	// Set up indicies for a rectangle
	unsigned int rectangleIndices[] = { 0, 1, 2, 0, 2, 3 };

	// Set up vertices for star 
	Vertex starVertices[]
	{
		{ XMFLOAT3(-0.7f, +0.6f, +0.0f), purple },
		{ XMFLOAT3(-0.5f, +0.6f, +0.0f), purple },
		{ XMFLOAT3(-0.5f, +0.3f, +0.0f), purple },
		{ XMFLOAT3(-0.7f, +0.3f, +0.0f), purple },
		{ XMFLOAT3(-0.6f, +1.0f, +0.0f), yellow },
		{ XMFLOAT3(-0.2f, +0.45f, +0.0f), yellow },
		{ XMFLOAT3(-0.6f, -0.1f, +0.0f), yellow },
		{ XMFLOAT3(-1.0f, +0.45f, +0.0f), yellow },
	};

	// Set up indices for star
	unsigned int starIndices[] = { 0, 1, 2, 0, 2, 3, 4, 1, 0, 5, 2, 1, 6, 3, 2, 7, 0, 3 };


	// Instantiate meshes
	triangle = std::make_shared<Mesh>(triangleVertices, triangleIndices, (int)ARRAYSIZE(triangleVertices), (int)ARRAYSIZE(triangleIndices), "Triangle");
	rectangle = std::make_shared<Mesh>(rectangleVertices, rectangleIndices, (int)ARRAYSIZE(rectangleVertices), (int)ARRAYSIZE(rectangleIndices), "Rectangle");
	star = std::make_shared<Mesh>(starVertices, starIndices, (int)ARRAYSIZE(starVertices), (int)ARRAYSIZE(starIndices), "Star");

	// Push meshes to vector
	meshes.push_back(triangle);
	meshes.push_back(rectangle);
	meshes.push_back(star);

	// Dont need diamond background code anymore, but keeping it for reference if I want to recreate this again

	// Creates a diamond background
	// I really just did this to see if I could
	// TODO: If I want to mess with this further, maybe make it support custom dimensions instead of being hardcoded to 11x11
	//for (int i = 0; i < 121; i++) {
	//	std::shared_ptr<Mesh> diamond;

	//	// Vertices are adjusted based on loop index
	//	Vertex diamondVertices[]
	//	{
	//		{ XMFLOAT3(+1.0f - ((i % 11)/5.0f), +1.1f - ((i / 11) / 5.0f), +0.0f), blue},
	//		{ XMFLOAT3(+1.1f - ((i % 11)/5.0f), +1.0f - ((i / 11) / 5.0f), +0.0f), red},
	//		{ XMFLOAT3(+1.0f - ((i % 11)/5.0f), +0.9f - ((i / 11) / 5.0f), +0.0f), blue},
	//		{ XMFLOAT3(+0.9f - ((i % 11)/5.0f), +1.0f - ((i / 11) / 5.0f), +0.0f), red},
	//	};

	//	// Indices always the same
	//	unsigned int diamondIndices[] = { 0,1,2,0,2,3 };

	//	// Make and push mesh
	//	diamond = std::make_shared<Mesh>(diamondVertices, diamondIndices, ARRAYSIZE(diamondVertices), ARRAYSIZE(diamondIndices), "Diamond " + std::to_string(i));
	//	meshes.push_back(diamond);
	//}

	// Make entities from meshes
	entities.push_back(GameEntity(star));
	entities.push_back(GameEntity(star));
	entities.push_back(GameEntity(star));
	entities.push_back(GameEntity(triangle));
	entities.push_back(GameEntity(rectangle));
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
		ImGui::Text("Cam Position: %f,%f,%f", activeCam->getTransform()->GetPosition().x,
			activeCam->getTransform()->GetPosition().y,
			activeCam->getTransform()->GetPosition().z);

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

			// Display info
			if (ImGui::TreeNode(name)) 
			{
				ImGui::SliderFloat3("Position", posArray, -2.0f, 2.0f);
				ImGui::SliderFloat3("Rotation", rotArray, -4.0f, 4.0f);
				ImGui::SliderFloat3("Scale", scaleArray, 0.0f, 2.0f);
				ImGui::TreePop();
			}

			// Set new values
			entities[i].GetTransform().get()->SetPosition(posArray[0], posArray[1], posArray[2]);
			entities[i].GetTransform().get()->SetRotation(rotArray[0], rotArray[1], rotArray[2]);
			entities[i].GetTransform().get()->SetScale(scaleArray[0], scaleArray[1], scaleArray[2]);
		}
	}

	// Color and offset editors
	if (ImGui::CollapsingHeader("Editors"))
	{
		// Color editors
		ImGui::ColorEdit4("Background color", color);
		ImGui::ColorEdit4("Tint color", tint);

		// Offset editors
		ImGui::SliderFloat3("Offset", offset, -2.0f, 2.0f);
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


	// Move entities!

	// Have some stars moving around
	entities[0].GetTransform().get()->MoveAbsolute(sinf(totalTime) * deltaTime, 0, 0);
	entities[1].GetTransform().get()->MoveAbsolute(0, -sinf(totalTime) * deltaTime, 0);

	// Rotate the triangle
	entities[3].GetTransform().get()->Rotate(0, 0, 1 * deltaTime);

	// Move rectangle to corner and make it pulse
	entities[4].GetTransform().get()->SetPosition(0.5, 0.5, 0);
	entities[4].GetTransform().get()->Scale(sinf(totalTime) * deltaTime, sinf(totalTime) * deltaTime, 0);
	entities[4].GetTransform().get()->Scale(sinf(totalTime) * deltaTime, sinf(totalTime) * deltaTime, 0);

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
			entities[i].Draw(constantBuffer, activeCam);
		}
	}

	ImGui::Render(); // Turns this frame’s UI into renderable triangles
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



