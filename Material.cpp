#include "Material.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

using namespace DirectX;

Material::Material(DirectX::XMFLOAT4 colorTint, float roughness, std::shared_ptr<SimpleVertexShader> vertexShader, std::shared_ptr<SimplePixelShader> pixelShader) :
	tint(colorTint),
	vs(vertexShader),
	ps(pixelShader),
	roughness(roughness)
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

	ps->SetFloat4("colorTint", tint);
	ps->SetFloat2("textureScale", scale);
	ps->SetFloat2("textureOffset", offset);
	ps->SetFloat("roughness", roughness);
	ps->CopyAllBufferData();
}

// Helper function for building ImGui menu
// TODO: Create something like this in for all classes that are displayed
void Material::CreateGUI() 
{
	for (auto& t : textureSRVs) 
	{
		ImGui::Image((ImTextureID)t.second.Get(), ImVec2(64,64));
	}

	(ImGui::ColorEdit4("Tint", &tint.x));
	(ImGui::SliderFloat2("Scale", &scale.x, 0.5f, 5.0f));
	(ImGui::SliderFloat2("Offset", &offset.x, -1.0f, 1.0f));
}
