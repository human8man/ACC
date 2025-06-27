#include "StaticMesh.h"
#include "DirectX/DirectX9.h"
#include "DirectX/DirectX11.h"
#include "Camera/Camera.h"

#include <stdlib.h>
#include <locale.h>

// �V�F�[�_�t�@�C����(�f�B���N�g�����܂�)
const TCHAR SHADER_NAME[] = _T( "Data\\Shader\\StaticMesh.hlsl" );


StaticMesh::StaticMesh()
	: m_pDx9				( nullptr )
	, m_pDevice9			( nullptr )

	, m_pDx11				( nullptr )
	, m_pDevice11			( nullptr )
	, m_pContext11			( nullptr )

	, m_pVertexShader		( nullptr )
	, m_pVertexLayout		( nullptr )
	, m_pPixelShader		( nullptr )

	, m_pCBufferPerMesh		( nullptr )
	, m_pCBufferPerMaterial	( nullptr )
	, m_pCBufferPerFrame	( nullptr )

	, m_pVertexBuffer		( nullptr )
	, m_ppIndexBuffer		( nullptr )
	, m_pSampleLinear		( nullptr )

	, m_Model				()
	, m_ModelForRay			()

	, m_pMaterials			( nullptr )
	, m_NumAttr				()
	, m_AttrID				()
	, m_EnableTexture		( false )
{
}
StaticMesh::~StaticMesh()
{
	// �������
	Release();
}


//=============================================================================
//		�������֐�
//=============================================================================
HRESULT StaticMesh::Init(LPCTSTR lpFileName )
{
	m_pDx9 = DirectX9::GetInstance();
	m_pDevice9 = m_pDx9->GetDevice();

	m_pDx11 = DirectX11::GetInstance();
	m_pDevice11 = m_pDx11->GetDevice();
	m_pContext11 = m_pDx11->GetContext();

	//�t�@�C���ǂݍ���
	if( FAILED( LoadXMesh( lpFileName ) )){ return E_FAIL; }
	//���f���쐬
	if( FAILED( CreateModel() ) ){ return E_FAIL; }
	//�V�F�[�_�쐬
	if( FAILED(CreateShader() )){ return E_FAIL; }
	//�R���X�^���g�o�b�t�@�쐬
	if( FAILED( CreateConstantBuffer() ) ){ return E_FAIL; }
	//�T���v���쐬
	if( FAILED( CreateSampler() ) ){ return E_FAIL; }

	return S_OK;
}


//=============================================================================
//		����֐�
//=============================================================================
void StaticMesh::Release()
{
	SAFE_DELETE_ARRAY(m_ppIndexBuffer);
	SAFE_DELETE_ARRAY(m_pMaterials);

	SAFE_RELEASE( m_Model.pD3DXMtrlBuffer );
	SAFE_RELEASE( m_Model.pMesh );
	SAFE_RELEASE( m_ModelForRay.pD3DXMtrlBuffer );
	SAFE_RELEASE( m_ModelForRay.pMesh );

	SAFE_RELEASE( m_pSampleLinear );
	SAFE_RELEASE( m_pVertexBuffer );
	SAFE_RELEASE( m_pCBufferPerFrame );
	SAFE_RELEASE( m_pCBufferPerMaterial );
	SAFE_RELEASE( m_pCBufferPerMesh );
	SAFE_RELEASE( m_pPixelShader );
	SAFE_RELEASE( m_pVertexLayout );
	SAFE_RELEASE( m_pVertexShader );

	m_pContext11 = nullptr;
	m_pDevice11 = nullptr;
	m_pDx11 = nullptr;

	m_pDevice9 = nullptr;
	m_pDx9 = nullptr;
}

void StaticMesh::Update()
{
}


