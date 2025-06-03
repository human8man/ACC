#include "CMeshLine.h"
#include "DirectX/CDirectX11.h"
#include "Mesh/Static/CStaticMeshObject.h"
#include "CGameObject.h"


// �V�F�[�_�t�@�C�����i�f�B���N�g�����܂ށj.
const TCHAR SHADER_NAME[] = _T("Data\\Shader\\Ray.hlsl");


//============================================================================
//		���C�N���X.
//============================================================================
CMeshLine::CMeshLine()
	: m_pDx11			( nullptr )
	, m_pDevice11		( nullptr )
	, m_pContext11		( nullptr )
	, m_pVertexShader	( nullptr )
	, m_pVertexLayout	( nullptr )
	, m_pPixelShader	( nullptr )
	, m_pConstantBuffer	( nullptr )
	, m_pVertexBuffer	( nullptr )
	, m_pIndexBuffer	()
	, m_vPosition		()
	, m_vRotation		()
{
}

CMeshLine::~CMeshLine()
{
	//����������Ăяo��.
	Release();

	//�ʂ̂Ƃ���ŊǗ����Ă���̂ł����ł�nullptr�����ď�����.
	m_pContext11	= nullptr;
	m_pDevice11		= nullptr;
	m_pDx11			= nullptr;
}


//============================================================================
//		������.
//============================================================================
HRESULT CMeshLine::Init()
{
	m_pDx11 = CDirectX11::GetInstance();
	m_pDevice11 = m_pDx11->GetDevice();
	m_pContext11 = m_pDx11->GetContext();

	// �V�F�[�_�쐬.
	if (FAILED(CreateShader())) { return E_FAIL; }

	return S_OK;
}


//============================================================================
//		���.
//============================================================================
void CMeshLine::Release()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pConstantBuffer);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pVertexLayout);
	SAFE_RELEASE(m_pVertexShader);

	m_pContext11 = nullptr;
	m_pDevice11 = nullptr;
}


