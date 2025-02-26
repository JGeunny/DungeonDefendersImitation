#ifndef Texture_h__
#define Texture_h__

#include "Resources.h"

BEGIN(Engine)

class ENGINE_DLL CTexture : public CResources
{
protected:
	explicit CTexture(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTexture(const CTexture& rhs);
public:
	virtual ~CTexture(void);


public:
	HRESULT		Ready_Texture(const _tchar* pPath,
							TEXTURETYPE eType,
							const _uint& iCnt);

	void		Render_Texture(const _uint& iIndex = 0);
	LPDIRECT3DBASETEXTURE9	Get_Texture(const _uint& iIndex = 0);
	void		Render_Texture(LPD3DXEFFECT pEffect, const char* pShaderName, const _uint& iIndex = 0);
	int			Get_MaxCount() { return m_vecTexture.size(); }
private:
	vector<LPDIRECT3DBASETEXTURE9>		m_vecTexture;

public:
	virtual CResources*		Clone(void);
	static CTexture*		Create(LPDIRECT3DDEVICE9 pGraphicDev,
									const _tchar* pPath, 
									TEXTURETYPE eType,
									const _uint& iCnt = 1);
	virtual void Free(void);

};

END

#endif // Texture_h__
