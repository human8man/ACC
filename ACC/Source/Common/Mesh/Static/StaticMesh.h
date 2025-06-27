#pragma once

// �x���ɂ��ẴR�[�h���͂𖳌��ɂ���. 4005:�Ē�`
#pragma warning( disable:4005)

// _declspec()	: DLL����(�֐�,�N���X,�N���X�̃����o�֐���)�G�N�X�|�[�g����
// align()		: (�����I��)16byte �Ŏg�p����
#define ALIGN16	_declspec( align(16) )

#include "Collision/Ray/Ray.h"

// �O���錾
class DirectX9;
class DirectX11;


class StaticMesh
{
public:
	//---------------------------------------------
	//	�\����
	//---------------------------------------------
	struct CBUFFER_PER_MESH
	{
		D3DXMATRIX	mW;		// ���[���h�s��
		D3DXMATRIX	mWVP;	// ���[���h,�r���[,�v���W�F�N�V�����̍����ϊ��s��.	
	};
	// �}�e���A���P�ʂœn�����
	struct CBUFFER_PER_MATERIAL
	{
		D3DXVECTOR4	Diffuse;	// �f�B�t���[�Y�F(�g�U���ːF)
		D3DXVECTOR4 Ambient;	// �A���r�G���g�F(���F)
		D3DXVECTOR4	Specular;	// �X�y�L�����F(���ʔ��ːF)
	};
	// �t���[���P�ʂœn�����
	struct CBUFFER_PER_FRAME
	{
		D3DXVECTOR4	CameraPos;	// �J�����ʒu(���_�ʒu)
		D3DXVECTOR4	vLightDir;	// ���C�g����
	};

	// ���_�̍\����
	struct VERTEX
	{
		D3DXVECTOR3 Pos;	// ���_���W
		D3DXVECTOR3	Normal;	// �@��(�A�e�v�Z�ɕK�{)
		D3DXVECTOR2	UV;		// �e�N�X�`�����W
	};
	// �}�e���A���\����
	struct MY_MATERIAL
	{
		D3DXVECTOR4	Diffuse;	// �f�B�t���[�Y
		D3DXVECTOR4	Ambient;	// �A���r�G���g
		D3DXVECTOR4	Specular;	// �X�y�L����
		D3DXVECTOR4 Emissive;	// �G�~�b�V�u
		float		Power;		// �X�y�L�����p���[
		TCHAR TextureName[64];	// �e�N�X�`���t�@�C����
		ID3D11ShaderResourceView* pTexture;// �e�N�X�`��
		DWORD dwNumFace;		// �}�e���A���̃|���S����

		// �R���X�g���N�^
		MY_MATERIAL()
			: Diffuse		()
			, Ambient		()
			, Specular		()
			, Emissive		()
			, Power			()
			, TextureName	()
			, pTexture		( nullptr )
			, dwNumFace		( 0 )
		{
		}
		// �f�X�g���N�^
		~MY_MATERIAL() {
			SAFE_RELEASE( pTexture );
		}
	};

	// X�t�@�C���̃��[�h�ŕK�v�B�󂯎�����
	struct MY_MODEL
	{
		TCHAR			FileName[64];	 // �t�@�C����
		LPD3DXMESH		pMesh;			 // ���b�V���I�u�W�F�N�g
		LPD3DXBUFFER	pD3DXMtrlBuffer; // �}�e���A���o�b�t�@
		DWORD			NumMaterials;	 // �}�e���A����

		MY_MODEL()
			: FileName			()
			, pMesh				( nullptr )
			, pD3DXMtrlBuffer	( nullptr )
			, NumMaterials		()
		{}
		~MY_MODEL(){
			SAFE_RELEASE( pMesh );
			SAFE_RELEASE( pD3DXMtrlBuffer );
		}
	};

public:
	StaticMesh();
	~StaticMesh();

	// ������
	HRESULT Init( LPCTSTR lpFileName );

	// ����֐�
	void Release();

	// �����_�����O�p
	void Render( D3DXMATRIX& mView, D3DXMATRIX& mProj, LIGHT& Light );

	// ���W���̐ݒ�
	void SetPos( const D3DXVECTOR3& pos )	{ m_Position = pos; }
	void SetPos(float x, float y, float z)	{ m_Position = D3DXVECTOR3(x, y, z); }
	void SetPosX( float x ){ m_Position.x = x; }
	void SetPosY( float y ){ m_Position.y = y; }
	void SetPosZ( float z ){ m_Position.z = z; }

	// ��]����ݒ�
	void SetRot(const D3DXVECTOR3& rot) { m_Rotation = rot; }
	void SetRot(float rot) { m_Rotation = D3DXVECTOR3(rot, rot, rot); }
	void SetRotY( float y ){ m_Rotation.y = y; }
	void SetRotX( float x ){ m_Rotation.x = x; }
	void SetRotZ( float z ){ m_Rotation.z = z; }

	// �g�k����ݒ�
	void SetScale( const D3DXVECTOR3& scale ) {	m_Scale = scale; }
	void SetScale(float x, float y, float z) { m_Scale = D3DXVECTOR3(x, y, z); }

