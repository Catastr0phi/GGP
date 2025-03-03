#include "Mesh.h"

// Constructor
Mesh::Mesh(Vertex vertices[], unsigned int indices[], int newVertexCount, int newIndexCount, std::string newName) {

	// Assign count data
	vertexCount = newVertexCount;
	indexCount = newIndexCount;
	name = newName;

	// Create vertex buffer

	// Describe the buffer
	D3D11_BUFFER_DESC vbd = {};
	vbd.Usage = D3D11_USAGE_IMMUTABLE;	
	vbd.ByteWidth = sizeof(Vertex) * vertexCount;      
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER; 
	vbd.CPUAccessFlags = 0;	
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	// Create the proper struct to hold the initial vertex data
	D3D11_SUBRESOURCE_DATA initialVertexData = {};
	initialVertexData.pSysMem = vertices; 

	// Create the buffer
	Graphics::Device->CreateBuffer(&vbd, &initialVertexData, vertexBuffer.GetAddressOf());


	// Create index buffer

	// Describe buffer
	D3D11_BUFFER_DESC ibd = {};
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(unsigned int) * indexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	// Specify the initial data for this buffer
	D3D11_SUBRESOURCE_DATA initialIndexData = {};
	initialIndexData.pSysMem = indices; 

	// Create the buffer
	Graphics::Device->CreateBuffer(&ibd, &initialIndexData, indexBuffer.GetAddressOf());
}

Mesh::Mesh(const char* objFile, std::string newName) {

}

// Destructor
// Empty since comptrs should do this
Mesh::~Mesh() {

}

// Functions for returning data
Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetVertexBuffer() {
	return vertexBuffer;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetIndexBuffer() {
	return indexBuffer;
}

int Mesh::GetVertexCount() {
	return vertexCount;
}

int Mesh::GetIndexCount(){
	return indexCount;
}

std::string Mesh::GetName() {
	return name;
}


/// <summary>
/// Draws mesh
/// </summary>
void Mesh::Draw() {
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	Graphics::Context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	Graphics::Context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	Graphics::Context->DrawIndexed(indexCount,0,0);    
}