#pragma once

#include <DirectXMath.h>
#include <memory>
#include "SimpleShader.h"
#include <string>

class Material 
{
private:
	DirectX::XMFLOAT4 tint;
	std::shared_ptr<SimpleVertexShader> vs;
	std::shared_ptr<SimplePixelShader> ps;

public:
	Material(DirectX::XMFLOAT4 colorTint, std::shared_ptr<SimpleVertexShader> vertexShader, std::shared_ptr<SimplePixelShader> pixelShader);

	// Getters
	DirectX::XMFLOAT4 GetTint();
	std::shared_ptr<SimpleVertexShader> GetVS();
	std::shared_ptr<SimplePixelShader> GetPS();
};
