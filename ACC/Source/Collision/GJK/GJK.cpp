#include "GJK.h"
#include "GameObject.h"


//==============================================================================
//		指定した方向に対して最も遠い頂点を求める関数
//==============================================================================
D3DXVECTOR3 Collider::FindFurthestPoint(D3DXVECTOR3 Direction) const
{
	D3DXVECTOR3 MaxPoint;			// 最遠の頂点用
	float MaxDistance = -FLT_MAX;	// 最大距離(最小値で初期化)

	// 全頂点を走査し、指定方向に最も遠い点を求める
	for (D3DXVECTOR3 Vertex : m_Vertices)
	{
		// 方向ベクトルとの内積を計算
		float Distance = D3DXVec3Dot(&Vertex, &Direction);

		// より遠い点が見つかった場合、情報を更新
		if (Distance > MaxDistance) {
			MaxDistance = Distance;
			MaxPoint = Vertex;
		}
	}

	return MaxPoint;
}


//==============================================================================
//		頂点情報をワールド座標に変換してセットする
//==============================================================================
void Collider::SetVertex(const ObjectInfo& object, const std::vector<D3DXVECTOR3>& vertex)
{
	// ワールド変換行列
	D3DXMATRIX mTrans, mRot, mScale;

	// 拡大縮小行列
	D3DXMatrixScaling(&mScale, object.scale.x, object.scale.y, object.scale.z);

	// 回転行列
	D3DXMATRIX mYaw, mPitch, mRoll;
	D3DXMatrixRotationY(&mYaw,	object.rot.y);
	D3DXMatrixRotationX(&mPitch,object.rot.x);
	D3DXMatrixRotationZ(&mRoll,	object.rot.z);
	mRot = mYaw * mPitch * mRoll;

	// 平行移動行列
	D3DXMatrixTranslation(&mTrans, object.pos.x, object.pos.y, object.pos.z);

	// ワールド行列 = 拡大 * 回転 * 平行移動
	D3DXMATRIX mWorld = mScale * mRot * mTrans;

	// 頂点情報をワールド座標に変換
	m_Vertices = vertex;

	// 頂点情報をセット
	for (auto& v : m_Vertices) {
		D3DXVec3TransformCoord(&v, &v, &mWorld);
	}
}


//======================================================================================
//		コライダーの頂点の重心（中心）を取得する関数
//======================================================================================
D3DXVECTOR3 Collider::GetCenter() const
{
	if (m_Vertices.empty()) return D3DXVECTOR3(0, 0, 0);

	D3DXVECTOR3 center = D3DXVECTOR3(0, 0, 0);
	for (const auto& v : m_Vertices) {
		center += v;
	}

	center /= static_cast<float>(m_Vertices.size());
	return center;
}


//======================================================================================
//		二つのコライダーと方向からミンコフスキー差のサポート点を計算する
//======================================================================================
D3DXVECTOR3 GJK::Support(const Collider& ColliderA, const Collider& ColliderB, D3DXVECTOR3 Direction)
{
	return ColliderA.FindFurthestPoint(Direction) - ColliderB.FindFurthestPoint(-Direction);
}


