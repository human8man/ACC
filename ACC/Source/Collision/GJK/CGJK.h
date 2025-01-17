#pragma once
#include <array>
#include <tuple>

//===========================================================
//	GJK 参考にしたサイト
//  https://winter.dev/
//===========================================================


//コライダー構造体
struct Collider {
	//特定の方向のサポートポイントを見つける関数
	virtual D3DXVECTOR3 FindFurthestPoint(D3DXVECTOR3 Direction) const = 0;
};

//メッシュの衝突判定
struct MeshCollider : Collider
{
public:
	D3DXVECTOR3 FindFurthestPoint(D3DXVECTOR3 Direction) const override;

	// 頂点情報をワールド座標に変換してセット
	void SetVertex(
		D3DXVECTOR3 pos,
		D3DXVECTOR3 rot, 
		D3DXVECTOR3 scale, 
		std::vector<D3DXVECTOR3> vertex);

private:
	//頂点
	std::vector<D3DXVECTOR3> m_Vertices;

};

//箱
struct BoxCollider : Collider
{
public:
	D3DXVECTOR3 FindFurthestPoint(D3DXVECTOR3 Direction) const override;

	// 頂点情報をワールド座標に変換してセット
	void SetVertex(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, std::vector<D3DXVECTOR3> vertex);
private:
	//頂点
	std::vector<D3DXVECTOR3> m_Vertices;
};

//シンプレックス
struct Simplex
{
public:
	Simplex()
		: m_Size ( 0 )
	{}

	Simplex& operator = (std::initializer_list<D3DXVECTOR3> list)
	{
		m_Size = 0;

		for (D3DXVECTOR3 Point : list)
		{
			m_Points[m_Size++] = Point;
		}

		return *this;
	}

	void Push_Front(D3DXVECTOR3 point)
	{
		m_Points = { point,m_Points[0],m_Points[1],m_Points[2] };
		m_Size = min(m_Size + 1,4);
	}

	D3DXVECTOR3& operator[](int i) { return m_Points[i]; }
	size_t Size() const { return m_Size; }

	auto Begin() const { return m_Points.begin(); }
	auto End() const { return m_Points.end() - (4 - m_Size); }

private:
	std::array<D3DXVECTOR3, 4> m_Points;
	int m_Size;
};

//衝突点構造体
struct CollisionPoints {
	D3DXVECTOR3 Normal	= ZEROVEC3;
	float  Depth	= 0.0f;
	bool   Col		= false;
};

//================================================================
//		GJKクラス.
//================================================================
class CGJK
{
public:
	//二つのコライダーと方向でミンコフスキー差の頂点を返す
	D3DXVECTOR3 Support(const Collider& ColliderA, const Collider& ColliderB, D3DXVECTOR3 Direction);

	CollisionPoints GJK(const Collider& ColliderA, const Collider& ColliderB);

	CollisionPoints EPA(
		const Simplex& simplex,
		const Collider& colliderA,
		const Collider& colliderB);

	//インデックスから頂点を取得
	std::tuple<std::vector<D3DXVECTOR4>, size_t> GetFaceNormals(
		const std::vector<D3DXVECTOR3>& Poly, 
		const std::vector<size_t>& Faces);

	//エッジの逆方向がリストに存在するか確認し、存在する場合削除
	void AddIfUniqueEdge(
		std::vector<std::pair<size_t,size_t>>& Edges, //エッジ
		const std::vector<size_t>& Faces,			  //インデックス
		size_t a,									  
		size_t b);									  

	//送られた頂点情報で一番低い位置にある頂点の位置情報を送る
	float SendMinVertexPosY(D3DXVECTOR3 _Pos, D3DXVECTOR3 _Rot, D3DXVECTOR3 _Scale, std::vector<D3DXVECTOR3> vertex);

	bool NextSimplex(Simplex& points, D3DXVECTOR3& Direction);

	bool SameDirection(const D3DXVECTOR3& Direction,const D3DXVECTOR3& ao);

	bool Line(Simplex& points, D3DXVECTOR3& Direction);

	bool Triangle(Simplex& points, D3DXVECTOR3& Direction);

	bool Tetrahedron(Simplex& points, D3DXVECTOR3& Direction);
};