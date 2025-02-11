#pragma once
#include "Transform.h"
#include "Mesh.h"
#include <memory>

class GameEntity 
{
// Private data
private:
	std::shared_ptr<Transform> transform;
	std::shared_ptr<Mesh> mesh;
// Public data
public:
	GameEntity(std::shared_ptr<Mesh> mesh);
	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Transform> GetTransform();
	void Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, float* offset, float* tint);
};