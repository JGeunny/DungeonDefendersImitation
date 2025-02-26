#ifndef TEXTUREADMIN_H__
#define TEXTUREADMIN_H__

#include "Component.h"
#include "Export_Resources.h"

BEGIN(Engine)

class ENGINE_DLL CTextureAdmin : public Engine::CComponent
{
protected:
	explicit				CTextureAdmin(Engine::CTexture** _ppTextureCom, RESOURCEID eResourceType, std::wstring _strImageKey = L"");
public:
	virtual					~CTextureAdmin(void);

public:	
	void					Render_Texture(const _uint& iIndex = 0);
	void					Render_Texture(LPD3DXEFFECT pEffect, const char* pShaderName, const _uint& iIndex = 0);
	void					Change_Texture(RESOURCEID eResourceType, std::wstring _strImageKey);

	std::wstring			Get_ImageKey() { return m_strImageKey; }
	int						Get_MaxCount();
public:
	Engine::CTexture**		m_ppTextureCom;

protected:
	std::wstring			m_strImageKey;

public:
	static CTextureAdmin*	Create(Engine::CTexture** _ppTextureCom, RESOURCEID eResourceType, std::wstring _strImageKey = L"");
	virtual void			Free(void);
};

END

#endif TEXTUREADMIN_H__