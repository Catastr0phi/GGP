#pragma once
#include <memory>
#include <Windows.h>
#include <d3d11.h>
#include <wrl/client.h>
#include "Mesh.h"
#include "SimpleShader.h"
#include "Camera.h"

class Sky 
{
private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencil;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<SimplePixelShader> ps;
	std::shared_ptr<SimpleVertexShader> vs;

public:
	Sky(std::shared_ptr<Mesh> skyMesh, Microsoft::WRL::ComPtr<ID3D11SamplerState> skySamplerState, wchar_t* texPath, wchar_t* psPath, wchar_t* vsPath);
	// Helper for creating a cubemap from 6 individual textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateCubemap(
		const wchar_t* right,
		const wchar_t* left,
		const wchar_t* up,
		const wchar_t* down,
		const wchar_t* front,
		const wchar_t* back);
	void Draw(std::shared_ptr<Camera> camera);
};