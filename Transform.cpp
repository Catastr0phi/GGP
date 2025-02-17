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

void Transform::SetPosition(XMFLOAT3 position)
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

void Transform::SetRotation(XMFLOAT3 rotation)
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

void Transform::SetScale(XMFLOAT3 scale)
{
	this->scale = scale;
	dirty = true;
}

XMFLOAT3 Transform::GetPosition() { return position; }

XMFLOAT3 Transform::GetPitchYawRoll() { return rotation; }

XMFLOAT3 Transform::GetScale() { return scale;
}
XMFLOAT4X4 Transform::GetWorldMatrix()
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

XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix() 
{ 
	// Call world matrix function if dirty, as it will also rebuild inverse matrix
	if (dirty) GetWorldMatrix();
	return worldInverseTranspose; 
}

XMFLOAT3 Transform::GetUp()
{
	// Create up vector
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);

	// Create quaternion based on p/y/r
	XMVECTOR rotQuat = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&rotation));

	// Rotate vector
	XMVECTOR localUpVec = XMVector3Rotate(up, rotQuat);

	// Return new vector
	XMFLOAT3 localUp;
	XMStoreFloat3(&localUp, localUpVec);
	return localUp;
}

XMFLOAT3 Transform::GetRight() 
{
	// Create right vector
	XMVECTOR right = XMVectorSet(1, 0, 0, 0);

	// Create quaternion based on p/y/r
	XMVECTOR rotQuat = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&rotation));

	// Rotate vector
	XMVECTOR localRightVec = XMVector3Rotate(right, rotQuat);

	// Return new vector
	XMFLOAT3 localRight;
	XMStoreFloat3(&localRight, localRightVec);
	return localRight;
}

XMFLOAT3 Transform::GetForward()
{
	// Create forward vector
	XMVECTOR forward = XMVectorSet(0, 0, 1, 0);

	// Create quaternion based on p/y/r
	XMVECTOR rotQuat = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&rotation));

	// Rotate vector
	XMVECTOR localForwardVec = XMVector3Rotate(forward, rotQuat);

	// Return new vector
	XMFLOAT3 localForward;
	XMStoreFloat3(&localForward, localForwardVec);
	return localForward;
}

void Transform::MoveAbsolute(float x, float y, float z)
{
	position.x += x;
	position.y += y;
	position.z += z;
	dirty = true;
}

void Transform::MoveAbsolute(XMFLOAT3 offset)
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

void Transform::MoveRelative(XMFLOAT3 offset)
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

void Transform::Rotate(XMFLOAT3 rotation)
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

void Transform::Scale(XMFLOAT3 scale)
{
	Scale(scale.x, scale.y, scale.z);
}
