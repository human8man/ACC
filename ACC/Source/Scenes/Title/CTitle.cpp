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
		// ortho行列なのでZはいらない.
		//	画面中央が0のやつやからちょっとめんどい後で変える.
		{ { -50.0f, 360.0f, 0.0f }, { 1, 0, 0, 1 } },  // 始点（赤）
		{ { 50.0f, -360.0f, 0.0f }, { 1, 0, 0, 1 } },  // 終点（赤）
	};

	// フレーム単位で渡す情報.
	struct CBUFFER_PER_FRAME
	{
		D3DXVECTOR4	CameraPos;	// カメラ位置(視点位置).
		D3DXVECTOR4	vLightDir;	// ライト方向.
	};

	struct CBUFFER_MATRIX {
		D3DXMATRIX mWorld;
		D3DXMATRIX mView;
		D3DXMATRIX mProj;
	};
}

//=================================================================================================
//		タイトルクラス.
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
	m_Light.vDirection = D3DXVECTOR3(1.f, 5.f, 0.f);	//ライト方向.
}
CTitle::~CTitle()
{
	Release();
}


//=================================================================================================
//		作成処理.
//=================================================================================================
void CTitle::Create()
{
	m_pTitleUI = std::make_unique<CTitleUI>();
	m_pTitleUI->Create();
	m_pEgg = new CStaticMesh();
	ShaderTestFunction();
}


//=================================================================================================
//		読込処理.
//=================================================================================================
HRESULT CTitle::LoadData()
{
	m_pEgg->Init(_T("Data\\Mesh\\Static\\Player\\egg.x"));
	m_pEgg->SetPos(D3DXVECTOR3(10.f, -1.f, 4.f));

	return S_OK;
}


//=================================================================================================
//		初期化処理.
//=================================================================================================
void CTitle::Init()
{
	m_Start = false;
	CCamera::GetInstance()->Init();	// カメラの初期化.
}


//=================================================================================================
//		更新処理.
//=================================================================================================
void CTitle::Update()
{
	// BGM再生.
	CSoundManager::GetInstance()->PlayLoop(CSoundManager::enList::BGM_Title);
	m_pTitleUI->Update();
	m_pEgg->SetRotY(m_pEgg->GetRot().y + 0.01f);

	if (m_pTitleUI->GetStart()) {
		// ゲームを開始する.
		CSceneManager::GetInstance()->LoadScene(SceneList::Game);
		CSoundManager::GetInstance()->AllStop();
	}


#ifdef _DEBUG
	EditLineImGui();
#endif
}


//=================================================================================================
//		描画処理.
//=================================================================================================
void CTitle::Draw()
{
	CCamera::GetInstance()->Camera(m_mView);
	CSceneBase::Projection(m_mProj);
	m_pTitleUI->Draw();
	m_pEgg->Render(m_mView, m_mProj, m_Light);
	// 3Dでの線描画.
	//CameraDrawShader(m_mView, m_mProj);
	// 2Dでの線描画.
	//UIDrawShader();
}


//=================================================================================================
//		解放処理.
//=================================================================================================
void CTitle::Release()
{
	m_pTitleUI.reset();
	SAFE_DELETE(m_pEgg);
}


//-----------------------------------------------------------------------------------
//		シェーダ読み込み関数（一回のみ）.
//-----------------------------------------------------------------------------------
HRESULT CTitle::ShaderTestFunction()
{
	auto directx11 = CDirectX11::GetInstance();
	ID3D11Device* device = directx11->GetDevice();
	ID3D11DeviceContext* context = directx11->GetContext();

	// シェーダ読み込み
	ID3D10Blob* vsBlob = nullptr;
	ID3D10Blob* psBlob = nullptr;
	D3DX11CompileFromFile(
		SHADER_NAME, nullptr, nullptr, "VSMain", "vs_5_0",
		0, 0, nullptr, &vsBlob, nullptr, nullptr);
	D3DX11CompileFromFile(
		SHADER_NAME, nullptr, nullptr, "PSMain", "ps_5_0",
		0, 0, nullptr, &psBlob, nullptr, nullptr);

	// シェーダ作成
	device->CreateVertexShader(
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(), nullptr, &m_pVertexShader);
	device->CreatePixelShader(
		psBlob->GetBufferPointer(),
		psBlob->GetBufferSize(), nullptr, &m_pPixelShader);

	// 入力レイアウト
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	ID3D11InputLayout* inputLayout = nullptr;
	device->CreateInputLayout(layout, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);

	// 頂点バッファ
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;  // ←ここ重要
	bd.ByteWidth = sizeof(lineVertices);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;  // ←ここも必要

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = lineVertices;

	device->CreateBuffer(&bd, &initData, &m_pVertexBuffer);

	// ← CreateInputLayoutの後とかに追記
	D3D11_BUFFER_DESC cbDesc = {};
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.ByteWidth = sizeof(CBUFFER_PER_FRAME); // 16の倍数か注意
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	if (FAILED(device->CreateBuffer(&cbDesc, nullptr, &m_pCBufferPerFrame)))
	{
		MessageBoxA(nullptr, "定数バッファ作成失敗", "Error", MB_OK);
		return E_FAIL;
	}

	// Matrixバッファの読み込み
	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(CBUFFER_MATRIX);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	device->CreateBuffer(&desc, nullptr, &m_pCBufferMatrix);

	return S_OK;
}


