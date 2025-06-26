#include "Mesh/Skin/CSkinMeshObject.h"
#include "Camera/CCamera.h"


//=============================================================================
//		�X�L�����b�V���I�u�W�F�N�g�N���X
//=============================================================================
CSkinMeshObject::CSkinMeshObject()
	: m_pMesh		( nullptr )
	, m_pAnimCtrl	( nullptr )
{
}

CSkinMeshObject::~CSkinMeshObject()
{
	DetachMesh();
}


//=============================================================================
//		�X�V����
//=============================================================================
void CSkinMeshObject::Update()
{
	if (m_pMesh == nullptr) { return; }
}


//=============================================================================
//		�`�揈��
//=============================================================================
void CSkinMeshObject::Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light)
{
	if (m_pMesh == nullptr) {
		return;
	}

	// �`�撼�O�ō��W���]���Ȃǂ��X�V
	m_pMesh->SetPosition(m_vPosition);
	m_pMesh->SetRotation(m_vRotation);
	m_pMesh->SetScale(m_vScale);

	// �����_�����O
	m_pMesh->Render( View, Proj, Light, CCamera::GetInstance()->GetPos(), m_pAnimCtrl );
}


//=============================================================================
//		���b�V����ڑ�����
//=============================================================================
void CSkinMeshObject::AttachMesh( CSkinMesh& pMesh )
{
	m_pMesh = &pMesh;

	// �A�j���[�V�����R���g���[�����擾
	LPD3DXANIMATIONCONTROLLER pAC = m_pMesh->GetAnimationController();

	// �A�j���[�V�����R���g���[���̃N���[�����쐬
	if (FAILED(
		pAC->CloneAnimationController(
			pAC->GetMaxNumAnimationOutputs(),
			pAC->GetMaxNumAnimationSets(),
			pAC->GetMaxNumTracks(),
			pAC->GetMaxNumEvents(),
			&m_pAnimCtrl)))		// (out)�N���[���쐬��
	{
		_ASSERT_EXPR(false, L"�A�j���[�V�����R���g���[���̃N���[���쐬���s");
	}
}


//=============================================================================
//		���b�V����؂藣��
//=============================================================================
void CSkinMeshObject::DetachMesh()
{
	m_pMesh = nullptr;

	SAFE_RELEASE( m_pAnimCtrl );
}
