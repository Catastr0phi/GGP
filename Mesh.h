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

	void CreateBuffers(Vertex vertices[], unsigned int indices[], int newVertexCount, int newIndexCount);

// Public methods
public:
	Mesh(Vertex vertices[], unsigned int indices[], int vertexCount, int indexCount, std::string newName);
	Mesh(const char* objFile, std::string newName);
	~Mesh();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	int GetIndexCount();
	int GetVertexCount();
	std::string GetName();
	void Draw();
	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);
};