//=============================================================================
//		�����_�����O�p
//=============================================================================
void StaticMesh::Draw(D3DXMATRIX& mView, D3DXMATRIX& mProj, LIGHT& Light)
{
	// ���[���h�s��A�X�P�[���s��A��]�s��A���s�ړ��s��
	D3DXMATRIX mWorld, mScale, mRot, mTran;
	D3DXMATRIX mYaw, mPitch, mRoll;

	//�g��k���s��쐬
	D3DXMatrixScaling(&mScale, m_vScale.x, m_vScale.y, m_vScale.z);

	D3DXMatrixRotationY(&mYaw, m_vRotation.y);	// Y����]�s��쐬
	D3DXMatrixRotationX(&mPitch, m_vRotation.x);// X����]�s��쐬
	D3DXMatrixRotationZ(&mRoll, m_vRotation.z);	// Z����]�s��쐬

	// ���s�ړ��s��쐬
	D3DXMatrixTranslation(&mTran, m_vPosition.x, m_vPosition.y, m_vPosition.z);

	// ��]�s����쐬
	mRot = mYaw * mPitch * mRoll;

	// ���[���h�s��쐬�i�K���g�k�~��]�~�ړ��̏��ɂ���j
	mWorld = mScale * mRot * mTran;

	// �g�p����V�F�[�_�̃Z�b�g
	m_pContext11->VSSetShader(m_pVertexShader, nullptr, 0);	// ���_�V�F�[�_
	m_pContext11->PSSetShader(m_pPixelShader, nullptr, 0);	// �s�N�Z���V�F�[�_

	// �V�F�[�_�̃R���X�^���g�o�b�t�@�Ɋe��f�[�^��n��
	D3D11_MAPPED_SUBRESOURCE pData;
	// �o�b�t�@���̃f�[�^�̏��������J�n����Map
	if (SUCCEEDED(m_pContext11->Map(
		m_pCBufferPerFrame, 0,
		D3D11_MAP_WRITE_DISCARD,
		0, &pData)))
	{
		// �R���X�^���g�o�b�t�@(�t���[���p)
		CBUFFER_PER_FRAME cb;

		D3DXVECTOR3 CamPos = Camera::GetInstance()->GetPos();

		// �J�����ʒu
		cb.CameraPos = D3DXVECTOR4(CamPos.x, CamPos.y, CamPos.z, 0.f);

		// ���C�g����
		cb.vLightDir = D3DXVECTOR4(Light.vDirection.x, Light.vDirection.y, Light.vDirection.z, 0.f);

		// ���C�g�����̐��K��(�m�[�}���C�Y�j
		//	�����f�����烉�C�g�֌���������. �f�B���N�V���i�����C�g�ŏd�v�ȗv�f
		D3DXVec4Normalize(&cb.vLightDir, &cb.vLightDir);

		memcpy_s(
			pData.pData,	// �R�s�[��̃o�b�t�@
			pData.RowPitch,	// �R�s�[��̃o�b�t�@�T�C�Y
			(void*)(&cb),	// �R�s�[���̃o�b�t�@
			sizeof(cb));	// �R�s�[���̃o�b�t�@�T�C�Y

		// �o�b�t�@���̃f�[�^�̏��������I������Unmap
		m_pContext11->Unmap(m_pCBufferPerFrame, 0);
	}

	// ���̃R���X�^���g�o�b�t�@���ǂ̃V�F�[�_�Ŏg�p���邩�H
	m_pContext11->VSSetConstantBuffers(2, 1, &m_pCBufferPerFrame);	//���_�V�F�[�_
	m_pContext11->PSSetConstantBuffers(2, 1, &m_pCBufferPerFrame);	//�s�N�Z���V�F�[�_


	// ���b�V���̃����_�����O
	RenderMesh(mWorld, mView, mProj);
}


//============================================================================
//		�C���f�b�N�X�o�b�t�@���擾
//============================================================================
std::vector<ID3D11Buffer*> StaticMesh::GetIndexBuffer() const
{
	std::vector<ID3D11Buffer*> indexBuffers;
	for (DWORD No = 0; No < m_NumAttr; No++)
	{
		indexBuffers.emplace_back(m_ppIndexBuffer[No]);
	}
	return indexBuffers;
}


//============================================================================
//		�C���f�b�N�X���擾
//============================================================================
std::vector<DWORD> StaticMesh::GetIndex() const
{
	std::vector<DWORD> index;

	// �C���f�b�N�X��std::vector<DWORD>�ɒǉ�
	for (DWORD No = 0; No < m_NumAttr; No++)
	{
		index.emplace_back(m_pMaterials[m_AttrID[No]].dwNumFace);
	}

	return index;
}


//============================================================================
//		���C�ƃ��b�V���̓����蔻��
//============================================================================
RayInfo StaticMesh::IsHitForRay(const RAY& pRay )
{
	FLOAT vDistance;
	D3DXVECTOR3 vAxis, StartPoint, EndPoint, vIntersect;
	D3DXMATRIX mTran, mRot, mScale, mWorld, mInverseWorld, mYaw, mPitch, mRoll;
	RayInfo info;

	// ���C�̕����x�N�g���ƈʒu��ݒ�
	vAxis		= pRay.Axis;	 // ���C�̕����i���x�N�g���j
	StartPoint	= pRay.Position; // ���C�̊J�n�ʒu
	EndPoint = StartPoint + (vAxis * pRay.Length); // ���C�̏I�_���v�Z

	// �ړ�����
	D3DXMatrixTranslation( &mTran, m_vPosition.x, m_vPosition.y, m_vPosition.z);

	// ��]����
	D3DXMatrixRotationY(&mYaw,	 m_vRotation.y);	// Y����]�s��쐬
	D3DXMatrixRotationX(&mPitch, m_vRotation.x);	// X����]�s��쐬
	D3DXMatrixRotationZ(&mRoll,	 m_vRotation.z);	// Z����]�s��쐬
	// ��]�s��쐬
	mRot = mYaw * mPitch * mRoll;

	// �g�k����
	D3DXMatrixScaling(&mScale, m_vScale.x, m_vScale.y, m_vScale.z);

	// ���[���h�s��v�Z
	mWorld = mScale * mTran;

	// �t�s������߂�
	D3DXMatrixInverse( &mInverseWorld, nullptr, &mWorld );
	// ���C�̎n�_�A�I�_�ɔ��f
	D3DXVec3TransformCoord( &StartPoint, &StartPoint, &mInverseWorld );
	D3DXVec3TransformCoord( &EndPoint, &EndPoint, &mInverseWorld );

	// �����ƒ����i�傫���j�����߂�
	D3DXVECTOR3 vDirection = EndPoint - StartPoint;

	BOOL bHit = FALSE;		// �����t���O
	DWORD dwIndex = 0;		// �C���f�b�N�X�ԍ�
	D3DXVECTOR3 Vertex[3];	// ���_���W
	FLOAT U = 0, V = 0;		// �d�S�q�b�g���W

	// ���b�V���ƃ��C�̌����𒲂ׂ�
	D3DXIntersect(
		m_ModelForRay.pMesh,	// �Ώۃ��b�V��
		&StartPoint,			// ���C�̎n�_
		&vDirection,			// ���C�̌����ƒ����i�傫���j
		&bHit,					// (out)���茋��
		&dwIndex,				// (out)bHit��TRUE���Ƀ��C�̎n�_�ɍł��߂��̖ʂ̃C���f�b�N�X�l�ւ̃|�C���^
		&U, &V,					// (out)�d�S�q�b�g���W
		&vDistance,				// (out)���b�V���Ƃ̋���
		nullptr, nullptr);

	// �����ɐL�т郌�C�̂ǂ����Ń��b�V�����������Ă�����
	if (bHit)
	{
		// ����������
		FindVerticesOnPoly( m_ModelForRay.pMesh, dwIndex, Vertex );

		// �d�S���W�����_���Z�o
		//	���[�J����_�� v0 + U*(v1-v0) + V*(v2-v0) �ŋ��܂�
		vIntersect = Vertex[0] + U * (Vertex[1] - Vertex[0]) + V * (Vertex[2] - Vertex[0]);

		// ���f���f�[�^���u�g�k�v�u��]�v�u�ړ��v���Ă���΍s�񂪕K�v
		D3DXVec3TransformCoord( &vIntersect, &vIntersect, &mWorld );

		// Hit��Ԃƌ����_��Ԃ�
		return { true, vIntersect ,vDistance }; 
	}

	return { false, ZEROVEC3, 9999.f };
}


