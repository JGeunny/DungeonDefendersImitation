#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	// ID_DYNAMIC : 실시간 업데이트가 필요한 컴포넌트
	enum COMPONENTID { ID_DYNAMIC, ID_STATIC, ID_END };

	enum WINMODE { MODE_FULL, MODE_WIN };

	enum POSITION	{ POS_X, POS_Y, POS_Z, POS_END };
	enum INFO		{ INFO_RIGHT, INFO_UP, INFO_LOOK, INFO_POS, INFO_END };
	enum ROTATION	{ ROT_X, ROT_Y, ROT_Z, ROT_END };

	enum RENDERID { RENDER_PRIORITY, RENDER_NONALPHA, RENDER_ALPHA, RENDER_UI, RENDER_END };

	enum BUFFERID { BUFFER_RCCOL, BUFFER_RCTEX, BUFFER_TRICOL, BUFFER_TERRAINTEX, BUFFER_CUBETEX, BUFFER_VIEWPORT };
	
	enum TEXTURETYPE { TEX_NORMAL, TEX_CUBE, TEX_END };
	
	enum COLLTYPE { COL_FALSE, COL_TRUE, COL_SELECT, COL_END };
	
	enum MESHTYPE { TYPE_STATIC, TYPE_DYNAMIC, TYPE_NAVI };

	enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB, DIM_END };
	enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };

	enum MESHID { MESH_DEFAULT, MESH_STATIC, MESH_DYNAMIC };

	enum COLLMESHTYPE { COLLMESHTYPE_BOX, COLLMESHTYPE_SPHERE, COLLMESHTYPE_TRIANGLE, COLLMESHTYPE_END };

	enum CAMERATYPE { CAMERATYPE_EYE, CAMERATYPE_AT, CAMERATYPE_END };

	enum CONTROLTYPE { CONTROLTYPE_BASE, CONTROLTYPE_BUILD, CONTROLTYPE_FISHING };

	enum TEAMTYPE { TEAMTYPE_NONE, TEAMTYPE_TEAM1, TEAMTYPE_TEAM2, TEAMTYPE_END };
}

enum RESOURCEID { RESOURCE_STATIC, RESOURCE_LOGO, RESOURCE_STAGE, RESOURCE_END };


#endif // Engine_Enum_h__