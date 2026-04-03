

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "LeCore/LeCore.h"
#include "LeCore/LeEngine/LeEngine.h"
#include "LeCore/LeCamera/LeCamera.h"
#include "LeCore/LeMesh/LeMesh.h"

#define DEFAULT_SCREEN_WIDTH 800
#define DEFAULT_SCREEN_HEIGHT 600


static LeCamera* pCam = NULL;

static LeBool s_bFramebufferResized = false;

static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{

	if (pCam)
		pCam->SetScreenSize(width, height);

	s_bFramebufferResized = true;
}

static void processInput(GLFWwindow* window, double deltaT)
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

static void manipulateMatblockVertex(LeMatBlock* pMb, double dt)
{
	static double LifeTime = 0;
	LifeTime += dt;

	pMb->Vertices[0].pos.z = (float)glm::sin(LifeTime);
	pMb->Vertices[pMb->Vertices.size() - 1].pos.z = (float)glm::sin(LifeTime);

	pMb->AddFlags(MATBLOCK_VERTS_DIRTY);


}

int main(void)
{

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	GLFWwindow* pWindow = glfwCreateWindow(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, "TestApp", nullptr, nullptr);
	glfwSetCursorPosCallback(pWindow, mouse_callback);
	glfwSetFramebufferSizeCallback(pWindow, framebufferResizeCallback);
	glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	InitEngineParams Params =
	{
		.RendererParams = 
		{
			.api = LeRenderApi::LE_RENDER_API_VULKAN,
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
	LeMaterial* Mat = LeMaterial::Create();
	Mat->SetShader(BasicShader);

	LeMatBlock* Mb = LeMatBlock::Create();
	Mb->SetMaterial(Mat);

	const std::vector<LeVertex> Vertices =
	{
		{{-0.5f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
		{{0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
		{{0.5f, 0.5f,  0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
		{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
	};

	const std::vector<LeFace> indices =
	{
		{0, 1, 2},
		{2, 3, 0}
	};

	Mb->Vertices = Vertices;
	Mb->AddFlags(MATBLOCK_VERTS_DIRTY);

	Mb->Faces = indices;
	Mb->AddFlags(MATBLOCK_FACES_DIRTY);

	LeMesh* pMesh = LeMesh::Create();
	pMesh->AddMatblock(Mb);

	int32 w, h, ch;
	void* pPixelData = stbi_load(TESTAPP_RESOURCES_PATH "textures/awesomeface.png", &w, &h, &ch, 4);
	stbi_image_free(pPixelData);

	LE_DEBUG_ASSERT(ch == 4);

	LeTexture* pBaseTexture = LeTexture::Create();
	pBaseTexture->SetPixels((LeRGBA*)pPixelData, w, h);
	Mat->SetBaseTexture(pBaseTexture);

	// The triangle is constructed!


	double deltaTime = 0.0;	// Time between current frame and last frame
	double lastFrame = 0.0; // Time of last frame

	while (!glfwWindowShouldClose(pWindow))
	{
		glfwPollEvents();

		double currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		manipulateMatblockVertex(Mb, deltaTime);

		processInput(pWindow, deltaTime);


		if (LeRenderInterface::BeginRender(pCam, s_bFramebufferResized))
			if (LeRenderInterface::Render(pMesh))
				LeRenderInterface::EndRender();

		s_bFramebufferResized = false;
	}

	LeEngineShutDown();

	delete pBaseTexture;
	delete Mat;
	delete BasicShader;
	delete pCam;
	delete pMesh;

	LeEngineDeInit();

	glfwTerminate();

}
