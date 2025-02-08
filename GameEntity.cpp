#include "GameEntity.h"
#include "BufferStructs.h"
#include <DirectXMath.h>

using namespace DirectX;

GameEntity::GameEntity(std::shared_ptr<Mesh> mesh)
{
	this->mesh = mesh;
	transform = std::make_shared<Transform>();
}

std::shared_ptr<Mesh> GameEntity::GetMesh() { return mesh; }

std::shared_ptr<Transform> GameEntity::GetTransform() { return transform; }

void GameEntity::Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, float* universalOffset, float* universalTint)
{
	// Rebind constant buffer
	// Shouldn't matter now, but relevant if we ever rebind the buffer elsewhere
	Graphics::Context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	// Store data locally
	VertexShaderData dataToCopy{};
	dataToCopy.tint = XMFLOAT4(universalTint[0], universalTint[1], universalTint[2], universalTint[3]);
	dataToCopy.world = transform.get()->GetWorldMatrix();

	// Code for adding the universal offset, disabled for now
	// Doing matrix math should probably be avoided in draw
	/*XMFLOAT4X4 world = transform.get()->GetWorldMatrix();
	XMMATRIX worldWithOffsetMat = XMMatrixMultiply(XMLoadFloat4x4(&world),
		XMMatrixTranslation(universalOffset[0], universalOffset[1], universalOffset[2]));

	XMFLOAT4X4 worldWithOffset;
	XMStoreFloat4x4(&worldWithOffset, worldWithOffsetMat);

	dataToCopy.world = worldWithOffset;*/

	// Map buffer
	D3D11_MAPPED_SUBRESOURCE mapped{};
	Graphics::Context->Map(
		constantBuffer.Get(),
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapped);

	memcpy(mapped.pData, &dataToCopy, sizeof(VertexShaderData));

	// Unmap when done
	Graphics::Context->Unmap(constantBuffer.Get(), 0);

	// Draw mesh
	mesh.get()->Draw();
}
