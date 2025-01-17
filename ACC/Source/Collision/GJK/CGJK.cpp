#include "CGJK.h"

#include "GameObject/CGameObject.h"

#if _DEBUG
#include "ImGui/CImGui.h"
#endif
D3DXVECTOR3 MeshCollider::FindFurthestPoint(D3DXVECTOR3 Direction) const
{
	D3DXVECTOR3 MaxPoint;
	float MaxDistance = -FLT_MAX;

	for (D3DXVECTOR3 Vertex : m_Vertices)
	{
		float Distance = D3DXVec3Dot(&Vertex, &Direction);
		if (Distance > MaxDistance)
		{
			MaxDistance = Distance;
			MaxPoint = Vertex;
		}
	}

	return MaxPoint;
}

void MeshCollider::SetVertex(
	D3DXVECTOR3 pos,
	D3DXVECTOR3 rot, 
	D3DXVECTOR3 scale, 
	std::vector<D3DXVECTOR3> vertex)
{
	D3DXVECTOR3 Pos = pos;
	D3DXVECTOR3 Rot = rot;
	D3DXVECTOR3 Scale = scale;

	D3DXMATRIX mTrans, mRot, mScale;

	//拡大縮小行列.
	D3DXMatrixScaling(&mScale,
		Scale.x, Scale.y, Scale.z);

	//回転行列.
	D3DXMATRIX mYaw, mPitch, mRoll;
	D3DXMatrixRotationY(&mYaw, Rot.y);
	D3DXMatrixRotationX(&mPitch, Rot.x);
	D3DXMatrixRotationZ(&mRoll, Rot.z);
	mRot = mYaw * mPitch * mRoll;

	//平行行列（平行移動）.
	D3DXMatrixTranslation(&mTrans,
		Pos.x, Pos.y, Pos.z);

	//ワールド行列変換
	D3DXMATRIX mWorld = mScale * mRot * mTrans;

	//サイズ変更
	m_Vertices = vertex;

	//頂点情報をセット
	for (int i = 0; i < vertex.size(); i++)
	{
		m_Vertices[i] = vertex[i];
		D3DXVec3TransformCoord(&m_Vertices[i], &m_Vertices[i], &mWorld);
	}
}

D3DXVECTOR3 BoxCollider::FindFurthestPoint(D3DXVECTOR3 Direction) const
{
	D3DXVECTOR3 MaxPoint;
	float MaxDistance = -FLT_MAX;

	for (D3DXVECTOR3 Vertex : m_Vertices)
	{
		float Distance = D3DXVec3Dot(&Vertex, &Direction);
		if (Distance > MaxDistance)
		{
			MaxDistance = Distance;
			MaxPoint = Vertex;
		}
	}

	return MaxPoint;
}

//頂点情報をワールド座標に変換してセット
void BoxCollider::SetVertex(
	D3DXVECTOR3 pos, 
	D3DXVECTOR3 rot, 
	D3DXVECTOR3 scale,
	std::vector<D3DXVECTOR3> vertex)
{
	D3DXVECTOR3 Pos   = pos;
	D3DXVECTOR3 Rot   = rot;
	D3DXVECTOR3 Scale = scale;

	D3DXMATRIX mTrans, mRot, mScale;

	//拡大縮小行列.
	D3DXMatrixScaling(&mScale,
		Scale.x, Scale.y, Scale.z);

	//回転行列.
	D3DXMATRIX mYaw, mPitch, mRoll;
	D3DXMatrixRotationY(&mYaw, Rot.y);
	D3DXMatrixRotationX(&mPitch, Rot.x);
	D3DXMatrixRotationZ(&mRoll, Rot.z);
	mRot = mYaw * mPitch * mRoll;

	//平行行列（平行移動）.
	D3DXMatrixTranslation(&mTrans,
		Pos.x, Pos.y, Pos.z);

	//ワールド行列変換
	D3DXMATRIX mWorld = mScale * mRot * mTrans;

	//サイズ変更
	m_Vertices.resize(vertex.size());

	//頂点情報をセット
	for (int i = 0; i < vertex.size(); i++)
	{
		m_Vertices[i] = vertex[i];
		D3DXVec3TransformCoord(&m_Vertices[i], &m_Vertices[i], &mWorld);
	}
}

