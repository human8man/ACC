#include "CCharacter.h"
#include "Camera/CCamera.h"


//============================================================================
//		キャラクタークラス.
//============================================================================
CCharacter::CCharacter()
	: m_Shot		( false )
	, m_pRayY		( nullptr )
	, m_pCrossRay	( nullptr )
{
	m_pRayY = new RAY();
	m_pRayY->Axis = D3DXVECTOR3(0.0f, -1.0f, 0.0f);	// 下向きの軸.
	m_pRayY->Length = 10.0f;

	m_pCrossRay = new CROSSRAY();
}

CCharacter::~CCharacter()
{
	SAFE_DELETE( m_pCrossRay );
	SAFE_DELETE( m_pRayY );
}


//============================================================================
//		更新処理.
//============================================================================
void CCharacter::Update()
{
	CStaticMeshObject::Update();
}


//============================================================================
//		描画処理
//============================================================================
void CCharacter::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light )
{
	CStaticMeshObject::Draw( View, Proj, Light );
}
