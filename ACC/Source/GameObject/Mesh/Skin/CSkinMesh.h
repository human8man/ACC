/***************************************************************************************************
*	SkinMeshCode Version 2.40
*	LastUpdate	: 2024/06/14.
**/
#pragma once
//警告についてのコード分析を無効にする。4005：再定義.
#pragma warning( disable : 4005 )

#include "CSkinMeshParser.h"

//前方宣言.
class CDirectX9;
class CDirectX11;

/**************************************************
*	スキンメッシュクラス
*
*
**/
class CSkinMesh
{
public:
	//======================================
	//	構造体.
	//======================================
	//コンスタントバッファのアプリ側の定義.
	//※シェーダ内のコンスタントバッファと一致している必要あり.
	//メッシュ単位.
	struct CBUFFER_PER_MESH
	{
		D3DXMATRIX	mW;			//ワールド行列.
		D3DXMATRIX	mWVP;		//ワールドから射影までの変換行列.
	};

	//マテリアル単位.
	struct CBUFFER_PER_MATERIAL
	{
		D3DXVECTOR4 Ambient;	//環境光（アンビエント）.
		D3DXVECTOR4 Diffuse;	//拡散反射光（ディフューズ）.
		D3DXVECTOR4 Specular;	//鏡面反射光（スペキュラ）.
	};

	//シェーダーに渡す値.
	struct CBUFFER_PER_FRAME
	{
		D3DXVECTOR4 CameraPos;		//カメラ位置.
		D3DXVECTOR4 vLightDir;		//ライト方向.
	};

	//ボーン単位.
	struct CBUFFER_PER_BONES
	{
		D3DXMATRIX mBone[D3DXPARSER::MAX_BONES];
		CBUFFER_PER_BONES()
		{
			for ( int i = 0; i < D3DXPARSER::MAX_BONES; i++ )
			{
				D3DXMatrixIdentity( &mBone[i] );
			}
		}
	};

	//頂点構造体.
	struct VERTEX
	{
		D3DXVECTOR3	Position;	//頂点位置.
		D3DXVECTOR3	vNormal;	//頂点法線.
		D3DXVECTOR2	Texture;	//UV座標.
		UINT BoneIndex[4];		//ボーン 番号.
		float BoneWeight[4];	//ボーン 重み.
		VERTEX()
			: Position		()
			, vNormal		()
			, Texture		()
			, BoneIndex		()
			, BoneWeight	()
		{}
	};

public:
	CSkinMesh();	//コンストラクタ.
	~CSkinMesh();	//デストラクタ.

	HRESULT Init( CDirectX9& pDx9, CDirectX11& pDx11, LPCTSTR FileName );

	//解放関数.
	HRESULT Release();

	//描画関数.
	void Render( const D3DXMATRIX& mView, 
		const D3DXMATRIX& mProj, 
		const LIGHT& Light,
		const D3DXVECTOR3& CamPos,
		const LPD3DXANIMATIONCONTROLLER pAC );	//nullptrでデフォルト使用する.

	double GetAnimSpeed()				{ return m_AnimSpeed;		}
	void SetAnimSpeed( double Speed )	{ m_AnimSpeed = Speed;		}

	double GetAnimTime()				{ return m_AnimTime;		}
	void SetAnimTime( double Time )		{ m_AnimTime = Time;		}

	//パーサークラスからアニメーションコントローラを取得する.
	LPD3DXANIMATIONCONTROLLER GetAnimationController() { return m_pD3dxMesh->m_pAnimController; }

	//アニメーションセットの切り替え.
	void ChangeAnimSet( int index, LPD3DXANIMATIONCONTROLLER pAC );
	//アニメーションセットの切り替え(開始フレーム指定可能版).
	void ChangeAnimSet_StartPos( int Index, double StartFramePos, LPD3DXANIMATIONCONTROLLER pAC );

	//アニメーション停止時間を取得.
	double GetAnimPeriod( int Index );
	//アニメーション数を取得.
	int GetAnimMax( LPD3DXANIMATIONCONTROLLER pAC = nullptr );

	//指定したボーン情報(座標・行列)を取得する関数.
	bool GetMatrixFromBone( LPCSTR BoneName, D3DXMATRIX* pOutMat );
	bool GetPosFromBone( LPCSTR BoneName, D3DXVECTOR3* pOutPos);
	bool GetDeviaPosFromBone( LPCSTR BoneName, D3DXVECTOR3* pOutPos, D3DXVECTOR3 SpecifiedPos = { 0.0f, 0.0f, 0.0f } );

	//座標情報を設定.
	void SetPosition( const D3DXVECTOR3& vPos ) { m_Position = vPos;	}
	void SetPositionX( float x )				{ m_Position.x = x;		}
	void SetPositionY( float y )				{ m_Position.y = y;		}
	void SetPositionZ( float z )				{ m_Position.z = z;		}

