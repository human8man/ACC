#pragma once

#include <array>
#include <tuple>


//=============================================================================
//		GJKクラス(↓参考サイト↓)
//			https://winter.dev/
//=============================================================================


//-----------------------------------------------------------------------------
//		コライダー構造体
//-----------------------------------------------------------------------------
struct Collider 
{
public :
	/****************************************************************
	// @brief	指定した方向に対して最も遠い頂点を求める関数
	// @param	Direction : 探索する方向ベクトル
	// @return	指定方向における最も遠い頂点の座標
	****************************************************************/
	virtual D3DXVECTOR3 FindFurthestPoint(D3DXVECTOR3 Direction) const final;


	/****************************************************************
	// @brief			頂点情報をワールド座標に変換してセットする
	// @param	obj		: オブジェクトの情報
	// @param	vertex	: ローカル座標系の頂点リスト
	****************************************************************/
	virtual void SetVertex(	const ObjectInfo& object, const std::vector<D3DXVECTOR3>& vertex) final;

	//-----------------------------------------------------------------------------
	// @brief コライダーの頂点の重心（中心）を取得する関数
	//-----------------------------------------------------------------------------
	virtual D3DXVECTOR3 GetCenter() const;

protected:
	// コライダーの頂点リスト
	std::vector<D3DXVECTOR3> m_Vertices;
};


//=============================================================================
//		シンプレックス (Simplex) クラス
//=============================================================================
struct Simplex
{
public:
	Simplex()
		: m_Size ( 0 )
	{}


	// 初期化リストから値を設定
	Simplex& operator = (std::initializer_list<D3DXVECTOR3> list)
	{
		m_Size = min(static_cast<int>(list.size()), 4);
		std::copy_n(list.begin(), m_Size, m_Points.begin());
		return *this;
	}


	// 先頭に新しい点を追加
	void Push_Front(const D3DXVECTOR3& point)
	{
		std::rotate(m_Points.rbegin(), m_Points.rbegin() + 1, m_Points.rend());
		m_Points[0] = point;
		m_Size = min(m_Size + 1, 4);
	}


	// 添字演算子
	D3DXVECTOR3& operator[](int i) { return m_Points[i]; }
	// シンプレックスのサイズを取得
	size_t Size() const { return m_Size; }
	// イテレータ取得
	std::array<D3DXVECTOR3, 4>::const_iterator Begin() const { return m_Points.begin(); }
	std::array<D3DXVECTOR3, 4>::const_iterator End() const { return m_Points.begin() + m_Size; }


private:
	int m_Size;
	//	頂点座標の保存用配列
	std::array<D3DXVECTOR3, 4> m_Points;
};


//=============================================================================
//		衝突点構造体
//=============================================================================
struct CollisionPoints {
	D3DXVECTOR3 Normal	= ZEROVEC3;
	float  Depth	= 0.0f;
	bool   Col		= false;
};


//=============================================================================
//		GJKクラス
//=============================================================================
class GJK
{
public:
	/****************************************************************
	// @brief	二つのコライダーと方向からミンコフスキー差のサポート点を計算する
	// @param	ColliderA : コライダーA
	// @param	ColliderB : コライダーB
	// @param	Direction : 探索する方向ベクトル
	// @return	サポート点 (最も遠い点の座標)
	****************************************************************/
	D3DXVECTOR3 Support(const Collider& ColliderA, const Collider& ColliderB, D3DXVECTOR3 Direction);


	/****************************************************************
	// @brief	GJKアルゴリズムを用いて二つのコライダーの衝突を判定する
	// @param	ColliderA : コライダーA
	// @param	ColliderB : コライダーB
	// @return	衝突情報 (衝突の有無や衝突点など)
	****************************************************************/
	CollisionPoints GJKC(const Collider& ColliderA, const Collider& ColliderB);


	/****************************************************************
	// @brief	EPA (Expanding Polytope Algorithm) により衝突情報を補完する
	// @param	simplex		: GJK によって生成された単体 (シンプレックス)
	// @param	colliderA	: コライダーA
	// @param	colliderB	: コライダーB
	// @return	衝突情報 (衝突深度や衝突法線)
	****************************************************************/
	CollisionPoints EPA(const Simplex& simplex, const Collider& colliderA, const Collider& colliderB);


	/****************************************************************
	// @brief	ポリトープ (多面体) から各面の法線ベクトルを計算する
	// @param	Poly	: 頂点リスト
	// @param	Faces	: 面のインデックスリスト
	// @return	面の法線ベクトルリストと最も近い面のインデックス
	****************************************************************/
	std::tuple<std::vector<D3DXVECTOR4>, size_t> GetFaceNormals(
		const std::vector<D3DXVECTOR3>& Poly,
		const std::vector<size_t>& Faces);


	/****************************************************************
	// @brief	エッジの逆方向がリストに存在するか確認し、重複があれば削除する
	// @param	Edges	: エッジリスト
	// @param	Faces	: 面のインデックスリスト
	// @param	a		: エッジの始点
	// @param	b		: エッジの終点
	****************************************************************/
	void AddIfUniqueEdge(
		std::vector<std::pair<size_t, size_t>>& Edges,
		const std::vector<size_t>& Faces, size_t a, size_t b);


	/****************************************************************
	// @brief	与えられた頂点群の中で最も低いY座標の頂点の位置を返す
	// @param	obj		: オブジェクトの情報
	// @param	vertex	: 頂点リスト (ローカル座標)
	// @return	最も低い頂点のY座標値
	****************************************************************/
	float SendMinVertexPosY( const ObjectInfo& obje, const std::vector<D3DXVECTOR3>& vertex);


	/****************************************************************
	// @brief	シンプレックスを拡張し、次の探索方向を決定する
	// @param	points		: 現在のシンプレックス (頂点集合)
	// @param	Direction	: 更新後の探索方向
	// @return	GJKの終了条件を満たした場合 true
	****************************************************************/
	bool NextSimplex(Simplex& points, D3DXVECTOR3& Direction);


	/****************************************************************
	// @brief	二つのベクトルが同じ方向を向いているか判定する
	// @param	Direction	: 方向ベクトル
	// @param	ao			: 判定するベクトル
	// @return	同じ方向なら true
	****************************************************************/
	bool SameDirection(const D3DXVECTOR3& Direction, const D3DXVECTOR3& ao);


	/****************************************************************
	// @brief	2点 (線分) のシンプレックスを処理し、次の探索方向を決定する
	// @param	points		: 現在のシンプレックス (2点)
	// @param	Direction	: 更新後の探索方向
	// @return	GJKの終了条件を満たした場合 true
	****************************************************************/
	bool Line(Simplex& points, D3DXVECTOR3& Direction);


	/****************************************************************
	// @brief	3点 (三角形) のシンプレックスを処理し、次の探索方向を決定する
	// @param	points		: 現在のシンプレックス (3点)
	// @param	Direction	: 更新後の探索方向
	// @return	GJKの終了条件を満たした場合 true
	****************************************************************/
	bool Triangle(Simplex& points, D3DXVECTOR3& Direction);


	/****************************************************************
	// @brief	4点 (四面体) のシンプレックスを処理し、次の探索方向を決定する
	// @param	points		: 現在のシンプレックス (4点)
	// @param	Direction	: 更新後の探索方向
	// @return	GJKの終了条件を満たした場合 true
	****************************************************************/
	bool Tetrahedron(Simplex& points, D3DXVECTOR3& Direction);
};