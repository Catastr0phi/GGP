#include "Camera.h"
#include "Input.h"

using namespace DirectX;

Camera::Camera(XMFLOAT3 pos, float moveSpeed, float lookSpeed, float fov, float aspectRatio) :
    movementSpeed(moveSpeed),
    mouseLookSpeed(lookSpeed),
    fieldOfView(fov)
{
    transform = std::make_shared<Transform>();
    transform->SetPosition(pos);

    UpdateViewMatrix();
    UpdateProjectionMatrix(aspectRatio);
}

Camera::~Camera()
{
}

void Camera::Update(float dt)
{
    // Input
    float speed = dt * movementSpeed;
    float lookSpeed = dt * mouseLookSpeed;

    if (Input::KeyDown('W')) { transform->MoveRelative(0, 0, speed); }
    if (Input::KeyDown('A')) { transform->MoveRelative(-speed, 0, 0); }
    if (Input::KeyDown('S')) { transform->MoveRelative(0, 0, -speed); }
    if (Input::KeyDown('D')) { transform->MoveRelative(speed, 0, 0); }
    if (Input::KeyDown(' ')) { transform->MoveAbsolute(0, speed, 0); }
    if (Input::KeyDown(VK_SHIFT)) { transform->MoveAbsolute(0, -speed, 0); }

    if (Input::MouseLeftDown()) 
    {
        float xRot = lookSpeed * Input::GetMouseXDelta();
        float yRot = lookSpeed * Input::GetMouseYDelta();

        transform->Rotate(yRot, xRot, 0);
    }

    UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
    XMFLOAT3 pos = transform->GetPosition();
    XMFLOAT3 fwd = transform->GetForward();
    XMFLOAT3 worldUp = XMFLOAT3(0, 1, 0);

    XMMATRIX view = XMMatrixLookToLH(XMLoadFloat3(&pos), XMLoadFloat3(&fwd), XMLoadFloat3(&worldUp));

    XMStoreFloat4x4(&viewMat, view);
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
    // Function makes a projection matrix
    XMMATRIX proj = XMMatrixPerspectiveFovLH(
        fieldOfView, // fov angle in radians
        aspectRatio,
        0.01f,
        100);

    XMStoreFloat4x4(&projMat, proj);
}

DirectX::XMFLOAT4X4 Camera::GetView() { return viewMat; }
DirectX::XMFLOAT4X4 Camera::getProjection() { return projMat; }
std::shared_ptr<Transform> Camera::getTransform() { return transform; };
float Camera::GetFOV() { return fieldOfView; };