//-----------------------------------------------------------------------------
//		���f���쐬
//-----------------------------------------------------------------------------
HRESULT StaticMesh::CreateModel()
{
	//�}�e���A���쐬
	if( FAILED( CreateMaterials() ) ) { return E_FAIL; }
	//�C���f�b�N�X�o�b�t�@�쐬
	if( FAILED( CreateIndexBuffer() ) ) { return E_FAIL; }
	//���_�o�b�t�@�쐬
	if( FAILED( CreateVertexBuffer() ) ) { return E_FAIL; }

	return S_OK;
}


//-----------------------------------------------------------------------------
//		HLSL�t�@�C����ǂݍ��݃V�F�[�_���쐬����
//-----------------------------------------------------------------------------
HRESULT StaticMesh::CreateShader()
{
	ID3DBlob* pCompiledShader = nullptr;
	ID3DBlob* pErrors = nullptr;
	UINT uCompileFlag = 0;
if(ISDEBUG){
	uCompileFlag =
		D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
}

	// HLSL����o�[�e�b�N�X�V�F�[�_�̃u���u���쐬
	if( m_EnableTexture == true ){
		if (FAILED(
			D3DX11CompileFromFile(
				SHADER_NAME, nullptr, nullptr, "VS_Main", "vs_5_0",
				uCompileFlag, 0, nullptr, &pCompiledShader, &pErrors, nullptr )))
		{
			_ASSERT_EXPR( false, _T( "hlsl�ǂݍ��ݎ��s" ) );
			return E_FAIL;
		}
	}
	else {
		if( FAILED(
			D3DX11CompileFromFile(
				SHADER_NAME, nullptr, nullptr, "VS_NoTex", "vs_5_0",
				uCompileFlag, 0, nullptr, &pCompiledShader, &pErrors, nullptr )))
		{
			_ASSERT_EXPR( false, _T( "hlsl�ǂݍ��ݎ��s" ) );
			return E_FAIL;
		}
	}
	SAFE_RELEASE( pErrors );

	// ��L�ō쐬�����u���u����u�o�[�e�b�N�X�V�F�[�_�v���쐬
	if( FAILED(
		m_pDevice11->CreateVertexShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			nullptr,
			&m_pVertexShader )))	//(out)�o�[�e�b�N�X�V�F�[�_
	{
		_ASSERT_EXPR( false, _T( "�o�[�e�b�N�X�V�F�[�_�쐬���s" ) );
		return E_FAIL;
	}

	// ���_�C���v�b�g���C�A�E�g���`
	D3D11_INPUT_ELEMENT_DESC layout[3];
	// ���_�C���v�b�g���C�A�E�g�̔z��v�f�����Z�o
	UINT numElements = 0;
	if( m_EnableTexture == true )
	{
		D3D11_INPUT_ELEMENT_DESC tmp[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		numElements = sizeof( tmp ) / sizeof( tmp[0] );	// �v�f���Z�o
		memcpy_s( layout, sizeof( layout ),
			tmp, sizeof( D3D11_INPUT_ELEMENT_DESC ) * numElements );
	}
	else
	{
		D3D11_INPUT_ELEMENT_DESC tmp[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		numElements = sizeof( tmp ) / sizeof( tmp[0] );	// �v�f���Z�o
		memcpy_s( layout, sizeof( layout ),
			tmp, sizeof( D3D11_INPUT_ELEMENT_DESC ) * numElements );
	}

	// ���_�C���v�b�g���C�A�E�g���쐬
	if (FAILED(
		m_pDevice11->CreateInputLayout(
			layout,
			numElements,
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			&m_pVertexLayout)))	// (out)���_�C���v�b�g���C�A�E�g
	{
		_ASSERT_EXPR(false, _T( "���_�C���v�b�g���C�A�E�g�쐬���s" ));
		return E_FAIL;
	}
	SAFE_RELEASE(pCompiledShader);

	// HLSL����s�N�Z���V�F�[�_�̃u���u���쐬
	if( m_EnableTexture == true ){
		if( FAILED(
			D3DX11CompileFromFile(
				SHADER_NAME, nullptr, nullptr, "PS_Main", "ps_5_0",
				uCompileFlag, 0, nullptr, &pCompiledShader, &pErrors, nullptr )))
		{
			_ASSERT_EXPR(false, _T( "hlsl�ǂݍ��ݎ��s" ));
			return E_FAIL;
		}
	}
	else {
		if( FAILED(
			D3DX11CompileFromFile(
				SHADER_NAME, nullptr, nullptr, "PS_NoTex", "ps_5_0",
				uCompileFlag, 0, nullptr, &pCompiledShader, &pErrors, nullptr )))
		{
			_ASSERT_EXPR(false, _T( "hlsl�ǂݍ��ݎ��s" ));
			return E_FAIL;
		}
	}
	SAFE_RELEASE( pErrors );

	// ��L�ō쐬�����u���u����u�s�N�Z���V�F�[�_�v���쐬
	if( FAILED(
		m_pDevice11->CreatePixelShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			nullptr,
			&m_pPixelShader )))	//(out)�s�N�Z���V�F�[�_
	{
		_ASSERT_EXPR( false, _T( "�s�N�Z���V�F�[�_�쐬���s" ) );
		return E_FAIL;
	}
	SAFE_RELEASE( pCompiledShader );

	return S_OK;
}


