#include "CStaticMeshObject.h"
#include "Camera/CCamera.h"

CStaticMeshObject::CStaticMeshObject()
	: m_pMesh		( nullptr )
	, m_pBSphere	( nullptr )
{
	m_pBSphere = new CBoundingSphere();
}

CStaticMeshObject::~CStaticMeshObject()
{
	SAFE_DELETE( m_pBSphere );
	DetachMesh();
}

void CStaticMeshObject::Update()
{
	if( m_pMesh == nullptr ){
		return;
	}
}

void CStaticMeshObject::Draw(
	D3DXMATRIX& View, D3DXMATRIX& Proj,
	LIGHT& Light )
{
	if( m_pMesh == nullptr ){
		return;
	}

	//描画直前で座標や回転情報などを更新.
	m_pMesh->SetPosition( m_vPosition );
	m_pMesh->SetRotation( m_vRotation );
	m_pMesh->SetScale( m_vScale );

	//レンダリング.
	m_pMesh->Render( View, Proj, Light );
}

//レイとメッシュの当たり判定.
bool CStaticMeshObject::IsHitForRay(
	const RAY& pRay,			//レイ構造体.
	float* pfDistance,			//(out)距離.
	D3DXVECTOR3* pvIntersect )	//(out)交差点.
{
	D3DXVECTOR3 vAxis;		//軸ベクトル.
	D3DXMATRIX	mRotationY;	//Y軸回転行列.

	//Y軸回転行列.
	D3DXMatrixRotationY( &mRotationY, pRay.RotationY );
	//Y軸回転行列を使って軸ベクトルの座標変換する.
	D3DXVec3TransformCoord( &vAxis, &pRay.Axis, &mRotationY );

	//レイの始点と終点.
	D3DXVECTOR3 StartPoint, EndPoint;
	StartPoint	= pRay.Position;
	EndPoint	= pRay.Position + ( vAxis * pRay.Length );	//レイの終点を設定.

	//レイを当てたいメッシュが移動している場合でも、
	//対象のWorld行列の逆行列を用いれば正しくレイが当たる.
	D3DXMATRIX mWorld, mInverseWorld;
	//移動処理.
	D3DXMATRIX mTran;
	D3DXMatrixTranslation(
		&mTran,
		m_vPosition.x, m_vPosition.y, m_vPosition.z);

	//回転処理.
	//※この行列計算は「CStaicMesh::Render()関数」と同じにする必要あり.
	D3DXMATRIX mRot, mYaw, mPitch, mRoll;
	//Y軸回転行列作成.
	D3DXMatrixRotationY(&mYaw, m_vRotation.y);
	//X軸回転行列作成.
	D3DXMatrixRotationX(&mPitch, m_vRotation.x);
	//Z軸回転行列作成.
	D3DXMatrixRotationZ(&mRoll, m_vRotation.z);
	//回転行列作成.
	mRot = mYaw * mPitch * mRoll;

	//拡縮処理.
	D3DXMATRIX mScale;
	D3DXMatrixScaling(&mScale, m_vScale.x, m_vScale.y, m_vScale.z);

	//ワールド行列計算.
	//拡縮×回転×移動 ※順番がとても大切！
	mWorld = mScale * mRot * mTran;

	//逆行列を求める.
	D3DXMatrixInverse( &mInverseWorld, nullptr, &mWorld );
	//レイの始点、終点に反映.
	D3DXVec3TransformCoord( &StartPoint, &StartPoint, &mInverseWorld );
	D3DXVec3TransformCoord( &EndPoint, &EndPoint, &mInverseWorld );

	//向きと長さ（大きさ）を求める.
	D3DXVECTOR3 vDirection = EndPoint - StartPoint;

	BOOL bHit = FALSE;		//命中フラグ.
	DWORD dwIndex = 0;		//インデックス番号.
	D3DXVECTOR3 Vertex[3];	//頂点座標.
	FLOAT U = 0, V = 0;		//重心ヒット座標.

	//メッシュとレイの交差を調べる.
	D3DXIntersect(
		m_pMesh->GetMeshForRay(),	//対象メッシュ.
		&StartPoint,				//レイの始点.
		&vDirection,				//レイの向きと長さ（大きさ）.
		&bHit,						//(out)判定結果.
		&dwIndex,//(out)bHitがTRUE時にレイの始点に最も近くの面のインデックス値へのポインタ.					
		&U, &V,						//(out)重心ヒット座標.
		pfDistance,					//(out)メッシュとの距離.
		nullptr, nullptr);

	//無限に伸びるレイのどこかでメッシュが当たっていたら.
	if (bHit == TRUE)
	{
		//命中した時.
		FindVerticesOnPoly(
			m_pMesh->GetMeshForRay(), dwIndex, Vertex);

		//重心座標から交点を算出.
		//ローカル交点は v0 + U*(v1-v0) + V*(v2-v0) で求まる.
		*pvIntersect =
			Vertex[0] + U * (Vertex[1] - Vertex[0]) + V * (Vertex[2] - Vertex[0]);

		//モデルデータが「拡縮」「回転」「移動」していれば行列が必要.
		D3DXVec3TransformCoord( pvIntersect, pvIntersect, &mWorld );

		//EndPointから見た距離で1.0fより小さければ当たっている.
		if (*pfDistance < 1.0f)
		{
			return true;	//命中している.
		}
	}

	return false;	//外れている.
}