	//回転情報を設定.
	void SetRotation( const D3DXVECTOR3& vRot ) { m_Rotation = vRot;	}
	void SetRotationY( float y )				{ m_Rotation.y = y;		}
	void SetRotationX( float x )				{ m_Rotation.x = x;		}
	void SetRotationZ( float z )				{ m_Rotation.z = z;		}

	//拡縮情報を設定・取得.
	void SetScale( const D3DXVECTOR3& Scale )	{ m_Scale = Scale;		}
	void SetScaleX( const float x )				{ m_Scale.x = x;		}
	void SetScaleY( const float y )				{ m_Scale.x = y;		}
	void SetScaleZ( const float z )				{ m_Scale.x = z;		}

private:
	//Xファイルからスキンメッシュを作成する.
	HRESULT LoadXMesh( LPCTSTR FileName );

	//シェーダを作成する.
	HRESULT CreateShader();
	//インデックスバッファを作成する.
	HRESULT CreateIndexBuffer( DWORD Size, int* pIndex, ID3D11Buffer** ppIndexBuffer );

	//メッシュを作成する.
	HRESULT CreateAppMeshFromD3DXMesh( LPD3DXFRAME pFrame );

	//コンスタントバッファ作成する.
	HRESULT CreateCBuffer( ID3D11Buffer** pConstantBuffer, UINT Size );
	//サンプラを作成する.
	HRESULT CreateSampler( ID3D11SamplerState** pSampler );

	//全てのメッシュを作成する.
	void BuildAllMesh( D3DXFRAME* pFrame );

	//Xファイルからスキン関連の情報を読み出す関数.
	HRESULT ReadSkinInfo( MYMESHCONTAINER* pContainer, VERTEX* pVB, SKIN_PARTS_MESH* pParts );

	//ボーンを次のポーズ位置にセットする関数.
	void SetNewPoseMatrices( SKIN_PARTS_MESH* pParts, int Frame, MYMESHCONTAINER* pContainer );
	//次の(現在の)ポーズ行列を返す関数.
	D3DXMATRIX GetCurrentPoseMatrix( SKIN_PARTS_MESH* pParts, int Index );

	//フレーム描画.
	void DrawFrame( LPD3DXFRAME pFrame );
	//パーツ描画.
	void DrawPartsMesh( SKIN_PARTS_MESH* pMesh, D3DXMATRIX World, MYMESHCONTAINER* pContainer );
	//ワールド行列算出.
	void CalcWorldMatrix();
	//各種シェーダに送るデータ.
	void SendCBufferPerBone( SKIN_PARTS_MESH* pMesh );
	void SendCBufferPerFrame();
	void SendCBufferPerMesh();
	void SendCBufferPerMaterial( MY_SKINMATERIAL* pMaterial );
	void SendTexture( MY_SKINMATERIAL* pMaterial );

	//全てのメッシュを削除.
	void DestroyAllMesh( D3DXFRAME* pFrame );
	HRESULT DestroyAppMeshFromD3DXMesh( LPD3DXFRAME p );

	//マルチバイト文字をUnicode文字に変換する.
	void ConvertCharaMultiByteToUnicode( WCHAR* Dest, size_t DestArraySize, const CHAR* str );

private:
	//Dx9.
	CDirectX9*				m_pDx9;
	LPDIRECT3DDEVICE9		m_pDevice9;	//Dx9デバイスオブジェクト.

	//Dx11.
	CDirectX11*				m_pDx11;
	ID3D11Device*			m_pDevice11;
	ID3D11DeviceContext*	m_pContext11;

	ID3D11VertexShader*		m_pVertexShader;
	ID3D11InputLayout*		m_pVertexLayout;
	ID3D11PixelShader*		m_pPixelShader;
	ID3D11SamplerState*		m_pSampleLinear;

	//コンスタントバッファ.
	ID3D11Buffer*			m_pCBufferPerMesh;		//コンスタントバッファ(メッシュ毎).
	ID3D11Buffer*			m_pCBufferPerMaterial;	//コンスタントバッファ(マテリアル毎).
	ID3D11Buffer*			m_pCBufferPerFrame;		//コンスタントバッファ(フレーム毎).
	ID3D11Buffer*			m_pCBufferPerBone;		//コンスタントバッファ(ボーン毎).

	D3DXVECTOR3		m_Position;		//位置(x,y,z).
	D3DXVECTOR3		m_Rotation;		//回転値(x,y,z).
	D3DXVECTOR3		m_Scale;		//拡大縮小値(x,y,z).

	D3DXMATRIX		m_mWorld;
	D3DXMATRIX		m_mRotation;

	D3DXMATRIX		m_mView;
	D3DXMATRIX		m_mProj;

	LIGHT			m_Light;
	D3DXVECTOR3		m_CamPos;

	//アニメーション速度.
	double m_AnimSpeed;
	double m_AnimTime;

	//解放処理用に.
//	SKIN_PARTS_MESH* m_pReleaseMaterial;

	//メッシュ.
	D3DXPARSER* m_pD3dxMesh;

	//Xファイルのパス.
	TCHAR	m_FilePath[256];

	//アニメーションフレーム.
	int		m_Frame;
};
