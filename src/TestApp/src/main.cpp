

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "LeCore/LeCore.h"
#include "LeCore/LeEngine/LeEngine.h"
#include "LeCore/LeCamera/LeCamera.h"
#include "LeCore/LeMesh/LeMesh.h"

#define DEFAULT_SCREEN_WIDTH 800
#define DEFAULT_SCREEN_HEIGHT 600


static LeCamera* pCam = NULL;


static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{

	if (pCam)
		pCam->SetScreenSize(width, height);
}

static void processInput(GLFWwindow* window, float deltaT)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		pCam->ProcessKeyboard(CamMovement::FORWARD, deltaT);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		pCam->ProcessKeyboard(CamMovement::BACKWARD, deltaT);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		pCam->ProcessKeyboard(CamMovement::LEFT, deltaT);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		pCam->ProcessKeyboard(CamMovement::RIGHT, deltaT);
}


static bool s_lastInit = false;
static float s_lastX = 0.0f;
static float s_lastY = 0.0f;

static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (s_lastInit)
	{
		float xoffset = (float)xpos - s_lastX;
		float yoffset = ((float)ypos - s_lastY);

		pCam->ProcessMouseMovement(xoffset, yoffset);
	}
	else
	{
		s_lastInit = true;
	}

	s_lastX = (float)xpos;
	s_lastY = (float)ypos;

}



int main(void)
{

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	GLFWwindow* pWindow = glfwCreateWindow(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, "TestApp", nullptr, nullptr);

	InitEngineParams Params =
	{
		.RendererParams = 
		{
			.api = LeRenderApi::Vulkan,
			.pWindow = pWindow,
			.pAppName = "TestApp"
		}
	};
	LeEngineInit(Params);

	pCam = new LeCamera(PROJECTION_TYPE_PERSPECTIVE, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
	glm::vec3 start = glm::vec3(0.0f, 0.0f, 3.0f);
	pCam->SetPosition(&start);

	// Construct the triangle mesh

	LeShader* BasicShader = LeShader::Create("Basic", TESTAPP_SHADER_BIN_PATH "basic.spv");
	LeMaterial Mat;
	Mat.SetShader(BasicShader);


	LeMatBlock* Mb = LeMatBlock::Create();
	Mb->SetMaterial(&Mat);

	const std::vector<LeVertex> Vertices =
	{
		{{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
		{{0.5f, 0.5f,  0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
		{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}}
	};

	Mb->Vertices = Vertices;
	Mb->AddFlags(MATBLOCK_VERTS_DIRTY);

	LeMesh Mesh;
	Mesh.AddMatblock(Mb);

	// The triangle is constructed!


	float deltaTime = 0.0f;	// Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame

	while (!glfwWindowShouldClose(pWindow))
	{
		glfwPollEvents();

		float currentFrame = float(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(pWindow, deltaTime);


		if (LeBeginRender(pCam))
		{
			LeRender(&Mesh);


			LeEndRender();
		}
	}

	delete Mb;
	delete BasicShader;

	delete pCam;

	LeEngineTerm();

	glfwTerminate();

}
