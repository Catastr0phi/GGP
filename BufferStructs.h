#pragma once
#include <DirectXMath.h>

struct VertexShaderData 
{
	DirectX::XMFLOAT4 tint;	    // The local position of the vertex
	DirectX::XMFLOAT3 offset;        // The color of the vertex
};