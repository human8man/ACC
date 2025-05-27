#include "CTitle.h"

#include "Camera/CCamera.h"
#include "Mesh/Static/CStaticMesh.h"
#include "DirectInput/CDirectInput.h"
#include "DirectSound/CSoundManager.h"
#include "Scenes/SceneManager/CSceneManager.h"
#include "Sprite/2D/SpriteManager/SpriteManager.h"
#include "Sprite/2D/UI/CTitleUI/CTitleUI.h"


#ifdef _DEBUG
#include "ImGui/CImGui.h"
#endif


namespace {
	TCHAR SHADER_NAME[] = _T("Data\\Shader\\TestShader.hlsl");

	struct Vertex
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR4 color;
	};

	Vertex lineVertices[] =
	{
		// ortho�s��Ȃ̂�Z�͂���Ȃ�.
		//	��ʒ�����0�̂�₩�炿����Ƃ߂�ǂ���ŕς���.
		{ { -50.0f, 360.0f, 0.0f }, { 1, 0, 0, 1 } },  // �n�_�i�ԁj
		{ { 50.0f, -360.0f, 0.0f }, { 1, 0, 0, 1 } },  // �I�_�i�ԁj
	};

	// �t���[���P�ʂœn�����.
	struct CBUFFER_PER_FRAME
	{
		D3DXVECTOR4	CameraPos;	// �J�����ʒu(���_�ʒu).
		D3DXVECTOR4	vLightDir;	// ���C�g����.
	};

	struct CBUFFER_MATRIX {
		D3DXMATRIX mWorld;
		D3DXMATRIX mView;
		D3DXMATRIX mProj;
	};
}

//=================================================================================================
//		�^�C�g���N���X.
//=================================================================================================
CTitle::CTitle(HWND hWnd)
	: m_Light			()
	, m_mView			()
	, m_mProj			()
	, m_pTitleUI		( nullptr )
	, m_pEgg			()
	, m_Start			( false )
	, m_pVertexShader	( nullptr )
	, m_pVertexLayout	( nullptr )
	, m_pPixelShader	( nullptr )
	, m_pVertexBuffer	( nullptr )
	, m_pCBufferPerFrame( nullptr )
	, m_pCBufferMatrix	( nullptr )
{
	m_hWnd = hWnd;
	m_Light.vDirection = D3DXVECTOR3(1.f, 5.f, 0.f);	//���C�g����.
}
CTitle::~CTitle()
{
	Release();
}


//=================================================================================================
//		�쐬����.
//=================================================================================================
void CTitle::Create()
{
	m_pTitleUI = std::make_unique<CTitleUI>();
	m_pTitleUI->Create();
	m_pEgg = new CStaticMesh();
	ShaderTestFunction();
}


//=================================================================================================
//		�Ǎ�����.
//=================================================================================================
HRESULT CTitle::LoadData()
{
	m_pEgg->Init(_T("Data\\Mesh\\Static\\Player\\egg.x"));
	m_pEgg->SetPos(D3DXVECTOR3(10.f, -1.f, 4.f));

	return S_OK;
}


//=================================================================================================
//		����������.
//=================================================================================================
void CTitle::Init()
{
	m_Start = false;
	CCamera::GetInstance()->Init();	// �J�����̏�����.
}


//=================================================================================================
//		�X�V����.
//=================================================================================================
void CTitle::Update()
{
	// BGM�Đ�.
	CSoundManager::GetInstance()->PlayLoop(CSoundManager::enList::BGM_Title);
	m_pTitleUI->Update();
	m_pEgg->SetRotY(m_pEgg->GetRot().y + 0.01f);

	if (m_pTitleUI->GetStart()) {
		// �Q�[�����J�n����.
		CSceneManager::GetInstance()->LoadScene(SceneList::Game);
		CSoundManager::GetInstance()->AllStop();
	}


#ifdef _DEBUG
	EditLineImGui();
#endif
}


//=================================================================================================
//		�`�揈��.
//=================================================================================================
void CTitle::Draw()
{
	CCamera::GetInstance()->Camera(m_mView);
	CSceneBase::Projection(m_mProj);
	m_pTitleUI->Draw();
	m_pEgg->Render(m_mView, m_mProj, m_Light);
	// 3D�ł̐��`��.
	//CameraDrawShader(m_mView, m_mProj);
	// 2D�ł̐��`��.
	//UIDrawShader();
}