//-----------------------------------------------------------------------------
//		�T���v���쐬
//-----------------------------------------------------------------------------
HRESULT StaticMesh::CreateSampler()
{
	// �e�N�X�`���p�̃T���v���\����
	D3D11_SAMPLER_DESC samDesc;
	ZeroMemory( &samDesc, sizeof( samDesc ) );
	samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;// ���j�A�t�B���^(���`���)
						// POINT:���������e��
	samDesc.AddressU
		= D3D11_TEXTURE_ADDRESS_WRAP;// ���b�s���O���[�h(WRAP:�J��Ԃ�)
	samDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	// MIRROR: ���]�J��Ԃ�
	// CLAMP : �[�̖͗l�������L�΂�
	// BORDER: �ʓr���E�F�����߂�

	// �T���v���쐬
	if( FAILED( m_pDevice11->CreateSamplerState( &samDesc, &m_pSampleLinear ) ) )
	{
		_ASSERT_EXPR( false, _T( "�T���v���쐬���s" ) );
		return E_FAIL;
	}

	return S_OK;
}


//-----------------------------------------------------------------------------
//		�}�e���A���쐬
//-----------------------------------------------------------------------------
HRESULT StaticMesh::CreateMaterials()
{
	// �t�@�C���̃p�X(�f�B���N�g��)���m��
	const int PATH_MAX = 64;
	TCHAR path[PATH_MAX] = _T( "" );
	int path_count = lstrlen( m_Model.FileName );
	for( int k = path_count; k >= 0; k-- ) {
		if( m_Model.FileName[k] == '\\' ) {
			for( int j = 0; j <= k; j++ ) {
				path[j] = m_Model.FileName[j];
			}
			path[k + 1] = '\0';
			break;
		}
	}

	// �ǂݍ��񂾏�񂩂�K�v�ȏ��𔲂��o��
	D3DXMATERIAL* d3dxMaterials = static_cast<D3DXMATERIAL*>( m_Model.pD3DXMtrlBuffer->GetBufferPointer() );
	// �}�e���A�������̗̈���m��
	m_pMaterials = new MY_MATERIAL[m_Model.NumMaterials]();
	// �}�e���A�������J��Ԃ�
	for( DWORD No = 0; No < m_Model.NumMaterials; No++ )
	{
		// �}�e���A�����̃R�s�[
		// �f�B�t���[�Y
		m_pMaterials[No].Diffuse.x = d3dxMaterials[No].MatD3D.Diffuse.r;
		m_pMaterials[No].Diffuse.y = d3dxMaterials[No].MatD3D.Diffuse.g;
		m_pMaterials[No].Diffuse.z = d3dxMaterials[No].MatD3D.Diffuse.b;
		m_pMaterials[No].Diffuse.w = d3dxMaterials[No].MatD3D.Diffuse.a;
		// �A���r�G���g
		m_pMaterials[No].Ambient.x = d3dxMaterials[No].MatD3D.Ambient.r;
		m_pMaterials[No].Ambient.y = d3dxMaterials[No].MatD3D.Ambient.g;
		m_pMaterials[No].Ambient.z = d3dxMaterials[No].MatD3D.Ambient.b;
		m_pMaterials[No].Ambient.w = d3dxMaterials[No].MatD3D.Ambient.a;		
		// �X�y�L����
		m_pMaterials[No].Specular.x = d3dxMaterials[No].MatD3D.Specular.r;
		m_pMaterials[No].Specular.y = d3dxMaterials[No].MatD3D.Specular.g;
		m_pMaterials[No].Specular.z = d3dxMaterials[No].MatD3D.Specular.b;
		m_pMaterials[No].Specular.w = d3dxMaterials[No].MatD3D.Specular.a;
		// �X�y�L�����p���[
		m_pMaterials[No].Power = d3dxMaterials[No].MatD3D.Power;
		// �G�~�b�V�u
		m_pMaterials[No].Emissive.x = d3dxMaterials[No].MatD3D.Emissive.r;
		m_pMaterials[No].Emissive.y = d3dxMaterials[No].MatD3D.Emissive.g;
		m_pMaterials[No].Emissive.z = d3dxMaterials[No].MatD3D.Emissive.b;
		m_pMaterials[No].Emissive.w = d3dxMaterials[No].MatD3D.Emissive.a;

		// (���̖ʂ�)�e�N�X�`�����\���Ă��邩�H
		if( d3dxMaterials[No].pTextureFilename != nullptr
			&& strlen( d3dxMaterials[No].pTextureFilename ) > 0 )
		{
#ifdef UNICODE
			WCHAR TexFilename_w[32] = L"";
			// �e�N�X�`�����̃T�C�Y���擾
			size_t charSize = strlen( d3dxMaterials[No].pTextureFilename ) + 1;
			size_t ret;	// �ϊ����ꂽ������

			// �}���`�o�C�g�����̃V�[�P���X��Ή����郏�C�h�����̃V�[�P���X�ɕϊ����܂�
			errno_t err = mbstowcs_s(
				&ret,
				TexFilename_w,
				charSize,
				d3dxMaterials[No].pTextureFilename,
				_TRUNCATE );

			LPTSTR filename = TexFilename_w;
#else// #ifdef UNICODE
			LPTSTR filename = d3dxMaterials[No].pTextureFilename;
#endif// #ifdef UNICODE

			// �e�N�X�`������̃t���O�𗧂Ă�
			m_EnableTexture = true;

			// �p�X���R�s�[
			lstrcpy( m_pMaterials[No].TextureName, path );
			// �e�N�X�`���t�@�C������A��
			lstrcat( m_pMaterials[No].TextureName, filename );

			// �e�N�X�`���쐬
			if( FAILED( D3DX11CreateShaderResourceViewFromFile(
				m_pDevice11, m_pMaterials[No].TextureName,// �e�N�X�`���t�@�C����
				nullptr, nullptr,
				&m_pMaterials[No].pTexture, // (out)�e�N�X�`���I�u�W�F�N�g
				nullptr ) ) )
			{
				_ASSERT_EXPR( false, _T( "�e�N�X�`���쐬���s" ) );
				return E_FAIL;
			}
		}
	}

	return S_OK;
}


