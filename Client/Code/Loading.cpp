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

std::wstring CLoading::Get_String(void)
{
	static int iCount = 0;
	++iCount;
	_tchar	m_szInfo[MID_STR];
	std::wstring m_strPoint = L"";
	int iMax = (iCount % 1000) / 100;
	for (int i = 0; i < iMax; ++i)
	{
		m_strPoint += L".";
	}
	ZeroMemory(m_szInfo, sizeof(m_szInfo));
	swprintf_s(m_szInfo, L"[%d / %d] %s %s", m_iCurLoading, m_iMaxLoading, m_szLoading, m_strPoint.c_str());
	return m_szInfo;
}

HRESULT CLoading::LoadingCount(HRESULT hResult)
{
	++m_iCurLoading;
	return S_OK;
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

	m_iMaxLoading = 26;

	return S_OK;
}

_uint CLoading::Loading_ForStage(void)
{
	// 텍스쳐
	lstrcpy(m_szLoading, L"그림 그리는 중");
	Loading_Sound();
	LoadingCount(Engine::Ready_Texture(m_pGraphicDev, RESOURCE_STAGE, L"Texture_Effect_Bust", Engine::TEX_NORMAL, SRC_PATH(L"../Bin/Resource/Texture/Effect/Bust5/Bust5_%d.png"), 32));
	LoadingCount(Engine::Ready_Texture(m_pGraphicDev, RESOURCE_STAGE, L"Texture_Effect_Hit", Engine::TEX_NORMAL, SRC_PATH(L"../Bin/Resource/Texture/Effect/Hit/Hit_%d.tga"), 6));
	LoadingCount(Engine::Ready_Texture(m_pGraphicDev, RESOURCE_STAGE, L"Texture_Effect_Aura", Engine::TEX_NORMAL, SRC_PATH(L"../Bin/Resource/Texture/Effect/Aura/Aura_%d.png"), 30));

	LoadingCount(Engine::Ready_Texture(m_pGraphicDev, RESOURCE_STATIC, L"Texture_Terrain", Engine::TEX_NORMAL, SRC_PATH(L"../Bin/Resource/Texture/Terrain/Terrain%d.png"), 1));
	LoadingCount(Engine::Ready_Texture(m_pGraphicDev, RESOURCE_STAGE, L"Texture_SkyBox", Engine::TEX_CUBE, L"../Bin/Resource/Texture/SkyBox/SkyBox%d.dds", 1));
	LoadingCount(Engine::Ready_Texture(m_pGraphicDev, RESOURCE_STAGE, L"Texture_UI", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/HUD/UI/UI%d.png", 5));

	LoadingCount(Engine::Ready_Texture(m_pGraphicDev, RESOURCE_STAGE, L"Texture_UI_PlayerGage", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/HUD/PlayerGage/PlayerGage%d.png", 3));
	
	LoadingCount(Engine::Ready_Texture(m_pGraphicDev, RESOURCE_STAGE, L"Texture_UI_LvUp", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/LvUp/LvUp%d.png", 30));
	LoadingCount(Engine::Ready_Texture(m_pGraphicDev, RESOURCE_STAGE, L"Texture_Gold", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/HUD/Gold/Gold%d.tga", 3));
	LoadingCount(Engine::Ready_Texture(m_pGraphicDev, RESOURCE_STAGE, L"Texture_ButtonArrow", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/HUD/ButtonArrow/ButtonArrow%d.png", 10));
	
	LoadingCount(Engine::Ready_Texture(m_pGraphicDev, RESOURCE_STAGE, L"Texture_UI_Skill", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Skill/Skill%d.png", 10));
	LoadingCount(Engine::Ready_Texture(m_pGraphicDev, RESOURCE_STAGE, L"Texture_Number", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/HUD/Number/%d.png", 10));
	LoadingCount(Engine::Ready_Texture(m_pGraphicDev, RESOURCE_STAGE, L"Texture_Popup", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/popup/popup%d.tga", 3));
	LoadingCount(Engine::Ready_Texture(m_pGraphicDev, RESOURCE_STAGE, L"Texture_MonsterGate", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/HUD/MonsterGate/MonsterGate%d.png", 4));
	LoadingCount(Engine::Ready_Texture(m_pGraphicDev, RESOURCE_STAGE, L"Texture_MarioHouse", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/HUD/MarioHouse/MarioHouse%d.png", 1));

	LoadingCount(Engine::Ready_Texture(m_pGraphicDev, RESOURCE_STAGE, L"Texture_Elect", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Effect/Elect/Elect%d.png", 32));
	LoadingCount(Engine::Ready_Texture(m_pGraphicDev, RESOURCE_STAGE, L"Texture_Fire", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Effect/Fire/F%d.png", 32));
	LoadingCount(Engine::Ready_Texture(m_pGraphicDev, RESOURCE_STAGE, L"Texture_TorchFire", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Effect/TorchFire/TorchFire%d.png", 36));
	
	LoadingCount(Engine::Ready_Texture(m_pGraphicDev, RESOURCE_STAGE, L"Texture_WaveInfo", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/WaveInfo/WaveInfo%d.png", 8));

	// 버퍼
	lstrcpy(m_szLoading, L"낚시 연습 중");
	//LoadingCount(Engine::Ready_Buffers(m_pGraphicDev, RESOURCE_STATIC, L"Buffer_RcTex", Engine::BUFFER_RCTEX));
	LoadingCount(Engine::Ready_Buffers(m_pGraphicDev, RESOURCE_STATIC, L"Buffer_CubeTex", Engine::BUFFER_CUBETEX));
	LoadingCount(Engine::Ready_Buffers(m_pGraphicDev, RESOURCE_STATIC, L"Buffer_ViewPort", Engine::BUFFER_VIEWPORT));
	LoadingCount(Engine::Ready_Buffers(m_pGraphicDev, RESOURCE_STATIC, L"Buffer_TerrainTex", Engine::BUFFER_TERRAINTEX, 129, 129, 1));

	// 메쉬
	lstrcpy(m_szLoading, L"몬스터 교육 중");
	LoadingCount(Engine::Ready_Meshes(m_pGraphicDev, RESOURCE_STATIC, L"Mesh_Default", Engine::TYPE_STATIC, SRC_PATH(L"../Bin/Resource/Mesh/StaticMesh/Default/"), L"Default.x"));
	LoadingCount(Engine::Ready_Meshes(m_pGraphicDev, RESOURCE_STATIC, L"Mesh_Default_Point", Engine::TYPE_STATIC, SRC_PATH(L"../Bin/Resource/Mesh/StaticMesh/Default/Point/"), L"Default_Point.x"));
	//LoadingCount(Engine::Ready_Meshes(m_pGraphicDev, RESOURCE_STAGE, L"Mesh_MapLast", Engine::TYPE_STATIC, L"../Bin/Resource/Mesh/StaticMesh/Map/", L"MapLast.x"));
	//LoadingCount(Engine::Ready_Meshes(m_pGraphicDev,	RESOURCE_STAGE,	L"Mesh_Stone", Engine::TYPE_STATIC, 		L"../Bin/Resource/Mesh/StaticMesh/TombStone/", L"TombStone.x"));
	//LoadingCount(Engine::Ready_Meshes(m_pGraphicDev, RESOURCE_STAGE, L"Mesh_Mage", Engine::TYPE_DYNAMIC, L"../Bin/Resource/Mesh/DynamicMesh/Mage/", L"Mage.x"));
	//LoadingCount(Engine::Ready_Meshes(m_pGraphicDev, RESOURCE_STAGE, L"Mesh_Sword", Engine::TYPE_STATIC, L"../Bin/Resource/Mesh/StaticMesh/sword/", L"Sword.x"));

	//LoadingCount(Engine::Ready_Meshes(m_pGraphicDev, RESOURCE_STAGE, L"Mesh_Navi", Engine::TYPE_NAVI, nullptr, nullptr));

	LoadingCount(Engine::Ready_ResourceFromPathFile(m_pGraphicDev, SRC_PATH(L"../Bin/Resource/Data/PathInfo.txt")));

	lstrcpy(m_szLoading, L"마리오 집 매수 중");
	LoadingCount(Load_NaviMesh(SRC_PATH(L"../Bin/Resource/Data/NaviInfo_Stage1.txt")));
	LoadingCount(Load_NaviMesh(SRC_PATH(L"../Bin/Resource/Data/NaviInfo_Stage2.txt")));
	lstrcpy(m_szLoading, L"로딩 완료");

	m_bFinish = true;

	return 0;
}

HRESULT CLoading::Loading_Sound()
{
	auto pSoundMgr = Engine::Get_SoundMgr();
	HRESULT hr = 0;

	////////////////////////////////////////////////////////////배경사운드
	
	hr = pSoundMgr->AddSoundFile(
		SRC_PATH(L"../Bin/Resource/Sound/Crystal_Activate.ogg"),
		L"Activate");
	FAILED_CHECK_RETURN(hr, E_FAIL);

	hr = pSoundMgr->AddSoundFile(
		SRC_PATH(L"../Bin/Resource/Sound/BuildUp_Dungeon.ogg"),
		L"BuildUp");
	FAILED_CHECK_RETURN(hr, E_FAIL);

	hr = pSoundMgr->AddSoundFile(
		SRC_PATH(L"../Bin/Resource/Sound/DD_IntroCin2.ogg"),
		L"Intro_Stage1");
	FAILED_CHECK_RETURN(hr, E_FAIL);

	hr = pSoundMgr->AddSoundFile(
		SRC_PATH(L"../Bin/Resource/Sound/Intro_Rooftops.ogg"),
		L"Win");
	FAILED_CHECK_RETURN(hr, E_FAIL);
	
	hr = pSoundMgr->AddSoundFile(
		SRC_PATH(L"../Bin/Resource/Sound/DLCstaff_shotsmall2.ogg"),
		L"Fire_ElectBall");
	FAILED_CHECK_RETURN(hr, E_FAIL);

	hr = pSoundMgr->AddSoundFile(
		SRC_PATH(L"../Bin/Resource/Sound/FireBall.wav"),
		L"Fire_FireBall");
	FAILED_CHECK_RETURN(hr, E_FAIL);

	hr = pSoundMgr->AddSoundFile(
		SRC_PATH(L"../Bin/Resource/Sound/HitBuild.wav"),
		L"HitBuild");
	FAILED_CHECK_RETURN(hr, E_FAIL);

	hr = pSoundMgr->AddSoundFile(
		SRC_PATH(L"../Bin/Resource/Sound/DeadBuild.wav"),
		L"DeadBuild");
	FAILED_CHECK_RETURN(hr, E_FAIL);
	
	hr = pSoundMgr->AddSoundFile(
		SRC_PATH(L"../Bin/Resource/Sound/swing_lv4.wav"),
		L"Melee_Attack");
	FAILED_CHECK_RETURN(hr, E_FAIL);

	hr = pSoundMgr->AddSoundFile(
		SRC_PATH(L"../Bin/Resource/Sound/Huntress_Jump3.ogg"),
		L"Jump_Player");
	FAILED_CHECK_RETURN(hr, E_FAIL);
	
	hr = pSoundMgr->AddSoundFile(
		SRC_PATH(L"../Bin/Resource/Sound/AchievementUnlocked.ogg"),
		L"LvUp_Player");
	FAILED_CHECK_RETURN(hr, E_FAIL);

	hr = pSoundMgr->AddSoundFile(
		SRC_PATH(L"../Bin/Resource/Sound/Huntress_Hurt2.ogg"),
		L"Hurt_Player");
	FAILED_CHECK_RETURN(hr, E_FAIL);


	hr = pSoundMgr->AddSoundFile(
		SRC_PATH(L"../Bin/Resource/Sound/Monster/Goblin/Goblin_attack1.ogg"),
		L"Att1_Goblin");
	FAILED_CHECK_RETURN(hr, E_FAIL);

	hr = pSoundMgr->AddSoundFile(
		SRC_PATH(L"../Bin/Resource/Sound/Monster/Goblin/Goblin_attack2.ogg"),
		L"Att2_Goblin");
	FAILED_CHECK_RETURN(hr, E_FAIL);

	hr = pSoundMgr->AddSoundFile(
		SRC_PATH(L"../Bin/Resource/Sound/Monster/Goblin/Goblin_attack3.ogg"),
		L"Att3_Goblin");
	FAILED_CHECK_RETURN(hr, E_FAIL);

	hr = pSoundMgr->AddSoundFile(
		SRC_PATH(L"../Bin/Resource/Sound/Monster/Goblin/Goblin_attack4.ogg"),
		L"Att4_Goblin");
	FAILED_CHECK_RETURN(hr, E_FAIL);

	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/Monster/Goblin/Goblin_attack5.ogg"), 
		L"Att5_Goblin");

	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/Monster/Goblin/Goblin_death1.ogg"), 
		L"Dead1_Goblin");
	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/Monster/Goblin/Goblin_death2.ogg"),
		L"Dead2_Goblin");
	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/Monster/Goblin/Goblin_death3.ogg"),
		L"Dead3_Goblin");
	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/Monster/Goblin/Goblin_death4.ogg"),
		L"Dead4_Goblin");
	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/Monster/Goblin/Goblin_death5.ogg"),
		L"Dead5_Goblin");
	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/Monster/Goblin/Goblin_death6.ogg"),
		L"Dead6_Goblin");

	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/Monster/Goblin/Goblin_hurt1.ogg"),
		L"Hurt1_Goblin");
	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/Monster/Goblin/Goblin_hurt2.ogg"),
		L"Hurt2_Goblin");
	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/Monster/Goblin/Goblin_hurt3.ogg"),
		L"Hurt3_Goblin");

	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/Monster/Ogre/Ogre_pound.ogg"),
		L"Att1_Ogre");
	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/Monster/Ogre/Ogre_HurtHuge2.ogg"),
		L"Dead1_Ogre");
	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/Monster/Ogre/Ogre_hurtsmall5.ogg"),
		L"Hurt1_Ogre");

	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/Monster/DarkElf/DarkElf_Arrow1.ogg"),
		L"Att1_DarkElf");
	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/Monster/DarkElf/DarkElf_Arrow2.ogg"),
		L"Att2_DarkElf");
	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/Monster/DarkElf/DarkElf_Arrow3.ogg"),
		L"Att3_DarkElf");

	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/Monster/DarkElf/DarkElfArcher_preshot.ogg"),
		L"AttReady1_DarkElf");
	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/Monster/DarkElf/DarkElfArcher_preshot2.ogg"),
		L"AttReady2_DarkElf");
	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/Monster/DarkElf/DarkElfArcher_preshot3.ogg"),
		L"AttReady3_DarkElf");

	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/Monster/DarkElf/DarkElf_Death1.ogg"),
		L"Dead1_DarkElf");
	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/Monster/DarkElf/DarkElf_Death2.ogg"),
		L"Dead2_DarkElf");

	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/Monster/DarkElf/DarkElf_Hurt1.ogg"),
		L"Hurt1_DarkElf");
	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/Monster/DarkElf/DarkElf_Hurt2.ogg"),
		L"Hurt2_DarkElf");
	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/Monster/DarkElf/DarkElf_Hurt3.ogg"),
		L"Hurt3_DarkElf");

	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/MiniGame/BLACKPINK - Forever Young (Harace Kim Remix) (192kbps).mp3"),
		L"Music1_MiniGame");
	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/MiniGame/Carol.mp3"),
		L"Music2_MiniGame");
	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/MiniGame/Chacarron  Out Now!.mp3"),
		L"Music3_MiniGame");
	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/MiniGame/sohot.mp3"),
		L"Music4_MiniGame");

	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/MiniGame/Ok.mp3"),
		L"Ok_MiniGame");

	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/MiniGame/Miss.mp3"),
		L"Miss_MiniGame");


	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/Monster/Demon/Demon_attack.ogg"),
		L"Att1_Boss");
	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/Monster/Demon/Demon_attack2.ogg"),
		L"Att2_Boss");
	
	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/Monster/Demon/Demon_hurtsmall.ogg"),
		L"Hurt1_Boss");
	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/Monster/Demon/Demon_hurtsmall2.ogg"),
		L"Hurt2_Boss");

	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/Monster/Demon/Demon_death.ogg"),
		L"Dead1_Boss");

	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/PutTower/Gate_SlamLarge1.ogg"),
		L"Put1_Tower");

	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/PutTower/Gate_SlamLarge2.ogg"),
		L"Put2_Tower");

	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/Aura/Liea_Hurt1.wav"),
		L"Hurt_Aura");
	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/Aura/Full_Swing.wav"),
		L"Full_Aura");

	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/Bullet/BOSS_Missile_Exp.ogg"),
		L"Bullet_Fireball_Dead");
	pSoundMgr->AddSoundFile(SRC_PATH(
		L"../Bin/Resource/Sound/Bullet/Charged.wav"),
		L"Bullet_Electball_Dead");
	return S_OK;
}

