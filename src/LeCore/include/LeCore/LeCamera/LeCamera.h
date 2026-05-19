

#pragma once

// Simple camera class from learnopengl.com slightly modified

enum class ProjectionType
{
    ORTHOGONAL,
    PERSPECTIVE,
};

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum class CamMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

#define CAMERA_DIR_VECTORS_DIRTY (1<<0) // The dir vectors need to be updated
#define CAMERA_VIEW_MATRIX_DIRTY (1<<1) // The view matrix needs to be updated
#define CAMERA_PROJ_MATRIX_DIRTY (1<<2) // The proj matrix needs to be updated

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class LeCamera
{
public:

    // constructor with vectors
    LeCamera(ProjectionType Proj, int32 screenWidth, int32 screenHeight);

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(CamMovement direction, float64 deltaTime);

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, LeBool constrainPitch = true);

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset);

    void SetPosition(glm::vec3* Pos);
    void SetScreenSize(int32 screenWidth, int32 screenHeight);

    const glm::vec3* GetPosition();

    const glm::vec3* GetFrontVector();
    const glm::vec3* GetUpVector();
    const glm::vec3* GetRightVector();

    const glm::mat4* GetViewMatrix(void);
    const glm::mat4* GetProjMatrix(void);
    const glm::mat4* GetViewProjMatrix(void);

 

private:


    ProjectionType m_ProjectionType;

    glm::vec3 m_Position;
    float m_Yaw;
    float m_Pitch;

    glm::vec2 m_ScreenSize;

    uint32 m_Flags;

    // camera options
    float m_MovementSpeed;
    float m_MouseSensitivity;
    float m_Fov;



    void CalculateDirVectors();
    glm::vec3 m_Front;
    glm::vec3 m_Up;
    glm::vec3 m_Right;

    void CalculateViewMatrix();
    glm::mat4 m_ViewMatrix;

    void CalculateProjMatrix();
    glm::mat4 m_ProjMatrix;

    void CalculateViewProjMatrix();
    glm::mat4 m_ViewProjMatrix;
};

