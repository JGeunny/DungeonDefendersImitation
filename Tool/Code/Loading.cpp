#include "stdafx.h"
#include "Loading.h"

#include "Export_Function.h"

CLoading::CLoading(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
{
	m_pGraphicDev->AddRef();
	ZeroMemory(m_szLoading, sizeof(_tchar) * 128);
}

CLoading::~CLoading(void)
{

}

_uint	APIENTRY CLoading::Thread_Main(void* pArg)
{
	CLoading*		pLoading = (CLoading*)pArg;


	_uint iFlag = 0;

	EnterCriticalSection(pLoading->Get_Crt());

	switch (pLoading->Get_LoadingID())
	{
	case LOADING_STAGE:
		iFlag = pLoading->Loading_ForStage();
		break;

	case LOADING_BOSS:
		break;
	}

	LeaveCriticalSection(pLoading->Get_Crt());

	_endthreadex(0);

	return iFlag;
}

HRESULT CLoading::Ready_Loading(LOADINGID eLoadingID)
{
	InitializeCriticalSection(&m_Crt);

	m_hThread = (HANDLE)_beginthreadex(NULL, 0, Thread_Main, this, 0, NULL);

	m_LoadingID = eLoadingID;

	return S_OK;
}

_uint CLoading::Loading_ForStage(void)
{
	// 텍스쳐
	lstrcpy(m_szLoading, L"Texture Loading.........................");
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, RESOURCEID::RESOURCE_STATIC, L"Texture_Default", Engine::TEX_NORMAL, SRC_PATH(L"../Bin/Resource/Texture/Default/Default_%d.png"), 1), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, RESOURCE_STATIC, L"Texture_Terrain", Engine::TEX_NORMAL, SRC_PATH(L"../Bin/Resource/Texture/Terrain/Terrain%d.png"), 1), E_FAIL);
	//FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, RESOURCE_STATIC, L"Texture_Terrain", Engine::TEX_NORMAL, SRC_PATH(L"../Bin/Resource/Texture/Terrain/Grass_%d.tga"), 2), E_FAIL);

	// 버퍼
	lstrcpy(m_szLoading, L"Buffer Loading.........................");
	FAILED_CHECK_RETURN(Engine::Ready_Buffers(m_pGraphicDev, RESOURCE_STATIC, L"Buffer_RcTex", Engine::BUFFERID::BUFFER_RCTEX), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Buffers(m_pGraphicDev, RESOURCE_STATIC, L"Buffer_CubeTex", Engine::BUFFER_CUBETEX), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Buffers(m_pGraphicDev, RESOURCE_STATIC, L"Buffer_TerrainTex", Engine::BUFFER_TERRAINTEX, 129, 129, 1), E_FAIL);

	// 메쉬
	lstrcpy(m_szLoading, L"Mesh Loading.........................");
	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev, RESOURCE_STATIC, L"Mesh_Default", Engine::TYPE_STATIC, SRC_PATH(L"../Bin/Resource/Mesh/StaticMesh/Default/"), L"Default.x"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev, RESOURCE_STATIC, L"Mesh_Default_Point", Engine::TYPE_STATIC, SRC_PATH(L"../Bin/Resource/Mesh/StaticMesh/Default/Point/"), L"Default_Point.x"), E_FAIL);
	//FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev, RESOURCE_STAGE, L"Mesh_MapLast", Engine::TYPE_STATIC, SRC_PATH(L"../Bin/Resource/Mesh/StaticMesh/Map/"), L"MapLast.x"), E_FAIL);
	//FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev, RESOURCE_STAGE, L"Mesh_Stone", Engine::TYPE_STATIC, SRC_PATH(L"../Bin/Resource/Mesh/StaticMesh/TombStone/"), L"TombStone.x"), E_FAIL);
	//FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev, RESOURCE_STAGE, L"Mesh_Mage", Engine::TYPE_DYNAMIC, SRC_PATH(L"../Bin/Resource/Mesh/DynamicMesh/Mage/"), L"Mage.x"), E_FAIL);
	//FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev, RESOURCE_STAGE, L"Mesh_Sword", Engine::TYPE_STATIC, SRC_PATH(L"../Bin/Resource/Mesh/StaticMesh/sword/"), L"Sword.x"), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_ResourceFromPathFile(m_pGraphicDev, SRC_PATH(L"../Bin/Resource/Data/PathInfo.txt")), E_FAIL);

	lstrcpy(m_szLoading, L"Loading Complete.......................");

	m_bFinish = true;

	return 0;
}

CLoading* CLoading::Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID eLoadingID)
{
	CLoading*	pInstance = new CLoading(pGraphicDev);

	if (FAILED(pInstance->Ready_Loading(eLoadingID)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CLoading::Free(void)
{
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
	DeleteCriticalSection(&m_Crt);

	Engine::Safe_Release(m_pGraphicDev);
}