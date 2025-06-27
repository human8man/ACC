#pragma once


#include "Scenes/SceneBase.h"
#include "DirectX/DirectX9.h"
#include "DirectX/DirectX11.h"
#include "Collision/GJK/GJK.h"
#include "Sprite/SpriteObject.h"


class StaticMesh;
class GJK;
class Ray;
class MeshLine;
class Enemy;
class Player;
class Character;
class GameUI;
class Random;
class VictoryUI;
class DefeatUI;
class EnemyFrame;


class Game
	: public SceneBase
{
public:
	Game( HWND hWnd );
	~Game();

	void Create()	override;	// �쐬����
	HRESULT LoadData()override;	// �f�[�^�Ǎ�
	void Release()	override;	// �������
	void Init()		override;	// ����������
	void Update()	override;	// �X�V����
	void Draw()		override;	// �`�揈��


private:
	// �G�ƃv���C���[�����_���ŃX�|�[��
	void InitEPPos(Random& random, std::unique_ptr<Player>& player, std::unique_ptr<Enemy>& enemy);

	// �����蔻��֐�
	void CollisionJudge();
	
	// �v���C���[�̏��ƒ��̓����蔻����܂Ƃ߂�֐�
	void PlayertoFloorCol	(CollisionPoints points);
	void PlayertoCylinderCol(CollisionPoints points);
	
	// �G�̏��ƒ��̓����蔻����܂Ƃ߂�֐�
	void EnemytoFloorCol	(CollisionPoints points);
	void EnemytoCylinderCol	(CollisionPoints points);

	// ���C�̓����蔻����܂Ƃ߂�֐�
	void RaytoObjeCol();

	// UI�������܂Ƃ߂�֐�
	void UIUpdate();

	// �I�[�g�G�C�����̃J��������
	void AutoAimProcess();

private:
	HWND	m_hWnd;	 // �E�B���h�E�n���h��
	LIGHT	m_Light; // ���C�g���

	D3DXMATRIX	m_mView; // �r���[(�J����)�s��
	D3DXMATRIX	m_mProj; // �ˉe�i�v���W�F�N�V�����j�s��

	std::unique_ptr<StaticMesh> m_pEgg;					// ���܂�
	std::unique_ptr<StaticMesh> m_pFloor;					// �n��
	std::vector<std::unique_ptr<StaticMesh>> m_pCylinders;	// ���z��
		
	std::unique_ptr<Player>	m_pPlayer;	// �v���C���[�N���X
	std::unique_ptr<Enemy>		m_pEnemy;	// �G�N���X

	// �����蔻��
	std::unique_ptr<GJK>		m_pGJK;			// GJK�N���X
	std::unique_ptr<Ray>		m_pCamRay;		// ���C�N���X
	std::unique_ptr<MeshLine>	m_pMeshLine;	// ���b�V�����N���X

	// UI
	std::unique_ptr<GameUI>	m_pGameUI;	// UI�N���X
	std::unique_ptr<VictoryUI>		m_pWinUI;	// �����N���X
	std::unique_ptr<DefeatUI>	m_pDefeatUI;	// �s�k�N���X

	std::vector<D3DXVECTOR3> m_SpawnPoints;	// �X�|�[���n�_�z��

	int	m_HitKind;		// Hit�̎��
	int	m_CylinderMax;	// ���̍ő吔

	bool m_PlayerAnyLanding;	// �v���C���[�������ɒ��n���Ă���ꍇ
	bool m_EnemyAnyLanding;		// �G�������ɒ��n���Ă���ꍇ

	bool m_SlowMode;			// �X���[���[�V����
	float m_SlowScalingTime;	// �X���[���[�V�����o�ߎ���
	float m_SlowScalingTimeMax;	// �X���[���[�V�����ő厞��
	float m_SlowScale;			// �X���[���[�V�����䗦
};