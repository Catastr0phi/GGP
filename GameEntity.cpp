#include "GameEntity.h"
#include "BufferStructs.h"
#include <DirectXMath.h>
#include "Camera.h"

using namespace DirectX;

GameEntity::GameEntity(std::shared_ptr<Mesh> mesh)
{
	this->mesh = mesh;
	transform = std::make_shared<Transform>();
}

std::shared_ptr<Mesh> GameEntity::GetMesh() { return mesh; }

std::shared_ptr<Transform> GameEntity::GetTransform() { return transform; }

void GameEntity::Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, std::shared_ptr<Camera> camera)
{
	// Rebind constant buffer
	// Shouldn't matter now, but relevant if we ever rebind the buffer elsewhere
	Graphics::Context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	// Store data locally
	VertexShaderData dataToCopy{};
	dataToCopy.tint = XMFLOAT4(1, 1, 1, 1);
	dataToCopy.world = transform.get()->GetWorldMatrix();
	dataToCopy.view = camera->GetView();
	dataToCopy.proj = camera->getProjection();

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
