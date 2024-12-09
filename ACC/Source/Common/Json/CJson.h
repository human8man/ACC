#pragma once
#include <iostream>
#include "nlohmann/json.hpp"
#include <fstream>

//namespace省略用
using Json = nlohmann::json;

/***********************************************
*	Jsonクラス
***/
class CJson
{
public:
	//インスタンス取得
	static CJson* GetInstance()
	{
		static CJson s_Instance;
		return &s_Instance;
	}

	~CJson();

	//指定したファイルを読み込んで返す
	Json LoadjsonFile(std::string FileName);

	//指定されたファイルにデータを保存する
	void SavejsonData(std::string FileName,Json& SaveData);

	//新しくjsonファイルを作成
	void Createjson(std::string FileName);

private:
	CJson();
	CJson(const CJson& rhs) = delete;
	CJson& operator = (const CJson& rhs) = delete;

};