//-----------------------------------------------------------------------------------
//		3Dシェーダー描画(マイフレーム呼び出す).
// 
//		note:これはViewとProj両方使うやつ.
//			Viewにはカメラから見えている情報.
//			Projにはそれを2D情報に落とし込める機能があるらしい.
//			それらを用いて描画.
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

    // Matrix バッファ更新
	D3D11_MAPPED_SUBRESOURCE mappedMatrix = {};
	if (SUCCEEDED(context->Map(m_pCBufferMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedMatrix)))
	{
		CBUFFER_MATRIX cbMatrix = {};
		D3DXMatrixIdentity(&cbMatrix.mWorld); // 固定ならIdentityでOK
		cbMatrix.mView = mView;
		cbMatrix.mProj = mProj;
		memcpy_s(mappedMatrix.pData, mappedMatrix.RowPitch, &cbMatrix, sizeof(cbMatrix));
		context->Unmap(m_pCBufferMatrix, 0);
	}
	context->VSSetConstantBuffers(0, 1, &m_pCBufferMatrix);

	// Lightバッファ更新（今のままでOK）
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
//		UIシェーダー描画(マイフレーム呼び出す).
// 
//		note:Proj(ortho行列)を新規で用意して.
//				平面での描画をする.
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

	// Ortho行列生成（画面サイズに合わせて）
	D3D11_VIEWPORT vp;
	UINT vpCount = 1;
	context->RSGetViewports(&vpCount, &vp);

	D3DXMATRIX mProj;
	D3DXMatrixOrthoLH(&mProj, vp.Width, vp.Height, 0.0f, 1.0f); // Near=0, Far=1

	// コンスタントバッファ（行列）更新
	D3D11_MAPPED_SUBRESOURCE mappedMatrix = {};
	if (SUCCEEDED(context->Map(m_pCBufferMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedMatrix)))
	{
		CBUFFER_MATRIX cbMatrix = {};
		D3DXMatrixIdentity(&cbMatrix.mWorld); // そのまま
		D3DXMatrixIdentity(&cbMatrix.mView);  // Orthoのみ使うのでView不要
		cbMatrix.mProj = mProj;
		memcpy_s(mappedMatrix.pData, mappedMatrix.RowPitch, &cbMatrix, sizeof(cbMatrix));
		context->Unmap(m_pCBufferMatrix, 0);
	}
	context->VSSetConstantBuffers(0, 1, &m_pCBufferMatrix);
	context->VSSetConstantBuffers(2, 1, &m_pCBufferPerFrame);
	context->PSSetConstantBuffers(2, 1, &m_pCBufferPerFrame);

	// 描画
	context->Draw(2, 0);
}


//-----------------------------------------------------------------------------------
//		ImGuiでの線いじり.
//-----------------------------------------------------------------------------------
void CTitle::EditLineImGui()
{
	ImGui::Begin("Line Editor");

	// 座標編集（開始点・終了点）
	ImGui::Text("Start Point");
	ImGui::DragFloat3("Start Pos", (float*)&lineVertices[0].pos, 0.1f);

	ImGui::Text("End Point");
	ImGui::DragFloat3("End Pos", (float*)&lineVertices[1].pos, 0.1f);

	// 色編集（RGBA）
	ImGui::Text("Start Color");
	ImGui::ColorEdit4("Start Color", (float*)&lineVertices[0].color);

	ImGui::Text("End Color");
	ImGui::ColorEdit4("End Color", (float*)&lineVertices[1].color);

	ImGui::End();

	// 頂点バッファ更新
	auto deviceContext = CDirectX11::GetInstance()->GetContext();
	D3D11_MAPPED_SUBRESOURCE mapped;
	if (SUCCEEDED(deviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
	{
		memcpy(mapped.pData, lineVertices, sizeof(lineVertices));
		deviceContext->Unmap(m_pVertexBuffer, 0);
	}
}