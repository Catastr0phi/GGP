#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <vector>
#include <memory>
#include <DirectXMath.h>
#include "Mesh.h"
#include "GameEntity.h"
#include "Camera.h"

class Game
{
public:
	// Basic OOP setup
	Game() = default;
	~Game();
	Game(const Game&) = delete; // Remove copy constructor
	Game& operator=(const Game&) = delete; // Remove copy-assignment operator

	// Primary functions
	void Initialize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);
	void OnResize();

private:

	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void LoadShaders();
	void LoadAssets();

	// ImGui helper functions
	void UpdateImGui(float deltaTime);
	void UpdateInspector(float deltaTime, float totalTime);

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Shaders and shader-related constructs

	// Additional variables
	bool imGuiDemoVisible;
	float color[4];
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<GameEntity> entities;
	std::vector<std::shared_ptr<Camera>> cameras;
	std::shared_ptr<Camera> activeCam;
	int activeCamIndex;
};