//======================================================================================
//		GJKアルゴリズムを用いて二つのコライダーの衝突を判定する
//======================================================================================
CollisionPoints GJK::GJKC(const Collider& ColliderA, const Collider& ColliderB)
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
//		EPA (Expanding Polytope Algorithm) により衝突情報を補完する
//======================================================================================
CollisionPoints GJK::EPA(const Simplex& simplex, const Collider& colliderA, const Collider& colliderB)
{
	// Simplexをポリトープ（凸包）として初期化
	std::vector<D3DXVECTOR3> Polytope(simplex.Begin(), simplex.End());
	// 初期の面（三角形）を定義
	std::vector<size_t> Faces = { 0, 1, 2, 0, 3, 1, 0, 2, 3, 1, 3, 2 };
	// 各面の法線情報を取得し、一番近い面のインデックスを取得
	auto [normals, minFace] = GJK::GetFaceNormals(Polytope, Faces);

	// 最も近い面の法線ベクトルと距離を取得
	D3DXVECTOR3 minNormal = { normals[minFace].x, normals[minFace].y, normals[minFace].z };
	float minDistance = normals[minFace].w;


	// 最大100回のループでポリトープを拡張
	for (int count = 0; count < 100; count++)
	{
		// 最も近い面の法線方向に対する新しいサポート点を取得
		D3DXVECTOR3 support = Support(colliderA, colliderB, minNormal);

		// 新しいサポート点と最も近い面の距離を計算
		float sDistance = D3DXVec3Dot(&minNormal, &support);

		// サポート点が十分に近ければ終了（収束判定）
		if (abs(sDistance - minDistance) <= 0.001f) { break; }

		// 収束しなかった場合、ポリトープを拡張
		minDistance = FLT_MAX;

		// 一意のエッジリスト（削除する面の周囲の辺を管理）
		std::vector<std::pair<size_t, size_t>> uniqueEdges;

		// 法線を確認し、削除すべき面を特定
		for (size_t i = 0; i < normals.size(); i++)
		{
			// サポート点の方向と法線が同じ向きなら、その面は不要
			if (SameDirection({ normals[i].x, normals[i].y, normals[i].z }, support)) {

				// 削除する面の3つのエッジを記録（重複がないように管理）
				size_t f = i * 3;
				AddIfUniqueEdge(uniqueEdges, Faces, f,		f + 1);
				AddIfUniqueEdge(uniqueEdges, Faces, f + 1,	f + 2);
				AddIfUniqueEdge(uniqueEdges, Faces, f + 2,	f	 );

				// 面を削除（最後の要素と入れ替えて削除）
				Faces[f + 2] = Faces.back(); Faces.pop_back();
				Faces[f + 1] = Faces.back(); Faces.pop_back();
				Faces[f] = Faces.back(); Faces.pop_back();

				// 法線リストも削除
				normals[i] = normals.back();
				normals.pop_back();
				i--;
			}
		}

		// 新しい面を作成
		std::vector<size_t> newFaces;
		for (const auto& [EdgeIndex1, EdgeIndex2] : uniqueEdges)
		{
			// 新しいサポート点を追加し、新しい面を構築
			newFaces.insert(newFaces.end(), { EdgeIndex1, EdgeIndex2, Polytope.size() });
		}

		// 新しいサポート点をポリトープに追加
		Polytope.push_back(support);

		// 新しい面の法線情報を取得
		auto [newNormals, newMinFace] = GetFaceNormals(Polytope, newFaces);

		// 既存の法線の中で最も近い面を探す
		float OldMinDistance = FLT_MAX;
		for (size_t i = 0; i < normals.size(); i++)
		{
			if (normals[i].w < OldMinDistance) {
				OldMinDistance = normals[i].w;
				minFace = i;
			}
		}

		// 新しく作成された面の中に、より近いものがあれば更新
		if (newNormals[newMinFace].w < OldMinDistance) {
			minFace = newMinFace + normals.size();
		}

		// 法線と面の情報を更新
		Faces.insert(Faces.end(), newFaces.begin(), newFaces.end());
		normals.insert(normals.end(), newNormals.begin(), newNormals.end());

		// 最も近い法線ベクトルと距離を更新
		minNormal = { normals[minFace].x, normals[minFace].y, normals[minFace].z };
		minDistance = normals[minFace].w;
	}

	// もし収束しなかった場合、最小距離を設定
	if (minDistance == FLT_MAX) { minDistance = 0.001f; }

	// 最終的な衝突点情報を返す
	return { minNormal, minDistance + 0.001f, true };
}


//======================================================================================
//		ポリトープ (多面体) から各面の法線ベクトルを計算する
//======================================================================================
std::tuple<std::vector<D3DXVECTOR4>, size_t> GJK::GetFaceNormals(
	const std::vector<D3DXVECTOR3>& Poly,
	const std::vector<size_t>& Faces)
{
	// 各面の法線ベクトルと原点までの距離を格納するリスト
	std::vector<D3DXVECTOR4> Normals;

	// 最も原点に近い三角形のインデックス
	size_t minTriangle = 0;

	// 原点までの最短距離
	float minDistance = FLT_MAX;

	// 各三角形の法線を計算
	for (size_t i = 0; i < Faces.size(); i += 3)
	{
		// 三角形の3つの頂点を取得
		D3DXVECTOR3 a = Poly[Faces[i    ]];
		D3DXVECTOR3 b = Poly[Faces[i + 1]];
		D3DXVECTOR3 c = Poly[Faces[i + 2]];

		// 法線ベクトルを求める
		D3DXVECTOR3 Normal;
		D3DXVECTOR3 b_a = b - a; // 辺ベクトル (AB)
		D3DXVECTOR3 c_a = c - a; // 辺ベクトル (AC)

		// 外積を計算して法線ベクトルを取得
		D3DXVECTOR3 CrossABC;
		D3DXVec3Cross(&CrossABC, &b_a, &c_a);
		D3DXVec3Normalize(&Normal, &CrossABC);

		// 法線ベクトルと1点Aの内積を計算し、平面の距離を求める
		float Distance = D3DXVec3Dot(&Normal, &a);

		// 内積が負の場合、法線ベクトルの向きを反転（原点方向を向くようにする）
		if (Distance < 0) {
			Normal *= -1;
			Distance *= -1;
		}

		// 計算した法線と距離をリストに追加
		Normals.emplace_back(Normal, Distance);

		// 原点に最も近い面を特定
		if (Distance < minDistance) {
			minTriangle = i / 3;	// 三角形のインデックス
			minDistance = Distance;
		}
	}

	// 法線リストと最も原点に近い三角形のインデックスを返す
	return { Normals, minTriangle };
}


