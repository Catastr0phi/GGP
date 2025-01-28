#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <wrl/client.h>
#include "Graphics.h"
#include "Vertex.h"
#include <string>


class Mesh 
{
// Private data
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	int vertexCount;
	int indexCount;

	std::string name;

// Public methods
public:
	Mesh(Vertex vertices[], unsigned int indices[], int vertexCount, int indexCount, std::string newName);
	~Mesh();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	int GetIndexCount();
	int GetVertexCount();
	std::string GetName();
	void Draw();
};