//-----------------------------------------------------------------------------
//		�C���f�b�N�X�o�b�t�@�쐬
//-----------------------------------------------------------------------------
HRESULT StaticMesh::CreateIndexBuffer()
{
	D3D11_BUFFER_DESC	bd = {}; // Dx11�o�b�t�@�\����
	D3D11_SUBRESOURCE_DATA	InitData = {}; // �������f�[�^

	// �}�e���A�������̗̈���m��
	m_ppIndexBuffer = new ID3D11Buffer*[m_Model.NumMaterials]();
	for( DWORD No = 0; No < m_Model.NumMaterials; No++ ){
		m_ppIndexBuffer[No] = nullptr;
	}

	// ���b�V���̑������𓾂�
	// �������ŃC���f�b�N�X�o�b�t�@����ׂ����}�e���A�����Ƃ̃C���f�b�N�X�o�b�t�@�𕪗��ł���
	D3DXATTRIBUTERANGE* pAttrTable = nullptr;

	// ���b�V���̖ʂ���ђ��_�̏��ԕύX�𐧌䂵�A�p�t�H�[�}���X���œK������
	// D3DXMESHOPT_COMPACT : �ʂ̏��Ԃ�ύX���A�g�p����Ă��Ȃ����_�Ɩʂ��폜����
	// D3DXMESHOPT_ATTRSORT : �p�t�H�[�}���X���グ��ׁA�ʂ̏��Ԃ�ύX���čœK�����s��
	m_Model.pMesh->OptimizeInplace( D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT,
		nullptr, nullptr, nullptr, nullptr );
	// �����e�[�u���̎擾
	m_Model.pMesh->GetAttributeTable( nullptr, &m_NumAttr );
	pAttrTable = new D3DXATTRIBUTERANGE[m_NumAttr];
	if( FAILED( m_Model.pMesh->GetAttributeTable( pAttrTable, &m_NumAttr ) ) )
	{
		_ASSERT_EXPR( false, _T( "�����e�[�u���擾���s" ) );
		return E_FAIL;
	}

	// ������Lock���Ȃ��Ǝ��o���Ȃ�
	int* pIndex = nullptr;
	m_Model.pMesh->LockIndexBuffer( D3DLOCK_READONLY, (void**)&pIndex );
	// �������Ƃ̃C���f�b�N�X�o�b�t�@���쐬
	for( DWORD No = 0; No < m_NumAttr; No++ )
	{
		m_AttrID[No] = pAttrTable[No].AttribId;
		// Dx9�̃C���f�b�N�X�o�b�t�@����̏��ŁADx11�̃C���f�b�N�X�o�b�t�@���쐬
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof( int )*pAttrTable[No].FaceCount * 3;// �ʐ��~3�Œ��_��
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		// �傫���C���f�b�N�X�o�b�t�@���̃I�t�Z�b�g(�~3����)
		InitData.pSysMem = &pIndex[pAttrTable[No].FaceStart * 3];

		if( FAILED( m_pDevice11->CreateBuffer( &bd, &InitData, &m_ppIndexBuffer[No] ) ) )
		{
			_ASSERT_EXPR( false, _T( "�C���f�b�N�X�o�b�t�@�쐬���s" ) );
			return E_FAIL;
		}
		// �ʂ̐����R�s�[
		m_pMaterials[m_AttrID[No]].dwNumFace = pAttrTable[No].FaceCount;
	}
	// �����e�[�u���̍폜
	delete[] pAttrTable;
	// �g�p�ς݂̃C���f�b�N�X�o�b�t�@�̉��
	m_Model.pMesh->UnlockIndexBuffer();

	return S_OK;
}


