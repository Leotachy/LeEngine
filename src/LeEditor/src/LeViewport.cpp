
#include "lecore/lecore.h"

#include "LeEditor/LeViewport.h"

LeViewport::LeViewport()
{
	m_camera = NULL;
}

void LeViewport::Update(ImTextureID RenderedScene)
{

}

void LeViewport::SetCamera(LeCamera* pCamera)
{
	m_camera = pCamera;
}

LeCamera* LeViewport::GetCamera(void)
{
	return m_camera;
}