//=================================================================================================
//		�������.
//=================================================================================================
void CTitle::Release()
{
	m_pTitleUI.reset();
	SAFE_DELETE(m_pEgg);
}


//-----------------------------------------------------------------------------------
//		�V�F�[�_�ǂݍ��݊֐��i���̂݁j.
//-----------------------------------------------------------------------------------
HRESULT CTitle::ShaderTestFunction()
{
	auto directx11 = CDirectX11::GetInstance();
	ID3D11Device* device = directx11->GetDevice();
	ID3D11DeviceContext* context = directx11->GetContext();

	// �V�F�[�_�ǂݍ���
	ID3D10Blob* vsBlob = nullptr;
	ID3D10Blob* psBlob = nullptr;
	D3DX11CompileFromFile(
		SHADER_NAME, nullptr, nullptr, "VSMain", "vs_5_0",
		0, 0, nullptr, &vsBlob, nullptr, nullptr);
	D3DX11CompileFromFile(
		SHADER_NAME, nullptr, nullptr, "PSMain", "ps_5_0",
		0, 0, nullptr, &psBlob, nullptr, nullptr);

	// �V�F�[�_�쐬
	device->CreateVertexShader(
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(), nullptr, &m_pVertexShader);
	device->CreatePixelShader(
		psBlob->GetBufferPointer(),
		psBlob->GetBufferSize(), nullptr, &m_pPixelShader);

	// ���̓��C�A�E�g
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	ID3D11InputLayout* inputLayout = nullptr;
	device->CreateInputLayout(layout, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);

	// ���_�o�b�t�@
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;  // �������d�v
	bd.ByteWidth = sizeof(lineVertices);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;  // ���������K�v

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = lineVertices;

	device->CreateBuffer(&bd, &initData, &m_pVertexBuffer);

	// �� CreateInputLayout�̌�Ƃ��ɒǋL
	D3D11_BUFFER_DESC cbDesc = {};
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.ByteWidth = sizeof(CBUFFER_PER_FRAME); // 16�̔{��������
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	if (FAILED(device->CreateBuffer(&cbDesc, nullptr, &m_pCBufferPerFrame)))
	{
		MessageBoxA(nullptr, "�萔�o�b�t�@�쐬���s", "Error", MB_OK);
		return E_FAIL;
	}

	// Matrix�o�b�t�@�̓ǂݍ���
	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(CBUFFER_MATRIX);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	device->CreateBuffer(&desc, nullptr, &m_pCBufferMatrix);

	return S_OK;
}


