
#include "LeCore/lecore.h"

#include "LeCore/LeCamera/LeCamera.h"

// Default camera values
constexpr static float s_YAW = -90.0f;
constexpr static float s_PITCH = 0.0f;
constexpr static float s_SPEED = 2.5f;
constexpr static float s_SENSITIVITY = 0.1f;
constexpr static float s_FOV = 45.0f;

constexpr static float s_zNEAR = 0.1f;
constexpr static float s_zFAR = 1000.0f;

LeCamera::LeCamera(ProjectionType Proj, int32 screenWidth, int32 screenHeight)
{

    m_ProjectionType = Proj;

    m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
    m_Yaw = s_YAW;
    m_Pitch = s_PITCH;

    m_ScreenSize.x = (float32)screenWidth;
    m_ScreenSize.y = (float32)screenHeight;

    m_Flags = CAMERA_DIR_VECTORS_DIRTY | CAMERA_VIEW_MATRIX_DIRTY | CAMERA_PROJ_MATRIX_DIRTY;


    m_MovementSpeed = s_SPEED;
    m_MouseSensitivity = s_SENSITIVITY;
    m_Fov = s_FOV;
}


// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void LeCamera::ProcessKeyboard(CamMovement direction, float deltaTime)
{
    float velocity = m_MovementSpeed * deltaTime;

    if (direction == FORWARD)
        m_Position += m_Front * velocity;
    if (direction == BACKWARD)
        m_Position -= m_Front * velocity;

    if (direction == LEFT)
        m_Position -= m_Right * velocity;
    if (direction == RIGHT)
        m_Position += m_Right * velocity;

    m_Flags |= CAMERA_VIEW_MATRIX_DIRTY;
}

// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void LeCamera::ProcessMouseMovement(float xoffset, float yoffset, LeBool constrainPitch)
{
    xoffset *= m_MouseSensitivity;
    yoffset *= m_MouseSensitivity;

    m_Yaw += xoffset;
    m_Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (m_Pitch > 89.0f)
            m_Pitch = 89.0f;
        if (m_Pitch < -89.0f)
            m_Pitch = -89.0f;
    }

    m_Flags |= CAMERA_DIR_VECTORS_DIRTY;
    m_Flags |= CAMERA_VIEW_MATRIX_DIRTY;
}

void LeCamera::ProcessMouseScroll(float yoffset)
{
    m_Fov -= (float)yoffset;
    if (m_Fov < 1.0f)
        m_Fov = 1.0f;
    if (m_Fov > 45.0f)
        m_Fov = 45.0f;

    m_Flags |= CAMERA_PROJ_MATRIX_DIRTY;
}

void LeCamera::SetPosition(glm::vec3* Pos)
{
    m_Position = *Pos;

    m_Flags |= CAMERA_VIEW_MATRIX_DIRTY;
}

void LeCamera::SetScreenSize(int32 screenWidth, int32 screenHeight)
{
    m_ScreenSize.x = (float32)screenWidth;
    m_ScreenSize.y = (float32)screenHeight;

    m_Flags |= CAMERA_PROJ_MATRIX_DIRTY;
}

const glm::vec3* LeCamera::GetPosition()
{
    return &m_Position;
}

const glm::vec3* LeCamera::GetFrontVector()
{
    if (m_Flags & CAMERA_DIR_VECTORS_DIRTY)
    {
        CalculateDirVectors();
        m_Flags &= ~CAMERA_DIR_VECTORS_DIRTY;
    }

    return &m_Front;
}

const glm::vec3* LeCamera::GetUpVector()
{
    if (m_Flags & CAMERA_DIR_VECTORS_DIRTY)
    {
        CalculateDirVectors();
        m_Flags &= ~CAMERA_DIR_VECTORS_DIRTY;
    }

    return &m_Up;
}

const glm::vec3* LeCamera::GetRightVector()
{
    if (m_Flags & CAMERA_DIR_VECTORS_DIRTY)
    {
        CalculateDirVectors();
        m_Flags &= ~CAMERA_DIR_VECTORS_DIRTY;
    }

    return &m_Right;
}

const glm::mat4* LeCamera::GetViewMatrix(void)
{
    if (m_Flags & CAMERA_VIEW_MATRIX_DIRTY)
    {
        CalculateViewMatrix();
        m_Flags &= ~CAMERA_VIEW_MATRIX_DIRTY;
    }

    return &m_ViewMatrix;
}

const glm::mat4* LeCamera::GetProjMatrix(void)
{
    if (m_Flags & CAMERA_PROJ_MATRIX_DIRTY)
    {
        CalculateProjMatrix();
        m_Flags &= ~CAMERA_PROJ_MATRIX_DIRTY;
    }

    return &m_ProjMatrix;
}

const glm::mat4* LeCamera::GetViewProjMatrix(void)
{
    if (m_Flags & CAMERA_VIEW_MATRIX_DIRTY || m_Flags & CAMERA_PROJ_MATRIX_DIRTY)
        CalculateViewProjMatrix();

    return &m_ViewProjMatrix;
}

void LeCamera::CalculateDirVectors()
{

    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    m_Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    m_Right = glm::normalize(glm::cross(m_Front, glm::vec3(0.0f, 1.0f, 0.0f)));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    m_Up = glm::normalize(glm::cross(m_Right, m_Front));


}

// returns the view matrix calculated using Euler Angles and the LookAt Matrix
void LeCamera::CalculateViewMatrix()
{

    const glm::vec3* Front = GetFrontVector();
    const glm::vec3* Up = GetUpVector();
    const glm::vec3* Right = GetRightVector();

    const glm::vec3* Position = GetPosition();

    // glm is column major

    glm::mat4 rotM(1.0f);
    float* rot = glm::value_ptr(rotM);

    rot[0] = Right->x;   rot[4] = Right->y;   rot[8] = Right->z;
    rot[1] = Up->x;      rot[5] = Up->y;      rot[9] = Up->z;
    rot[2] = -Front->x;   rot[6] = -Front->y;   rot[10] = -Front->z;

    glm::mat4 posM(1.0f);
    float* pos = glm::value_ptr(posM);

    pos[12] = -Position->x;
    pos[13] = -Position->y;
    pos[14] = -Position->z;

    m_ViewMatrix = rotM * posM;
}

void LeCamera::CalculateProjMatrix()
{
    if (m_ProjectionType == PROJECTION_TYPE_PERSPECTIVE)
    {
        m_ProjMatrix = glm::perspective(m_Fov, m_ScreenSize.x / m_ScreenSize.y, s_zNEAR, s_zFAR);
    }
    else if (m_ProjectionType == PROJECTION_TYPE_ORTHOGONAL)
    {
        m_ProjMatrix = glm::ortho(0.0f, m_ScreenSize.x, 0.0f, m_ScreenSize.y);
    }
    else
    {
        LE_DEBUG_ERROR("Unsupported projection type");
    }
}

void LeCamera::CalculateViewProjMatrix()
{

    const glm::mat4* pProjMatrix = GetProjMatrix();
    const glm::mat4* pViewMatrix = GetViewMatrix();

    m_ViewProjMatrix = (*pProjMatrix) * (*pViewMatrix);
}