//======================================================================================
//		エッジの逆方向がリストに存在するか確認し、存在する場合削除
//======================================================================================
void GJK::AddIfUniqueEdge(
	std::vector<std::pair<size_t, size_t>>& Edges,
	const std::vector<size_t>& Faces,
	size_t a,size_t b)
{
	// `Faces[a]` と `Faces[b]` を結ぶエッジがすでにリスト内に逆向きで存在するかチェック
	auto Reverse = std::find(
		Edges.begin(),
		Edges.end(),
		std::make_pair(Faces[b], Faces[a])); // 逆向き (b, a) があるか確認

	// もし逆向きのエッジが存在する場合、削除する（重複を防ぐ）
	if (Reverse != Edges.end()) {
		Edges.erase(Reverse);
	}
	// 逆向きのエッジが存在しない場合、新しいエッジ (a, b) を追加
	else {
		Edges.emplace_back(Faces[a], Faces[b]);
	}
}


//======================================================================================
//		与えられた頂点群の中で最も低いY座標の頂点の位置を返す
//======================================================================================
float GJK::SendMinVertexPosY( const ObjectInfo& obje, const std::vector<D3DXVECTOR3>& vertex)
{
	std::vector<D3DXVECTOR3> Vertex;	// 変換後の頂点リスト
	float MinPosY = FLT_MAX;			// Y座標の最小値（初期値は最大値）

	// 位置・回転・スケールの値を取得
	D3DXVECTOR3 Pos = obje.pos, Rot = obje.rot, Scale = obje.scale;

	// 各種変換行列
	D3DXMATRIX mTrans, mRot, mScale;

	// 拡大縮小行列
	D3DXMatrixScaling(&mScale, Scale.x, Scale.y, Scale.z);

	// 回転行列
	D3DXMATRIX mYaw, mPitch, mRoll;
	D3DXMatrixRotationY(&mYaw, Rot.y);	// Yaw
	D3DXMatrixRotationX(&mPitch, Rot.x);// Pitch
	D3DXMatrixRotationZ(&mRoll, Rot.z);	// Roll

	// 回転の適用順序: Yaw → Pitch → Roll
	//	※回転の順番を変えると結果も変化する
	mRot = mYaw * mPitch * mRoll; 

	// 平行移動行列
	D3DXMatrixTranslation(&mTrans, Pos.x, Pos.y, Pos.z);

	// ワールド座標変換を適用（スケール → 回転 → 平行移動 の順に適用）
	D3DXMATRIX mWorld = mScale * mRot * mTrans;


	//====================================
	// 頂点変換処理
	//====================================

	// 変換後の頂点リストを元の頂点数に合わせてリサイズ
	Vertex.resize(vertex.size());

	// 各頂点をワールド座標に変換
	for (int i = 0; i < vertex.size(); i++)
	{
		// 頂点をコピー
		Vertex[i] = vertex[i];

		// ワールド行列を適用して座標変換
		D3DXVec3TransformCoord(&Vertex[i], &Vertex[i], &mWorld);

		// 最小Y座標を更新
		if (MinPosY > Vertex[i].y) { MinPosY = Vertex[i].y; }
	}

	// 最も低いY座標を返す
	return MinPosY;
}


//======================================================================================
//		シンプレックスを拡張し、次の探索方向を決定する
//======================================================================================
bool GJK::NextSimplex(Simplex& points, D3DXVECTOR3& Direction)
{
	// 現在のシンプレックスのサイズに応じて処理を分岐
	switch (points.Size())
	{
	case 2: return Line			( points, Direction );	// 2点の場合（線分のケース）
	case 3: return Triangle		( points, Direction );	// 3点の場合（三角形のケース）
	case 4: return Tetrahedron	( points, Direction );	// 4点の場合（四面体のケース）
	}

	return false;
}


//======================================================================================
//		二つのベクトルが同じ方向を向いているか判定する
//======================================================================================
bool GJK::SameDirection(const D3DXVECTOR3& Direction, const D3DXVECTOR3& ao)
{
	// ドット積が正なら同じ方向、負なら逆方向
	return D3DXVec3Dot(&Direction, &ao) > 0.0f;
}


