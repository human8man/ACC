#pragma once


#include "Scenes/CSceneBase.h"
#include "DirectX/CDirectX9.h"
#include "DirectX/CDirectX11.h"

#include "Sprite/CSpriteObject.h"
#include "Collision/GJK/CGJK.h"


class CStaticMesh;
class CGJK;
class CRay;
class CMeshLine;
class CEnemy;
class CPlayer;
class CCharacter;
class CGameUI;
class CRandom;
class CWinUI;
class CLoseUI;
class CEnemyFrame;

//============================================================================
//		�Q�[���N���X.
//============================================================================
class CGame
	: public CSceneBase
{
public:
	CGame( HWND hWnd );
	~CGame();

	void Create()	override;	// �쐬����.
	HRESULT LoadData()override;	// �f�[�^�Ǎ�.
	void Release()	override;	// �������.
	void Init()		override;	// ����������.
	void Update()	override;	// �X�V����.
	void Draw()		override;	// �`�揈��.


private:
	// �G�ƃv���C���[�����_���ŃX�|�[��.
	void InitEPPos(CRandom& random, std::unique_ptr<CPlayer>& player, std::unique_ptr<CEnemy>& enemy);

	// �����蔻��֐�.
	void CollisionJudge();
	
	// �v���C���[�̏��ƒ��̓����蔻����܂Ƃ߂�֐�.
	void PlayertoFloorCol	(CollisionPoints points);
	void PlayertoCylinderCol(CollisionPoints points);
	
	// �G�̏��ƒ��̓����蔻����܂Ƃ߂�֐�.
	void EnemytoFloorCol	(CollisionPoints points);
	void EnemytoCylinderCol	(CollisionPoints points);

	// ���C�̓����蔻����܂Ƃ߂�֐�.
	void RaytoObjeCol();

	// UI�������܂Ƃ߂�֐�.
	void UIUpdate();

private:
	HWND	m_hWnd;	 // �E�B���h�E�n���h��.
	LIGHT	m_Light; // ���C�g���.

	D3DXMATRIX	m_mView; // �r���[(�J����)�s��.
	D3DXMATRIX	m_mProj; // �ˉe�i�v���W�F�N�V�����j�s��.

	std::unique_ptr<CStaticMesh> m_pEgg;					// ���܂�.
	std::unique_ptr<CStaticMesh> m_pFloor;					// �n��.
	std::vector<std::unique_ptr<CStaticMesh>> m_pCylinders;	// ���z��.
		
	std::unique_ptr<CPlayer>	m_pPlayer;	// �v���C���[�N���X.
	std::unique_ptr<CEnemy>		m_pEnemy;	// �G�N���X.

	// �����蔻��.
	std::unique_ptr<CGJK>		m_pGJK;			// GJK�N���X.
	std::unique_ptr<CRay>		m_pCamRay;		// ���C�N���X.
	std::unique_ptr<CMeshLine>	m_pMeshLine;	// ���b�V�����N���X.

	// UI.
	std::unique_ptr<CGameUI>	m_pGameUI;	// UI�N���X.
	std::unique_ptr<CWinUI>		m_pWinUI;	// �����N���X.
	std::unique_ptr<CLoseUI>	m_pLoseUI;	// �s�k�N���X.

	std::vector<D3DXVECTOR3> m_SpawnPoints;	// �X�|�[���n�_�z��.

	int	m_HitKind;		// Hit�̎��.
	int	m_CylinderMax;	// ���̍ő吔.
};