//============================================================================
//		HLSL�t�@�C����ǂݍ��݃V�F�[�_���쐬����.
//============================================================================
HRESULT CMeshLine::CreateShader()
{
	ID3DBlob* pCompiledShader = nullptr;
	ID3DBlob* pErrors = nullptr;
	UINT uCompileFlag = 0;

#ifdef _DEBUG
	uCompileFlag = D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	//---------------------------------------------------------------
	//	HLSL����o�[�e�b�N�X�V�F�[�_�̃u���u���쐬.
	//---------------------------------------------------------------
	if (FAILED(
		D3DX11CompileFromFile(
			SHADER_NAME,		// �V�F�[�_�t�@�C�����iHLSL�t�@�C���j.
			nullptr,			// �}�N����`�̔z��ւ̃|�C���^�i���g�p�j.
			nullptr,			// �C���N���[�h�t�@�C���������C���^�[�t�F�C�X�ւ̃|�C���^�i���g�p�j.
			"VS_Main",			// �V�F�[�_�G���g���[�|�C���g�֐��̖��O.
			"vs_5_0",			// �V�F�[�_�̃��f�����w�肷�镶����i�v���t�@�C���j.
			uCompileFlag,		// �V�F�[�_�R���p�C���t���O.
			0,					// �G�t�F�N�g�R���p�C���t���O�i���g�p�j.
			nullptr,			// �X���b�h �|���v �C���^�[�t�F�C�X�ւ̃|�C���^�i���g�p�j.
			&pCompiledShader,	// �u���u���i�[���郁�����ւ̃|�C���^.
			&pErrors,			// �G���[�ƌx���ꗗ���i�[���郁�����ւ̃|�C���^.
			nullptr)))			// �߂�l�ւ̃|�C���^�i���g�p�j.
	{
		_ASSERT_EXPR(false, _T("hlsl�ǂݍ��ݎ��s"));
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);


	//---------------------------------------------------------------
	//	��L�ō쐬�����u���u����u�o�[�e�b�N�X�V�F�[�_�v���쐬.
	//---------------------------------------------------------------
	if (FAILED(
		m_pDevice11->CreateVertexShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			nullptr,
			&m_pVertexShader)))	// (out)�o�[�e�b�N�X�V�F�[�_.
	{
		_ASSERT_EXPR(false, _T("�o�[�e�b�N�X�V�F�[�_�쐬���s"));
		return E_FAIL;
	}


	//---------------------------------------------------------------
	//	���_�C���v�b�g���C�A�E�g���`.
	//---------------------------------------------------------------
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{
			"POSITION",						// �ʒu.
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,	// DXGI�̃t�H�[�}�b�g(32bit float�^*3).
			0,
			0,								// �f�[�^�̊J�n�ʒu.
			D3D11_INPUT_PER_VERTEX_DATA, 0
		}
	};
	// ���_�C���v�b�g���C�A�E�g�̔z��v�f�����Z�o.
	UINT numElements = sizeof(layout) / sizeof(layout[0]);


	//---------------------------------------------------------------
	//	���_�C���v�b�g���C�A�E�g���쐬.
	//---------------------------------------------------------------
	if (FAILED(
		m_pDevice11->CreateInputLayout(
			layout,
			numElements,
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			&m_pVertexLayout)))	// (out)���_�C���v�b�g���C�A�E�g.
	{
		_ASSERT_EXPR(false, _T("���_�C���v�b�g���C�A�E�g�쐬���s"));
		return E_FAIL;
	}
	SAFE_RELEASE(pCompiledShader);


	//---------------------------------------------------------------
	//	HLSL����s�N�Z���V�F�[�_�̃u���u���쐬.
	//---------------------------------------------------------------
	if (FAILED(
		D3DX11CompileFromFile(
			SHADER_NAME,		// �V�F�[�_�t�@�C�����iHLSL�t�@�C���j.
			nullptr,			// �}�N����`�̔z��ւ̃|�C���^�i���g�p�j.
			nullptr,			// �C���N���[�h�t�@�C���������C���^�[�t�F�C�X�ւ̃|�C���^�i���g�p�j.
			"PS_Main",			// �V�F�[�_�G���g���[�|�C���g�֐��̖��O.
			"ps_5_0",			// �V�F�[�_�̃��f�����w�肷�镶����i�v���t�@�C���j.
			uCompileFlag,		// �V�F�[�_�R���p�C���t���O.
			0,					// �G�t�F�N�g�R���p�C���t���O�i���g�p�j.
			nullptr,			// �X���b�h �|���v �C���^�[�t�F�C�X�ւ̃|�C���^�i���g�p�j.
			&pCompiledShader,	// �u���u���i�[���郁�����ւ̃|�C���^.
			&pErrors,			// �G���[�ƌx���ꗗ���i�[���郁�����ւ̃|�C���^.
			nullptr)))			// �߂�l�ւ̃|�C���^�i���g�p�j.
	{
		_ASSERT_EXPR(false, _T("hlsl�ǂݍ��ݎ��s"));
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);


	//---------------------------------------------------------------
	//	��L�ō쐬�����u���u����u�s�N�Z���V�F�[�_�v���쐬.
	//---------------------------------------------------------------
	if (FAILED(
		m_pDevice11->CreatePixelShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			nullptr,
			&m_pPixelShader)))	// (out)�s�N�Z���V�F�[�_.
	{
		_ASSERT_EXPR(false, _T("�s�N�Z���V�F�[�_�쐬���s"));
		return E_FAIL;
	}
	SAFE_RELEASE(pCompiledShader);


	//---------------------------------------------------------------
	// �R���X�^���g�i�萔�j�o�b�t�@�쐬.
	// �V�F�[�_�ɓ���̐��l�𑗂�o�b�t�@.
	//---------------------------------------------------------------
	D3D11_BUFFER_DESC cb;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;		// �R���X�^���g�o�b�t�@���w��.
	cb.ByteWidth = sizeof(SHADER_CONSTANT_BUFFER);	// �R���X�^���g�o�b�t�@�̃T�C�Y.
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;		// �������݂ŃA�N�Z�X.
	cb.MiscFlags = 0;								// ���̑��̃t���O�i���g�p�j.
	cb.StructureByteStride = 0;						// �\���̂̃T�C�Y�i���g�p�j.
	cb.Usage = D3D11_USAGE_DYNAMIC;					// �g�p���@�F���ڏ�������.


	//---------------------------------------------------------------
	//	�R���X�^���g�o�b�t�@�̍쐬.
	//---------------------------------------------------------------
	if (FAILED(
		m_pDevice11->CreateBuffer(
			&cb,
			nullptr,
			&m_pConstantBuffer)))
	{
		_ASSERT_EXPR(false, _T("�R���X�^���g�o�b�t�@�쐬���s"));
		return E_FAIL;
	}

	return S_OK;
}


