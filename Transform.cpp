#include "Transform.h"

using namespace DirectX;

Transform::Transform() :
	position(0, 0, 0),
	rotation(0, 0, 0),
	scale(1, 1, 1)
{
	XMStoreFloat4x4(&world, XMMatrixIdentity());;
	XMStoreFloat4x4(&worldInverseTranspose, XMMatrixIdentity());

	dirty = false;
}

void Transform::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
	dirty = true;
}

void Transform::SetPosition(DirectX::XMFLOAT3 position)
{
	this->position = position;
	dirty = true;
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
	rotation.x = pitch;
	rotation.y = yaw;
	rotation.z = roll;
	dirty = true;
}

void Transform::SetRotation(DirectX::XMFLOAT3 rotation)
{
	this->rotation = rotation;
	dirty = true;
}

void Transform::SetScale(float x, float y, float z)
{
	scale.x = x;
	scale.y = y;
	scale.z = z;
	dirty = true;
}

void Transform::SetScale(DirectX::XMFLOAT3 scale)
{
	this->scale = scale;
	dirty = true;
}

DirectX::XMFLOAT3 Transform::GetPosition() { return position; }

DirectX::XMFLOAT3 Transform::GetPitchYawRoll() { return rotation; }

DirectX::XMFLOAT3 Transform::GetScale() { return scale;
}
DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
	// Only remake matrix if something has changed
	if (dirty) {
		// Make separate matrices
		XMMATRIX translateMat = XMMatrixTranslation(position.x, position.y, position.z);
		XMMATRIX rotationMat = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
		XMMATRIX scaleMat = XMMatrixScaling(scale.x, scale.y, scale.z);

		// Multiply matrices
		XMMATRIX worldMat = scaleMat * rotationMat * translateMat;

		// Store world matrix and inverse
		XMStoreFloat4x4(&world, worldMat);
		XMStoreFloat4x4(&worldInverseTranspose,
			XMMatrixInverse(0, XMMatrixTranspose(worldMat)));
	}

	return world;
}

DirectX::XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix() 
{ 
	// Call world matrix function if dirty, as it will also rebuild inverse matrix
	if (dirty) GetWorldMatrix();
	return worldInverseTranspose; 
}

void Transform::MoveAbsolute(float x, float y, float z)
{
	position.x += x;
	position.y += y;
	position.z += z;
	dirty = true;
}

void Transform::MoveAbsolute(DirectX::XMFLOAT3 offset)
{
	MoveAbsolute(offset.x, offset.y, offset.z);
}

void Transform::MoveRelative(float x, float y, float z) 
{
	// Move along local axes

	// Create vector
	XMVECTOR movement = XMVectorSet(x, y, z, 0);

	// Create quaternion based on p/y/r
	XMVECTOR rotQuat = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&rotation));

	// Rotate vector
	XMVECTOR dir = XMVector3Rotate(movement, rotQuat);

	// Add direction
	XMStoreFloat3(&position, XMLoadFloat3(&position) + dir);
	dirty = true;
}

void Transform::MoveRelative(DirectX::XMFLOAT3 offset)
{
	MoveRelative(offset.x, offset.y, offset.z);
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
	rotation.x += pitch;
	rotation.y += yaw;
	rotation.z += roll;
	dirty = true;
}

void Transform::Rotate(DirectX::XMFLOAT3 rotation)
{
	Rotate(rotation.x, rotation.y, rotation.z);
}

void Transform::Scale(float x, float y, float z)
{
	scale.x += x;
	scale.y += y;
	scale.z += z;
	dirty = true;
}

void Transform::Scale(DirectX::XMFLOAT3 scale)
{
	Scale(scale.x, scale.y, scale.z);
}