//-----------------------------------------------------------------------------
//		���_�o�b�t�@�쐬
//-----------------------------------------------------------------------------
HRESULT StaticMesh::CreateVertexBuffer()
{
	D3D11_BUFFER_DESC	bd = {}; // Dx11�o�b�t�@�\����
	D3D11_SUBRESOURCE_DATA	InitData = {}; // �������f�[�^

	// Dx9�̏ꍇ�Amap�ł͂Ȃ�Lock�Œ��_�o�b�t�@����f�[�^�����o��
	LPDIRECT3DVERTEXBUFFER9 pVB = nullptr;
	m_Model.pMesh->GetVertexBuffer( &pVB );
	DWORD dwStride = m_Model.pMesh->GetNumBytesPerVertex();
	BYTE* pVertices = nullptr;
	VERTEX* pVertex = nullptr;
	if( SUCCEEDED( pVB->Lock( 0, 0, (VOID**)&pVertices, 0 ) ) )
	{
		pVertex = (VERTEX*)pVertices;
		// Dx9�̒��_�o�b�t�@����̏��ŁADx11���_�o�b�t�@���쐬
		bd.Usage = D3D11_USAGE_DEFAULT;
		// ���_���i�[����̂ɕK�v�ȃo�C�g��
		bd.ByteWidth = m_Model.pMesh->GetNumBytesPerVertex()*m_Model.pMesh->GetNumVertices();
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		InitData.pSysMem = pVertex;
		if( FAILED( m_pDevice11->CreateBuffer( &bd, &InitData, &m_pVertexBuffer ) ) )
		{
			_ASSERT_EXPR( false, _T( "���_�o�b�t�@�쐬���s" ) );
			return E_FAIL;
		}
		pVB->Unlock();
	}
	SAFE_RELEASE( pVB );	// ���_�o�b�t�@���
	return S_OK;
}


//-----------------------------------------------------------------------------
//		�R���X�^���g�o�b�t�@�쐬
//-----------------------------------------------------------------------------
HRESULT StaticMesh::CreateConstantBuffer()
{
	// �R���X�^���g�o�b�t�@(���b�V���p)
	D3D11_BUFFER_DESC cb;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// �R���X�^���g�o�b�t�@���w��
	cb.ByteWidth = sizeof( CBUFFER_PER_MESH );	// �R���X�^���g�o�b�t�@�̃T�C�Y
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// �������݂ŃA�N�Z�X
	cb.MiscFlags = 0;							// ���̑��̃t���O(���g�p)
	cb.StructureByteStride = 0;					// �\���̂̃T�C�Y(���g�p)
	cb.Usage = D3D11_USAGE_DYNAMIC;				// �g�p���@:���ڏ�������

	// �R���X�^���g�o�b�t�@�̍쐬
	if( FAILED( m_pDevice11->CreateBuffer( &cb, nullptr, &m_pCBufferPerMesh ) ) )
	{
		_ASSERT_EXPR( false, _T( "�R���X�^���g�o�b�t�@(���b�V��)�쐬���s" ) );
		return E_FAIL;
	}

	// �R���X�^���g�o�b�t�@(�}�e���A���p)
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;		// �R���X�^���g�o�b�t�@���w��
	cb.ByteWidth = sizeof( CBUFFER_PER_MATERIAL );	// �R���X�^���g�o�b�t�@�̃T�C�Y
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;		// �������݂ŃA�N�Z�X
	cb.MiscFlags = 0;								// ���̑��̃t���O(���g�p)
	cb.StructureByteStride = 0;						// �\���̂̃T�C�Y(���g�p)
	cb.Usage = D3D11_USAGE_DYNAMIC;					// �g�p���@:���ڏ�������

	// �R���X�^���g�o�b�t�@�̍쐬
	if( FAILED( m_pDevice11->CreateBuffer( &cb, nullptr, &m_pCBufferPerMaterial ) ) )
	{
		_ASSERT_EXPR( false, _T( "�R���X�^���g�o�b�t�@(�}�e���A���p)�쐬���s" ) );
		return E_FAIL;
	}

	// �R���X�^���g�o�b�t�@(�t���[���p)
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// �R���X�^���g�o�b�t�@���w��
	cb.ByteWidth = sizeof( CBUFFER_PER_FRAME );	// �R���X�^���g�o�b�t�@�̃T�C�Y
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// �������݂ŃA�N�Z�X
	cb.MiscFlags = 0;							// ���̑��̃t���O(���g�p)
	cb.StructureByteStride = 0;					// �\���̂̃T�C�Y(���g�p)
	cb.Usage = D3D11_USAGE_DYNAMIC;				// �g�p���@:���ڏ�������

	// �R���X�^���g�o�b�t�@�̍쐬
	if( FAILED( m_pDevice11->CreateBuffer( &cb, nullptr, &m_pCBufferPerFrame ) ) )
	{
		_ASSERT_EXPR( false, _T( "�R���X�^���g�o�b�t�@(�t���[���p)�쐬���s" ) );
		return E_FAIL;
	}

	return S_OK;
}