//============================================================================
//		�����_�����O�p.
//============================================================================
void CMeshLine::Render(D3DXMATRIX& mView, D3DXMATRIX& mProj)
{
	// ���[���h�s��.
	D3DXMATRIX	mWorld;
	D3DXMATRIX	mTrans, mRot;

	// ��]�s��.
	D3DXMATRIX mYaw, mPitch, mRoll;
	D3DXMatrixRotationY(&mYaw, m_vRotation.y);
	D3DXMatrixRotationX(&mPitch, m_vRotation.x);
	D3DXMatrixRotationZ(&mRoll, m_vRotation.z);
	mRot = mYaw * mPitch * mRoll;

	// ���s�s��i���s�ړ��j.
	D3DXMatrixTranslation(&mTrans, m_vPosition.x, m_vPosition.y, m_vPosition.z);

	// ���[���h���W�ϊ�.
	mWorld = mRot * mTrans;

	// �g�p����V�F�[�_�̓o�^.
	m_pContext11->VSSetShader(m_pVertexShader, nullptr, 0);
	m_pContext11->PSSetShader(m_pPixelShader, nullptr, 0);

	// �V�F�[�_�̃R���X�^���g�o�b�t�@�Ɋe��f�[�^��n��.
	D3D11_MAPPED_SUBRESOURCE pData;
	SHADER_CONSTANT_BUFFER cb;


	//---------------------------------------------------------------
	//	�o�b�t�@���̃f�[�^�̏��������J�n����map.
	//---------------------------------------------------------------
	if (SUCCEEDED(
		m_pContext11->Map(
			m_pConstantBuffer,
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&pData)))
	{
		// ���[���h,�r���[,�v���W�F�N�V�����s���n��.
		D3DXMATRIX m = mWorld * mView * mProj;
		D3DXMatrixTranspose(&m, &m);	// �s���]�u����.
		cb.mWVP = m;

		// �J���[.
		cb.vColor = D3DXVECTOR4(1.f, 1.f, 1.f, 1.f);

		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));

		m_pContext11->Unmap(m_pConstantBuffer, 0);
	}

	// ���̃R���X�^���g�o�b�t�@���ǂ̃V�F�[�_�Ŏg�����H.
	m_pContext11->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pContext11->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	// ���_�o�b�t�@���Z�b�g.
	UINT stride = sizeof(D3DXVECTOR3);	// �f�[�^�̊Ԋu.
	UINT offset = 0;
	m_pContext11->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// ���_�C���v�b�g���C�A�E�g���Z�b�g.
	m_pContext11->IASetInputLayout(m_pVertexLayout);
	// �v���~�e�B�u�E�g�|���W�[���Z�b�g(LINELIST:���_�f�[�^����̈ꗗ�Ƃ��ĉ���).
	m_pContext11->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// �v���~�e�B�u�������_�����O.
	// m_pContext11->Draw(2, 0);	// ���̃R�[�h (����1��)

	// ���b�V���̕ӂ̐��Ɋ�Â��ĕ`�悷�钸�_�����v�Z
	UINT vertexCount = 0;
	if (!m_Vertices.empty())
	{
		vertexCount = static_cast<UINT>(m_Vertices.size());
		if (vertexCount % 3 == 0) // �O�p�`���b�V���Ɖ���
		{
			vertexCount = vertexCount * 2; // �e�ӂ�2�̒��_
		}
		else
		{
			// �O�p�`�ȊO�̃|���S���̏ꍇ�́A�ӂ̐���K�؂Ɍv�Z����K�v������܂�
			// �����ł͒P���ɒ��_����2�{�Ƃ��Ă��܂����A���m�ł͂���܂���
			// ���m�ȏ����̓��b�V���̍\���Ɉˑ����܂�
			vertexCount = vertexCount * 2;
		}
	}
	m_pContext11->Draw(vertexCount, 0);
}


