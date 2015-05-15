// test.cpp : �������̨Ӧ�ó������ڵ㡣
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
//����ֵ����ݵ�hashMap
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
//changeTxtEncoding�޸��ַ����ı���
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
	//��ȡXML����
    TiXmlDocument doc; 
	//װ���ļ�
    doc.LoadFile("configure.xml");
	//��ȡdics
	TiXmlElement* dicsLevel = (doc.RootElement())->FirstChildElement();
	//��ȡdic
	TiXmlElement* dicLevel=dicsLevel->FirstChildElement();
    TiXmlElement* pathLevel,*weightLevel;
	cout<<"Process Begin, Please Wait Patient"<<endl;
    while(dicLevel!=NULL)
    {
		pathLevel = dicLevel->FirstChildElement();//��ȡpath
		weightLevel = pathLevel->NextSiblingElement();//��ȡweight		
		//cout<<pathLevel->GetText()<<":"<<weightLevel->GetText()<<endl;
		//�����ı�
		string filename =pathLevel->GetText();
		float weight = atof(weightLevel->GetText());
		//cout<<filename<<"|"<<weight+1<<endl;
		processDictionary(filename,weight);
        dicLevel=dicLevel->NextSiblingElement();
    }
}

int _tmain(int argc, _TCHAR* argv[])
{
	NLPIR_Init();//��ʼ���ִ�ϵͳ
	processConfigure();//��ʼ������ֵ�
	ifstream in("suicide.txt");//����������
	ifstream in2("no-suicide.txt");//���븺����
	string strLine="";
	const result_t *pVecResult;	
	//һ��������������ֵ���ʵĸ���,΢������,΢���Ĵ�������ֵ����΢����Ŀ,һ�����ӷִʺ������Ŀ
	int count=0,numSentence=0,countn=0,nCount;
	if (in)
	{
	   while(!in.eof()){
		getline(in,strLine);
		//ת������utf-8�ַ���ansi��ʽ
		string strAsni=changeTextFromUtf8ToAnsi(strLine);
		numSentence++;
		pVecResult=NLPIR_ParagraphProcessA(strAsni.c_str(),&nCount);
		for(int i=0;i<nCount;++i)
		{
			string tmp = strAsni.substr(pVecResult[i].start,pVecResult[i].length);
			auto itr=hmDic.find(tmp);
			if(itr!=hmDic.end())
				{//����ƥ��ʱ��ƥ��Ķ��ǡ��ǡ�����������Щ���ֵ����������ġ����ǡ����������ӡ�!!!
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

