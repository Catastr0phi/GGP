#pragma once

#include "Transform.h"
#include <DirectXMath.h>
#include <memory>

class Camera 
{
public:
	Camera(DirectX::XMFLOAT3 pos, float moveSpeed, float lookSpeed, float fov, float aspectRatio);
	~Camera();

	// Update methods
	void Update(float dt);
	void UpdateViewMatrix();
	void UpdateProjectionMatrix(float aspectRatio);

	// Getters
	DirectX::XMFLOAT4X4 GetView();
	DirectX::XMFLOAT4X4 getProjection();
	std::shared_ptr<Transform> getTransform();
	float GetFOV();

private:
	// Camera matrices
	DirectX::XMFLOAT4X4 viewMat;
	DirectX::XMFLOAT4X4 projMat;

	// Transform
	std::shared_ptr<Transform> transform;

	// Other variables
	float fieldOfView;
	float movementSpeed;
	float mouseLookSpeed;
};
