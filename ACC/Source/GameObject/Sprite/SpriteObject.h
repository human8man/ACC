#pragma once
#include "GameObject.h"


class SpriteObject
	: public GameObject
{
public:
	//------------------------------------------
	//		�\����
	//------------------------------------------
	//�������\����
	struct WHSIZE
	{
		float w;
		float h;
	};
	// �X�v���C�g�\����
	struct SPRITE_STATE
	{
		WHSIZE Disp;		// �\��������
		WHSIZE Base;		// ���摜������
		WHSIZE Stride;		// 1�R�}������̕�����
		D3DXVECTOR3 Pos;	// �摜���W
		D3DXVECTOR3 Scale;	// �X�P�[��
		std::string Path;	// �p�X
		std::string Name;	// ���O
		bool IsDisp;		// �\�����邩
		bool IsGrab;		// ���܂�Ă��邩(UIEditor�Ŏg�p)
		bool IsLock;		// �Œ肷�邩(UIEditor�Ŏg�p)

		SPRITE_STATE()
			: Disp(0, 0)
			, Base(0, 0)
			, Stride(0, 0)
			, Pos(0.f, 0.f, 0.f)
			, Scale(1.f, 1.f, 1.f)
			, Path("")
			, Name("")
			, IsDisp(true)
			, IsGrab(false)
			, IsLock(false)
		{
		}
	};

	// ���_�̍\����
	struct VERTEX
	{
		D3DXVECTOR3 Pos; // ���_���W
		D3DXVECTOR2	Tex; // �e�N�X�`�����W
	};

public:
	SpriteObject();
	virtual ~SpriteObject() override;

	// �X�V����
	virtual void Update() override;
	// �`�揈��
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj );

	//�X�v���C�g��ڑ�����
	void AttachSprite(SpriteObject& pSprite ){ m_pSprite = &pSprite; }
	// �X�v���C�g��؂藣��
	void DetachSprite(){ m_pSprite = nullptr; }

	// json�t�@�C���̃X�v���C�g�����擾
	HRESULT SpriteStateDataLoad(const std::string& FilePath);
	// �X�v���C�g�����܂Ƃ߂�json�t�@�C���̍쐬
	HRESULT CreateSpriteState(const std::string& FilePath);


	// �p�^�[���ԍ�(�}�X��)��ݒ�
	void SetPatternNo(SHORT x, SHORT y) { m_PatternNo = POINTS(x,y); }
	// �X�v���C�g�̃t�@�C���p�X��ݒ�
	void SetFilePath(const std::string& FilePath) { m_SpriteState.Path = FilePath; }

	// �X�v���C�g�̃t�@�C���p�X��ݒ�
	void SetSpriteName(const std::string& name) { m_SpriteState.Name = name; }

	// �ő�p�^�[����(�}�X��)���擾
	POINTS GetPatternMax()	const { return m_PatternMax; }
	// �摜�����擾
	SPRITE_STATE GetSpriteData()const { return m_SpriteState; }

protected:
	// �q�N���X�𒊏ۃN���X�ɂ��Ȃ����߂ɕK�v
	void Draw(D3DXMATRIX& mView, D3DXMATRIX& mProj, LIGHT& Light ) override final;

protected:
	SpriteObject* m_pSprite;
	SPRITE_STATE m_SpriteState; // �X�v���C�g���

	POINTS m_PatternNo;		// �p�^�[���ԍ�(�}�X��)
	POINTS m_PatternMax;	// �ő�p�^�[��(�}�X�̍ő�l)
};