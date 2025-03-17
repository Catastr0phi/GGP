#include "GameEntity.h"
#include "BufferStructs.h"
#include <DirectXMath.h>

using namespace DirectX;

GameEntity::GameEntity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> mat) :
	mesh(mesh),
	material(mat)
{
	transform = std::make_shared<Transform>();
}

std::shared_ptr<Mesh> GameEntity::GetMesh() { return mesh; }

std::shared_ptr<Material> GameEntity::GetMat() { return material; }

void GameEntity::SetMat(std::shared_ptr<Material> mat) { material = mat; }

std::shared_ptr<Transform> GameEntity::GetTransform() { return transform; }

void GameEntity::Draw(std::shared_ptr<Camera> camera)
{
	material->GetVS()->SetShader();
	material->GetPS()->SetShader();
	material->PrepareMaterial();

	// Copy data to cbuffers
	
	// vertex shader
	std::shared_ptr<SimpleVertexShader> vs = material->GetVS();

	vs->SetMatrix4x4("world", transform->GetWorldMatrix()); 
	vs->SetMatrix4x4("view", camera->GetView()); 
	vs->SetMatrix4x4("proj", camera->GetProjection()); 

	vs->CopyAllBufferData();

	// pixel shader
	std::shared_ptr<SimplePixelShader> ps = material->GetPS();

	ps->SetFloat4("colorTint", material->GetTint());
	ps->SetFloat2("textureScale", material->GetScale());
	ps->SetFloat2("textureOffset", material->GetOffset());

	ps->CopyAllBufferData();

	// Draw mesh
	mesh.get()->Draw();
}