//壁からの位置を計算する.
void CStaticMeshObject::CalculatePositionFromWall(CROSSRAY* pCrossRay)
{
	FLOAT Distance[CROSSRAY::max];			//各レイごとの距離.
	D3DXVECTOR3 Intersect[CROSSRAY::max];	//各レイごとの交差点.

	//レイの向きにより当たる壁までの距離を求める.軸ベクトル（前後左右）.
	for (int i = 0; i < CROSSRAY::max; i++)
	{
		IsHitForRay(pCrossRay->Ray[i], &Distance[i], &Intersect[i]);
	}

	float RotY;	//Y軸回転値.
	//回転情報は全てのレイで同じはずなのでXLを使用する.
	RotY = fabsf( pCrossRay->Ray[CROSSRAY::XL].RotationY );//fabsf関数:絶対値(float版).
	ClampDirection( &RotY );	//0～2πの間に収める.

	//----------------------------
	//	定数宣言.
	//----------------------------
	static constexpr float WSPACE = 0.8f;	//壁との限界距離.
	static constexpr float DEG45  = D3DXToRadian(  45.0f );	//0.785f.
	static constexpr float DEG135 = D3DXToRadian( 135.0f );	//2.355f.
	static constexpr float DEG225 = D3DXToRadian( 225.0f );	//3.925f.
	static constexpr float DEG315 = D3DXToRadian( 315.0f );	//5.496f.

	float Dis = 0.0f;
	float TrgRotY = pCrossRay->Ray[CROSSRAY::XL].RotationY;
	D3DXVECTOR3 TrgPos = pCrossRay->Ray[CROSSRAY::XL].Position;

	//前が壁に接近.
	Dis = Distance[CROSSRAY::ZF];
	if (0.01f < Dis && Dis < WSPACE) {
		//時計回り.
		if (TrgRotY < 0.0f)
		{
			//右から.
			if (DEG45 <= RotY && RotY < DEG135)			{ TrgPos.x += WSPACE - Dis; }
			//前から.
			else if(DEG135 <= RotY && RotY < DEG225)	{ TrgPos.z += WSPACE - Dis; }
			//左から.
			else if (DEG225 <= RotY && RotY < DEG315)	{ TrgPos.x -= WSPACE - Dis; }
			//奥から.
			else										{ TrgPos.z -= WSPACE - Dis; }
		}
		//反時計回り.
		else
		{
			//右から.
			if (DEG45 <= RotY && RotY < DEG135)			{ TrgPos.x -= WSPACE - Dis; }
			//前から.
			else if(DEG135 <= RotY && RotY < DEG225)	{ TrgPos.z += WSPACE - Dis; }
			//左から.
			else if (DEG225 <= RotY && RotY < DEG315)	{ TrgPos.x += WSPACE - Dis; }
			//奥から.
			else										{ TrgPos.z -= WSPACE - Dis; }
		}
	}

	//後ろが壁に接近.
	Dis = Distance[CROSSRAY::ZB];
	if (0.01f < Dis && Dis < WSPACE) {
		//時計回り.
		if (TrgRotY < 0.0f)
		{
			//右から.
			if (DEG45 <= RotY && RotY < DEG135)			{ TrgPos.x -= WSPACE - Dis; }
			//前から.
			else if(DEG135 <= RotY && RotY < DEG225)	{ TrgPos.z -= WSPACE - Dis; }
			//左から.
			else if (DEG225 <= RotY && RotY < DEG315)	{ TrgPos.x += WSPACE - Dis; }
			//奥から.
			else										{ TrgPos.z += WSPACE - Dis; }
		}
		//反時計回り.
		else
		{
			//右から.
			if (DEG45 <= RotY && RotY < DEG135)			{ TrgPos.x += WSPACE - Dis; }
			//前から.
			else if(DEG135 <= RotY && RotY < DEG225)	{ TrgPos.z -= WSPACE - Dis; }
			//左から.
			else if (DEG225 <= RotY && RotY < DEG315)	{ TrgPos.x -= WSPACE - Dis; }
			//奥から.
			else										{ TrgPos.z += WSPACE - Dis; }
		}
	}

	//右が壁に接近.
	Dis = Distance[CROSSRAY::XR];
	if (0.01f < Dis && Dis < WSPACE) {
		//時計回り.
		if (TrgRotY < 0.0f)
		{
			//右から.
			if (DEG45 <= RotY && RotY < DEG135)			{ TrgPos.z -= WSPACE - Dis; }
			//前から.
			else if(DEG135 <= RotY && RotY < DEG225)	{ TrgPos.x += WSPACE - Dis; }
			//左から.
			else if (DEG225 <= RotY && RotY < DEG315)	{ TrgPos.z += WSPACE - Dis; }
			//奥から.
			else										{ TrgPos.x -= WSPACE - Dis; }
		}
		//反時計回り.
		else
		{
			//右から.
			if (DEG45 <= RotY && RotY < DEG135)			{ TrgPos.z += WSPACE - Dis; }
			//前から.
			else if(DEG135 <= RotY && RotY < DEG225)	{ TrgPos.x += WSPACE - Dis; }
			//左から.
			else if (DEG225 <= RotY && RotY < DEG315)	{ TrgPos.z -= WSPACE - Dis; }
			//奥から.
			else										{ TrgPos.x -= WSPACE - Dis; }
		}
	}

	//左が壁に接近.
	Dis = Distance[CROSSRAY::XL];
	if (0.01f < Dis && Dis < WSPACE) {
		//時計回り.
		if (TrgRotY < 0.0f)
		{
			//右から.
			if (DEG45 <= RotY && RotY < DEG135)			{ TrgPos.z += WSPACE - Dis; }
			//前から.
			else if(DEG135 <= RotY && RotY < DEG225)	{ TrgPos.x -= WSPACE - Dis; }
			//左から.
			else if (DEG225 <= RotY && RotY < DEG315)	{ TrgPos.z -= WSPACE - Dis; }
			//奥から.
			else										{ TrgPos.x += WSPACE - Dis; }
		}
		//反時計回り.
		else
		{
			//右から.
			if (DEG45 <= RotY && RotY < DEG135)			{ TrgPos.z -= WSPACE - Dis; }
			//前から.
			else if(DEG135 <= RotY && RotY < DEG225)	{ TrgPos.x -= WSPACE - Dis; }
			//左から.
			else if (DEG225 <= RotY && RotY < DEG315)	{ TrgPos.z += WSPACE - Dis; }
			//奥から.
			else										{ TrgPos.x += WSPACE - Dis; }
		}
	}

	//最終的な座標をレイに返す.
	for (int dir = 0; dir < CROSSRAY::max; dir++) {
		pCrossRay->Ray[dir].Position = TrgPos;
	}
}