//-----------------------------------------------------------------------------------
//		3D�V�F�[�_�[�`��(�}�C�t���[���Ăяo��).
// 
//		note:�����View��Proj�����g�����.
//			View�ɂ̓J�������猩���Ă�����.
//			Proj�ɂ͂����2D���ɗ��Ƃ����߂�@�\������炵��.
//			������p���ĕ`��.
// 
//-----------------------------------------------------------------------------------
void CTitle::CameraDrawShader(D3DXMATRIX& mView, D3DXMATRIX& mProj)
{
	auto directx11 = CDirectX11::GetInstance();
	ID3D11DeviceContext* context = directx11->GetContext();

	context->IASetInputLayout(m_pVertexLayout);
	context->VSSetShader(m_pVertexShader, nullptr, 0);
	context->PSSetShader(m_pPixelShader, nullptr, 0);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    // Matrix �o�b�t�@�X�V
	D3D11_MAPPED_SUBRESOURCE mappedMatrix = {};
	if (SUCCEEDED(context->Map(m_pCBufferMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedMatrix)))
	{
		CBUFFER_MATRIX cbMatrix = {};
		D3DXMatrixIdentity(&cbMatrix.mWorld); // �Œ�Ȃ�Identity��OK
		cbMatrix.mView = mView;
		cbMatrix.mProj = mProj;
		memcpy_s(mappedMatrix.pData, mappedMatrix.RowPitch, &cbMatrix, sizeof(cbMatrix));
		context->Unmap(m_pCBufferMatrix, 0);
	}
	context->VSSetConstantBuffers(0, 1, &m_pCBufferMatrix);

	// Light�o�b�t�@�X�V�i���̂܂܂�OK�j
	D3D11_MAPPED_SUBRESOURCE mapped = {};
	if (SUCCEEDED(context->Map(m_pCBufferPerFrame, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
	{
		CBUFFER_PER_FRAME cb;
		D3DXVECTOR3 camPos = CCamera::GetInstance()->GetPos();
		cb.CameraPos = D3DXVECTOR4(camPos.x, camPos.y, camPos.z, 0.f);
		cb.vLightDir = D3DXVECTOR4(m_Light.vDirection.x, m_Light.vDirection.y, m_Light.vDirection.z, 0.f);
		D3DXVec4Normalize(&cb.vLightDir, &cb.vLightDir);
		memcpy_s(mapped.pData, mapped.RowPitch, &cb, sizeof(cb));
		context->Unmap(m_pCBufferPerFrame, 0);
	}
	context->VSSetConstantBuffers(2, 1, &m_pCBufferPerFrame);
	context->PSSetConstantBuffers(2, 1, &m_pCBufferPerFrame);


	context->Draw(2, 0);
}


//-----------------------------------------------------------------------------------
//		UI�V�F�[�_�[�`��(�}�C�t���[���Ăяo��).
// 
//		note:Proj(ortho�s��)��V�K�ŗp�ӂ���.
//				���ʂł̕`�������.
// 
//-----------------------------------------------------------------------------------
void CTitle::UIDrawShader()
{
	auto directx11 = CDirectX11::GetInstance();
	ID3D11DeviceContext* context = directx11->GetContext();
	ID3D11Device* device = directx11->GetDevice();

	context->IASetInputLayout(m_pVertexLayout);
	context->VSSetShader(m_pVertexShader, nullptr, 0);
	context->PSSetShader(m_pPixelShader, nullptr, 0);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// Ortho�s�񐶐��i��ʃT�C�Y�ɍ��킹�āj
	D3D11_VIEWPORT vp;
	UINT vpCount = 1;
	context->RSGetViewports(&vpCount, &vp);

	D3DXMATRIX mProj;
	D3DXMatrixOrthoLH(&mProj, vp.Width, vp.Height, 0.0f, 1.0f); // Near=0, Far=1

	// �R���X�^���g�o�b�t�@�i�s��j�X�V
	D3D11_MAPPED_SUBRESOURCE mappedMatrix = {};
	if (SUCCEEDED(context->Map(m_pCBufferMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedMatrix)))
	{
		CBUFFER_MATRIX cbMatrix = {};
		D3DXMatrixIdentity(&cbMatrix.mWorld); // ���̂܂�
		D3DXMatrixIdentity(&cbMatrix.mView);  // Ortho�̂ݎg���̂�View�s�v
		cbMatrix.mProj = mProj;
		memcpy_s(mappedMatrix.pData, mappedMatrix.RowPitch, &cbMatrix, sizeof(cbMatrix));
		context->Unmap(m_pCBufferMatrix, 0);
	}
	context->VSSetConstantBuffers(0, 1, &m_pCBufferMatrix);
	context->VSSetConstantBuffers(2, 1, &m_pCBufferPerFrame);
	context->PSSetConstantBuffers(2, 1, &m_pCBufferPerFrame);

	// �`��
	context->Draw(2, 0);
}


//-----------------------------------------------------------------------------------
//		ImGui�ł̐�������.
//-----------------------------------------------------------------------------------
void CTitle::EditLineImGui()
{
	ImGui::Begin("Line Editor");

	// ���W�ҏW�i�J�n�_�E�I���_�j
	ImGui::Text("Start Point");
	ImGui::DragFloat3("Start Pos", (float*)&lineVertices[0].pos, 0.1f);

	ImGui::Text("End Point");
	ImGui::DragFloat3("End Pos", (float*)&lineVertices[1].pos, 0.1f);

	// �F�ҏW�iRGBA�j
	ImGui::Text("Start Color");
	ImGui::ColorEdit4("Start Color", (float*)&lineVertices[0].color);

	ImGui::Text("End Color");
	ImGui::ColorEdit4("End Color", (float*)&lineVertices[1].color);

	ImGui::End();

	// ���_�o�b�t�@�X�V
	auto deviceContext = CDirectX11::GetInstance()->GetContext();
	D3D11_MAPPED_SUBRESOURCE mapped;
	if (SUCCEEDED(deviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
	{
		memcpy(mapped.pData, lineVertices, sizeof(lineVertices));
		deviceContext->Unmap(m_pVertexBuffer, 0);
	}
}