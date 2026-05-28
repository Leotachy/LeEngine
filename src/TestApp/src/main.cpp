

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "LeCore/LeCore.h"
#include "LeCore/LeEngine/LeEngine.h"
#include "LeCore/LeCamera/LeCamera.h"
#include "LeCore/LeMesh/LeMesh.h"
#include "LeCore/LeTexture/LeTextureDictionary.h"
#include "LeCore/LeScene/LeScene.h"

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

	pMb->GetVertexData()[0].pos.z = (float)glm::cos(LifeTime);
	pMb->GetVertexData()[3].pos.z = (float)glm::sin(LifeTime);

	pMb->Flags |= (MATBLOCK_VERTS_DIRTY);


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
			.API = LeRenderApi::Vulkan,
			.WindowParams = 
			{
				.pWindow = pWindow,
				.pAppName = "TestApp"
			},
			.ViewportParams = 
			{
				.ViewportWidth = DEFAULT_SCREEN_WIDTH,
				.ViewportHeight = DEFAULT_SCREEN_HEIGHT,
				.bRenderViewportToImGuiTexture = false
			},
			.PathParams = 
			{
				.pShaderBinaryPath = TESTAPP_SHADER_BIN_PATH
			}
		}
	};
	LeEngineInit(&Params);

	pCam = new LeCamera(ProjectionType::PERSPECTIVE, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
	glm::vec3 start = glm::vec3(0.0f, 0.0f, 3.0f);
	pCam->SetPosition(&start);

	// Construct the triangle mesh

	LeMaterial* Mat = LeMaterial::Create();
	Mat->SetName("uwu");

	LeScene* pScene = new LeScene;
	LeTexture* pBaseTexture = pScene->GetTextureDictionary()->LoadTexture(TESTAPP_RESOURCES_PATH "textures/", "awesomeface.png");
	LE_ASSERT(pBaseTexture);
	Mat->SetBaseTexture(pBaseTexture);

	pScene->GetMaterialDictionary()->AddMaterial(Mat);

	LeMatBlock* Mb = LeMatBlock::Create();
	Mb->SetMaterial(Mat);

	const std::vector<LeVertex> verts =
	{
		{{-0.5f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
		{{0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
		{{0.5f, 0.5f,  0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
		{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

		{{ -0.5f, -0.5f, -2.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
		{{0.5f, -0.5f, -2.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
		{{0.5f, 0.5f,  -2.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
		{{-0.5f, 0.5f, -2.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
	};

	const std::vector<LeFace> indices =
	{
		{0, 1, 2},
		{2, 3, 0},

		{4, 5, 6},
		{6, 7, 4}
	};

	Mb->SetNumVerts(uint32(verts.size()));
	LeMemcpy(Mb->GetVertexData(), verts.data(), verts.size()*sizeof(LeVertex));
	Mb->Flags |= (MATBLOCK_VERTS_DIRTY);

	Mb->SetNumFaces(uint32(indices.size()));
	LeMemcpy(Mb->GetFaceData(), indices.data(), indices.size() * sizeof(LeFace));
	Mb->Flags |= (MATBLOCK_FACES_DIRTY);

	Mb->Compile();
	
	LeWorld* pWorld = pScene->GetWorld();

	LeMesh* pMesh = pWorld->GetMesh();

	pMesh->AddMatblock(Mb);



	// The triangle is constructed!


	double deltaTime = 0.0;	// Time between current frame and last frame
	double lastFrame = 0.0; // Time of last frame

	while (!glfwWindowShouldClose(pWindow))
	{
		glfwPollEvents();

		double currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		if (deltaTime > 0.0)
		{
			int32 FPS = int32(1.0 / deltaTime);
			LE_DEBUG_MESSAGE("FPS: %d", FPS);
		}


		manipulateMatblockVertex(Mb, deltaTime);

		processInput(pWindow, deltaTime);


		LeRenderInterface::RenderScene(pScene, pCam, s_bFramebufferResized);

		s_bFramebufferResized = false;
	}

	LeStreamFile StreamFile;
	StreamFile.Open(LeStreamAccessType::WRITE, TESTAPP_SCENES_PATH "test_level.ble");
	pScene->SerializeBinaryChunk(&StreamFile, NULL);

	LeEngineShutDown();

	delete pCam;
	delete pScene;

	LeEngineDeInit();

	glfwTerminate();

}
