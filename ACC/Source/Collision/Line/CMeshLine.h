#pragma once

// �x���ɂ��ẴR�[�h���͂𖳌��ɂ���.4005:�Ē�`.
#pragma warning(disable:4005)

// �O���錾.
class CDirectX11;
class CStaticMesh;

// ���_�̍\����.
struct VERTEX
{
	D3DXVECTOR3 Pos;
};

//============================================================================
//		���b�V���̕\���N���X.
//============================================================================
class CMeshLine
{
public:
	//------------------------------------------------------
	//	�\����.
	//------------------------------------------------------
	// �R���X�^���g�o�b�t�@�̃A�v�����̒�`.
	//	���V�F�[�_���̃R���X�^���g�o�b�t�@�ƈ�v���Ă���K�v����.
	struct SHADER_CONSTANT_BUFFER
	{
		D3DXMATRIX	mWVP;
		D3DXVECTOR4	vColor;
	};

public:
	CMeshLine();
	~CMeshLine();

	// ������.
	HRESULT Init();

	// ���.
	void Release();

	// �V�F�[�_�쐬.
	HRESULT CreateShader();

	// �����_�����O�p.
	void Render(D3DXMATRIX& mView, D3DXMATRIX& mProj);


	void DrawMeshWireframeFromVertices(
		const CStaticMesh& originalVertices,
		const D3DXMATRIX& worldMatrix,
		const D3DXMATRIX& viewMatrix,
		const D3DXMATRIX& projectionMatrix,
		const D3DXVECTOR4& color);

private:
	CDirectX11*				m_pDx11;
	ID3D11Device*			m_pDevice11;
	ID3D11DeviceContext*	m_pContext11;

	ID3D11VertexShader*	m_pVertexShader;	// ���_�V�F�[�_.
	ID3D11InputLayout*	m_pVertexLayout;	// ���_���C�A�E�g.
	ID3D11PixelShader*	m_pPixelShader;		// �s�N�Z���V�F�[�_.
	
	ID3D11Buffer*	m_pConstantBuffer;	// �R���X�^���g�o�b�t�@.
	ID3D11Buffer*	m_pVertexBuffer;	// ���_�o�b�t�@.
	ID3D11Buffer*	m_pIndexBuffer;		// ���_�o�b�t�@.

	std::vector<D3DXVECTOR3> m_Vertices;	// ���b�V�����_���.
	D3DXVECTOR3		m_vPosition;	// ���W.
	D3DXVECTOR3		m_vRotation;	// ��].
};