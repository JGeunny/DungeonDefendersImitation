#ifndef Shader_h__
#define Shader_h__

#include "Engine_Defines.h"
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader : public CComponent
{
private:
	explicit	CShader(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CShader(const CShader& rhs);
	virtual		~CShader(void);

public:
	LPD3DXEFFECT	Get_EffectHandle(void) { return m_pEffect; }

public:
	HRESULT		Ready_Shader(const _tchar* pFilePath);

private:
	LPDIRECT3DDEVICE9			m_pGraphicDev;
	LPD3DXEFFECT				m_pEffect;
	LPD3DXBUFFER				m_pErrMsg;

public:
	HRESULT Set_Value(D3DXHANDLE hParameter, void* pValue, size_t iSize);
	HRESULT Set_Texture(D3DXHANDLE hParameter, LPDIRECT3DBASETEXTURE9 pTexture);
	HRESULT Set_Bool(D3DXHANDLE hParameter, _bool Value);
public:
	HRESULT Commit_Changes();
	HRESULT Begin_Shader(_uint* pNumPasses);
	HRESULT Begin_Pass(_uint iPassIndex);
	HRESULT End_Shader();
	HRESULT End_Pass();

public:
	static	CShader*		Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath);
	virtual CComponent*		Clone(void);
	virtual void			Free(void);

};

END
#endif // Shader_h__