HRESULT CLoading::Load_NaviMesh(std::wstring strPath)
{
	HANDLE hFile = CreateFile(strPath.c_str(), GENERIC_READ, 0, 0,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		ERR_MSG(L"오브젝트 불러오기 실패");
		return E_FAIL;
	}

	DWORD dwBytes = 0;

	while (true)
	{
		Engine::NAVI_SAVEDATA tNaviData = Engine::NAVI_SAVEDATA();

		//데이터 가져오기
		ReadFile(hFile, &tNaviData, sizeof(Engine::NAVI_SAVEDATA), &dwBytes, nullptr);
		if (0 == dwBytes)
			break;
		FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev, RESOURCE_STAGE, tNaviData.szObjectKey, Engine::TYPE_NAVI, nullptr, nullptr));
		
		auto m_pNaviMeshCom = dynamic_cast<Engine::CNaviMesh*>(Engine::Original(RESOURCE_STAGE, tNaviData.szObjectKey));
		
		//Cell Save/////////////////////////////////////////////////////////
		for (int idx_Cell = 0; idx_Cell < tNaviData.iCount_Cell; ++idx_Cell)
		{
			Engine::CELL_SAVEDATA tCellData = Engine::CELL_SAVEDATA();
			ReadFile(hFile, &tCellData, sizeof(Engine::CELL_SAVEDATA), &dwBytes, nullptr);
			Engine::CCell* pCell = nullptr;

			HRESULT hSuccess = m_pNaviMeshCom->Add_Cell(&pCell,
				tCellData.vPoint[Engine::CCell::POINT_A],
				tCellData.vPoint[Engine::CCell::POINT_B],
				tCellData.vPoint[Engine::CCell::POINT_C]);

			//오브젝트 정보 저장
			if (pCell->m_pObjectInfo)
			{
				auto pObjectInfoCom = pCell->m_pObjectInfo;
				pObjectInfoCom->Set_ObjectKey(tCellData.szObjectKey);
				pObjectInfoCom->Set_Option(tCellData.iOption);
			}
		}
		m_pNaviMeshCom->Link_Cell();
	}
	CloseHandle(hFile);

	return S_OK;
}

CLoading* CLoading::Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID eLoadingID)
{
	CLoading*	pInstance = new CLoading(pGraphicDev);

	if (FAILED(pInstance->Ready_Loading(eLoadingID)))
		Safe_Release(pInstance);

	return pInstance;
}

void CLoading::Free(void)
{
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
	DeleteCriticalSection(&m_Crt);

	Safe_Release(m_pGraphicDev);
}