//-----------------------------------------------------------------------------
//		���b�V���ǂݍ���
//-----------------------------------------------------------------------------
HRESULT StaticMesh::LoadXMesh( LPCTSTR lpFileName )
{
	//�t�@�C�������R�s�[
	lstrcpy( m_Model.FileName, lpFileName );
	lstrcpy( m_ModelForRay.FileName, lpFileName );

	// �}�e���A���o�b�t�@
	LPD3DXBUFFER pD3DXMtrlBuffer = nullptr;

	//X�t�@�C���̃��[�h
	if( FAILED( D3DXLoadMeshFromX(
		lpFileName,					// �t�@�C����
		D3DXMESH_SYSTEMMEM			// �V�X�e���������ɓǂݍ���
		| D3DXMESH_32BIT,			// 32bit
		m_pDevice9, nullptr,
		&m_Model.pD3DXMtrlBuffer,	// (out)�}�e���A�����
		nullptr,
		&m_Model.NumMaterials,		// (out)�}�e���A����
		&m_Model.pMesh ) ) )		// (out)���b�V���I�u�W�F�N�g
	{
		_ASSERT_EXPR( false, _T( "X�t�@�C���Ǎ����s" ) );
		return E_FAIL;
	}

	// ���_���̕ۑ�
	SaveVertices(m_Model.pMesh);

	// X�t�@�C���̃��[�h(���C�Ƃ̔���p�ɕʐݒ�œǂݍ���)
	if( FAILED( D3DXLoadMeshFromX(
		lpFileName,						// �t�@�C����
		D3DXMESH_SYSTEMMEM,				// �V�X�e���������ɓǂݍ���
		m_pDevice9, nullptr,
		&m_ModelForRay.pD3DXMtrlBuffer,	// (out)�}�e���A�����
		nullptr,
		&m_ModelForRay.NumMaterials,	// (out)�}�e���A����
		&m_ModelForRay.pMesh ) ) )		// (out)���b�V���I�u�W�F�N�g
	{
		_ASSERT_EXPR( false, _T( "X�t�@�C���Ǎ����s" ) );
		return E_FAIL;
	}

	return S_OK;
}


//-----------------------------------------------------------------------------
//		�����_�����O�֐�(�N���X���ł̂ݎg�p����)
//-----------------------------------------------------------------------------
void StaticMesh::RenderMesh( D3DXMATRIX& mWorld, D3DXMATRIX& mView, D3DXMATRIX& mProj)
{
	// �V�F�[�_�̃R���X�^���g�o�b�t�@�Ɋe��f�[�^��n��
	D3D11_MAPPED_SUBRESOURCE pData;
	// �o�b�t�@���̃f�[�^�̏��������J�n����Map
	if( SUCCEEDED( m_pContext11->Map(
		m_pCBufferPerMesh, 0,
		D3D11_MAP_WRITE_DISCARD,
		0, &pData )))
	{
		// �R���X�^���g�o�b�t�@(���b�V���p)
		CBUFFER_PER_MESH cb;

		// ���[���h�s���n��
		cb.mW = mWorld;
		D3DXMatrixTranspose( &cb.mW, &cb.mW );

		// ���[���h,�r���[,�v���W�F�N�V�����s���n��
		D3DXMATRIX mWVP = mWorld * mView * mProj;
		D3DXMatrixTranspose( &mWVP, &mWVP );	// �s���]�u����
		// ���s��̌v�Z���@��DirectX��GPU�ňقȂ邽�ߓ]�u���K�v
		cb.mWVP = mWVP;

		memcpy_s(
			pData.pData,	// �R�s�[��̃o�b�t�@
			pData.RowPitch,	// �R�s�[��̃o�b�t�@�T�C�Y
			(void*)(&cb),	// �R�s�[���̃o�b�t�@
			sizeof(cb));	// �R�s�[���̃o�b�t�@�T�C�Y

		// �o�b�t�@���̃f�[�^�̏��������I������Unmap
		m_pContext11->Unmap( m_pCBufferPerMesh, 0 );
	}

	// ���̃R���X�^���g�o�b�t�@���ǂ̃V�F�[�_�Ŏg�p���邩�H
	m_pContext11->VSSetConstantBuffers( 0, 1, &m_pCBufferPerMesh ); // ���_�V�F�[�_
	m_pContext11->PSSetConstantBuffers( 0, 1, &m_pCBufferPerMesh ); // �s�N�Z���V�F�[�_

	// ���_�C���v�b�g���C�A�E�g���Z�b�g
	m_pContext11->IASetInputLayout( m_pVertexLayout );

	// �v���~�e�B�u�E�g�|���W�[���Z�b�g
	m_pContext11->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// ���_�o�b�t�@���Z�b�g
	UINT stride = m_Model.pMesh->GetNumBytesPerVertex();
	UINT offset = 0;
	m_pContext11->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &stride, &offset );

	// �����̐������A���ꂼ��̑����̃C���f�b�N�X�o�b�t�@��`��
	for( DWORD No = 0; No < m_NumAttr; No++ )
	{
		// �g�p����Ă��Ȃ��}�e���A���΍�
		if( m_pMaterials[m_AttrID[No]].dwNumFace == 0 ) { continue; }
		
		// �C���f�b�N�X�o�b�t�@���Z�b�g
		m_pContext11->IASetIndexBuffer( m_ppIndexBuffer[No], DXGI_FORMAT_R32_UINT, 0 );

		// �}�e���A���̊e�v�f���V�F�[�_�ɓn��
		D3D11_MAPPED_SUBRESOURCE pDataMat;
		if( SUCCEEDED(
			m_pContext11->Map(m_pCBufferPerMaterial,
				0, D3D11_MAP_WRITE_DISCARD, 0, &pDataMat )))
		{
			// �R���X�^���g�o�b�t�@(�}�e���A���p)
			CBUFFER_PER_MATERIAL cb;
			// �f�B�t���[�Y,�A���r�G���g,�X�y�L�������V�F�[�_�ɓn��
			cb.Diffuse	= m_pMaterials[m_AttrID[No]].Diffuse;
			cb.Ambient	= m_pMaterials[m_AttrID[No]].Ambient;
			cb.Specular = m_pMaterials[m_AttrID[No]].Specular;

			memcpy_s(pDataMat.pData, pDataMat.RowPitch,
				(void*)&cb, sizeof(cb));

			m_pContext11->Unmap( m_pCBufferPerMaterial, 0 );
		}

		// ���̃R���X�^���g�o�b�t�@���ǂ̃V�F�[�_�Ŏg�����H
		m_pContext11->VSSetConstantBuffers( 1, 1, &m_pCBufferPerMaterial );
		m_pContext11->PSSetConstantBuffers( 1, 1, &m_pCBufferPerMaterial );

		// �e�N�X�`�����V�F�[�_�ɓn��
		if (m_pMaterials[m_AttrID[No]].pTexture != nullptr) {
			// �e�N�X�`��������Ƃ�
			m_pContext11->PSSetSamplers( 0, 1, &m_pSampleLinear );
			m_pContext11->PSSetShaderResources(
				0, 1, &m_pMaterials[m_AttrID[No]].pTexture );
		}
		else {
			// �e�N�X�`�����Ȃ��Ƃ�
			ID3D11ShaderResourceView* pNothing[1] = { 0 };
			m_pContext11->PSSetShaderResources( 0, 1, pNothing );
		}

		// �v���~�e�B�u(�|���S��)�������_�����O
		m_pContext11->DrawIndexed( m_pMaterials[m_AttrID[No]].dwNumFace * 3, 0, 0 );
	}
}


