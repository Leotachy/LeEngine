
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRender.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeMesh/LeMesh.h"
#include "LeCore/LeMesh/LeMeshVulkan/LeMeshVulkan.h"

LeMatBlock::LeMatBlock()
{
	m_Flags = 0;

	m_material = NULL;
}

LeMatBlock::~LeMatBlock()
{

}

LeMatBlock* LeMatBlock::Create(void)
{

	switch (LeRenderInterface::GetRenderApi())
	{
	case LeRenderApi::LE_RENDER_API_VULKAN:
		return new LeMatBlockVk();
		break;
	}

	LE_DEBUG_ASSERT(0);
	return NULL;

}



void LeMatBlock::SetMaterial(LeMaterial* m)
{
	m_material = m;
}

LeMaterial* LeMatBlock::GetMaterial(void)
{
	return m_material;
}






LeMesh::LeMesh()
{

}

LeMesh::~LeMesh()
{
	PurgeAllMatblocks();
}

LeMesh* LeMesh::Create(void)
{
	switch (LeRenderInterface::GetRenderApi())
	{
	case LeRenderApi::LE_RENDER_API_VULKAN:
		return new LeMeshVk();
		break;
	}

	LE_DEBUG_ASSERT(0);
	return NULL;
}

void LeMesh::AddMatblock(LeMatBlock* pMb)
{
	m_Matblocks.push_back(pMb);
}

uint32 LeMesh::GetNumMatblocks(void)
{ 
	return uint32(m_Matblocks.size()); 

}

LeMatBlock* LeMesh::GetMatblock(uint32 i)
{
	LE_DEBUG_ASSERT(i < m_Matblocks.size());
	return m_Matblocks[i];
}

void LeMesh::RemoveMatblock(uint32 i)
{
	LE_DEBUG_ASSERT(i < m_Matblocks.size());
	m_Matblocks.erase(m_Matblocks.begin() + i);
}

void LeMesh::PurgeAllMatblocks(void)
{
	for (LeMatBlock* pMb : m_Matblocks)
		delete pMb;

	m_Matblocks.clear();
}