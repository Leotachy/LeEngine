
#include "LeCore/lecore.h"

#include "LeCore/LeScene/LeScene.h"
#include "LeCore/LeRender/LeRender.h"

LeScene::LeScene()
{
	
}

void LeScene::SerializeBinaryChunk(LeStreamFile* pFileStream)
{
	LeBinaryChunk::SerializeBinaryChunk(pFileStream);

	m_textureDictionary.SerializeBinaryChunk(pFileStream);
	m_materialDictionary.SerializeBinaryChunk(pFileStream);
	m_world.SerializeBinaryChunk(pFileStream);

}

void LeScene::DeserializeBinaryChunk(LeStreamFile* pFileStream)
{

	LeBinaryChunk::DeserializeBinaryChunk(pFileStream);

	m_textureDictionary.DeserializeBinaryChunk(pFileStream);
	m_materialDictionary.DeserializeBinaryChunk(pFileStream);
	m_world.DeserializeBinaryChunk(pFileStream);

	Fixup();
}

void LeScene::Fixup(void)
{
	m_materialDictionary.ForEachMaterial([this](LeMaterial* pMaterial)
		{

			const LeShaderDictionary* pShaderDictionary = LeRenderInterface::GetShaderDictionary();
			LeShader* pShader = pShaderDictionary->FindShader(pMaterial->GetAttributes()->ShaderHash);
			if (pShader == NULL)
			{
				LE_ERROR("Couldn't find the shader for material \"%s\"", pMaterial->GetName());
			}
			pMaterial->SetShader(pShader);

			
			uint64 BaseTextureHash = pMaterial->GetAttributes()->BaseTextureHash;
			if (BaseTextureHash)
			{
				LeTexture* pTexture = m_textureDictionary.FindTexture(BaseTextureHash);

				if (pTexture == NULL)
				{
					LE_ERROR("Couldn't find the texture for material \"%s\"", pMaterial->GetName());
				}
				pMaterial->SetBaseTexture(pTexture);
			}

		});

	LeMesh* pWorldMesh = m_world.GetMesh();
	pWorldMesh->ForEachMatblock([this](LeMatBlock* pMatblock)
		{

			uint64 MaterialHash = pMatblock->GetMaterialHash();

			if (!MaterialHash)
			{
				LE_ERROR("Matblock doesn't have a material!");
				return;
			}

			LeMaterial* pMaterial = m_materialDictionary.FindMaterial(MaterialHash);
			
			if (!pMaterial)
			{
				LE_ERROR("Couldn't find the material for matblock!");
			}

				pMatblock->SetMaterial(pMaterial);
		});
}