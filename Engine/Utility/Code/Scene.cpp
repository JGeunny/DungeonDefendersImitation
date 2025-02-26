#include "Scene.h"

USING(Engine)

Engine::CScene::CScene(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
{
	m_pGraphicDev->AddRef();
}

Engine::CScene::~CScene(void)
{

}

HRESULT Engine::CScene::Ready_Scene(void)
{
	return S_OK;
}

_int Engine::CScene::Update_Scene(const _float& fTimeDelta)
{
	_int iEvent = 0;

	for (auto& pair : m_mapLayer)
		iEvent = pair.second->Update_Layer(fTimeDelta);

	return EVENT_DEFAULT;
}

void CScene::LateUpdate_Scene(void)
{
	for (auto& pair : m_mapLayer)
	{
		pair.second->LateUpdate_Layer();
	}
}

void Engine::CScene::Free(void)
{
	Safe_Release(m_pGraphicDev);

	for_each(m_mapLayer.begin(), m_mapLayer.end(), CDeleteMap());
	m_mapLayer.clear();
}

CComponent* Engine::CScene::Get_Component(const _tchar* pLayerTag,
											const _tchar* pObjTag,
											const _tchar* pComponentTag,
											COMPONENTID eID)
{
	auto	pair = m_mapLayer.find(pLayerTag);
	//없다면 만들어라~
	if (pair == m_mapLayer.end())
	{
		CLayer* pLayer = CLayer::Create();
		m_mapLayer.emplace(pLayerTag, pLayer);
	}
	return pair->second->Get_Component(pObjTag, pComponentTag, eID);
}

multimap<std::wstring, CGameObject*>* CScene::Get_ObjectMultimap(const _tchar* pLayerTag)
{
	auto	pair = m_mapLayer.find(pLayerTag);

	if (pair == m_mapLayer.end())
		return nullptr;

	return pair->second->Get_ObjectMultimap();
}

HRESULT CScene::Add_Object(const _tchar * pLayerTag, const _tchar * pObjTag, CGameObject * pGameObject)
{
	HRESULT hr = E_FAIL;
	auto	iter = m_mapLayer.find(pLayerTag);

	if (iter == m_mapLayer.end())
	{
		Engine::CLayer*		pLayer = Engine::CLayer::Create();
		NULL_CHECK_RETURN(pLayer, E_FAIL);
		m_mapLayer.emplace(pLayerTag, pLayer);
		hr = pLayer->Add_Object(pObjTag, pGameObject);
	}
	else
	{
		hr = iter->second->Add_Object(pObjTag, pGameObject);
	}

	return hr;
}
