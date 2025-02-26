#ifndef Engine_Macro_h__
#define Engine_Macro_h__

#define			USING(NAMESPACE)	using namespace NAMESPACE;

#define			MSG_BOX(_message)			MessageBox(NULL, TEXT(_message), L"System Message", MB_OK)
#define			TAGMSG_BOX(_tag,_message)	MessageBoxW(NULL, _message, _tag, MB_OK)

#define			BEGIN(NAMESPACE)		namespace NAMESPACE {
#define			END						}

#ifdef ENGINE_EXPORTS
#define ENGINE_DLL _declspec(dllexport)
#else
#define ENGINE_DLL _declspec(dllimport)
#endif // ENGINE_EXPORTS

#ifdef TOOL
#define ERR_MSG(msg)	\
	AfxMessageBox(msg)
#else
#define ERR_MSG(msg)	\
	MessageBoxW(nullptr, msg, L"Error", MB_OK)
#endif

#define NULL_CHECK( _ptr)	\
	{if( _ptr == 0){__asm { int 3 };return;}}

#define NULL_CHECK_RETURN( _ptr, _return)	\
	{if( _ptr == 0){__asm { int 3 };return _return;}}

#define NULL_CHECK_MSG( _ptr, _message )		\
	{if( _ptr == 0){ERR_MSG(_message);__asm { int 3 };return;}}

#define NULL_CHECK_RETURN_MSG( _ptr, _return, _message )	\
	{if( _ptr == 0){ERR_MSG(_message);__asm { int 3 };return _return;}}


#define FAILED_CHECK(_hr)	if( ((HRESULT)(_hr)) < 0 )	\
	{ __asm { int 3 }; return;}

#define FAILED_CHECK_RETURN(_hr, _return)	if( ((HRESULT)(_hr)) < 0 )		\
	{ __asm { int 3 }; return _return;}

#define FAILED_CHECK_MSG( _hr, _message)	if( ((HRESULT)(_hr)) < 0 )	\
	{ ERR_MSG(_message); __asm { int 3 };return;}

#define FAILED_CHECK_RETURN_MSG( _hr, _return, _message)	if( ((HRESULT)(_hr)) < 0 )	\
	{ ERR_MSG(_message); __asm { int 3 };return _return;}



#define NO_COPY(CLASSNAME)							\
	private:										\
	CLASSNAME(const CLASSNAME&);					\
	CLASSNAME& operator = (const CLASSNAME&);		

#define DECLARE_SINGLETON(CLASSNAME)				\
	NO_COPY(CLASSNAME)								\
	private:										\
	static CLASSNAME*	m_pInstance;				\
	public:											\
	static CLASSNAME*	GetInstance( void );		\
	static void DestroyInstance( void );			

#define IMPLEMENT_SINGLETON(CLASSNAME)				\
	CLASSNAME*	CLASSNAME::m_pInstance = NULL;		\
	CLASSNAME*	CLASSNAME::GetInstance( void )	{	\
		if(NULL == m_pInstance) {					\
			m_pInstance = new CLASSNAME;			\
		}											\
		return m_pInstance;							\
	}												\
	void CLASSNAME::DestroyInstance( void ) {		\
		if(NULL != m_pInstance)	{					\
			delete m_pInstance;						\
			m_pInstance = NULL;						\
		}											\
	}


//경로 동기화
#define SRC_PATH(path)			L"../../Client/Bin/" path

// Value
////화면 크기
#define WINCX					1280
#define WINCY					720
//화면 크기
//#define WINCX					1920
//#define WINCY					1080

//기본 색깔
#define COLOR_RED				D3DCOLOR_ARGB(255, 255, 0, 0)
#define COLOR_GREEN				D3DCOLOR_ARGB(255, 0, 255, 0)
#define COLOR_BLUE				D3DCOLOR_ARGB(255, 0, 0, 255)
#define COLOR_GRAY				D3DCOLOR_ARGB(255, 155, 155, 155)
#define COLOR_WHITE				D3DCOLOR_ARGB(255, 255, 255, 255)
#define COLOR_MAGENTA			D3DCOLOR_ARGB(255, 255, 0, 255)
#define COLOR_BLACK				D3DCOLOR_ARGB(255, 0, 0, 0)

#define COLOR_SCREEN			COLOR_GRAY

//충돌 색깔
#define COLOR_COLLTYPE_FALSE	COLOR_WHITE
#define COLOR_COLLTYPE_TRUE		COLOR_RED
#define COLOR_COLLTYPE_SELECT	COLOR_GREEN

//네비매쉬 지형 색깔
#define COLOR_CELL_NONE			COLOR_BLACK
#define COLOR_CELL_NOCAM		COLOR_BLUE
#define COLOR_CELL_NOUNIT		COLOR_RED
#define COLOR_CELL_NOBUILD		COLOR_GRAY
#define COLOR_CELL_YESFISH		COLOR_MAGENTA
#define COLOR_CELL_SELECT		COLOR_GREEN

//글자 길이
#define MIN_STR					64
#define MID_STR					128
#define MAX_STR					256

//InputDev 키 플레그
#define KEY_UP 					0x00000001
#define KEY_DOWN				0x00000002
#define KEY_LEFT				0x00000004
#define KEY_RIGHT				0x00000008

#define KEY_RETURN				0x00000010
#define KEY_SPACE				0x00000020

#define KEY_LBUTTON				0x00000040
#define KEY_RBUTTON				0x00000080

#define KEY_W 					0x00000100
#define KEY_S					0x00000200
#define KEY_A					0x00000400
#define KEY_D					0x00000800
#define KEY_Q					0x00001000

#define KEY_F1					0x00002000
#define KEY_F2					0x00004000
#define KEY_E					0x00008000
#define KEY_F12					0x00010000

#define KEY_1					0x00020000
#define KEY_2					0x00040000
#define KEY_3					0x00080000
#define KEY_4					0x00100000
#define KEY_0					0x00200000

//CollisionMgr 충돌 플레그
#define	COLLISION_NONE			0x00000000
#define COLLISION_BASE_SRC 		0x00000001
#define COLLISION_BASE_DST		0x00000002
#define COLLISION_PUSH_SRC		0x00000004
#define COLLISION_PUSH_DST		0x00000008

//GameObject 이벤트 플레그
#define EVENT_DEFAULT			0x00000000
#define EVENT_DEAD				0x00000001

//Option_Cell 플레그
#define CELL_NONE				0x00000000
#define CELL_NOUNIT				0x00000001
#define CELL_NOBUILD			0x00000002
#define CELL_NOCAM				0x00000004
#define CELL_YESFISH			0x00000008

//Cell
#define POINT_END_CELL			3

#endif // Engine_Macro_h__
