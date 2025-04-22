#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <vector>
#include <memory>
#include <DirectXMath.h>
#include "Mesh.h"
#include "GameEntity.h"
#include "Camera.h"
#include "Lights.h"
#include "Sky.h"

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
	void LoadAssets();
	void LightSetup();
	void ShadowSetup();

	// ImGui helper functions
	void UpdateImGui(float deltaTime);
	void UpdateInspector(float deltaTime, float totalTime);

	// Draw helpers
	void DrawShadowMap();

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Assets and cameras 
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<GameEntity> entities;
	std::vector<std::shared_ptr<Material>> materials;
	std::vector<std::shared_ptr<Camera>> cameras;
	std::shared_ptr<Camera> activeCam;
	int activeCamIndex;

	// Lights
	std::vector<Light> lights;

	// Sky
	std::shared_ptr<Sky> skybox;

	// Shadow variables
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> shadowDSV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shadowSRV;
	DirectX::XMFLOAT4X4 lightViewMatrix;
	DirectX::XMFLOAT4X4 lightProjectionMatrix;
	std::shared_ptr<SimpleVertexShader> shadowVS;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> shadowSampler;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> shadowRasterizer;
	int shadowMapSize;

	// Additional variables
	bool imGuiDemoVisible;
	float color[4];
};

