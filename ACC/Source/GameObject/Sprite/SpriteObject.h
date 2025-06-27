#pragma once
#include "GameObject.h"


class SpriteObject
	: public GameObject
{
public:
	//------------------------------------------
	//		構造体
	//------------------------------------------
	//幅高さ構造体
	struct WHSIZE
	{
		float w;
		float h;
	};
	// スプライト構造体
	struct SPRITE_STATE
	{
		WHSIZE Disp;		// 表示幅高さ
		WHSIZE Base;		// 元画像幅高さ
		WHSIZE Stride;		// 1コマあたりの幅高さ
		D3DXVECTOR3 Pos;	// 画像座標
		D3DXVECTOR3 Scale;	// スケール
		std::string Path;	// パス
		std::string Name;	// 名前
		bool IsDisp;		// 表示するか
		bool IsGrab;		// つかまれているか(UIEditorで使用)
		bool IsLock;		// 固定するか(UIEditorで使用)

		SPRITE_STATE()
			: Disp(0, 0)
			, Base(0, 0)
			, Stride(0, 0)
			, Pos(0.f, 0.f, 0.f)
			, Scale(1.f, 1.f, 1.f)
			, Path("")
			, Name("")
			, IsDisp(true)
			, IsGrab(false)
			, IsLock(false)
		{
		}
	};

	// 頂点の構造体
	struct VERTEX
	{
		D3DXVECTOR3 Pos; // 頂点座標
		D3DXVECTOR2	Tex; // テクスチャ座標
	};

public:
	SpriteObject();
	virtual ~SpriteObject() override;

	// 更新処理
	virtual void Update() override;
	// 描画処理
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj );

	//スプライトを接続する
	void AttachSprite(SpriteObject& pSprite ){ m_pSprite = &pSprite; }
	// スプライトを切り離す
	void DetachSprite(){ m_pSprite = nullptr; }

	// jsonファイルのスプライト情報を取得
	HRESULT SpriteStateDataLoad(const std::string& FilePath);
	// スプライト情報をまとめたjsonファイルの作成
	HRESULT CreateSpriteState(const std::string& FilePath);


	// パターン番号(マス目)を設定
	void SetPatternNo(SHORT x, SHORT y) { m_PatternNo = POINTS(x,y); }
	// スプライトのファイルパスを設定
	void SetFilePath(const std::string& FilePath) { m_SpriteState.Path = FilePath; }

	// スプライトのファイルパスを設定
	void SetSpriteName(const std::string& name) { m_SpriteState.Name = name; }

	// 最大パターン数(マス目)を取得
	POINTS GetPatternMax()	const { return m_PatternMax; }
	// 画像情報を取得
	SPRITE_STATE GetSpriteData()const { return m_SpriteState; }

protected:
	// 子クラスを抽象クラスにしないために必要
	void Draw(D3DXMATRIX& mView, D3DXMATRIX& mProj, LIGHT& Light ) override final;

protected:
	SpriteObject* m_pSprite;
	SPRITE_STATE m_SpriteState; // スプライト情報

	POINTS m_PatternNo;		// パターン番号(マス目)
	POINTS m_PatternMax;	// 最大パターン(マスの最大値)
};