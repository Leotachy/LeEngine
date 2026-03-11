

#include "LeCore/LeCore.h"
#include "LeCore/LeTexture/LeTexture.h"

LeTexture::LeTexture()
{
	m_name[0] = '\0';
	m_pixels = NULL;
	m_width = m_height = 0;
}

LeTexture::~LeTexture()
{

}