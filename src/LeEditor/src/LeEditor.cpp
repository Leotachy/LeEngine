
// Lukas Andrijauskas, 11th April 2025
// Simple editor for LeEngine

#include "LeCore/lecore.h"

#include "LeEditor/LeEditor.h"
#include "LeEditor/LeViewport.h"

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

			.windowParams =
			{
				.pWindow = m_gltfWindow,
				.pAppName = EDITOR_APP_NAME
			},
			.viewportParams = 
			{
				.uViewportWidth = DEFAULT_SCREEN_WIDTH,
				.uViewportHeight = DEFAULT_SCREEN_HEIGHT,
				.bRenderViewportToImGuiTexture = true
			}
		}
	};
	LeEngineInit(Params);

	m_bDocksBuilt = false;
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

		Update();

		ImGui::Render();


		LeRenderInterface::RenderScene(NULL, &Camera, false);
	}

}

void LeEditorInstance::Update(void)
{
	if (!m_bDocksBuilt)
	{
		MakeDockWindows();
		m_bDocksBuilt = true;
	}
	else
	{
		ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
		ImGui::DockSpaceOverViewport(dockspace_id, ImGui::GetMainViewport());
	}




}

void LeEditorInstance::MakeDockWindows(void)
{

	ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus;

	ImGui::Begin("DockHost", nullptr, flags);

	ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");

	ImGui::DockSpace(dockspace_id);

	ImGui::DockBuilderRemoveNode(dockspace_id);
	ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
	ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

	ImGuiID treeViewDockId;
	ImGuiID viewportDockId;
	ImGuiID toolbarDockId;
	ImGuiID propertiesDockId;
	ImGuiID temp;

	ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.1f, &temp, &toolbarDockId);
	ImGui::DockBuilderSplitNode(temp, ImGuiDir_Left, 0.3f, &treeViewDockId, &viewportDockId);
	ImGui::DockBuilderSplitNode(treeViewDockId, ImGuiDir_Down, 0.6f, &propertiesDockId, &treeViewDockId);

	ImGui::DockBuilderDockWindow("Tree view", treeViewDockId);
	ImGui::DockBuilderDockWindow("Viewport", viewportDockId);
	ImGui::DockBuilderDockWindow("Toolbar", toolbarDockId);
	ImGui::DockBuilderDockWindow("Properties", propertiesDockId);
	ImGui::DockBuilderFinish(dockspace_id);

	ImGui::End();
}

int main(void)
{
	LeEditorInstance EditorInstance;
	EditorInstance.Run();

	return 0;
}