//======================================================================================
//		二つのコライダーと方向でミンコフスキー差の頂点を返す
//======================================================================================
D3DXVECTOR3 CGJK::Support(const Collider& ColliderA, const Collider& ColliderB, D3DXVECTOR3 Direction)
{
	return ColliderA.FindFurthestPoint(Direction) - ColliderB.FindFurthestPoint(-Direction);
}


//======================================================================================
// 
//======================================================================================
CollisionPoints CGJK::GJK(const Collider& ColliderA, const Collider& ColliderB)
{
	CollisionPoints outPoints;

	D3DXVECTOR3 sup = Support(ColliderA, ColliderB, D3DXVECTOR3(1.0f, 0.0f, 0.0f));
	//sup = sup + D3DXVECTOR3(0.001f, 0.001f, 0.001f);

	Simplex points;
	points.Push_Front(sup);

	D3DXVECTOR3 Direction = -sup;

	int count = 0;

	while (true)
	{
		if (count >= 100)
		{
			return outPoints;
		}
		sup = Support(ColliderA, ColliderB, Direction);
		//sup = sup + D3DXVECTOR3(0.001f, 0.001f, 0.001f);

		if (D3DXVec3Dot(&sup, &Direction) <= 0.0f) {
			return outPoints;
		}

		points.Push_Front(sup);

		if (NextSimplex(points, Direction)) {
			return EPA(points, ColliderA, ColliderB);
		}

		count++;
	}

	return outPoints;
}


//======================================================================================
//		EPA
//======================================================================================
CollisionPoints CGJK::EPA(
	const Simplex& simplex, 
	const Collider& colliderA,
	const Collider& colliderB)
{
	std::vector<D3DXVECTOR3> Polytope(simplex.Begin(),simplex.End());
	std::vector<size_t> Faces = {
		0, 1, 2,
		0, 3, 1,
		0, 2, 3,
		1, 3, 2
	};

	auto result = GetFaceNormals(Polytope,Faces);

	auto normals = std::get<0>(result);
	auto minFace = std::get<1>(result);

	D3DXVECTOR3 minNormal;
	float minDistance = FLT_MAX;

	//while文を抜ける用
	int count = 0;

	while (minDistance == FLT_MAX)
	{
		//100回回ったら終了
		if (count >= 100)
		{
			break;
		}

		minNormal.x = normals[minFace].x;
		minNormal.y = normals[minFace].y;
		minNormal.z = normals[minFace].z;
		minDistance = normals[minFace].w;

		D3DXVECTOR3 support = Support(colliderA,colliderB,minNormal);
		float sDistance = D3DXVec3Dot(&minNormal,&support);

		if (abs(sDistance - minDistance) > 0.001f)
		{
			minDistance = FLT_MAX;

			std::vector<std::pair<size_t, size_t>> uniqueEdges;

			for(size_t i = 0; i < normals.size(); i++)
			{
				D3DXVECTOR3 normals_xyz = D3DXVECTOR3(normals[i].x, normals[i].y, normals[i].z);
				if (SameDirection(normals_xyz, support))
				{
					size_t f = i * 3;

					AddIfUniqueEdge(uniqueEdges,Faces,f,    f + 1);
					AddIfUniqueEdge(uniqueEdges,Faces,f + 1,f + 2);
					AddIfUniqueEdge(uniqueEdges,Faces,f + 2,f    );

					Faces[f + 2] = Faces.back(); Faces.pop_back();
					Faces[f + 1] = Faces.back(); Faces.pop_back();
					Faces[f    ] = Faces.back(); Faces.pop_back();

					normals[i] = normals.back();
					normals.pop_back();

					i--;
				}
			}

			std::vector<size_t> newFaces;

			for (auto Edge : uniqueEdges)
			{
				size_t EdgeIndex1 = Edge.first;
				size_t EdgeIndex2 = Edge.second;

				newFaces.push_back(EdgeIndex1);
				newFaces.push_back(EdgeIndex2);
				newFaces.push_back(Polytope.size());
			}

			Polytope.push_back(support);

			auto result3 = GetFaceNormals(Polytope,newFaces);

			auto newNormals = std::get<0>(result3);
			auto newMinFace = std::get<1>(result3);

			float OldMinDistance = FLT_MAX;
			for (size_t i = 0; i < normals.size(); i++)
			{
				if (normals[i].w < OldMinDistance) {
					OldMinDistance = normals[i].w;
					minFace = i;
				}
			}

			if (newNormals[newMinFace].w < OldMinDistance) {
				minFace = newMinFace + normals.size();
			}

			Faces  .insert(Faces  .end(), newFaces  .begin(), newFaces  .end());
			normals.insert(normals.end(), newNormals.begin(), newNormals.end());
		}

		count++;
	}

	//float最大値が入っていたら
	if (minDistance == FLT_MAX)
	{
		minDistance = 0.001f;
	}

	CollisionPoints points;

	points.Normal = minNormal;
	points.Depth = minDistance + 0.001f;
	points.Col = true;

	return points;
}


