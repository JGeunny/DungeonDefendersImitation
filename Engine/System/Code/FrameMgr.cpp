#include "FrameMgr.h"

USING(Engine)
IMPLEMENT_SINGLETON(CFrameMgr)

Engine::CFrameMgr::CFrameMgr(void)
{

}

Engine::CFrameMgr::~CFrameMgr(void)
{
	Free();
}

_bool Engine::CFrameMgr::IsPermit_Call(const _tchar* pFrameTag, const _float& fTimeDelta)
{
	CFrame*	pInstance = Find_Frame(pFrameTag);

	if (nullptr == pInstance)
		return false;

	return pInstance->IsPermit_Call(fTimeDelta);

}

HRESULT Engine::CFrameMgr::Ready_Frame(const _tchar* pFrameTag, const _float& fCallLimit)
{
	CFrame*	pInstance = Find_Frame(pFrameTag);

	if (nullptr != pInstance)
		return E_FAIL;

	pInstance = CFrame::Create(fCallLimit);
	NULL_CHECK_RETURN(pInstance, E_FAIL);

	m_mapFrame.emplace(pFrameTag, pInstance);

	return S_OK;
}

_int CFrameMgr::Get_FPS(const _tchar * pFrameTag)
{
	auto	iter = find_if(m_mapFrame.begin(), m_mapFrame.end(), CTag_Finder(pFrameTag));

	if (iter == m_mapFrame.end())
		return 0;

	return iter->second->Get_FPS();
}

CFrame* Engine::CFrameMgr::Find_Frame(const _tchar* pFrameTag)
{
	auto	iter = find_if(m_mapFrame.begin(), m_mapFrame.end(), CTag_Finder(pFrameTag));

	if (iter == m_mapFrame.end())
		return nullptr;

	return iter->second;
}

void Engine::CFrameMgr::Free(void)
{
	for_each(m_mapFrame.begin(), m_mapFrame.end(), CDeleteMap());
	m_mapFrame.clear();
}

