
#include "LeCore/LeCore.h"
#include "LeCore/LeMesh/LeMesh.h"
#include "LeCore/LeMesh/LeMeshVulkan/LeMeshVulkan.h"

LeMatBlock::LeMatBlock()
{
	m_Flags = 0;

	m_pMaterial = NULL;
}

LeMatBlock::~LeMatBlock()
{

}

LeMatBlock* LeMatBlock::Create(void)
{

	switch (LeGetRenderApi())
	{
	case LeRenderApi::None:
		LE_DEBUG_ASSERT(0);
		break;

	case LeRenderApi::Vulkan:
		return new LeMatblockVk();
		break;
	}

}



void LeMatBlock::SetMaterial(LeMaterial* m)
{
	m_pMaterial = m;
	m_Flags |= MATBLOCK_MATERIAL_DIRTY;
}

const LeMaterial* LeMatBlock::GetMaterial(void)
{
	return m_pMaterial;
}






LeMesh::LeMesh()
{

}

LeMesh::~LeMesh()
{

}

void LeMesh::AddMatblock(LeMatBlock* pMb)
{

}

uint32 LeMesh::GetNumMatblocks(void)
{ 
	return uint32(m_Matblocks.size()); 

}

LeMatBlock* LeMesh::GetMatblock(uint32 i)
{
	if (i > m_Matblocks.size())
		return NULL;

	return m_Matblocks[i];
}

void LeMesh::RemoveMatblock(uint32 i)
{

}

void LeMesh::RemoveAllMatblocks(void)
{

}