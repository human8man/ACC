#include "GameObject/Mesh/Skin/CSkinMeshObject.h"
#include "../GameObject/Camera/CCamera.h"

CSkinMeshObject::CSkinMeshObject()
	: m_pMesh		( nullptr )
	, m_pAnimCtrl	( nullptr )
{
}

CSkinMeshObject::~CSkinMeshObject()
{
	DetachMesh();
}

void CSkinMeshObject::Update()
{
	if (m_pMesh == nullptr) {
		return;
	}
}

void CSkinMeshObject::Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light)
{
	if (m_pMesh == nullptr) {
		return;
	}

	//描画直前で座標や回転情報などを更新.
	m_pMesh->SetPosition(m_vPosition);
	m_pMesh->SetRotation(m_vRotation);
	m_pMesh->SetScale(m_vScale);

	//レンダリング.
	m_pMesh->Render( View, Proj, Light,
		CCamera::GetInstance()->GetPos(),
		m_pAnimCtrl );	//クローンを設定.
}

//メッシュを接続する.
void CSkinMeshObject::AttachMesh( CSkinMesh& pMesh )
{
	m_pMesh = &pMesh;

	//アニメーションコントローラを取得.
	LPD3DXANIMATIONCONTROLLER pAC = m_pMesh->GetAnimationController();

	//アニメーションコントローラのクローンを作成.
	if (FAILED(
		pAC->CloneAnimationController(
			pAC->GetMaxNumAnimationOutputs(),
			pAC->GetMaxNumAnimationSets(),
			pAC->GetMaxNumTracks(),
			pAC->GetMaxNumEvents(),
			&m_pAnimCtrl)))		//(out)クローン作成先.
	{
		_ASSERT_EXPR(false, L"アニメーションコントローラのクローン作成失敗");
	}
}

//メッシュを切り離す.
void CSkinMeshObject::DetachMesh()
{
	m_pMesh = nullptr;

	SAFE_RELEASE( m_pAnimCtrl );
}