//======================================================================================
// 
//======================================================================================
std::tuple<std::vector<D3DXVECTOR4>, size_t> CGJK::GetFaceNormals(
	const std::vector<D3DXVECTOR3>& Poly,
	const std::vector<size_t>& Faces)
{
	std::vector<D3DXVECTOR4> Normals;
	size_t minTriangle = 0;
	float minDistance = FLT_MAX;

	for (size_t i = 0; i < Faces.size(); i += 3)
	{
		D3DXVECTOR3 a = Poly[Faces[i    ]];
		D3DXVECTOR3 b = Poly[Faces[i + 1]];
		D3DXVECTOR3 c = Poly[Faces[i + 2]];

		D3DXVECTOR3 Normal;
		D3DXVECTOR3 b_a = b - a;
		D3DXVECTOR3 c_a = c - a;
		D3DXVECTOR3 CrossABC;
		D3DXVec3Cross(&CrossABC,&b_a,&c_a);
		D3DXVec3Normalize(&Normal,&CrossABC);
		float Distance = D3DXVec3Dot(&Normal,&a);

		if (Distance < 0) {
			Normal *= -1;
			Distance *= -1;
		}

		Normals.emplace_back(Normal,Distance);

		if (Distance < minDistance) {
			minTriangle = i / 3;
			minDistance = Distance;
		}
	}

	return { Normals,minTriangle };
}


//======================================================================================
//		エッジの逆方向がリストに存在するか確認し、存在する場合削除.
//======================================================================================
void CGJK::AddIfUniqueEdge(
	std::vector<std::pair<size_t, size_t>>& Edges, //エッジ
	const std::vector<size_t>& Faces,			  //インデックス
	size_t a,
	size_t b)
{
	auto Reverse = std::find(
		Edges.begin(),
		Edges.end(),
		std::make_pair(Faces[b], Faces[a]));

	if (Reverse != Edges.end()) {
		Edges.erase(Reverse);
	}
	else {
		Edges.emplace_back(Faces[a],Faces[b]);
	}
}


//======================================================================================
//		送られた頂点情報で一番低い位置にある頂点の位置情報を送る
//======================================================================================
float CGJK::SendMinVertexPosY(D3DXVECTOR3 _Pos, D3DXVECTOR3 _Rot, D3DXVECTOR3 _Scale, std::vector<D3DXVECTOR3> vertex)
{
	std::vector<D3DXVECTOR3> Vertex;
	float MinPosY = FLT_MAX;

	D3DXVECTOR3 Pos = _Pos;
	D3DXVECTOR3 Rot = _Rot;
	D3DXVECTOR3 Scale = _Scale;

	D3DXMATRIX mTrans, mRot, mScale;

	//拡大縮小行列.
	D3DXMatrixScaling(&mScale,
		Scale.x, Scale.y, Scale.z);

	//回転行列.
	D3DXMATRIX mYaw, mPitch, mRoll;
	D3DXMatrixRotationY(&mYaw, Rot.y);
	D3DXMatrixRotationX(&mPitch, Rot.x);
	D3DXMatrixRotationZ(&mRoll, Rot.z);
	mRot = mYaw * mPitch * mRoll;
	//※Yaw, Pitch, Roll の掛ける順番を変えると結果も変わる.

	//平行行列（平行移動）.
	D3DXMatrixTranslation(&mTrans,
		Pos.x, Pos.y, Pos.z);

	//ワールド行列変換
	D3DXMATRIX mWorld = mScale * mRot * mTrans;

	//サイズ変更
	Vertex.resize(vertex.size());

	//頂点情報をセット
	for (int i = 0; i < vertex.size(); i++)
	{
		Vertex[i] = vertex[i];
		D3DXVec3TransformCoord(&Vertex[i], &Vertex[i], &mWorld);
		if (MinPosY > Vertex[i].y) {
			MinPosY = Vertex[i].y;
		}
	}

	return MinPosY;
}


