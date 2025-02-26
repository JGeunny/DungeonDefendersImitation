#include "Export_Utility.h"

USING(Engine)
IMPLEMENT_SINGLETON(CManagement)

Engine::CManagement::CManagement(void)
{

}

Engine::CManagement::~CManagement(void)
{
	Free();
}

HRESULT Engine::CManagement::SetUp_Scene(CScene* pScene)
{
	Safe_Release(m_pScene);

	Get_Renderer()->Clear_RenderGroup();

	m_pScene = pScene;

	return S_OK;
}

HRESULT CManagement::Ready_Management(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CShader*	pShader = nullptr;

	//pShader = CShader::Create(pGraphicDev, L"../../Reference/Header/Shader_Sample.hpp");
	//NULL_CHECK_RETURN(pShader, E_FAIL);
	//FAILED_CHECK_RETURN(Ready_Prototype(L"Shader_Sample", pShader), E_FAIL);

	pShader = CShader::Create(pGraphicDev, L"../../Reference/Header/Shader_Terrain.hpp");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Shader_Terrain", pShader), E_FAIL);

	pShader = CShader::Create(pGraphicDev, L"../../Reference/Header/Shader_Mesh.hpp");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Shader_Mesh", pShader), E_FAIL);

	pShader = CShader::Create(pGraphicDev, L"../../Reference/Header/Shader_Blend.fx");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Shader_Blend", pShader), E_FAIL);

	pShader = CShader::Create(pGraphicDev, L"../../Reference/Header/Shader_LightAcc.fx");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Shader_LightAcc", pShader), E_FAIL);

	return S_OK;
}

_int Engine::CManagement::Update_Scene(const _float& fTimeDelta)
{
	_int iEvent = 0;
	if (nullptr != m_pScene)
	{
		iEvent = m_pScene->Update_Scene(fTimeDelta);
	}

	return EVENT_DEFAULT;
}

void CManagement::LateUpdate_Scene(void)
{
	if (nullptr != m_pScene)
	{
		m_pScene->LateUpdate_Scene();
	}
}

void Engine::CManagement::Render_Scene(LPDIRECT3DDEVICE9 pGraphicDev)
{
	Get_Renderer()->Render_GameObject(pGraphicDev);

	if (nullptr != m_pScene)
		m_pScene->Render_Scene();
}

void Engine::CManagement::Free(void)
{
	Safe_Release(m_pScene);
}

CComponent* Engine::CManagement::Get_Component(const _tchar* pLayerTag,
												const _tchar* pObjTag,
												const _tchar* pComponentTag,
												COMPONENTID eID)
{
	if (nullptr == m_pScene)
		return nullptr;

	return m_pScene->Get_Component(pLayerTag, pObjTag, pComponentTag, eID);
}

multimap<std::wstring, CGameObject*>* CManagement::Get_ObjectMultimap(const _tchar * pLayerTag)
{
	if (nullptr == m_pScene)
		return nullptr;

	return m_pScene->Get_ObjectMultimap(pLayerTag);
}

HRESULT CManagement::Add_Object(const _tchar * pLayerTag, const _tchar * pObjTag, CGameObject * pGameObject)
{
	HRESULT hr = E_FAIL;
	if (nullptr == m_pScene) return hr;
	hr = m_pScene->Add_Object(pLayerTag, pObjTag, pGameObject);
	return hr;
}

Engine::CGameObject*	CManagement::Find_NearTarget(const _tchar* pszLayer, const _tchar* pszTarget, Engine::CGameObject* pGameObject_MySelf, _float fMaxDist)
{
	//auto pair_Target = Get_ObjectMultimap(pszLayer)->equal_range(pszTarget);
	//auto vPos_MySelf = ((Engine::CTransform*)pGameObject_MySelf->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC))->Get_Pos();
	//auto find_Target = std::find_if(pair_Target.first, pair_Target.second, [&](auto _pTarget) {
	//	if (pGameObject_MySelf == _pTarget.second)
	//		return false;
	//	auto vPos_Target = ((Engine::CTransform*)_pTarget.second->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC))->Get_Pos();
	//	return fMaxDist > D3DXVec3Length(&(vPos_Target - vPos_MySelf));
	//});

	//if (find_Target != pair_Target.second)
	//	return find_Target->second;

	//return nullptr;

	auto pair_Target = Get_ObjectMultimap(pszLayer)->equal_range(pszTarget);
	auto vPos_MySelf = ((Engine::CTransform*)pGameObject_MySelf->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC))->Get_Pos();
	Engine::CGameObject* pTarget = nullptr;
	std::for_each(pair_Target.first, pair_Target.second, [&](auto _pTarget) {
		if (pGameObject_MySelf == _pTarget.second)
			return;
		auto vPos_Target = ((Engine::CTransform*)_pTarget.second->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC))->Get_Pos();
		auto fDist = D3DXVec3Length(&(vPos_Target - vPos_MySelf));
		if (fMaxDist > fDist)
		{
			fMaxDist = fDist;
			pTarget = _pTarget.second;
		}
	});
	return pTarget;
}

Engine::CGameObject * CManagement::Find_NearTarget(const _tchar * pszLayer, Engine::TEAMTYPE eTeam, Engine::CGameObject* pGameObject_MySelf, _float fMaxDist)
{
	//auto pLayer = Get_ObjectMultimap(pszLayer);
	//auto vPos_MySelf = ((Engine::CTransform*)pGameObject_MySelf->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC))->Get_Pos();
	//auto find_Target = std::find_if(pLayer->begin(), pLayer->end(), [&](auto _pTarget) {
	//	if (pGameObject_MySelf == _pTarget.second)
	//		return false;
	//	if (_pTarget.first == L"Bullet")
	//		return false;
	//	if(_pTarget.second->Get_Team() == Engine::TEAMTYPE::TEAMTYPE_NONE)
	//		return false;
	//	if (eTeam != _pTarget.second->Get_Team())
	//		return false;
	//	auto vPos_Target = ((Engine::CTransform*)_pTarget.second->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC))->Get_Pos();
	//	return fMaxDist > D3DXVec3Length(&(vPos_Target - vPos_MySelf));
	//});
	//
	//if (find_Target != pLayer->end())
	//	return find_Target->second;

	//return nullptr;
	
	auto pLayer = Get_ObjectMultimap(pszLayer);
	auto vPos_MySelf = ((Engine::CTransform*)pGameObject_MySelf->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC))->Get_Pos();
	Engine::CGameObject* pTarget = nullptr;
	std::for_each(pLayer->begin(), pLayer->end(), [&](auto _pTarget) {
		if (_pTarget.second->Get_Event() == EVENT_DEAD)
			return;
		if (pGameObject_MySelf == _pTarget.second)
			return;
		if (_pTarget.first == L"Bullet")
			return;
		if (_pTarget.second->Get_Team() == Engine::TEAMTYPE::TEAMTYPE_NONE)
			return;
		if (eTeam != _pTarget.second->Get_Team())
			return;
		auto vPos_Target = ((Engine::CTransform*)_pTarget.second->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC))->Get_Pos();
		auto fDist = D3DXVec3Length(&(vPos_Target - vPos_MySelf));
		if (fMaxDist > fDist)
		{
			fMaxDist = fDist;
			pTarget = _pTarget.second;
		}
	});
	return pTarget;
}
