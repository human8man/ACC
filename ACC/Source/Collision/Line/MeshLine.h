#pragma once

// �x���ɂ��ẴR�[�h���͂𖳌��ɂ���.4005:�Ē�`
#pragma warning(disable:4005)

// �O���錾
class DirectX11;
class StaticMesh;
class GameObject;

// ���_�̍\����
struct VERTEX
{
	D3DXVECTOR3 Pos;
};

//============================================================================
//		���b�V���̕\���N���X
//============================================================================
class MeshLine
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

public:
	MeshLine();
	~MeshLine();

	// ������
	HRESULT Init();

	// ���
	void Release();

	// �V�F�[�_�쐬
	HRESULT CreateShader();

	// �����_�����O�p
	void Render(D3DXMATRIX& mView, D3DXMATRIX& mProj);


	/****************************************************************
	// @brief ���b�V�����烏�C���[�t���[��
	// @param mesh				���b�V��
	// @param object			�I�u�W�F�N�g
	// @param viewMatrix		�r���[�s��
	// @param projectionMatrix	�v���W�F�N�V�����s��
	// @param color				�F
	// @param scale				�X�P�[��
	****************************************************************/
	void DrawMeshWireframeFromVertices(
		const StaticMesh&	mesh,
		const GameObject&	object,
		const D3DXMATRIX&	viewMatrix,
		const D3DXMATRIX&	projectionMatrix,
		const D3DXVECTOR4&	color,
		const float&		scale);

private:
	DirectX11*				m_pDx11;
	ID3D11Device*			m_pDevice11;
	ID3D11DeviceContext*	m_pContext11;

	ID3D11VertexShader*	m_pVertexShader;	// ���_�V�F�[�_
	ID3D11InputLayout*	m_pVertexLayout;	// ���_���C�A�E�g
	ID3D11PixelShader*	m_pPixelShader;		// �s�N�Z���V�F�[�_
	
	ID3D11Buffer*	m_pConstantBuffer;	// �R���X�^���g�o�b�t�@
	ID3D11Buffer*	m_pVertexBuffer;	// ���_�o�b�t�@
	std::vector<ID3D11Buffer*>	m_pIndexBuffer;	// �C���f�b�N�X�o�b�t�@
	std::vector<DWORD>			m_Indices;		// �C���f�b�N�X��

	std::vector<D3DXVECTOR3> m_Vertices;	// ���b�V�����_���
	D3DXVECTOR3		m_vPosition;	// ���W
	D3DXVECTOR3		m_vRotation;	// ��]
};