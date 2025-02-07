#include "Transform.h"

using namespace DirectX;

Transform::Transform() :
	position(0, 0, 0),
	rotation(0, 0, 0),
	scale(1, 1, 1)
{
	XMStoreFloat4x4(&world, XMMatrixIdentity());;
	XMStoreFloat4x4(&worldInverseTranspose, XMMatrixIdentity());
}

void Transform::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}

void Transform::SetPosition(DirectX::XMFLOAT3 position)
{
	this->position = position;
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
	rotation.x = pitch;
	rotation.y = yaw;
	rotation.z = roll;
}

void Transform::SetRotation(DirectX::XMFLOAT3 rotation)
{
	this->rotation = rotation;
}

void Transform::SetScale(float x, float y, float z)
{
	scale.x = x;
	scale.y = y;
	scale.z = z;
}

void Transform::SetScale(DirectX::XMFLOAT3 scale)
{
	this->scale = scale;
}

DirectX::XMFLOAT3 Transform::GetPosition() { return position; }

DirectX::XMFLOAT3 Transform::GetPitchYawRoll() { return rotation; }

DirectX::XMFLOAT3 Transform::GetScale() { return scale;
}
DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
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

	return world;
}

DirectX::XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix() 
{ 
	GetWorldMatrix();
	return worldInverseTranspose; 
}

void Transform::MoveAbsolute(float x, float y, float z)
{
	position.x += x;
	position.y += y;
	position.z += z;
}

void Transform::MoveAbsolute(DirectX::XMFLOAT3 offset)
{
	position = XMFLOAT3(position.x + offset.x, position.y + offset.y, position.z + offset.z);
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
	rotation.x += pitch;
	rotation.y += yaw;
	rotation.z += roll;
}

void Transform::Rotate(DirectX::XMFLOAT3 rotation)
{
	this->rotation = XMFLOAT3(this->rotation.x + rotation.x, this->rotation.y + rotation.y, this->rotation.z + rotation.z);
}

void Transform::Scale(float x, float y, float z)
{
	scale.x += x;
	scale.y += y;
	scale.z += z;
}

void Transform::Scale(DirectX::XMFLOAT3 scale)
{
	this->scale = XMFLOAT3(this->scale.x + scale.x, this->scale.y + scale.y, this->scale.z + scale.z);
}