	D3DXVECTOR3 GetScale()	const { return m_Scale; }		// �T�C�Y���擾
	D3DXVECTOR3 GetPos()	const { return m_Position; }	// ���b�V���ʒu���擾
	D3DXVECTOR3 GetRot()	const { return m_Rotation; }	// ���b�V���p�x���擾
	// �I�u�W�F�N�g���(���W���,��]���,�T�C�Y)���擾
	ObjectInfo GetObjeInfo() const { return ObjectInfo{ m_Position, m_Rotation, m_Scale }; }

	LPD3DXMESH GetMesh()	const { return m_Model.pMesh; }	// ���b�V�����擾
	LPD3DXMESH GetMeshForRay()	const { return m_ModelForRay.pMesh; }		// ���C�Ƃ̓����蔻��p�̃��b�V�����擾
	std::vector<D3DXVECTOR3>GetVertices()	const { return m_Vertices; }	// ���b�V���̒��_�����擾
	ID3D11Buffer*		GetVertexBuffer() const { return m_pVertexBuffer; }
	ID3D11InputLayout*	GetVertexLayout() const { return m_pVertexLayout; }

	// �C���f�b�N�X�o�b�t�@���擾
	std::vector<ID3D11Buffer*> GetIndexBuffer() const;
	// �C���f�b�N�X���擾
	std::vector<DWORD> GetIndex() const;

	// ���C�ƃ��b�V���̓����蔻��( Hit����A�����_�A������Ԃ�)
	RayInfo IsHitForRay( const RAY& pRay  );
private:
	HRESULT CreateModel();					// ���f���쐬
	HRESULT CreateShader();					// �V�F�[�_�쐬
	HRESULT CreateSampler();				// �T���v���쐬
	HRESULT CreateMaterials();				// �}�e���A���쐬
	HRESULT CreateIndexBuffer();			// �C���f�b�N�X�o�b�t�@�쐬
	HRESULT CreateVertexBuffer();			// ���_�o�b�t�@�쐬
	HRESULT CreateConstantBuffer();			// �R���X�^���g�o�b�t�@�쐬
	HRESULT LoadXMesh( LPCTSTR lpFileName );// ���b�V���ǂݍ���

	// �����_�����O�֐�(�N���X���ł̂ݎg�p����)
	void RenderMesh( D3DXMATRIX& mWorld, D3DXMATRIX& mView, D3DXMATRIX& mProj );
	// �e���_���̕ۑ�
	void SaveVertices(LPD3DXMESH pMesh);

	// �����ʒu�̃|���S���̒��_��������
	HRESULT FindVerticesOnPoly(
		LPD3DXMESH pMesh,
		DWORD dwPolyIndex,
		D3DXVECTOR3* pVertices);
private:
	DirectX9*				m_pDx9;
	LPDIRECT3DDEVICE9		m_pDevice9;	// Dx9�f�o�C�X�I�u�W�F�N�g

	DirectX11*				m_pDx11;
	ID3D11Device*			m_pDevice11;	// �f�o�C�X�I�u�W�F�N�g
	ID3D11DeviceContext*	m_pContext11;	// �f�o�C�X�R���e�L�X�g

	ID3D11VertexShader*		m_pVertexShader;	// ���_�V�F�[�_
	ID3D11InputLayout*		m_pVertexLayout;	// ���_���C�A�E�g
	ID3D11PixelShader*		m_pPixelShader;		// �s�N�Z���V�F�[�_

	ID3D11Buffer*			m_pCBufferPerMesh;		// �R���X�^���g�o�b�t�@(���b�V����)
	ID3D11Buffer*			m_pCBufferPerMaterial;	// �R���X�^���g�o�b�t�@(�}�e���A����)
	ID3D11Buffer*			m_pCBufferPerFrame;		// �R���X�^���g�o�b�t�@(�t���[����)

	ID3D11Buffer*			m_pVertexBuffer;	// ���_�o�b�t�@
	ID3D11Buffer**			m_ppIndexBuffer;	// �C���f�b�N�X�o�b�t�@
	ID3D11SamplerState*		m_pSampleLinear;	// �T���v��:�e�N�X�`���Ɋe��t�B���^��������

	MY_MODEL		m_Model;
	MY_MODEL		m_ModelForRay;			// ���C�Ƃ̓����蔻��Ŏg�p����
	std::vector<D3DXVECTOR3> m_Vertices;	// ���b�V���R���C�_�[�Ŏg�p

	MY_MATERIAL*	m_pMaterials;	// �}�e���A���\����
	DWORD			m_NumAttr;		// ������
	DWORD			m_AttrID[300];	// ����ID ��300�����܂�

	bool			m_EnableTexture;// �e�N�X�`������

	D3DXVECTOR3		m_Position;	// �ʒu(x,y,z)
	D3DXVECTOR3		m_Rotation;	// ��]�l(x,y,z)x=Pitch, y=Yaw, z=Roll
	D3DXVECTOR3		m_Scale;	// �g��k���l(x,y,z���{)
 };
