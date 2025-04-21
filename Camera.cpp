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
    float multiplier;

    // Keyboard
    if (Input::KeyDown(VK_CONTROL)) { multiplier = 3; }
    else { multiplier = 1; }
    if (Input::KeyDown('W')) { transform->MoveRelative(0, 0, speed * multiplier); }
    if (Input::KeyDown('A')) { transform->MoveRelative(-speed * multiplier, 0, 0); }
    if (Input::KeyDown('S')) { transform->MoveRelative(0, 0, -speed * multiplier); }
    if (Input::KeyDown('D')) { transform->MoveRelative(speed * multiplier, 0, 0); }
    if (Input::KeyDown(' ')) { transform->MoveAbsolute(0, speed * multiplier, 0); }
    if (Input::KeyDown(VK_SHIFT)) { transform->MoveAbsolute(0, -speed * multiplier, 0); }

    // Mouse
    if (Input::MouseLeftDown()) 
    {
        float xRot = lookSpeed * Input::GetMouseXDelta();
        float yRot = lookSpeed * Input::GetMouseYDelta();

        transform->Rotate(yRot, xRot, 0);

        // Clamp rotation
        if (transform->GetPitchYawRoll().x >= XM_PIDIV2) 
            transform->SetRotation(XM_PIDIV2 - 0.0001f, transform->GetPitchYawRoll().y, transform->GetPitchYawRoll().z);
        if (transform->GetPitchYawRoll().x <= -XM_PIDIV2)
            transform->SetRotation(-XM_PIDIV2 + 0.0001f, transform->GetPitchYawRoll().y, transform->GetPitchYawRoll().z);
    }

    UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
    // View matrix requires camera position, camera forward vector, and world up vector
    XMFLOAT3 pos = transform->GetPosition();
    XMFLOAT3 fwd = transform->GetForward();
    XMFLOAT3 worldUp = XMFLOAT3(0, 1, 0);

    // This does the work for me. yay!
    XMMATRIX view = XMMatrixLookToLH(XMLoadFloat3(&pos), XMLoadFloat3(&fwd), XMLoadFloat3(&worldUp));

    XMStoreFloat4x4(&viewMat, view);
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
    // Function makes a projection matrix
    XMMATRIX proj = XMMatrixPerspectiveFovLH(
        fieldOfView,
        aspectRatio,
        0.01f,
        100);

    XMStoreFloat4x4(&projMat, proj);
}

DirectX::XMFLOAT4X4 Camera::GetView() { return viewMat; }
DirectX::XMFLOAT4X4 Camera::GetProjection() { return projMat; }
std::shared_ptr<Transform> Camera::GetTransform() { return transform; };
float Camera::GetFOV() { return fieldOfView; };
