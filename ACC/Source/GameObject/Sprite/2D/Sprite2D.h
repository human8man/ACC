#pragma once

// �x���ɂ��ẴR�[�h���͂𖳌��ɂ���.4005:�Ē�`
#pragma warning(disable:4005)

// _declspec()	: DLL����i�֐��A�N���X�A�N���X�̃����o�֐��j�G�N�X�|�[�g����
// align()		:�i�����I�Ɂj16byte�Ŏg�p����
#define ALIGN16	_declspec( align(16) )

// �O���錾
class CDirectX11;


class Sprite2D
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
			: Disp		( 0, 0 )
			, Base		( 0, 0 )
			, Stride	( 0, 0 )
			, Pos		( 0.f,0.f,0.f )
			, Scale		( 1.f,1.f,1.f )
			, Path		( "" )
			, Name		( "" )
			, IsDisp	( true )
			, IsGrab	( false )
			, IsLock	( false )
		{}
	};


	// �R���X�^���g�o�b�t�@�̃A�v�����̒�`
	//	���V�F�[�_���̃R���X�^���g�o�b�t�@�ƈ�v���Ă���K�v����
	struct SHADER_CONSTANT_BUFFER
	{
		ALIGN16	D3DXMATRIX	mWorld;		// ���[���h�s��
		ALIGN16	D3DXVECTOR4	vColor;		// �J���[�iRGBA�̌^�ɍ��킹��j
		ALIGN16	D3DXVECTOR4	vUV;		// UV���W�ix,y�̂ݎg�p�j
		ALIGN16 float fViewPortWidth;	// �r���[�|�[�g��
		ALIGN16 float fViewPortHeight;	// �r���[�|�[�g����
	};
	// ���_�̍\����
	struct VERTEX
	{
		D3DXVECTOR3 Pos; // ���_���W
		D3DXVECTOR2	Tex; // �e�N�X�`�����W
	};

public:
	Sprite2D();
	~Sprite2D();

	// ����������
	HRESULT Init( const std::string& lpFileName );

	// �������
	void Release();

	// �V�F�[�_�쐬
	HRESULT CreateShader();
	// ���f���쐬
	HRESULT CreateModel();
	// �e�N�X�`���쐬
	HRESULT CreateTexture( LPCTSTR lpFileName );
	// �T���v���쐬
	HRESULT CreateSampler();

	// �����_�����O�p
	void Render();

	// ���W����ݒ�
	void SetPos(const D3DXVECTOR3& pos) {	m_vPosition = pos;	}
	// ��]����ݒ�
	void SetPivot(const D3DXVECTOR3& pivot){	m_vPivot = pivot;}
	// ��]����ݒ�
	void SetRot(const D3DXVECTOR3& rot){	m_vRotation = rot;	}
	// �g�k����ݒ�
	void SetScale(const D3DXVECTOR3& scale) { m_vScale = scale; }
	// �J���[��ݒ�
	void SetColor(const D3DXVECTOR3& color) { m_vColor = color; }
	// ���l��ݒ�
	void SetAlpha(float alpha) { m_Alpha = alpha; }

	// �p�^�[���ԍ�(�}�X��)��ݒ�
	void SetPatternNo(SHORT x, SHORT y) {
		m_PatternNo.x = x;
		m_PatternNo.y = y;
	}

	// �摜�̌��T�C�Y��ݒ�
	void SetBase(D3DXVECTOR2 base) { 
		m_SpriteState.Base = WHSIZE(base.x, base.y); 
		CreateModel(); 
	}
	// �摜�̕\���͈͂�ݒ�
	void SetDisp(D3DXVECTOR2 disp) { 
		m_SpriteState.Disp = WHSIZE(disp.x, disp.y);
		CreateModel(); 
	}
	// �p�^�[���ԍ���ݒ肵���ۂɏ�Z����镝�A�����̐ݒ�
	void SetStride(D3DXVECTOR2 stride) {
		m_SpriteState.Stride = WHSIZE(stride.x, stride.y); 
		CreateModel(); 
	}
	// �X�v���C�g�̃t�@�C���p�X��ݒ�
	void SetFilePath(const std::string& FilePath) { 
		m_SpriteState.Path = FilePath; 
	}

	// �X�v���C�g�̃t�@�C���p�X��ݒ�
	void SetSpriteName(const std::string& name) {
		m_SpriteState.Name = name;
	}

	// json�t�@�C���̃X�v���C�g�����擾
	HRESULT SpriteStateDataLoad	(const std::string& FilePath);
	// �X�v���C�g�����܂Ƃ߂�json�t�@�C���̍쐬
	HRESULT CreateSpriteState	(const std::string& FilePath);

	// �J���[���擾
	D3DXVECTOR3 GetColor()	const { return m_vColor; }	
	// ���ߓx���擾
	float		GetAlpha()	const { return m_Alpha; }
	// �X�P�[���l���擾
	D3DXVECTOR3 GetScale()	const{ return m_vScale;}
	// ���W�����擾
	D3DXVECTOR3 GetPos()	const{ return m_vPosition;}
	// ����W�����擾
	D3DXVECTOR3 GetPivot()	const{ return m_vPivot;}
	// ��]�����擾
	D3DXVECTOR3 GetRot()	const{ return m_vRotation;}
	// �ő�p�^�[����(�}�X��)���擾
	POINTS GetPatternMax()	const { return m_PatternMax; }
	// �摜�����擾
	SPRITE_STATE GetSpriteData()const { return m_SpriteState; }
private:
	CDirectX11*				m_pDx11;
	ID3D11Device*			m_pDevice11;
	ID3D11DeviceContext*	m_pContext11;

	ID3D11VertexShader*		m_pVertexShader;	// ���_�V�F�[�_
	ID3D11InputLayout*		m_pVertexLayout;	// ���_���C�A�E�g
	ID3D11PixelShader*		m_pPixelShader;		// �s�N�Z���V�F�[�_
	ID3D11Buffer*			m_pConstantBuffer;	// �R���X�^���g�o�b�t�@

	ID3D11Buffer*			m_pVertexBuffer;	// ���_�o�b�t�@

	ID3D11ShaderResourceView*	m_pTexture;		// �e�N�X�`��
	ID3D11SamplerState*			m_pSampleLinear;// �T���v��:�e�N�X�`���Ɋe��t�B���^��������

	D3DXVECTOR3		m_vPosition;	// ���W
	D3DXVECTOR3		m_vPivot;	// ��]�����W
	D3DXVECTOR3		m_vRotation;	// ��]
	D3DXVECTOR3		m_vScale;		// �g�k

	D3DXVECTOR2		m_UV;		// �e�N�X�`��UV���W

	D3DXVECTOR3		m_vColor;	// �J���[
	float			m_Alpha;	// ���l(0:�����A1:���S�s����)

	SPRITE_STATE	m_SpriteState;		// �X�v���C�g���
	POINTS			m_PatternNo;		// �p�^�[���ԍ�(�}�X��)
	POINTS			m_PatternMax;		// �ő�p�^�[��(�}�X�̍ő�l)

};