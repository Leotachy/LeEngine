
// Lukas Andrijauskas, 11th April 2025
// Simple editor for LeEngine

#include "LeCore/lecore.h"

#include "LeEditor/LeEditor.h"
#include "LeEditor/LeViewport/LeViewport.h"

#include "LeCore/LeEngine/LeEngine.h"

#include "LeCore/LeCamera/LeCamera.h"

#define DEFAULT_SCREEN_WIDTH 800
#define DEFAULT_SCREEN_HEIGHT 600
#define EDITOR_APP_NAME "LeEditor"

LeEditorInstance::LeEditorInstance(void)
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	m_gltfWindow = glfwCreateWindow(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, EDITOR_APP_NAME, nullptr, nullptr);

	InitEngineParams Params =
	{
		.RendererParams =
		{
			.api = LeRenderApi::LE_RENDER_API_VULKAN,
			.pWindow = m_gltfWindow,
			.pAppName = EDITOR_APP_NAME
		}
	};
	LeEngineInit(Params);
}

LeEditorInstance::~LeEditorInstance(void)
{
	LeEngineShutDown();
	LeEngineDeInit();
	glfwTerminate();
}

void LeEditorInstance::Run(void)
{

	LeCamera Camera(PROJECTION_TYPE_PERSPECTIVE, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);

	while (!glfwWindowShouldClose(m_gltfWindow))
	{
		glfwPollEvents();

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Hello, world!");
		ImGui::Text("This is some useful text.");
		ImGui::End();

		ImGui::Render();


		if (LeRenderInterface::BeginRender(&Camera, false))
			//if (LeRenderInterface::Render(pMesh))
				LeRenderInterface::EndRender();
	}

}

int main(void)
{
	LeEditorInstance EditorInstance;
	EditorInstance.Run();

	return 0;
}