//=============================================================================
//		���b�V�����烏�C���[�t���[��.
//=============================================================================
void CMeshLine::DrawMeshWireframeFromVertices(
	const CStaticMesh&	mesh,
	const CGameObject&	object,
	const D3DXMATRIX&	viewMatrix,
	const D3DXMATRIX&	projectionMatrix,
	const D3DXVECTOR4&	color,
	const float&		scale)
{
	// ���C���[�t���[���p���X�^���C�U�[�X�e�[�g�쐬
	ID3D11RasterizerState* pWireRasterState = nullptr;
	ID3D11RasterizerState* pOldRasterState = nullptr;

	D3D11_RASTERIZER_DESC rasterDesc = {};
	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;   // �� ���ꂪ�d�v
	rasterDesc.CullMode = D3D11_CULL_BACK;        // ���ʃJ�����O�i�K�v�Ȃ�D3D11_CULL_NONE�ł�OK�j
	rasterDesc.DepthClipEnable = true;

	CDirectX11* directx11 = CDirectX11::GetInstance();
	auto* context = directx11->GetContext();
	auto* device = directx11->GetDevice();

	device->CreateRasterizerState(&rasterDesc, &pWireRasterState);

	// ���݂̏�Ԃ��擾���đޔ�
	context->RSGetState(&pOldRasterState);

	// ���C���[�t���[����Ԃɐ؂�ւ�
	context->RSSetState(pWireRasterState);

	//-----------------------------------------------------
	//		���[���h�s��v�Z.
	//-----------------------------------------------------
	// ���[���h�ϊ��s��.
	D3DXMATRIX mTrans, mRot, mScale;

	// �g��k���s��.
	D3DXMatrixScaling(&mScale,
		object.GetScale().x * scale,
		object.GetScale().y * scale,
		object.GetScale().z * scale);
	
	// ��]�s��.
	D3DXMATRIX mYaw, mPitch, mRoll;
	D3DXMatrixRotationY(&mYaw,		object.GetRot().y);
	D3DXMatrixRotationX(&mPitch,	object.GetRot().x);
	D3DXMatrixRotationZ(&mRoll,		object.GetRot().z);
	mRot = mYaw * mPitch * mRoll;

	// ���s�ړ��s��.
	D3DXMatrixTranslation(&mTrans,
		object.GetPos().x,
		object.GetPos().y,
		object.GetPos().z);

	// ���[���h�s�� = �g�� * ��] * ���s�ړ�.
	D3DXMATRIX mWorld = mScale * mRot * mTrans;

	//-----------------------------------------------------
	//		���b�V���̒��_������̃��X�g�`���ɕϊ�.
	//-----------------------------------------------------
	m_pVertexBuffer = mesh.GetVertexBuffer();
	m_pIndexBuffer = mesh.GetIndexBuffer();
	m_Indices = mesh.GetIndex();

	// �g�p����V�F�[�_�̓o�^.
	directx11->GetContext()->VSSetShader(m_pVertexShader, nullptr, 0);
	directx11->GetContext()->PSSetShader(m_pPixelShader, nullptr, 0);

	// �R���X�^���g�o�b�t�@�̐ݒ�.
	D3D11_MAPPED_SUBRESOURCE pData;
	SHADER_CONSTANT_BUFFER cb;
	if (SUCCEEDED(
		directx11->GetContext()->Map(
			m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		D3DXMATRIX m = mWorld * viewMatrix * projectionMatrix;
		D3DXMatrixTranspose(&m, &m);
		cb.mWVP = m;
		cb.vColor = color;
		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
		directx11->GetContext()->Unmap(m_pConstantBuffer, 0);
		directx11->GetContext()->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
		directx11->GetContext()->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	}

	// ���C���[�t���[���p�̒��_�o�b�t�@���Z�b�g.
	UINT stride = mesh.GetMesh()->GetNumBytesPerVertex();
	UINT offset = 0;
	directx11->GetContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	

	// ���_�C���v�b�g���C�A�E�g���Z�b�g.
	directx11->GetContext()->IASetInputLayout(mesh.GetVertexLayout());

	// �v���~�e�B�u�E�g�|���W�[���Z�b�g (LINELIST).
	directx11->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// �v���~�e�B�u�������_�����O.
	for (size_t i = 0; i < m_pIndexBuffer.size();i++)
	{
		directx11->GetContext()->IASetIndexBuffer(m_pIndexBuffer[i], DXGI_FORMAT_R32_UINT, 0);
		directx11->GetContext()->DrawIndexed(m_Indices[i] * 3, 0, 0);
	}
	// ���̃��X�^���C�U�[�X�e�[�g�ɖ߂�
	context->RSSetState(pOldRasterState);
}