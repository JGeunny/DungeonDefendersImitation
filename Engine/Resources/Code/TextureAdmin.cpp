#include "TextureAdmin.h"
#include "Texture.h"

USING(Engine)

CTextureAdmin::CTextureAdmin(Engine::CTexture ** _ppTextureCom, RESOURCEID eResourceType, std::wstring _strImageKey)
	:m_ppTextureCom(_ppTextureCom)
{
	*m_ppTextureCom = nullptr;
	Change_Texture(eResourceType, _strImageKey);
}

CTextureAdmin::~CTextureAdmin(void)
{
}

void CTextureAdmin::Render_Texture(const _uint & iIndex)
{
	if (nullptr == *m_ppTextureCom)
		return;
	(*m_ppTextureCom)->Render_Texture(iIndex);
}

void CTextureAdmin::Render_Texture(LPD3DXEFFECT pEffect, const char * pShaderName, const _uint & iIndex)
{
	if (nullptr == *m_ppTextureCom)
		return;
	(*m_ppTextureCom)->Render_Texture(pEffect, pShaderName, iIndex);
}

CTextureAdmin * CTextureAdmin::Create(Engine::CTexture ** _ppTextureCom, RESOURCEID eResourceType, std::wstring _strImageKey)
{
	return new CTextureAdmin(_ppTextureCom, eResourceType, _strImageKey);
}

void CTextureAdmin::Change_Texture(RESOURCEID eResourceType, std::wstring _strImageKey)
{
	if (m_strImageKey == _strImageKey ) return;
	m_strImageKey = _strImageKey;

	Engine::Safe_Delete(*m_ppTextureCom);
	*m_ppTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(eResourceType, m_strImageKey.c_str()));
}

int CTextureAdmin::Get_MaxCount()
{
	if (nullptr == *m_ppTextureCom)
		return 0;
	return (*m_ppTextureCom)->Get_MaxCount();
}

void CTextureAdmin::Free(void)
{
	Engine::Safe_Release(*m_ppTextureCom);
}
