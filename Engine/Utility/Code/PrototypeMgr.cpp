#include "PrototypeMgr.h"

USING(Engine)
IMPLEMENT_SINGLETON(CPrototypeMgr)

Engine::CPrototypeMgr::CPrototypeMgr(void)
{

}

Engine::CPrototypeMgr::~CPrototypeMgr(void)
{
	Free();
}

HRESULT Engine::CPrototypeMgr::Ready_Prototype(const _tchar* pProtoTag, CComponent* pInstance)
{
	CComponent*		pPrototype = Find_Prototype(pProtoTag);

	if (nullptr != pPrototype)
		return E_FAIL;

	m_mapProtoType.emplace(pProtoTag, pInstance);

	return S_OK;
}

CComponent* Engine::CPrototypeMgr::Clone_Prototype(const _tchar* pProtoTag)
{
	CComponent*		pInstance = Find_Prototype(pProtoTag);

	NULL_CHECK_RETURN(pInstance, nullptr);

	return pInstance->Clone();
}

CComponent* Engine::CPrototypeMgr::Find_Prototype(const _tchar* pProtoTag)
{
	auto	iter = m_mapProtoType.find(pProtoTag);
	if (iter == m_mapProtoType.end())
		return nullptr;

	return iter->second;
}

void Engine::CPrototypeMgr::Free(void)
{
	for_each(m_mapProtoType.begin(), m_mapProtoType.end(), CDeleteMap());
	m_mapProtoType.clear();
}

