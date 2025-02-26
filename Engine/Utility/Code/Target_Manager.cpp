#include "Target_Manager.h"
#include "RenderTarget.h"

USING(Engine)
IMPLEMENT_SINGLETON(CTarget_Manager)

CTarget_Manager::CTarget_Manager()
{
}


CTarget_Manager::~CTarget_Manager()
{
	Free();
}

LPDIRECT3DTEXTURE9 CTarget_Manager::Get_TargetTexture(const _tchar * pTargetTag)
{
	CRenderTarget* pRenderTarget = Find_Target(pTargetTag);
	if (nullptr == pRenderTarget)
		return nullptr;

	return pRenderTarget->Get_TargetTexture();
}

HRESULT CTarget_Manager::Add_RenderTarget(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar * pTargetTag, _uint iWidth, _uint iHeight, D3DFORMAT Format, D3DXCOLOR ClearColor)
{
	if (nullptr != Find_Target(pTargetTag))
		return E_FAIL;

	CRenderTarget*	pRenderTarget = CRenderTarget::Create(pGraphic_Device, iWidth, iHeight, Format, ClearColor);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_RenderTargets.insert(RENDERTARGETS::value_type(pTargetTag, pRenderTarget));

	return NOERROR;
}

HRESULT CTarget_Manager::Add_MRT(const _tchar * pMRTTag, const _tchar * pTargetTag)
{
	CRenderTarget* pRenderTarget = Find_Target(pTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	list<CRenderTarget*>*	pMRTList = Find_MRT(pMRTTag);

	if (nullptr == pMRTList)
	{
		list<CRenderTarget*>		MRTList;
		MRTList.push_back(pRenderTarget);

		m_MRT.insert(MRT::value_type(pMRTTag, MRTList));
	}
	else
		pMRTList->push_back(pRenderTarget);

	Safe_AddRef(pRenderTarget);

	return NOERROR;
}

HRESULT CTarget_Manager::Begin_MRT(const _tchar * pMRTTag)
{
	list<CRenderTarget*>*	pMRTList = Find_MRT(pMRTTag);

	if (nullptr == pMRTList)
		return E_FAIL;

	for (auto& pTarget : *pMRTList)
	{
		if (nullptr != pTarget)
			pTarget->Clear_RenderTarget();
	}

	_uint		iIndex = 0;

	for (auto& pTarget : *pMRTList)
	{
		if (FAILED(pTarget->SetUp_OnGraphicDev(iIndex++)))
			return E_FAIL;
	}

	return NOERROR;
}

HRESULT CTarget_Manager::End_MRT(const _tchar * pMRTTag)
{
	list<CRenderTarget*>*	pMRTList = Find_MRT(pMRTTag);

	if (nullptr == pMRTList)
		return E_FAIL;

	_uint		iIndex = 0;

	for (auto& pTarget : *pMRTList)
	{
		if (FAILED(pTarget->Release_OnGraphicDev(iIndex++)))
			return E_FAIL;
	}

	return NOERROR;
}

HRESULT CTarget_Manager::Ready_Debug_Buffer(const _tchar * pTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	CRenderTarget* pRenderTarget = Find_Target(pTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Ready_Debug_Buffer(fX, fY, fSizeX, fSizeY);
}

HRESULT CTarget_Manager::Render_Debug_Buffer(const _tchar * pMRTTag)
{
	list<CRenderTarget*>*	pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	for (auto& pRenderTarget : *pMRTList)
	{
		if (nullptr != pRenderTarget)
			pRenderTarget->Render_Debug_Buffer();
	}

	return NOERROR;
}

CRenderTarget * CTarget_Manager::Find_Target(const _tchar * pTargetTag)
{
	auto	iter = find_if(m_RenderTargets.begin(), m_RenderTargets.end(), CTag_Finder(pTargetTag));

	if (iter == m_RenderTargets.end())
		return nullptr;

	return iter->second;
}

list<CRenderTarget*>* CTarget_Manager::Find_MRT(const _tchar * pMRTTag)
{
	auto	iter = find_if(m_MRT.begin(), m_MRT.end(), CTag_Finder(pMRTTag));

	if (iter == m_MRT.end())
		return nullptr;

	return &iter->second;
}

void CTarget_Manager::Free()
{
	for (auto& Pair : m_MRT)
	{
		for (auto& pRenderTarget : Pair.second)		
			Safe_Release(pRenderTarget);

		Pair.second.clear();
	}
	m_MRT.clear();
		

	for (auto& Pair : m_RenderTargets)
		Safe_Release(Pair.second);

	m_RenderTargets.clear();
}
