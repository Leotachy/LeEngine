
#pragma once

#include "LeCore/LeMesh/LeMesh.h"

class LeWorld
{
public:
	LeWorld(void);
	~LeWorld(void);

private:

	LeMesh* m_worldMesh;
};