//======================================================================================
//		2点 (線分) のシンプレックスを処理し、次の探索方向を決定する
//======================================================================================
bool GJK::Line(Simplex& points, D3DXVECTOR3& Direction)
{
	// シンプレックスの2点を取得
	D3DXVECTOR3 a = points[0];
	D3DXVECTOR3 b = points[1];

	// ベクトル AB と AO を計算
	D3DXVECTOR3 ab = b - a;
	D3DXVECTOR3 ao = -a;  // 原点O から A へのベクトル

	// AB が AO と同じ方向を向いている場合
	if (SameDirection(ab, ao)) {
		// AB に垂直な方向を探索方向に設定
		D3DXVECTOR3 C;
		D3DXVec3Cross(&C, &ab, &ao);
		D3DXVec3Cross(&Direction, &C, &ab);
	}
	else {
		// B を削除し、A のみを残す
		points = { a };
		Direction = ao;  // 原点方向に探索
	}

	// 原点を包囲できていないので false
	return false;
}


//======================================================================================
//		3点 (三角形) のシンプレックスを処理し、次の探索方向を決定する
//======================================================================================
bool GJK::Triangle(Simplex& points, D3DXVECTOR3& Direction)
{
	// シンプレックスの3点を取得
	D3DXVECTOR3 a = points[0];
	D3DXVECTOR3 b = points[1];
	D3DXVECTOR3 c = points[2];

	// ベクトル AB, AC, AO を計算
	D3DXVECTOR3 ab = b - a;
	D3DXVECTOR3 ac = c - a;
	D3DXVECTOR3 ao = -a;

	// 三角形の法線ベクトルを求める (ABC の外積)
	D3DXVECTOR3 abc;
	D3DXVec3Cross(&abc, &ab, &ac);

	// ABC の法線と AC から探索するか判定
	D3DXVECTOR3 abc_ac;
	D3DXVec3Cross(&abc_ac, &abc, &ac);
	if (SameDirection(abc_ac, ao))
	{
		// AC が AO と同じ方向を向いている場合
		if (SameDirection(ac, ao)) {
			// B を削除し、A と C を残す
			points = { a, c };

			// AO に対して AC に垂直な方向を探索
			D3DXVECTOR3 ac_ao;
			D3DXVec3Cross(&ac_ao, &ac, &ao);
			D3DXVec3Cross(&Direction, &ac_ao, &ac);
		}
		else {
			// AC 方向ではない場合、AB を処理する
			return Line(points = { a, b }, Direction);
		}
	}
	else
	{
		// ABC の法線と AB から探索するか判定
		D3DXVECTOR3 ab_abc;
		D3DXVec3Cross(&ab_abc, &ab, &abc);
		if (SameDirection(ab_abc, ao)) {
			// AB 方向で探索
			return Line(points = { a, b }, Direction);
		}
		else {
			// AO の方向に応じて探索方向を設定
			if (SameDirection(abc, ao)) {
				Direction = abc;	// ABC の法線方向へ探索
			}
			else {
				points = { a, c, b };	// BC の順番を入れ替える
				Direction = -abc;		// 逆方向へ探索
			}
		}
	}

	return false;
}


//======================================================================================
//		4点 (四面体) のシンプレックスを処理し、次の探索方向を決定する
//======================================================================================
bool GJK::Tetrahedron(Simplex& points, D3DXVECTOR3& Direction)
{
	// シンプレックスの4点を取得
	D3DXVECTOR3 a = points[0];
	D3DXVECTOR3 b = points[1];
	D3DXVECTOR3 c = points[2];
	D3DXVECTOR3 d = points[3];

	// 各ベクトルを計算
	D3DXVECTOR3 ab = b - a;
	D3DXVECTOR3 ac = c - a;
	D3DXVECTOR3 ad = d - a;
	D3DXVECTOR3 ao = -a;

	// 三角形の法線ベクトルを求める
	D3DXVECTOR3 abc, acd, adb;
	D3DXVec3Cross(&abc, &ab, &ac);	// ABC の法線
	D3DXVec3Cross(&acd, &ac, &ad);	// ACD の法線
	D3DXVec3Cross(&adb, &ad, &ab);	// ADB の法線

	// 原点が ABC の上にあるか
	if (SameDirection(abc, ao)) {
		return Triangle(points = { a, b, c }, Direction);
	}

	// 原点が ACD の上にあるか
	if (SameDirection(acd, ao)) {
		return Triangle(points = { a, c, d }, Direction);
	}

	// 原点が ADB の上にあるか
	if (SameDirection(adb, ao)) {
		return Triangle(points = { a, d, b }, Direction);
	}

	// 原点が四面体内に含まれている場合、交差しているので trueを返す
	return true;
}