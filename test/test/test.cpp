// test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "NLPIR.h"
#include "tinystr.h"
#include "tinyxml.h"
#include <iostream>
#include <fstream>
#include <hash_map>
#include <string>
#include <Windows.h>

using namespace std;
#pragma comment(lib,"NLPIR.lib");
#pragma comment(lib,"tinyxml.lib");
//存放字典数据的hashMap
hash_map<string,float> hmDic;

void processDictionary(string filename,float weight)
{
	ifstream fin(filename.c_str());  
    if( !fin ) 
    {   
        cout << "Error opening " << filename << " for input" << endl;   
        return;  
	}
	string buf;
	while(fin>>buf)
	{
		hmDic.insert(make_pair(buf,weight));
	}
}
//changeTxtEncoding修改字符串的编码
char* changeTxtEncoding(char* szU8){ 
	int wcsLen = ::MultiByteToWideChar(CP_UTF8, NULL, szU8, strlen(szU8), NULL, 0);
	wchar_t* wszString = new wchar_t[wcsLen + 1];
	::MultiByteToWideChar(CP_UTF8, NULL, szU8, strlen(szU8), wszString, wcsLen);
	wszString[wcsLen] = '\0';

	int ansiLen = ::WideCharToMultiByte(CP_ACP, NULL, wszString, wcslen(wszString), NULL, 0, NULL, NULL);
	char* szAnsi = new char[ansiLen + 1];
	::WideCharToMultiByte(CP_ACP, NULL, wszString, wcslen(wszString), szAnsi, ansiLen, NULL, NULL);
	szAnsi[ansiLen] = '\0';
	return szAnsi;
}

string changeTextFromUtf8ToAnsi(string line)
{
		string strResult="";
		strResult+=line+"\n";
		char* changeTemp=new char[strResult.length()];
		strcpy(changeTemp,strResult.c_str());
		char* changeResult=changeTxtEncoding(changeTemp);
		strResult=changeResult;
		return strResult;
}

void processConfigure()
{
	//获取XML对象
    TiXmlDocument doc; 
	//装载文件
    doc.LoadFile("configure.xml");
	//获取dics
	TiXmlElement* dicsLevel = (doc.RootElement())->FirstChildElement();
	//获取dic
	TiXmlElement* dicLevel=dicsLevel->FirstChildElement();
    TiXmlElement* pathLevel,*weightLevel;
	cout<<"Process Begin, Please Wait Patient"<<endl;
    while(dicLevel!=NULL)
    {
		pathLevel = dicLevel->FirstChildElement();//获取path
		weightLevel = pathLevel->NextSiblingElement();//获取weight		
		//cout<<pathLevel->GetText()<<":"<<weightLevel->GetText()<<endl;
		//处理文本
		string filename =pathLevel->GetText();
		float weight = atof(weightLevel->GetText());
		//cout<<filename<<"|"<<weight+1<<endl;
		processDictionary(filename,weight);
        dicLevel=dicLevel->NextSiblingElement();
    }
}

int _tmain(int argc, _TCHAR* argv[])
{
	NLPIR_Init();//初始化分词系统
	processConfigure();//初始化情感字典
	ifstream in("suicide.txt");//输入正样本
	ifstream in2("no-suicide.txt");//输入负样本
	string strLine="";
	const result_t *pVecResult;	
	//一条句子里在情感字典里词的个数,微博条数,微博的词在情感字典里的微博数目,一条句子分词后词语数目
	int count=0,numSentence=0,countn=0,nCount;
	if (in)
	{
	   while(!in.eof()){
		getline(in,strLine);
		//转换输入utf-8字符到ansi格式
		string strAsni=changeTextFromUtf8ToAnsi(strLine);
		numSentence++;
		pVecResult=NLPIR_ParagraphProcessA(strAsni.c_str(),&nCount);
		for(int i=0;i<nCount;++i)
		{
			string tmp = strAsni.substr(pVecResult[i].start,pVecResult[i].length);
			auto itr=hmDic.find(tmp);
			if(itr!=hmDic.end())
				{//发现匹配时，匹配的都是“是”“给”，这些在字典中是这样的“很是”，“给面子”!!!
					count++;
				}
		}
		if(count>0)
			countn++;
		count=0;
	   }
	}
	
	cout<<"there are totally "<<numSentence<<" sentences"<<endl;
	cout<<"there are "<<countn<<" in dictionary";
	system("pause");
	return 0;
}

