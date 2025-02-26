#ifndef Target_Manager_h__
#define Target_Manager_h__

#include "Engine_Defines.h"
#include "Base.h"
#include "Component.h"
#include "Optimization.h"

BEGIN(Engine)

class CRenderTarget;
class ENGINE_DLL CTarget_Manager final : public CBase
{
	DECLARE_SINGLETON(CTarget_Manager)
private:
	explicit CTarget_Manager();
	virtual ~CTarget_Manager();
public:
	LPDIRECT3DTEXTURE9 Get_TargetTexture(const _tchar* pTargetTag);
public:
	HRESULT Add_RenderTarget(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pTargetTag, _uint iWidth, _uint iHeight, D3DFORMAT Format, D3DXCOLOR ClearColor);
	HRESULT Add_MRT(const _tchar* pMRTTag, const _tchar* pTargetTag);
public:
	HRESULT Begin_MRT(const _tchar* pMRTTag);
	HRESULT End_MRT(const _tchar* pMRTTag);
public:
	HRESULT Ready_Debug_Buffer(const _tchar* pTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug_Buffer(const _tchar* pMRTTag);
private:
	unordered_map<const _tchar*, CRenderTarget*>			m_RenderTargets;
	typedef unordered_map<const _tchar*, CRenderTarget*>	RENDERTARGETS;
private:
	unordered_map<const _tchar*, list<CRenderTarget*>>			m_MRT;
	typedef unordered_map<const _tchar*, list<CRenderTarget*>>	MRT;
private:
	CRenderTarget* Find_Target(const _tchar* pTargetTag);
	list<CRenderTarget*>* Find_MRT(const _tchar* pMRTTag);

private:
	virtual void Free();	
};

END

#endif // Target_Manager_h__