//-----------------------------------------------------------------------------
//		�e���_���̕ۑ�
//-----------------------------------------------------------------------------
void StaticMesh::SaveVertices(LPD3DXMESH pMesh)
{
	if (!pMesh) return;

	// ���_�o�b�t�@�����b�N���ăf�[�^�ւ̃A�N�Z�X���m��
	void* pVertices = nullptr;
	if (FAILED(pMesh->LockVertexBuffer(D3DLOCK_READONLY, &pVertices))) { return; }

	// ���_���ƒ��_1������̃o�C�g�T�C�Y���擾
	DWORD numVertices = pMesh->GetNumVertices();		// �����_��
	DWORD vertexSize = pMesh->GetNumBytesPerVertex();	// �e���_�̃o�C�g��

	// �ۑ���̒��_���X�g���N���A�i�ė��p���̈��S��j
	m_Vertices.clear();

	// ���_�o�b�t�@����S���_���擾
	for (DWORD i = 0; i < numVertices; ++i)
	{
		// ���݂̒��_�̈ʒu���v�Z
		BYTE* pVertex = static_cast<BYTE*>(pVertices) + i * vertexSize;

		// ���_�̈ʒu���擾
		D3DXVECTOR3 position = *(D3DXVECTOR3*)pVertex;
		m_Vertices.push_back(position);
	}

	// ���_�o�b�t�@�̃��b�N������
	pMesh->UnlockVertexBuffer();
}


//----------------------------------------------------------------------------
//		�����ʒu�̃|���S���̒��_��������
//			�����_���W�̓��[�J�����W
//----------------------------------------------------------------------------
HRESULT StaticMesh::FindVerticesOnPoly( LPD3DXMESH pMesh, DWORD dwPolyIndex, D3DXVECTOR3* pVertices)
{
	DWORD dwStride	  = pMesh->GetNumBytesPerVertex();// ���_���Ƃ̃o�C�g�����擾
	DWORD dwVertexAmt = pMesh->GetNumVertices();	  // ���_�����擾
	DWORD dwPoly	  = pMesh->GetNumFaces();		  // �ʐ����擾
	WORD* pwPoly	  = nullptr;

	// �C���f�b�N�X�o�b�t�@�����b�N�i�ǂݍ��݃��[�h�j
	pMesh->LockIndexBuffer( D3DLOCK_READONLY, reinterpret_cast<VOID**>(&pwPoly));

	BYTE* pbVertices = nullptr;				// ���_(�o�C�g�^)
	FLOAT* pfVertices = nullptr;			// ���_(float�^)
	LPDIRECT3DVERTEXBUFFER9 VB = nullptr;	// ���_�o�b�t�@

	// ���_���̎擾
	pMesh->GetVertexBuffer(&VB);

	// ���_�o�b�t�@�̃��b�N
	if (SUCCEEDED( VB->Lock(0, 0, reinterpret_cast<VOID**>(&pbVertices), 0 ) ))
	{
		// �|���S���̒��_�P�ڂ��擾
		pfVertices = reinterpret_cast<FLOAT*>(&pbVertices[dwStride * pwPoly[dwPolyIndex * 3]]);
		pVertices[0].x = pfVertices[0];
		pVertices[0].y = pfVertices[1];
		pVertices[0].z = pfVertices[2];

		// �|���S���̒��_�Q�ڂ��擾
		pfVertices = reinterpret_cast<FLOAT*>(&pbVertices[dwStride * pwPoly[dwPolyIndex * 3 + 1]]);
		pVertices[1].x = pfVertices[0];
		pVertices[1].y = pfVertices[1];
		pVertices[1].z = pfVertices[2];

		// �|���S���̒��_�R�ڂ��擾
		pfVertices = reinterpret_cast<FLOAT*>(&pbVertices[dwStride * pwPoly[dwPolyIndex * 3 + 2]]);
		pVertices[2].x = pfVertices[0];
		pVertices[2].y = pfVertices[1];
		pVertices[2].z = pfVertices[2];

		pMesh->UnlockIndexBuffer();	// ���b�N����
		VB->Unlock();	// ���b�N����
	}
	VB->Release();	// �s�v�ɂȂ����̂ŉ��

	return S_OK;
}