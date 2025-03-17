#include "Material.h"

using namespace DirectX;

Material::Material(DirectX::XMFLOAT4 colorTint, std::shared_ptr<SimpleVertexShader> vertexShader, std::shared_ptr<SimplePixelShader> pixelShader) :
	tint(colorTint),
	vs(vertexShader),
	ps(pixelShader)
{
	scale = XMFLOAT2(1, 1);
	offset = XMFLOAT2(0, 0);
}

XMFLOAT4 Material::GetTint(){ return tint;}

XMFLOAT2 Material::GetScale() { return scale; }

XMFLOAT2 Material::GetOffset() { return offset; }

std::shared_ptr<SimpleVertexShader> Material::GetVS() { return vs; }

std::shared_ptr<SimplePixelShader> Material::GetPS() { return ps; }

void Material::SetTint(DirectX::XMFLOAT4 newTint) { tint = newTint; }

void Material::SetScale(DirectX::XMFLOAT2 newScale){ scale = newScale; }

void Material::SetOffset(DirectX::XMFLOAT2 newOffset) { offset = newOffset; }

void Material::AddTextureSRV(std::string shaderVariableName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv)
{
	textureSRVs.insert({ shaderVariableName, srv });
}

void Material::AddSampler(std::string shaderVariableName, Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState)
{
	samplers.insert({ shaderVariableName, samplerState });
}

void Material::PrepareMaterial()
{
	for (auto& t : textureSRVs) { ps->SetShaderResourceView(t.first.c_str(), t.second); }
	for (auto& s : samplers) { ps->SetSamplerState(s.first.c_str(), s.second); }
}
