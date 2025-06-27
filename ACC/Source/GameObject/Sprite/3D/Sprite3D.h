#pragma once
#include "Sprite/SpriteObject.h"

// �x���ɂ��ẴR�[�h���͂𖳌��ɂ���.4005:�Ē�`
#pragma warning(disable:4005)

// �O���錾
class DirectX11;


class Sprite3D
	:public SpriteObject
{
public:
	// �R���X�^���g�o�b�t�@�̃A�v�����̒�`
	//	���V�F�[�_���̃R���X�^���g�o�b�t�@�ƈ�v���Ă���K�v����
	struct SHADER_CONSTANT_BUFFER
	{
		// ���[���h,�r���[,�v���W�F�N�V�����̍����ϊ��s��.	
		D3DXMATRIX	mWVP = D3DXMATRIX(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
		// �J���[�iRGBA�̌^�ɍ��킹��j
		D3DXVECTOR4	vColor = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		// UV���W�ix,y�̂ݎg�p�j
		D3DXVECTOR4	vUV = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
	};

public:
	Sprite3D();
	~Sprite3D();

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
	void Render(D3DXMATRIX& mView, D3DXMATRIX& mProj);

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

	// �r���{�[�h��ON/OFF�؂�ւ�
	void SetBillboard( bool flag ) { m_Billboard = flag; }

private:
	DirectX11*				m_pDx11;
	ID3D11Device*			m_pDevice11;
	ID3D11DeviceContext*	m_pContext11;

	ID3D11VertexShader*		m_pVertexShader;	// ���_�V�F�[�_
	ID3D11InputLayout*		m_pVertexLayout;	// ���_���C�A�E�g
	ID3D11PixelShader*		m_pPixelShader;		// �s�N�Z���V�F�[�_
	ID3D11Buffer*			m_pConstantBuffer;	// �R���X�^���g�o�b�t�@

	ID3D11Buffer*			m_pVertexBuffer;	// ���_�o�b�t�@

	ID3D11ShaderResourceView*	m_pTexture;		// �e�N�X�`��
	ID3D11SamplerState*			m_pSampleLinear;// �T���v��:�e�N�X�`���Ɋe��t�B���^��������

	D3DXVECTOR2		m_UV;		// �e�N�X�`��UV���W

	bool			m_Billboard;	// �r���{�[�hON/OFF
};