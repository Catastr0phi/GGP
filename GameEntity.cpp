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

void GameEntity::Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, float* offset, float* tint)
{
	// Rebind constant buffer
	// Shouldn't matter now, but relevant if we ever rebind the buffer elsewhere
	Graphics::Context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	// Store data locally
	VertexShaderData dataToCopy{};
	dataToCopy.tint = XMFLOAT4(tint[0], tint[1], tint[2], tint[3]);

	// Adding the offset
	// Theres probably a smarter solution that doesn't involve doing this in draw(), but this will likely be removed soon anyway
	XMFLOAT4X4 world = transform.get()->GetWorldMatrix();
	XMMATRIX worldWithOffsetMat = XMMatrixMultiply(XMLoadFloat4x4(&world),
		XMMatrixTranslation(offset[0], offset[1], offset[2]));

	XMFLOAT4X4 worldWithOffset;
	XMStoreFloat4x4(&worldWithOffset, worldWithOffsetMat);

	dataToCopy.world = worldWithOffset;

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
