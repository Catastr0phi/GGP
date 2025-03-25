#pragma once

#include <DirectXMath.h>
#include <memory>
#include "SimpleShader.h"
#include <string>
#include <unordered_map>

class Material 
{
private:
	DirectX::XMFLOAT4 tint;
	DirectX::XMFLOAT2 scale;
	DirectX::XMFLOAT2 offset;
	float roughness;
	std::shared_ptr<SimpleVertexShader> vs;
	std::shared_ptr<SimplePixelShader> ps;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureSRVs;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> samplers;

public:
	Material(DirectX::XMFLOAT4 colorTint, float roughness, std::shared_ptr<SimpleVertexShader> vertexShader, std::shared_ptr<SimplePixelShader> pixelShader);

	// Getters
	DirectX::XMFLOAT4 GetTint();
	DirectX::XMFLOAT2 GetScale();
	DirectX::XMFLOAT2 GetOffset();
	std::shared_ptr<SimpleVertexShader> GetVS();
	std::shared_ptr<SimplePixelShader> GetPS();

	void SetTint(DirectX::XMFLOAT4 tint);
	void SetScale(DirectX::XMFLOAT2 scale);
	void SetOffset(DirectX::XMFLOAT2 offset);
	void AddTextureSRV(std::string shaderVariableName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv);
	void AddSampler(std::string shaderVariableName, Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState);
	void PrepareMaterial();
	void CreateGUI();
};
