#pragma once
#include "Transform.h"
#include "Mesh.h"
#include <memory>
#include "Camera.h"
#include "Material.h"

class GameEntity 
{
// Private data
private:
	std::shared_ptr<Transform> transform;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;
// Public data
public:
	GameEntity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> mat);
	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Material> GetMat();
	void SetMat(std::shared_ptr<Material> mat);
	std::shared_ptr<Transform> GetTransform();
	void Draw(std::shared_ptr<Camera> camera);
};