//交差位置のポリゴンの頂点を見つける.
//※頂点座標はローカル座標.
HRESULT CStaticMeshObject::FindVerticesOnPoly(
	LPD3DXMESH pMesh,
	DWORD dwPolyIndex,
	D3DXVECTOR3* pVertices)
{
	//頂点ごとのバイト数を取得.
	DWORD dwStride = pMesh->GetNumBytesPerVertex();
	//頂点数を取得.
	DWORD dwVertexAmt = pMesh->GetNumVertices();
	//面数を取得.
	DWORD dwPoly = pMesh->GetNumFaces();

	WORD* pwPoly = nullptr;

	//インデックスバッファをロック（読み込みモード）.
	pMesh->LockIndexBuffer(
		D3DLOCK_READONLY,
		reinterpret_cast<VOID**>(&pwPoly));

	BYTE* pbVertices = nullptr;	//頂点(バイト型).
	FLOAT* pfVertices = nullptr;//頂点(float型).
	LPDIRECT3DVERTEXBUFFER9 VB = nullptr;	//頂点バッファ.

	//頂点情報の取得.
	pMesh->GetVertexBuffer(&VB);

	//頂点バッファのロック.
	if (SUCCEEDED(
		VB->Lock(0, 0, reinterpret_cast<VOID**>(&pbVertices), 0)))
	{
		//ポリゴンの頂点１つ目を取得.
		pfVertices =
			reinterpret_cast<FLOAT*>(&pbVertices[dwStride * pwPoly[dwPolyIndex * 3]]);
		pVertices[0].x = pfVertices[0];
		pVertices[0].y = pfVertices[1];
		pVertices[0].z = pfVertices[2];

		//ポリゴンの頂点２つ目を取得.
		pfVertices =
			reinterpret_cast<FLOAT*>(&pbVertices[dwStride * pwPoly[dwPolyIndex * 3 + 1]]);
		pVertices[1].x = pfVertices[0];
		pVertices[1].y = pfVertices[1];
		pVertices[1].z = pfVertices[2];

		//ポリゴンの頂点３つ目を取得.
		pfVertices =
			reinterpret_cast<FLOAT*>(&pbVertices[dwStride * pwPoly[dwPolyIndex * 3 + 2]]);
		pVertices[2].x = pfVertices[0];
		pVertices[2].y = pfVertices[1];
		pVertices[2].z = pfVertices[2];

		pMesh->UnlockIndexBuffer();	//ロック解除.
		VB->Unlock();	//ロック解除.
	}
	VB->Release();	//不要になったので解放.

	return S_OK;
}

//回転値調整（１周以上している時の調整）.
void CStaticMeshObject::ClampDirection(float* dir)
{
	if (*dir > D3DX_PI * 2.0f) {
		//１周以上している.
		*dir -= D3DX_PI * 2.0f;	//2π(360°)分引く.
	}

	if (*dir > D3DX_PI * 2.0f) {
		//再帰関数.
		ClampDirection( dir );
	}
}
