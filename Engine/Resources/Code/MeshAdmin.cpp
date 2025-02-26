#include "MeshAdmin.h"

#include "Engine_Defines.h"
#include "Export_Resources.h"
USING(Engine)
Engine::CMeshAdmin::CMeshAdmin(CMesh** ppMeshCom, CCollider** ppColliderCom)
	:m_ppMeshCom(ppMeshCom)
	, m_ppColliderCom(ppColliderCom)
	, m_strMeshKey(L"")
	, m_eResourceType(RESOURCEID::RESOURCE_END)
{
	*m_ppMeshCom = nullptr;
}

Engine::CMeshAdmin::~CMeshAdmin(void)
{

}

HRESULT CMeshAdmin::Ready_MeshAdmin()
{
	//Change_Mesh(RESOURCE_STATIC, L"Mesh_Default");
	return S_OK;
}

void CMeshAdmin::Change_Mesh(RESOURCEID eResourceType, const _tchar * pMeshName)
{
	if (m_strMeshKey == pMeshName && m_eResourceType == eResourceType)
		return;
	m_strMeshKey = pMeshName;
	m_eResourceType = eResourceType;

	Safe_Release(*m_ppMeshCom);
	*m_ppMeshCom = dynamic_cast<CMesh*>(Engine::Clone(eResourceType, pMeshName));
	if (Engine::MESHID::MESH_DYNAMIC == (*m_ppMeshCom)->Get_MeshId())
		dynamic_cast<CDynamicMesh*>(*m_ppMeshCom)->Set_AnimationSet(0);
	
	if (m_ppColliderCom)
	{
		(*m_ppColliderCom)->Change_Collider((*m_ppMeshCom)->Get_Range());
	}
}

const D3DXFRAME_DERIVED * CMeshAdmin::Get_FrameByName(const char * pFrameName)
{
	if (Engine::MESHID::MESH_DYNAMIC == (*m_ppMeshCom)->Get_MeshId())
		return dynamic_cast<CDynamicMesh*>(*m_ppMeshCom)->Get_FrameByName(pFrameName);
	return nullptr;
}

void CMeshAdmin::Set_AnimationSet(std::string strAniName)
{
	if (Engine::MESHID::MESH_DYNAMIC == Get_MeshId())
		dynamic_cast<Engine::CDynamicMesh*>(*m_ppMeshCom)->Set_AnimationSet(strAniName);
}

void CMeshAdmin::Set_AnimationSet(_int index)
{
	if (Engine::MESHID::MESH_DYNAMIC == Get_MeshId())
		dynamic_cast<Engine::CDynamicMesh*>(*m_ppMeshCom)->Set_AnimationSet(index);
}

std::string CMeshAdmin::Get_AnimationSet()
{
	if (Engine::MESHID::MESH_DYNAMIC == Get_MeshId())
		return dynamic_cast<Engine::CDynamicMesh*>(*m_ppMeshCom)->Get_AnimationSet();
	return "";
}

_bool CMeshAdmin::Is_AnimationSetEnd(RANGEFLOAT* pRangeFloat)
{
	if (Engine::MESHID::MESH_DYNAMIC == Get_MeshId())
	{
		return dynamic_cast<Engine::CDynamicMesh*>(*m_ppMeshCom)->Is_AnimationSetEnd(pRangeFloat);
	}
	return false;
}

void CMeshAdmin::Play_Animation(_float fTimeDelta, _int* piCount)
{
	if (Engine::MESHID::MESH_DYNAMIC == Get_MeshId())
		dynamic_cast<Engine::CDynamicMesh*>(*m_ppMeshCom)->Play_Animation(fTimeDelta, piCount);
}

CMeshAdmin * CMeshAdmin::Create(CMesh** ppMeshCom, CCollider** ppColliderCom)
{
	CMeshAdmin* pInstance = new CMeshAdmin(ppMeshCom, ppColliderCom);

	if (FAILED(pInstance->Ready_MeshAdmin()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void Engine::CMeshAdmin::Free(void)
{
	Safe_Release(*m_ppMeshCom);
}

