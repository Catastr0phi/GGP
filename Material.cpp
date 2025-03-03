#include "Material.h"

Material::Material(DirectX::XMFLOAT4 colorTint, std::shared_ptr<SimpleVertexShader> vertexShader, std::shared_ptr<SimplePixelShader> pixelShader) :
	tint(colorTint),
	vs(vertexShader),
	ps(pixelShader)
{
}

DirectX::XMFLOAT4 Material::GetTint(){ return tint;}

std::shared_ptr<SimpleVertexShader> Material::GetVS() { return vs; }

std::shared_ptr<SimplePixelShader> Material::GetPS() { return ps; }
