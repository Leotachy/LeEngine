
#pragma once

#include "LeCore/LeCamera/LeCamera.h"

class LeViewport
{
public:
	LeViewport();

	void Update(ImTextureID RenderedScene);


	void SetCamera(LeCamera* pCamera);
	LeCamera* GetCamera(void);

private:
	LeCamera* m_camera;

};