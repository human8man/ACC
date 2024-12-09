#include "CJson.h"
#include <sstream>

CJson::CJson()
{
}

CJson::~CJson()
{
}

//指定したファイルを読み込んで返す
Json CJson::LoadjsonFile(std::string FileName)
{
	//データの読み込み先
	std::ifstream LoadData;

	//読み込み状態でファイルを開く
	std::string a = FileName + ".json";
	LoadData.open(a, std::ios::in);
	
	//読み込んだデータを渡す
	Json outdata;
	
	outdata = Json::parse(LoadData);

	//ファイルを閉じる
	LoadData.close();

	return outdata;
}

//指定されたファイルにデータを保存する
void CJson::SavejsonData(std::string FileName, Json& SaveData)
{
	std::ofstream WriteData(FileName + ".json");
	
	//ostringstreamでjsonデータを文字列で取得
	std::ostringstream os;
	os << std::setw(4) << SaveData; //4つインデントを開けてjsonデータをセット

	std::string OutData = os.str(); //jsonデータを文字列として取得

	//データを保存
	WriteData << OutData;

	//ファイルを閉じる
	WriteData.close();
}

//新しくjsonファイルを作成
void CJson::Createjson(std::string FileName)
{
	std::string Text = "New Data";
	std::ofstream OutFile;

	//jsonファイルを作成
	OutFile.open(FileName + ".json", std::ios::app);
	OutFile.write(Text.data(),Text.size());
	OutFile.close();
}
