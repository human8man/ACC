#pragma once
#include <iostream>
#include "nlohmann/json.hpp"
#include <fstream>

//namespace�ȗ��p
using Json = nlohmann::json;

/***********************************************
*	Json�N���X
***/
class CJson
{
public:
	//�C���X�^���X�擾
	static CJson* GetInstance()
	{
		static CJson s_Instance;
		return &s_Instance;
	}

	~CJson();

	//�w�肵���t�@�C����ǂݍ���ŕԂ�
	Json LoadjsonFile(std::string FileName);

	//�w�肳�ꂽ�t�@�C���Ƀf�[�^��ۑ�����
	void SavejsonData(std::string FileName,Json& SaveData);

	//�V����json�t�@�C�����쐬
	void Createjson(std::string FileName);

private:
	CJson();
	CJson(const CJson& rhs) = delete;
	CJson& operator = (const CJson& rhs) = delete;

};
