#include "Layer.h"

USING(Engine)

Engine::CLayer::CLayer(void)
{

}

Engine::CLayer::~CLayer(void)
{

}

HRESULT Engine::CLayer::Ready_Layer(void)
{
	return S_OK;
}

_int Engine::CLayer::Update_Layer(const _float& fTimeDelta)
{
	static _int	iEvent = 0;
	
	auto pair = m_mapObject.begin();
	while(pair != m_mapObject.end())
	{
		iEvent = pair->second->Update_Object(fTimeDelta);
		if (iEvent == EVENT_DEAD)
		{
			Engine::Safe_Release(pair->second);
			pair = m_mapObject.erase(pair);
		}
		else
			++pair;
	}
	return iEvent;
}

void CLayer::LateUpdate_Layer(void)
{
	for (auto& pair : m_mapObject)
	{
		pair.second->LateUpdate_Object();
	}
}

void CLayer::Get_ObjectList(std::list<CGameObject*>* plstObject)
{
	plstObject->clear();
	for (auto pair_Object : m_mapObject)
		plstObject->emplace_back(pair_Object.second);
}

CLayer* Engine::CLayer::Create(void)
{
	CLayer*		pInstance = new CLayer;

	if (FAILED(pInstance->Ready_Layer()))
	{
		MSG_BOX("Layer Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Engine::CLayer::Free(void)
{
	for_each(m_mapObject.begin(), m_mapObject.end(), CDeleteMap());
	m_mapObject.clear();
}

HRESULT Engine::CLayer::Add_Object(const _tchar* pObjTag, CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_mapObject.emplace(pObjTag, pGameObject);

	return S_OK;
}

CComponent* Engine::CLayer::Get_Component(const _tchar* pObjTag, const _tchar* pComponentTag, COMPONENTID eID)
{
	auto	pair = m_mapObject.find(pObjTag);
	if (pair == m_mapObject.end())
		return nullptr;
	return pair->second->Get_Component(pComponentTag, eID);
}

multimap<std::wstring, CGameObject*>* CLayer::Get_ObjectMultimap()
{
	return &m_mapObject;
}