#pragma once

// �x���ɂ��ẴR�[�h���͂𖳌��ɂ���.4005:�Ē�`
#pragma warning(disable:4005)

// �O���錾
class DirectX11;


//------------------------------------------------------
//	���C�\����
//------------------------------------------------------
struct RAY
{
	D3DXVECTOR3	Axis;		// ��
	D3DXVECTOR3	Position;	// �ʒu
	float		Length;		// ����
	float		RotationY;	// Y����]�l

	RAY() : Axis(), Position(), Length(), RotationY() {}
};


//------------------------------------------------------
//		���C�����蔻����\����
//------------------------------------------------------
struct RayInfo{
	bool		Hit;	// Hit����
	D3DXVECTOR3	HitPos;	// Hit���W
	FLOAT		Length;	// ���C�J�n�n�_����I�_�܂ł̒���
};


//------------------------------------------------------
//	�\�����C�\����
//------------------------------------------------------
struct CROSSRAY
{
	enum enDir { ZF, ZB, XL, XR, max };
	RAY Ray[enDir::max];

	CROSSRAY() : Ray()
	{
		Ray[enDir::ZF].Axis = D3DXVECTOR3( 0.f, 0.f, 1.f );
		Ray[enDir::ZB].Axis = D3DXVECTOR3( 0.f, 0.f,-1.f );
		Ray[enDir::XL].Axis = D3DXVECTOR3(-1.f, 0.f, 0.f );
		Ray[enDir::XR].Axis = D3DXVECTOR3( 1.f, 0.f, 0.f );

		for( int i = 0; i < enDir::max; i++ ){
			Ray[i].Length = 1.f;
		}
	}
};


//============================================================================
//		���C�i�����j�̕\���N���X
//============================================================================
class Ray
{
public:
	//------------------------------------------------------
	//	�\����
	//------------------------------------------------------
	// �R���X�^���g�o�b�t�@�̃A�v�����̒�`
	//	���V�F�[�_���̃R���X�^���g�o�b�t�@�ƈ�v���Ă���K�v����
	struct SHADER_CONSTANT_BUFFER
	{
		D3DXMATRIX	mWVP;
		D3DXVECTOR4	vColor;
	};
	// ���_�̍\����
	struct VERTEX
	{
		D3DXVECTOR3 Pos;
	};

public:
	Ray();
	~Ray();

	// ������
	HRESULT Init( RAY& pRay);

	// ���
	void Release();

	// �V�F�[�_�쐬
	HRESULT CreateShader();
	// ���f���쐬
	HRESULT CreateModel();

	// �����_�����O�p
	void Render(D3DXMATRIX& mView, D3DXMATRIX& mProj, RAY Ray);

private:
	DirectX11*				m_pDx11;
	ID3D11Device*			m_pDevice11;
	ID3D11DeviceContext*	m_pContext11;

	ID3D11VertexShader*		m_pVertexShader;	// ���_�V�F�[�_
	ID3D11InputLayout*		m_pVertexLayout;	// ���_���C�A�E�g
	ID3D11PixelShader*		m_pPixelShader;		// �s�N�Z���V�F�[�_
	ID3D11Buffer*			m_pConstantBuffer;	// �R���X�^���g�o�b�t�@

	ID3D11Buffer*			m_pVertexBuffer;	// ���_�o�b�t�@

	D3DXVECTOR3		m_vPosition;	// ���W
	D3DXVECTOR3		m_vRotation;	// ��]

	RAY	m_Ray;	// ���C�\����
};