//======================================================================================
// 
//======================================================================================
bool CGJK::NextSimplex(Simplex& points, D3DXVECTOR3& Direction)
{
	switch (points.Size())
	{
	case 2: return Line        ( points, Direction );
	case 3: return Triangle	   ( points, Direction );
	case 4: return Tetrahedron ( points, Direction );
	}

	return false;
}


//======================================================================================
// 
//======================================================================================
bool CGJK::SameDirection(const D3DXVECTOR3& Direction, const D3DXVECTOR3& ao)
{
	return D3DXVec3Dot(&Direction,&ao) > 0.0f;
}


//======================================================================================
// 
//======================================================================================
bool CGJK::Line(Simplex& points, D3DXVECTOR3& Direction)
{
	D3DXVECTOR3 a = points[0];
	D3DXVECTOR3 b = points[1];

	D3DXVECTOR3 ab = b - a;
	D3DXVECTOR3 ao = -a;

	if (SameDirection(ab, ao)) {
		D3DXVECTOR3 C;
		D3DXVec3Cross(&C,&ab, &ao);
		D3DXVec3Cross(&Direction,&C, &ab);
	}
	else
	{
		points = { a };
		Direction = ao;
	}

	return false;
}


//======================================================================================
// 
//======================================================================================
bool CGJK::Triangle(Simplex& points, D3DXVECTOR3& Direction)
{
	D3DXVECTOR3 a = points[0];
	D3DXVECTOR3 b = points[1];
	D3DXVECTOR3 c = points[2];
	
	D3DXVECTOR3 ab = b - a;
	D3DXVECTOR3 ac = c - a;
	D3DXVECTOR3 ao = -a;

	D3DXVECTOR3 abc = ZEROVEC3;
	D3DXVec3Cross(&abc,&ab,&ac);

	D3DXVECTOR3 abc_ac;
	D3DXVec3Cross(&abc_ac,&abc, &ac);
	if (SameDirection(abc_ac, ao))
	{
		if (SameDirection(ac, ao))
		{
			points = { a, c };
			D3DXVECTOR3 ac_ao;
			D3DXVec3Cross(&ac_ao,&ac,&ao);
			D3DXVec3Cross(&Direction,&ac_ao,&ac);
		}
		else
		{
			return Line(points = { a, b }, Direction);
		}
	}
	else
	{
		D3DXVECTOR3 ab_abc;
		D3DXVec3Cross(&ab_abc,&ab,&abc);
		if (SameDirection(ab_abc, ao))
		{
			return Line(points = { a, b },Direction);
		}
		else
		{
			if (SameDirection(abc, ao))
			{
				Direction = abc;
			}
			else
			{
				points = { a, c, b };
				Direction = -abc;
			}
		}
	}

	return false;

}


//======================================================================================
// 
//======================================================================================
bool CGJK::Tetrahedron(Simplex& points, D3DXVECTOR3& Direction)
{
	D3DXVECTOR3 a = points[0];
	D3DXVECTOR3 b = points[1];
	D3DXVECTOR3 c = points[2];
	D3DXVECTOR3 d = points[3];

	D3DXVECTOR3 ab = b - a;
	D3DXVECTOR3 ac = c - a;
	D3DXVECTOR3 ad = d - a;
	D3DXVECTOR3 ao = -a;

	D3DXVECTOR3 CrossA, CrossB, CrossC;
	D3DXVec3Cross(&CrossA, &ab, &ac);
	D3DXVec3Cross(&CrossB, &ac, &ad);
	D3DXVec3Cross(&CrossC, &ad, &ab);

	D3DXVECTOR3 abc = CrossA;
	D3DXVECTOR3 acd = CrossB;
	D3DXVECTOR3 adb = CrossC;

	if (SameDirection(abc, ao)) {
		return Triangle(points = { a, b, c }, Direction);
	}

	if (SameDirection(acd, ao)) {
		return Triangle(points = { a, c, d }, Direction);
	}

	if (SameDirection(adb, ao)) {
		return Triangle(points = { a, d, b }, Direction);
	}

	return true;
}