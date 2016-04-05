/*
 * Project------ CArchitecture
 * Class Name--- TestCase.cpp
 * Author------- wxf891201@gmail.com
 * Date--------- 2013-4-8
 * Edition------ 1.0

 * Description-- 对多个文档进行分析, 得到simplex，每个类间关系对应一个CSimplex对象 

 * Change Log:
 *		Date----- 2013-4-18
 *		Content-- 对多个文档分析，由内存读取改为直接Cstdio读取。并取得类的对象名，用于后续查找其他函数中是否出现该类对象

 * Change Log:
 *		Date----- 2013-5-5
 *		Content-- 修改了函数解析部分的思路，得到函数间被调用关系，即某个类中某个函数被其他哪些类中的哪些函数调用过。这种被调用关系作为类间关系的依据，并作为建立超边的依据

 * Change Log:
 *		Date----- 2013-5-6
 *		Content-- 开始实现超图建模，分别由类得到顶点及类间关系得到超边

 * Change Log:
 *		Date----- 2013-5-13
 *		Content-- 将Mainfrm中的所有实现封装在了CTestCase类中，便于进行多线程操作，也使得代码与界面脱离，便于移植
*/

#include "TestCase.h"
#include <iostream>
#include "CLog.h"
#include "CInput.h"
//#include "Shlwapi.h"



calledclass::calledclass()
{
	next = NULL;
	count = 1;
	bIsmemberVar = false;
}

CTestCase::CTestCase ()
{
	
}

void CTestCase::doModeling()
{
}

void CTestCase::doParsing()
{
}

void CTestCase::doReading()
{

}

void CTestCase::doWriting()
{
}

CTestCaseCpp::CTestCaseCpp ( int index, string strNameTest )
{
#ifdef WIN32
	runtime = ::GetTickCount();
#else
	gettimeofday(&runtime,NULL);
#endif
	pMyGraph = NULL;
	dataIn = NULL;
	pHyper = NULL;

	writeTime = "";
	stage			 = INIT_STAGE;
	head			 =	NULL;
	lastans		 = "";
	cutnum       = 0;
	allTime		 = 0;
	strPara  = strNameTest;
	testcasename = "";
	strTestPath = strNameTest.substr(0,strNameTest.find(" "));
	for(int i=int(strTestPath.length())-1;i>=0&&strTestPath.at(i)!='/';i--)
	{
		testcasename = strTestPath.at(i) + testcasename;
	}
	
	//cout<<"TestCase: "<<testcasename<<"  start...."<<endl;


	CLogNote(string("now class name: "+strNameTest).c_str());
}

CTestCase::~CTestCase ( )
{

}

CTestCaseCpp::~CTestCaseCpp ( )
{
	CReadCpp *p = head;
	CReadCpp *q;
	while(p!=NULL)
	{
		q=p;
		p=p->next;
		delete q;
	}
	map <string, CReadH*>::iterator it;
	for(it = hmap.begin();it!=hmap.end();++it)
	{
		CReadH *m = it->second;
		delete m;
	}
	if(pHyper)
	{
		delete pHyper;
		pHyper = NULL;
	}
	if(dataIn)
	{
		delete dataIn;
		dataIn = NULL;
	}
}

void CTestCase::setNext ( LPTESTCASE _nextcase )
{
	m_pNext_test	= _nextcase;
}

LPTESTCASE CTestCase::getNext ( )
{
	return m_pNext_test;
}

void CTestCase::setIONext ( LPTESTCASE _nextcase )
{
	m_pIONext_test = _nextcase;
}

LPTESTCASE CTestCase::getIONext ( )
{
	return m_pIONext_test;
}

int CTestCase::getStage()
{
	return stage;
}

void CTestCase::setStage(int s)
{
	stage = s;
}

//字符串分割函数
vector<string> CTestCaseCpp::split(string line,string pattern)
{
	string str = line+",";
    std::vector<std::string> result;
	int position = -1;

	if(line.find("{")!=string::npos||line.find("<")!=string::npos||line.find("/")!=string::npos||line.find("(")!=string::npos||line.find("}")!=string::npos)
		return result;

	for(int i=0;i<int(str.length());i++)
	{
		for(int j=0;j<int(pattern.length());j++)
		{
			if(str.at(i)==pattern.at(j))
			{
				string ans = str.substr(position+1,i-1-position);
				if(ans!="")
				{
					result.push_back(ans);
				}
				position = i;
				break;
			}
		}
	}
   
    return result;
}
/*
 * doReading: 遍历每个testcase，获取其中的所有.cpp和.h文件，存于m_ary_file数组中
*/

void CTestCaseCpp::dfsFolder(string path)  //递归深度优先遍历整个testcase文件夹，找出所有.cpp,.cc和.h文件
{
	#ifdef WIN32
	struct _finddata_t fileinfo;
	int hFile = 0;
	string strFind = path+"//*.*";
	hFile	= int(_findfirst (strFind.c_str(), &fileinfo ));
	if(hFile!=-1L)
	{
		do{
			 //判断是否有子目录
			if (fileinfo.attrib & _A_SUBDIR)    
			{
				//这个语句很重要
				if( (strcmp(fileinfo.name,".") != 0 ) &&(strcmp(fileinfo.name,"..") != 0))   
				{
					string newPath = path + "//" + fileinfo.name;
					dfsFolder(newPath);
				}
			}
			else  
			{
				string				strFileName, strCpp_ext, strCc_ext, strC_ext, strH_ext; 
				strFileName = fileinfo.name;
				int iLen			= int(strFileName.length());
				if(iLen>=4)
					strCpp_ext		= strFileName.substr ( iLen-4, 4 );
				else
					strCpp_ext     = "";
				if(iLen>=3)
					strCc_ext		= strFileName.substr ( iLen-3, 3 );
				else
					strCc_ext = "";
				if(iLen>=2)
					strC_ext		= strFileName.substr ( iLen-2, 2 );
				else
					strC_ext = "";

				if(iLen>=2)
					strH_ext		= strFileName.substr ( iLen-2, 2 );
				else
					strH_ext     = "";
				if ( strCpp_ext == ".cpp" || strCc_ext == ".cc" || strC_ext == ".C" || strC_ext == ".c")   
				{  
					arycpp_fileName.push_back(path+"//"+strFileName);
				} 
				else if ( strH_ext == ".h" || strH_ext == ".H")
				{
					aryh_fileName.push_back(path+"//"+strFileName);
				}
			}
		}while (_findnext(hFile, &fileinfo) == 0);

		
	}
	_findclose( hFile );

	#else
	DIR              *pDir ;  
        struct dirent    *ent  ;  
        int               i=0  ;  
        char              childpath[512];  
  
        pDir=opendir(path.c_str());  
        memset(childpath,0,sizeof(childpath));  
  
  
        while((ent=readdir(pDir))!=NULL)  
        {  
  
           if(ent->d_type & DT_DIR)  
           {  
  
              if(strcmp(ent->d_name,".")==0 || strcmp(ent->d_name,"..")==0)  
                       continue;  
  
              sprintf(childpath,"%s/%s",path.c_str(),ent->d_name);  
              //printf("path:%s/n",childpath);  
  
              dfsFolder(string(childpath));  
  
           }  
	   else
	   {
		if(strstr(ent->d_name, "cpp")!=NULL)
             	{
                   arycpp_fileName.push_back(path+"//"+ent->d_name);
                   //cout<<"add:"<<path+"//"+ent->d_name<<endl;
             	}
	        if(strstr(ent->d_name, "h")!=NULL)
             	{
                   aryh_fileName.push_back(path+"//"+ent->d_name);
                   //cout<<"add:"<<path+"//"+ent->d_name<<endl;
             	}
	   }
        }  
	#endif
  
	
}

void CTestCaseCpp::doReading ( )
{

	//cout<<"TestCase: "<<testcasename<<"  start reading...."<<endl;
#ifdef WIN32
    runtime = ::GetTickCount();
	dfsFolder(strTestPath);

	int now = runtime;
	runtime = ::GetTickCount();
	now = runtime - now;
	char gettime[100];
	sprintf_s(gettime,"%d",now);
	writeTime += "read path time : " + string(gettime)+"ms\n";
	allTime += now;



	
#else
    gettimeofday(&runtime,NULL);
    dfsFolder(strTestPath);
    timeval endtime;
    gettimeofday(&endtime,NULL);
    int now = 1000*(endtime.tv_sec - runtime.tv_sec) + (endtime.tv_usec - runtime.tv_usec)/1000; 
    char gettime[100];
    sprintf(gettime,"%d",now);
    writeTime += "read path time : " + string(gettime)+"ms\n";
    allTime += now;
#endif
	readEachfile();
    
}

void CTestCaseCpp::readEachfile()
{
	
#ifdef WIN32
	runtime = ::GetTickCount();
#else
	gettimeofday(&runtime,NULL);
#endif
	for ( int i=0; i < int(aryh_fileName.size()); i++ )
	{
		CReadH *p		= new CReadH (aryh_fileName.at(i));
		
		int k;
		for(k=int(aryh_fileName.at(i).length())-1;aryh_fileName.at(i).at(k)!='/';k--);
		string headname = aryh_fileName.at(i).substr(k+1,aryh_fileName.at(i).length()-1-k);   //只取文件名，不要路径
		hmap.insert(pair <string,CReadH*>(headname, p));
	}

	for ( int i=0; i < int(arycpp_fileName.size()); i++ )
	{
		CReadCpp *p		= new CReadCpp (arycpp_fileName.at(i));
		p->setnext(head);
		head = p;
	}

	//cout<<"TestCase: "<<testcasename<<"  end reading...."<<endl;
#ifdef WIN32
	int now = runtime;
	runtime = ::GetTickCount();
	now = runtime - now;
	char gettime[100];
	sprintf_s(gettime,"%d",now);
	writeTime += "readfile time : " + string(gettime)+"ms\n";
	allTime += now;
#else
	timeval endtime;
    	gettimeofday(&endtime,NULL);
    	int now = 1000*(endtime.tv_sec - runtime.tv_sec) + (endtime.tv_usec - runtime.tv_usec)/1000; 
    	char gettime[100];
    	sprintf(gettime,"%d",now);
    	writeTime += "readfile time : " + string(gettime)+"ms\n";
    	allTime += now;
#endif
}

// 解析部分

void CTestCaseCpp::dealwithTypeDef(string content)
{
	int pos1 = int(content.find("typedef"));
	int pos2 = int(content.find(";",pos1));
	while(pos1!=string::npos&&pos2!=string::npos)
	{
		string line=content.substr(pos1,pos2-pos1);
		vector<string> words = split(line," \t,\n*");
		string name1="",name2="";
		if(words.size()>=3)
		{
			if(words.at(1)=="class")
			{
				name1=words.at(2);
				for(int k=3;k<int(words.size());k++)
				{
					mapForTypeDef.insert(pair<string,string>(words.at(k),name1));
				}
			}
			else
			{
				name1=words.at(1);
				for(int k=2;k<int(words.size());k++)
				{
					mapForTypeDef.insert(pair<string,string>(words.at(k),name1));
				}
			}
		}
		
		pos1 = int(content.find("typedef",pos1+int(strlen("typedef"))));
		pos2 = int(content.find(";",pos1));
	}
}

void CTestCaseCpp::doParsing ( )
{ 
	//cout<<"TestCase: "<<testcasename<<"  start parsing...."<<endl;

#ifdef WIN32
	runtime = ::GetTickCount();
#else
	gettimeofday(&runtime,NULL);
#endif
	
	//结果存在mapForTypeDef里
	map<string, CReadH*>::iterator iterForTypeDef;
	for(iterForTypeDef = hmap.begin();iterForTypeDef!=hmap.end();++iterForTypeDef)
	{
		dealwithTypeDef(iterForTypeDef->second->globalStr);
	}
	CReadCpp *p = head;
	while(p!=NULL)
	{
		dealwithTypeDef(p->globalStr);
		p=p->getnext();
	}

#ifdef FOR_OBJECT
	p = head;
	while(p!=NULL)
	{
		CLogNote(string("cppname::"+p->CppName).c_str());
		map<string,CMethodBody*>::iterator iter;
		for(iter=p->cppclass.begin();iter!=p->cppclass.end();++iter)
		{
			string classname  = iter->first;
			CMethodBody *q  = iter->second;
			while(q!=NULL)
			{
				InitCalledParameter(q);
				q=q->next;
			}


		}
		p=p->getnext();
	}
#endif

	p = head;
	while(p!=NULL)
	{
		CLogNote(string("cppname::"+p->CppName).c_str());
		map<string,CMethodBody*>::iterator iter;
		for(iter=p->cppclass.begin();iter!=p->cppclass.end();++iter)
		{
			string classname  = iter->first;
			CMethodBody *q  = iter->second;
			while(q!=NULL)
			{
				string methodbody = q->body;
				findsymbol("->",methodbody,classname,p);
				findsymbol(".",methodbody,classname,p);
				q=q->next;
			}


		}
		p=p->getnext();
	}
	//cout<<lastans<<endl;


	map <string, CReadH*>::iterator fatherclassmap;
	for(fatherclassmap = hmap.begin();fatherclassmap!=hmap.end();++fatherclassmap)
	{
		map <string,CClassContent*>::iterator it;
		for(it=fatherclassmap->second->classdef.begin();it!=fatherclassmap->second->classdef.end();++it)
		{
			string sonclass = it->first;
			string fatherclass = it->second->fatherclass;
			if(fatherclass!="")
				fathermap.insert(pair<string,string>(sonclass,fatherclass));
		}
	}

	// create father-son hypergraph
	
	map<string,string>::iterator fatherEdge;
	for(fatherEdge = fathermap.begin();fatherEdge!=fathermap.end();++fatherEdge)
	{
		calledclass *newrelation = new calledclass();
		newrelation->name = fatherEdge->first;
		newrelation->next = NULL;
		newrelation->bIsmemberVar = true;
		map<string,calledclass*>::iterator itera;
		itera = calledrelation.find(fatherEdge->second);
		if(itera != calledrelation.end())
		{
			calledclass *temp = itera->second;
			calledclass *last;
			bool belong = false;
			while(temp!=NULL)
			{
				if(temp->name==fatherEdge->first)
				{
					temp->count++;
					belong = true;
					break;
				}
				last  = temp;
				temp=temp->next;
			}
			if(!belong)
				last->next = newrelation;	
		}
		else
		{
			calledrelation.insert(pair<string,calledclass*>(fatherEdge->second,newrelation));
		}
	}
	
	
#ifdef WIN32
	int now = runtime;
	runtime = ::GetTickCount();
	now = runtime - now;
	char gettime[100];
	sprintf_s(gettime,"%d",now);
	writeTime += "parsing time : " + string(gettime)+"ms\n";
	allTime += now;
#else
	timeval endtime;
    gettimeofday(&endtime,NULL);
    int now = 1000*(endtime.tv_sec - runtime.tv_sec) + (endtime.tv_usec - runtime.tv_usec)/1000; 
    char gettime[100];
    sprintf(gettime,"%d",now);
    writeTime += "parsing time : " + string(gettime)+"ms\n";
    allTime += now;
#endif
	//cout<<"TestCase: "<<testcasename<<"  end parsing...."<<endl;

	doModeling();
}
void CTestCaseCpp::InitCalledParameter(CMethodBody *p)
{
	string methodbody = p->body;
	string funcPara = "";
	int posStart = methodbody.find("(");
	int posEnd = methodbody.find(")");
	for(int i=posStart+1;i<posEnd;i++)
		funcPara += methodbody.at(i);
	
	int pos = int(methodbody.find("->"));
	while(pos!=string::npos)
	{
		string valuename = "";
		string typeofname ="";
		int declarationPos = -1; // 声明的位置，1表示在当前函数体内，2表示在.h类的定义里，3表示在全局变量里
		for(int i=pos-1;(methodbody.at(i)>=65&&methodbody.at(i)<=90)||(methodbody.at(i)>=97&&methodbody.at(i)<=122)||(methodbody.at(i)>=48&&methodbody.at(i)<=57)||(methodbody.at(i)=='_');i--)
		{
			valuename = methodbody.at(i) + valuename;
		}

		int pos2 = int(funcPara.find(valuename));
		if(pos2!=string::npos)
		{
			int countdou = 0;
			for(int i=0;i<pos2;i++)
			{
				if(funcPara.at(i)==',')
					countdou++;
			}
			p->CallParemeter.push_back(countdou+1);
		}
		
		pos = int(methodbody.find("->",pos+2));
	}

}
void CTestCaseCpp::findsymbol(string symbol,string methodbody,string classname,CReadCpp *p)
{
	map<string,calledclass*> objectrelation;
	int pos = int(methodbody.find(symbol));
	while(pos!=string::npos)
	{
		string valuename = "";
		string typeofname ="";
		string funcname = "";
		int declarationPos = -1; // 声明的位置，1表示在当前函数体内，2表示在.h类的定义里，3表示在全局变量里
		int i = 0;
		for(i=pos-1;(methodbody.at(i)>=65&&methodbody.at(i)<=90)||(methodbody.at(i)>=97&&methodbody.at(i)<=122)||(methodbody.at(i)>=48&&methodbody.at(i)<=57)||(methodbody.at(i)=='_');i--)
		{
			valuename = methodbody.at(i) + valuename;
		}
#ifdef FOR_OBJECT
		for(i=pos+symbol.length();(methodbody.at(i)>=65&&methodbody.at(i)<=90)||(methodbody.at(i)>=97&&methodbody.at(i)<=122)||(methodbody.at(i)>=48&&methodbody.at(i)<=57)||(methodbody.at(i)=='_');i++)
		{
			funcname += methodbody.at(i);
		}
		bool is_func = false;
		int douCnt = 0;
		string funcPara[20] = {""};
		if(methodbody.at(i)=='(')
		{
			is_func = true;
			int posStart = methodbody.find("(",pos);
			int posEnd = methodbody.find(")",pos);
			for(int j=posStart+1;j<posEnd;j++)
			{
				funcPara[douCnt] += methodbody.at(j);
				if(methodbody.at(j)==',')
					douCnt++;
			}
		}
#endif	

		


		int pos2 = int(methodbody.find(valuename));
		if(valuename != "")
		{
			typeofname = findtype(methodbody,valuename,int(pos));   // 在当前的方法体中寻找变量的声明

			if(typeofname=="")                                                 //在.h的类的定义里寻找变量声明
			{
				for(int i=0;i<int(p->headfile.size());i++)
				{
					string headname = p->headfile.at(i);
					map<string,CReadH*>::iterator it;
					map<string,CClassContent*>::iterator ite;
					if(headname.find("/")!=string::npos)
					{
						int k;
						for(k=int(headname.length())-1;headname.at(k)!='/';k--);
						headname = headname.substr(k+1,headname.length()-1-k);
					}
					
					it = hmap.find(headname);
					if(it!=hmap.end())
					{
						ite = it->second->classdef.find(classname);
					
						if(ite != it->second->classdef.end())
						{
							typeofname = findtype(ite->second->defbody,valuename,int(ite->second->defbody.length()));
							if(typeofname!="")
								declarationPos = 2;
							break;
						}
					}
				}
			}

			if(typeofname=="")                                           //在包含的.h文件和当前文件的全局变量里寻找
			{
				typeofname = findtype(p->globalStr,valuename,int(p->globalStr.length()));
				if(typeofname=="")
				{
					map <string,bool> donemap;
					queue <string> allheadfile;

					for(int i=0;i<int(p->headfile.size());i++)
					{
						string headname = p->headfile.at(i);
						if(headname.find("/")!=string::npos)
						{
							int k;
							for(k=int(headname.length())-1;headname.at(k)!='/';k--);
							headname = headname.substr(k+1,headname.length()-1-k);
						}
						map <string,bool>::iterator iterfordone;
						iterfordone = donemap.find(headname);
						if(iterfordone==donemap.end())
						{
							donemap.insert(pair<string,bool>(headname,true));
							allheadfile.push(headname);
						}
					}
					while(!allheadfile.empty())
					{
						string oneheadfile = allheadfile.front();
						allheadfile.pop();
						map<string,CReadH*>::iterator iterforH;
						iterforH = hmap.find(oneheadfile);
						if(iterforH!=hmap.end())
						{
							typeofname = findtype(iterforH->second->globalStr,valuename,int(iterforH->second->globalStr.length()));
							if(typeofname!="")
								break;
							else
							{
								for(int count=0;count<int(iterforH->second->headfile.size());count++)
								{
									string oneheadname = iterforH->second->headfile.at(count);
									if(oneheadname.find("/")!=string::npos)
									{
										int k;
										for(k=int(oneheadname.length())-1;oneheadname.at(k)!='/';k--);
										oneheadname = oneheadname.substr(k+1,oneheadname.length()-1-k);
									}
									map <string,bool>::iterator iterfordone;
									iterfordone = donemap.find(oneheadname);
									if(iterfordone==donemap.end())
									{
										donemap.insert(pair<string,bool>(oneheadname,true));
										allheadfile.push(oneheadname);
									}
								}
							}
						}
					}

				}
			}

			if(typeofname!="")
			{
				map<string,string>::iterator iterfordef;
				iterfordef = mapForTypeDef.find(typeofname);
				if(iterfordef!=mapForTypeDef.end())
				{
					typeofname = iterfordef->second;
				}

				map<string,CReadH*>::iterator it;
				for(it = hmap.begin();it!=hmap.end();++it)
				{
					map<string,CClassContent*>::iterator ite;
					ite = it->second->classdef.find(typeofname);
					if(ite!=it->second->classdef.end())
					{
						lastans += classname+"  called  "+typeofname + "\n";
						if(classname != typeofname)
						{
							calledclass *newrelation = new calledclass();
							newrelation->name = classname;
							newrelation->next = NULL;
							if(declarationPos==2)   //是类成员变量
							{
								newrelation->bIsmemberVar = true;
							}
							map<string,calledclass*>::iterator itera;
							itera = calledrelation.find(typeofname);
							if(itera != calledrelation.end())
							{
								calledclass *temp = itera->second;
								calledclass *last;
								bool belong = false;
								while(temp!=NULL)
								{
									if(temp->name==classname)
									{
										temp->count++;
										belong = true;
										break;
									}
									last  = temp;
									temp=temp->next;
								}
								if(!belong)
									last->next = newrelation;	
							}
							else
							{
								calledrelation.insert(pair<string,calledclass*>(typeofname,newrelation));
							}
							
						}
#ifdef FOR_OBJECT
						if(is_func)
						{
							CReadCpp *pCppTemp = head;
							bool notfound = true; 
							while(pCppTemp!=NULL&&notfound)
							{
								map<string,CMethodBody*>::iterator CppIter;
								CppIter = pCppTemp->cppclass.find(typeofname);
								if(CppIter!=pCppTemp->cppclass.end())
								{
									CMethodBody *pMethodTemp = CppIter->second;
									while(pMethodTemp!=NULL)
									{
										if(pMethodTemp->funcline==funcname&&pMethodTemp->paraCnt==douCnt+1)
										{
											for(int i=0;i<pMethodTemp->CallParemeter.size();i++)
											{
												//funcPara[pMethodTemp->CallParemeter.at(i)-1]
												calledclass *newrelation = new calledclass();
												newrelation->name = valuename;
												newrelation->next = NULL;
												map<string,calledclass*>::iterator itera;
												itera = objectrelation.find(funcPara[pMethodTemp->CallParemeter.at(i)-1]);

												if(itera != objectrelation.end())
												{
													calledclass *temp = itera->second;
													calledclass *last;
													bool belong = false;
													while(temp!=NULL)
													{
														if(temp->name==valuename)
														{
															temp->count++;
															belong = true;
															break;
														}
														last  = temp;
														temp=temp->next;
													}
													if(!belong)
														last->next = newrelation;	
												}
												else
												{
													objectrelation.insert(pair<string,calledclass*>(funcPara[pMethodTemp->CallParemeter.at(i)-1],newrelation));
												}
											}
											notfound = false;
											break;
										}
										pMethodTemp = pMethodTemp->next;
									}
								}
								pCppTemp = pCppTemp->next;

							}
						}
#endif
					}
				}
			}
		}
		pos = int(methodbody.find(symbol,pos+symbol.length()));
	}

}
string CTestCaseCpp::findtype(string content,string valuename,int endpos)  //找变量类型
{
	char num[100];
	sprintf_s(num, "%d", endpos);
	if(valuename =="AllocationSiteInfo")
	{
		int tem = -1;
	}

	CLogNote(string("Find Type---valuename:"+valuename+"---endpos:"+string(num)).c_str());
	string typeofname="";
	int pos2 = int(content.find(valuename));
	while(pos2!=string::npos)
	{
		if(valuename.length()+pos2>content.length()-1)
			return "";
		char c = content.at(valuename.length()+pos2);
		if((c>=65&&c<=90)||(c>=97&&c<=122)||(c>=48&&c<=57)||(c=='_'))
		{
			pos2 = int(content.find(valuename,valuename.length()+pos2));
			continue;
		}
		if(pos2-1<0)
			return "";
		c = content.at(pos2-1);
		if((c>=65&&c<=90)||(c>=97&&c<=122)||(c>=48&&c<=57)||(c=='_'))
		{
			pos2 = int(content.find(valuename,valuename.length()+pos2));
			continue;
		}

		if(pos2>=int(endpos))
			break;
		int k = pos2-1;
		c = content.at(k);
		while(c=='*'||c==' '||c=='\t')
		{
			k--;
			c = content.at(k);
		}
		while((c>=65&&c<=90)||(c>=97&&c<=122)||(c>=48&&c<=57)||(c=='_'))
		{
			typeofname = content.at(k) + typeofname;
			k--;
			if(k<0)
				return "";
			c = content.at(k);
		}

		break;
	}
	return typeofname;
}

void CTestCaseCpp::doModeling ( )
{
	//cout<<"TestCase: "<<testcasename<<"  start modeling...."<<endl;
#ifdef WIN32
	runtime = ::GetTickCount();
#else
	gettimeofday(&runtime,NULL);
#endif

	if(READ_FROM_HRG)
		createHypergraphFromHrg(strTestPath+"//Hgraph.hgr");
	else
		createHypergraph();

	//int *part = new int[pHyper->nvtxs];
	
	//cout<<"TestCase: "<<testcasename<<"  end modeling...."<<endl;

	//cout<<"TestCase: "<<testcasename<<"  start cuting...."<<endl;

	

	doHmetis();
	
	

	//cout<<"TestCase: "<<testcasename<<"  end cuting...."<<endl;

#ifdef WIN32
	int now = runtime;
	runtime = ::GetTickCount();
	now = runtime - now;
	char gettime[100];
	sprintf_s(gettime,"%d",now);
	writeTime += "modeling and cuting time : " + string(gettime)+"ms\n";
	allTime += now;
#else
	timeval endtime;
    gettimeofday(&endtime,NULL);
    int now = 1000*(endtime.tv_sec - runtime.tv_sec) + (endtime.tv_usec - runtime.tv_usec)/1000; 
    char gettime[100];
    sprintf(gettime,"%d",now);
    writeTime += "modeling and cuting time : " + string(gettime)+"ms\n";
    allTime += now;
#endif

}

void CTestCaseCpp::doHmetis()
{
	int *hcut = &cutnum;
	vector<CHypergraph*> endParts;
	CMultilevel *pSolve = new CMultilevel();
	queue <CHypergraph*> subGraph;
	pHyper->floor = 1;
	pHyper->coupling =0;
	pHyper->cohesion =1;
	int nowfloor =1;
	float nowcoupling = 0;
	int couplingnum=0;
	float nowcohesion = 0;
	int cohesionnum = 0;
	pHyper->trueNvtxs = pHyper->nvtxs;
	pHyper->Modu = 0.0;
	subGraph.push(pHyper);

	double NumCnt = 0;
	double AllEdgeValue = 0;
	

	while(!subGraph.empty())
	{
		CHypergraph *p=subGraph.front();
		
		if(p->nvtxs>END_NUM-1)
		{
			p->Modu = 0.0;
			pSolve->myPartition(p,hcut,AllEdgeValue,1);
			if(p->floor==1)
				pMyGraph = pSolve->getMyGraph();
			if(p->is_parted)
			{
				CHypergraph *subG1,*subG2;
				subG1 = new CHypergraph();
				subG2 = new CHypergraph();
				createSubGraph(p,subG1,subG2);
			
				p->leftSon = subG1;
				p->rightSon = subG2;
				if(subG1->nvtxs!=0&&subG2->nvtxs!=0)
				{
					subG1->floor = p->floor+1;
					subG2->floor = p->floor+1;
					subGraph.push(subG1);
					subGraph.push(subG2);
				}
			}
			else
			{
				endParts.push_back(p);
			}
			/*
			HMETIS_PartRecursive(p->nvtxs, p->nhedges, p->vwgts, \
				p->eptr, p->eind, p->hewgts, p->npart, p->ubfactor, \
				p->inOptions, p->part, hcut);
			*/
			
			/* 先做二分，多层分割，后面再加上
			
			CHypergraph *subG1,*subG2;
			subG1 = new CHypergraph();
			subG2 = new CHypergraph();
			createSubGraph(p,subG1,subG2);
			
			p->leftSon = subG1;
			p->rightSon = subG2;
			if(subG1->nvtxs!=0&&subG2->nvtxs!=0)
			{
				subGraph.push(subG1);
				subGraph.push(subG2);
			}

			*/

			
		}
		else
		{
			endParts.push_back(p);
		}
		
		subGraph.pop();
	}

	if(pMyGraph==NULL)
		pMyGraph = new CMygraph(pHyper);

	mergeParts();

}

void CTestCaseCpp::mergeParts()
{
	CHypergraph *pGraphForWrite = pHyper;
	int partnum = dfsForWriteParts(pGraphForWrite,0);
	int **EdgesBetweenParts;
	EdgesBetweenParts = new int*[partnum];
	int *PartSize = new int[partnum+1];
    memset(PartSize,0,sizeof(int)*(partnum+1));

	for(int i=0;i<partnum;i++)
		EdgesBetweenParts[i] = new int[partnum];

	float maxCutEdge = -1;
	int SecmaxCutEdge = -1;
	int Cnt = partnum;
	while(1)
	{
		for(int i=0;i<partnum;i++)
			for(int j=0;j<partnum;j++)
				EdgesBetweenParts[i][j]=0;
		maxCutEdge = -1;
		SecmaxCutEdge = -1;

		for(int i=0;i<pMyGraph->nhedges;i++)
		{
			ArcNodeV *thisnode = pMyGraph->pAllEdges[i].firstarc;
			map<int,bool> PartExit;
			vector<int> vectorForParts;
			while(thisnode!=NULL)
			{
				int whichpart = pHyper->part[thisnode->node];
				map<int,int>::iterator myiter;
				while(1)
				{
					myiter = mapForLastPart.find(whichpart);
					if(myiter!=mapForLastPart.end())
						whichpart = myiter->second;
					else
						break;
				}
				map<int,bool>::iterator iter;
				iter = PartExit.find(whichpart);
				if(iter==PartExit.end())
				{
					for(int j=0;j<int(vectorForParts.size());j++)
					{
						int min = vectorForParts.at(j)>whichpart?whichpart:vectorForParts.at(j);
						int max = vectorForParts.at(j)>whichpart?vectorForParts.at(j):whichpart;
						EdgesBetweenParts[min][max] += pMyGraph->pAllEdges[i].Hweight;
					}
					vectorForParts.push_back(whichpart);
					PartExit.insert(pair<int,bool>(whichpart,true));
				}
				thisnode = thisnode->nextarc;
			}
		}
		
		memset(PartSize,0,sizeof(int)*(partnum+1));
		for(int i=0;i<pMyGraph->nvetes;i++)
		{
			int whichpart = pHyper->part[i];
			map<int,int>::iterator myiter;
			while(1)
			{
				myiter = mapForLastPart.find(whichpart);
				if(myiter!=mapForLastPart.end())
					whichpart = myiter->second;
				else
					break;
			}
			PartSize[whichpart] += pMyGraph->pAllVertices[i].Vweight;
		}

		int m=0,n=0;
		for(int i=0;i<partnum;i++)
		{
			int tmppart1 = i;
			map<int,int>::iterator myiter;
			while(1)
			{
				myiter = mapForLastPart.find(tmppart1);
				if(myiter!=mapForLastPart.end())
					tmppart1 = myiter->second;
				else
					break;
			}
			for(int j=i+1;j<partnum;j++)
			{
				int tmppart2 = j;
				while(1)
				{
					myiter = mapForLastPart.find(tmppart2);
					if(myiter!=mapForLastPart.end())
						tmppart2 = myiter->second;
					else
						break;
				}
				if(float(EdgesBetweenParts[tmppart1][tmppart2])/(PartSize[tmppart1]+PartSize[tmppart2])>maxCutEdge)
				{
					m = tmppart1;
					n = tmppart2;
					maxCutEdge = float(EdgesBetweenParts[tmppart1][tmppart2])/(PartSize[tmppart1]+PartSize[tmppart2]);
				}
			}
		}
		if(maxCutEdge>0.5)
		{
			mapForLastPart.insert(pair<int,int>(n,m));
			Cnt--;
		}
		else
			break;
	}
}
void CTestCaseCpp::createSubGraph(CHypergraph *G,CHypergraph *subG1,CHypergraph *subG2)
{
	subG1->innerEdgeValue = G->innerEdgeValueLeft;
	subG1->outEdgeValue = G->outEdgeValueLeft;
	subG2->innerEdgeValue = G->innerEdgeValueRight;
	subG2->outEdgeValue = G->outEdgeValueRight;

	subG1->fatherGraph = G;
	subG1->npart		 = 2;
	subG1->hewgts   = new int[G->nhedges];
	subG1->eptr		 = new int[G->nhedges+1];
	subG1->eind		 = new int[G->eptr[G->nhedges]];
	subG1->part        = new int[G->nvtxs];
	subG1->vwgts      = new int[G->nvtxs];
	subG1->inOptions = new int[9];
	subG1->ubfactor   = G->ubfactor;
	subG1->eptr[0] = 0;
	for(int i=0;i<9;i++)
	{
		subG1->inOptions[i] = G->inOptions[i];
	}

	subG2->fatherGraph = G;
	subG2->npart		 = 2;
	subG2->hewgts   = new int[G->nhedges];
	subG2->eptr		 = new int[G->nhedges+1];
	subG2->eind		 = new int[G->eptr[G->nhedges]];
	subG2->part        = new int[G->nvtxs];
	subG2->vwgts      = new int[G->nvtxs];
	subG2->inOptions = new int[9];
	subG2->ubfactor   = G->ubfactor;
	subG2->eptr[0] = 0;
	for(int i=0;i<9;i++)
	{
		subG2->inOptions[i] = G->inOptions[i];
	}

	int G1num=0,G2num=0;
	int eindnum_G1=0,eindnum_G2=0;
	int edgenumG1=0,edgenumG2=0;

	
	
	//按照超边处理

	//重新构建新的子超图

	int wightG1=0,wightG2=0,wightG1_G2=0;
	for(int i=0;i<G->nhedges;i++)
	{
		int startG1=-1,startG2=-1,endG1=-1,endG2=-1;
		bool allzero=true,allone=true;
		
		for(int j=G->eptr[i];j<G->eptr[i+1];j++)
		{
			
			if(G->part[G->eind[j]]==0)
			{
				map<int,int>::iterator iter;
				iter = G->mapForG1.find(G->eind[j]);
				if(iter!=G->mapForG1.end())
				{	
					subG1->eind[eindnum_G1] = iter->second;
					startG1 = startG1==-1?eindnum_G1:startG1;
					endG1 = eindnum_G1;
					eindnum_G1 ++;
				}
				else
				{
					G->mapForG1.insert(pair<int,int>(G->eind[j],G1num));
					subG1->mapForFather.insert(pair<int,int>(G1num,G->eind[j]));
					subG1->vwgts[G1num] = G->vwgts[G->eind[j]]; 
					subG1->eind[eindnum_G1] = G1num;
					startG1 = startG1==-1?eindnum_G1:startG1;
					endG1 = eindnum_G1;
					G1num++;
					eindnum_G1 ++;
					
				}
				allone = false;
			}
			else if(G->part[G->eind[j]]==1)
			{

				map<int,int>::iterator iter;
				iter = G->mapForG2.find(G->eind[j]);
				if(iter!=G->mapForG2.end())
				{	
					subG2->eind[eindnum_G2] = iter->second;
					startG2 = startG2==-1?eindnum_G2:startG2;
					endG2 = eindnum_G2;
					eindnum_G2 ++;
					
				}
				else
				{
					G->mapForG2.insert(pair<int,int>(G->eind[j],G2num));
					subG2->mapForFather.insert(pair<int,int>(G2num,G->eind[j]));
					subG2->eind[eindnum_G2] = G2num;
					subG2->vwgts[G2num] = G->vwgts[G->eind[j]]; 
					startG2 = startG2==-1?eindnum_G2:startG2;
					endG2 = eindnum_G2;
					G2num++;
					eindnum_G2 ++;
					
				}
				allzero = false;

			}
			else
				cout<<"cut error"<<endl;
		} //END FOR

		
		if(allzero)
		{
			edgenumG1++;
			subG1->eptr[edgenumG1] = endG1+1; 
			subG1->hewgts[edgenumG1-1] = G->hewgts[i];
			wightG1 += G->hewgts[i];
		}
		else if(allone)
		{
			edgenumG2++;
			subG2->eptr[edgenumG2] = endG2+1; 
			subG2->hewgts[edgenumG2-1] = G->hewgts[i];
			wightG2 += G->hewgts[i];
		}
		else
		{
			edgenumG1++;
			subG1->eptr[edgenumG1] = endG1+1; 
			subG1->hewgts[edgenumG1-1] = 0;
			edgenumG2++;
			subG2->eptr[edgenumG2] = endG2+1;
			subG2->hewgts[edgenumG2-1] = 0;
			wightG1_G2 += G->hewgts[i];
		}

	}
	if(wightG1+wightG2+wightG1_G2 ==0 )
	{
		subG1->coupling = 0;
		subG2->coupling = 0;
	}
	else
	{
		subG1->coupling = float(wightG1_G2)/float(wightG1+wightG2+wightG1_G2);
		subG2->coupling = float(wightG1_G2)/float(wightG1+wightG2+wightG1_G2);
	}
	if(wightG1==0)
	{
		subG1->cohesion = 0;
	}
	else
		subG1->cohesion = float(wightG1)/float(wightG1+wightG1_G2);
	if(wightG2==0)
		subG2->cohesion = 0;
	else
		subG2->cohesion = float(wightG2)/float(wightG2+wightG1_G2);
	subG1->nhedges = edgenumG1;
	subG2->nhedges = edgenumG2;
	subG1->nvtxs = G1num;
	subG2->nvtxs = G2num;
	

	//在原图基础上构建子超图

	/*
	int wightG1=0,wightG2=0,wightG1_G2=0;
	for(int i=0;i<G->nhedges;i++)
	{
		int startG1=-1,startG2=-1,endG1=-1,endG2=-1;
		bool allzero=true,allone=true;
		
		for(int j=G->eptr[i];j<G->eptr[i+1];j++)
		{
			
			if(G->part[G->eind[j]]==0)
			{
				subG1->eind[eindnum_G1] = G->eind[j];
				startG1 = startG1==-1?eindnum_G1:startG1;
				endG1 = eindnum_G1;
				eindnum_G1 ++;
				subG1->vwgts[G->eind[j]] = G->vwgts[G->eind[j]]; 
				subG2->vwgts[G->eind[j]] = 0; 
				
				allone = false;
			}
			else if(G->part[G->eind[j]]==1)
			{
				subG2->eind[eindnum_G2] = G->eind[j];
				startG2 = startG2==-1?eindnum_G2:startG2;
				endG2 = eindnum_G2;
				eindnum_G2 ++;
				subG1->vwgts[G->eind[j]] = 0; 
				subG2->vwgts[G->eind[j]] = G->vwgts[G->eind[j]]; 
				
				allzero = false;

			}
			else
				cout<<"cut error"<<endl;
		} //END FOR

		
		if(allzero)
		{
			edgenumG1++;
			subG1->eptr[edgenumG1] = endG1+1; 
			subG1->hewgts[edgenumG1-1] = G->hewgts[i];
			wightG1 += G->hewgts[i];
		}
		else if(allone)
		{
			edgenumG2++;
			subG2->eptr[edgenumG2] = endG2+1; 
			subG2->hewgts[edgenumG2-1] = G->hewgts[i];
			wightG2 += G->hewgts[i];
		}
		else
		{
			edgenumG1++;
			subG1->eptr[edgenumG1] = endG1+1; 
			subG1->hewgts[edgenumG1-1] = 0;
			edgenumG2++;
			subG2->eptr[edgenumG2] = endG2+1;
			subG2->hewgts[edgenumG2-1] = 0;
			wightG1_G2 += G->hewgts[i];
		}

	}
	
	subG1->nhedges = edgenumG1;
	subG2->nhedges = edgenumG2;
	subG1->nvtxs = G->nvtxs;
	subG2->nvtxs = G->nvtxs;

	for(int i=0;i<G->nvtxs;i++)
	{
		if(G->part[i]==0)
		{
			subG1->vwgts[i] = G->vwgts[i]; 
			subG2->vwgts[i] = 0; 
			if(subG1->vwgts[i]!=0)
				G1num++;
		}
		else if(G->part[i]==1)
		{
			subG1->vwgts[i] = 0; 
			subG2->vwgts[i] = G->vwgts[i];
			if(subG2->vwgts[i]!=0)
				G2num++;
		}
		else
			cout<<"error"<<endl;
	}
	subG1->trueNvtxs = G1num;
	subG2->trueNvtxs = G2num;
	*/

}
void CTestCaseCpp::createHypergraphFromHrg(string filename)
{
	dataIn = new CInput(filename.c_str(),0,0) ;
	pHyper = new CHypergraph(dataIn);
	pHyper->fatherGraph = NULL;

}
void CTestCaseCpp::createHypergraph()
{
	pHyper = new CHypergraph();

	pHyper->fatherGraph = NULL;

	int classnum = 1;
	int i=0;
	int eind_length = 0;
	string hyperstring="";
	string verticestring="";
	string answerstring="";
	map<string,calledclass*>::iterator iterat;
	for(iterat = calledrelation.begin();iterat!=calledrelation.end();++iterat)
	{
		map<string,int>::iterator ensurenum;
		ensurenum = classtonum.find(iterat->first);
		hyperstring += "1 ";
		if(ensurenum == classtonum.end())
		{
			classtonum.insert(pair<string,int>(iterat->first,classnum));
			char nownum[100];
			sprintf_s(nownum,"%d",classnum);
			verticestring += string(nownum)+"  "+iterat->first;
			VectorV.push_back(iterat->first);
			NodesResult.push_back(iterat->first);
			
			map<string,string>::iterator findfather;
			findfather = fathermap.find(iterat->first);
			if(findfather!=fathermap.end())
			{
				verticestring += "  father: ";
				verticestring += findfather->second;
			}
			
			verticestring += "\n";
			eind_length ++;
			hyperstring +=string(nownum)+" ";
			//hypergraph.write(nownum,strlen(nownum));
			//hypergraph.write(" ",1);

			classnum ++;
		}
		else
		{
			char nownum[100];
			sprintf_s(nownum,"%d",ensurenum->second);
			hyperstring +=string(nownum)+" ";
			eind_length ++;
			//hypergraph.write(nownum,strlen(nownum));
			//hypergraph.write(" ",1);
		}
		
		answerstring += iterat->first+"   ->   ";;
		
		calledclass *p = iterat->second;
		while(p!=NULL)
		{
			answerstring += p->name+"  ";
		

			ensurenum = classtonum.find(p->name);
			if(ensurenum == classtonum.end())
			{
				classtonum.insert(pair<string,int>(p->name,classnum));
				char nownum[100];
				sprintf_s(nownum,"%d",classnum);
				verticestring += string(nownum)+"  "+p->name+"\n";
				VectorV.push_back(p->name);
				NodesResult.push_back(p->name);

				hyperstring +=string(nownum)+" ";
				eind_length ++;
				//hypergraph.write(nownum,strlen(nownum));
				//hypergraph.write(" ",1);

				classnum ++;
			}
			else
			{
				char nownum[100];
				sprintf_s(nownum,"%d",ensurenum->second);
				hyperstring +=string(nownum)+" ";
				eind_length ++;
				//hypergraph.write(nownum,strlen(nownum));
				//hypergraph.write(" ",1);
			}
			p=p->next;
		}
		answerstring +="\n";
		hyperstring +="\n";
		//hypergraph.write("\n",1);
		
		i++;

	}

	char stredgenum[100],strverticenum[100];
	sprintf_s(stredgenum,"%d",i);
	sprintf_s(strverticenum,"%d",classnum-1);
	hyperstring = string(stredgenum) + " " + string(strverticenum) + " " +"11" +"\n" + hyperstring;
	for(int k=0;k<classnum-1;k++)
		hyperstring += "1\n";

	pHyper->hyperstring = hyperstring;
	pHyper->answerstring = answerstring;
	pHyper->verticestring = verticestring;
	pHyper->nhedges = i;
	pHyper->nvtxs = classnum-1;
	pHyper->hewgts = new int[i];
	for(int loop=0;loop<i;loop++)
		pHyper->hewgts[loop]=1;

	pHyper->eptr = new int[i+1];
	pHyper->eind = new int[eind_length];
	pHyper->vwgts = new int[classnum-1];
	pHyper->part = new int[classnum-1];
	for(int loop=0;loop<classnum-1;loop++)
		pHyper->vwgts[loop]=1;

	pHyper->inOptions = new int[9];

	int eindnum = 0;
	int edgenum = 0;
	for(iterat = calledrelation.begin();iterat!=calledrelation.end();++iterat)
	{
		map<string,int>::iterator ensurenum;
		ensurenum = classtonum.find(iterat->first);
		
		pHyper->eind[eindnum] = ensurenum->second-1;   //顶点编号从0开始
		pHyper->eptr[edgenum] = eindnum;

		eindnum++;
		
		calledclass *p = iterat->second;
		while(p!=NULL)
		{
			ensurenum = classtonum.find(p->name);
			
			pHyper->eind[eindnum] = ensurenum->second-1;
			eindnum++;
				
			p=p->next;
		}
		edgenum++;


	}
	
	pHyper->eptr[edgenum] = eindnum;


	int NumOfLineWord=0;
	string str[10];
	int lastpos = 0;
	int pos = int(strPara.find(" "));
	while(pos!=string::npos)
	{
		str[NumOfLineWord]=strPara.substr(lastpos,pos-lastpos);
		NumOfLineWord ++;
		lastpos = pos;
		pos = int(strPara.find(" ",pos+1));
	}
			
	switch(NumOfLineWord+1)
	{
		case 3:			pHyper->npart				=  atoi(str[1].c_str());
						pHyper->ubfactor			=  atoi(str[2].c_str());
						pHyper->inOptions[0]=0;
						pHyper->inOptions[1]=10;
						pHyper->inOptions[2]=1;
						pHyper->inOptions[3]=1;
						pHyper->inOptions[4]=1;
						pHyper->inOptions[5]=0;
						pHyper->inOptions[6]=0;
						pHyper->inOptions[7]=-1;
						pHyper->inOptions[8]=0;
						break;
		
		case 9:			pHyper->npart				=  atoi(str[1].c_str());
						pHyper->ubfactor			=  atoi(str[2].c_str());
						pHyper->inOptions[0]      =  1;
						pHyper->inOptions[1]		=  atoi(str[3].c_str());
						pHyper->inOptions[2]		=  atoi(str[4].c_str());                     
						pHyper->inOptions[3]		=  atoi(str[5].c_str());
						pHyper->inOptions[4]		=  atoi(str[6].c_str());
						pHyper->inOptions[5]		=  atoi(str[7].c_str());  
						pHyper->inOptions[6]		=  0;
						pHyper->inOptions[7]      =  -1;
						pHyper->inOptions[8]		=  atoi(str[8].c_str());	
						break;
				
		default:		cout<<"hypergraphes wrong format"<<endl;
			
	}
	


}

void CTestCaseCpp::doWriting ( )
{

	//cout<<"TestCase: "<<testcasename<<"  start writing...."<<endl;

#ifdef WIN32
	runtime = ::GetTickCount();
#else
	gettimeofday(&runtime,NULL);
#endif

	string outFoldPath = "..//output";
	#ifdef WIN32
	_mkdir(outFoldPath.c_str());
	#else
	if(NULL==opendir(outFoldPath.c_str()))
	{
		mkdir((outFoldPath.c_str()),0755);
	}
	#endif

	outFoldPath += "//";
	
	outFoldPath += testcasename;
	#ifdef WIN32
	_mkdir(outFoldPath.c_str());
	#else
	if(NULL==opendir(outFoldPath.c_str()))
	{
		mkdir((outFoldPath.c_str()),0755);
	}
	#endif
	
	ofstream ansfile,ansfile2,hypergraph,vertice,timefile,partition, excelFile;
	ansfile.open(string(outFoldPath+"//ans.txt").c_str());
	ansfile.write(lastans.c_str(),int(lastans.length()));
	ansfile.close();

	ansfile2.open(string(outFoldPath+"//relation.txt").c_str());
	hypergraph.open(string(outFoldPath+"//hypergraph.hgr").c_str());
	vertice.open(string(outFoldPath+"//vertice.txt").c_str());
	timefile.open(string(outFoldPath+"//time.txt").c_str());
	partition.open(string(outFoldPath+"//partition.txt").c_str());

	ansfile2.write(pHyper->answerstring.c_str(),int(pHyper->answerstring.length()));
	ansfile2.close();
	hypergraph.write(pHyper->hyperstring.c_str(),int(pHyper->hyperstring.length()));
	hypergraph.close();
	vertice.write(pHyper->verticestring.c_str(),int(pHyper->verticestring.length()));
	vertice.close();

	/*
	//* writing
	CHypergraph *pGraphForWrite = pHyper;
	dfsForWriteParts(pGraphForWrite,0);
	
	for(int i=0;i<pHyper->nvtxs;i++)
	{
		char strpart[128];
		sprintf_s(strpart,"%d",pHyper->part[i]);
		partition.write(strpart,int(strlen(strpart)));
		partition.write("\n",1);
	}
	partition.close();
	*/
	
	map<int,int> mapForWrite;
	int iCnt = 0;
	for(int i=0;i<pHyper->nvtxs;i++)
	{
		char strpart[128];
		int whichpart = pHyper->part[i];
		map<int,int>::iterator myiter;
		while(1)
		{
			myiter = mapForLastPart.find(whichpart);
			if(myiter!=mapForLastPart.end())
				whichpart = myiter->second;
			else
				break;
		}
		map<int,int>::iterator iter;
		iter = mapForWrite.find(whichpart);
		if(iter==mapForWrite.end())
		{
			mapForWrite.insert(pair<int,int>(whichpart,iCnt));
			whichpart = iCnt;
			iCnt ++;
		}
		else
			whichpart = iter->second;
		sprintf_s(strpart,"%d",whichpart);
		pHyper->part[i] = whichpart;
		partition.write(strpart,int(strlen(strpart)));
		
		PartsResult.push_back(whichpart);
		
		partition.write("\n",1);
	}
	partition.close();
	
	double GlobalQ = GetGlobalQ(pHyper,iCnt);



#ifdef WIN32
	int now = runtime;
	runtime = ::GetTickCount();
	now = runtime - now;
	char gettime[100];
	sprintf_s(gettime,"%d",now);
	writeTime += "writing time : " + string(gettime)+"ms\n";
	allTime += now;
#else
	timeval endtime;
    gettimeofday(&endtime,NULL);
    int now = 1000*(endtime.tv_sec - runtime.tv_sec) + (endtime.tv_usec - runtime.tv_usec)/1000; 
    char gettime[100];
    sprintf(gettime,"%d",now);
    writeTime += "writing time : " + string(gettime)+"ms\n";
    allTime += now;
#endif

	writeTime = "testcase name : " + strTestPath + "\n" + writeTime;


	timefile.write(writeTime.c_str(),int(writeTime.length()));
	timefile.close();

	string	filePathName = "..//recordsForExcel.txt";
	excelFile.open ( filePathName.c_str(), ios::app );
	//excelFile.write ( "file name \t hcut \t coupling \t cohesion \t time \n ", strlen("file name \t hcut \t coupling \t cohesion \t time \n ") );
	excelFile.write ( testcasename.c_str(), int(testcasename.length()) );
	excelFile.write("\t", 1 );
	char cpCutNum[128];
	sprintf_s ( cpCutNum, "%d", cutnum );
	excelFile.write ( cpCutNum, int(strlen(cpCutNum)) );
	excelFile.write("\t",1);
	excelFile.write ( "000000", int(strlen("000000")));
	excelFile.write ("\t",1);
	excelFile.write ("000000", 6 );
	excelFile.write ("\t",1);
	sprintf_s(gettime,"%d",allTime);
	excelFile.write (gettime, int(strlen(gettime)) );
	excelFile.write ("\n",1);
	excelFile.close();

	//cout<<"TestCase: "<<testcasename<<"  end writing...."<<endl;
	
	
	//vector<double> res = ComputeMoDuList(PartsResult);
	ofstream finalResFile;
	filePathName = "..//res.txt";
	finalResFile.open(filePathName.c_str(),ios::app);
	/*
	for(int i=0;i<res.size();i++)
	{
		char sValue[128] = {0};
		sprintf(sValue,"%4f",res[i]);
		finalResFile.write(sValue,strlen(sValue));
		finalResFile.write("\n",1);
	}
	*/
	finalResFile.write(testcasename.c_str(),testcasename.length());
	finalResFile.write("\t",1);
	char sValue1[128] = {0};
	sprintf(sValue1,"%d",iCnt);
	finalResFile.write(sValue1,strlen(sValue1));
	finalResFile.write("\t",1);
	char sValue2[128] = {0};
	sprintf(sValue2,"%.4f",GlobalQ);
	finalResFile.write(sValue2,strlen(sValue2));
	finalResFile.write("\n",1);

	finalResFile.close();
}

vector<double> CTestCaseCpp::ComputeMoDuList(vector<int> part_res)
{
	CMygraph *pInitGraph = new CMygraph(pHyper);
	int maxPart = 0;
	for(int i=0;i<part_res.size();i++)
	{
		maxPart = maxPart>part_res[i]?maxPart:part_res[i];
	}
	for(int i=0;i<=maxPart;i++)
		CohenList.push_back(0.0);
	double totalCompling = 0.0;
	for(int i=0;i<=maxPart;i++)
	{
		for(int j=i+1;j<=maxPart;j++)
		{
			double ModuNow = ComputeMoDuBtwTwoParts(part_res,pInitGraph,i,j);
			ModuList.push_back(ModuNow);
		}
		totalCompling += ComputeTotalCompling(part_res,pInitGraph,i);
	}

	int *inheritCnt = new int[maxPart+1];
	memset(inheritCnt,0,sizeof(int)*(maxPart+1));
	for(int i=0;i<part_res.size();i++)
	{
		map<string,string>::iterator iterForFather = fathermap.find(VectorV[i]);
		if(iterForFather==fathermap.end())
			continue;
		map<string,int>::iterator iterForNum = classtonum.find(iterForFather->second);
		if(iterForNum!=classtonum.end()&&part_res[i]!=part_res[iterForNum->second-1])
		{
			inheritCnt[part_res[i]] = 1;
			inheritCnt[part_res[iterForNum->second-1]] = 1;
		}
	}
	int totalInheritCnt = 0;
	for(int i=0;i<=maxPart;i++)
		totalInheritCnt += inheritCnt[i];

	int *SizeCnt = new int[maxPart+1];
	memset(SizeCnt,0,sizeof(int)*(maxPart+1));
	for(int i=0;i<part_res.size();i++)
	{
		SizeCnt[part_res[i]] += 1;
	}
	double variance = 0.0;
	for(int i=0;i<=maxPart;i++)
	{
		variance += (SizeCnt[i] - (double(part_res.size()) / double(maxPart+1))) * (SizeCnt[i] - (double(part_res.size()) / double(maxPart+1)));
	}
	double sizeModu = (double(part_res.size()) / double(maxPart+1)) / ((double(part_res.size()) / double(maxPart+1))+variance);
	double resCompling = totalCompling / (maxPart+1);
	double totalCohen = 0.0;
	for(int i=0;i<CohenList.size();i++)
	{
		totalCohen += CohenList[i];
	}
	double resInheritCompling = double(totalInheritCnt) / double(maxPart+1);
	double resCohen = totalCohen / (maxPart+1);
	double resModu = resCohen / (resCohen+resCompling);
	delete inheritCnt;
	delete SizeCnt;
	double dAns[] = {resCompling,resInheritCompling,resCohen,sizeModu,resModu};
	vector<double> res(dAns,dAns+5);
	return res;
}
double CTestCaseCpp::ComputeMoDuBtwTwoParts(vector<int> part_res,CMygraph *pInitGraph,int PartOne,int PartTwo)
{
	double innerEdgeValue[2];
	int innerVertCount[2];
	double cutEdgeValue = 0.0;
	int cutVertCount = 0;
	for(int i=0;i<2;i++)
	{
		innerVertCount[i]=0;
		innerEdgeValue[i] = 0.0;
	}
	int VerCount[2] = {0};
	for(int i=0;i<pInitGraph->nvetes;i++)
	{
		if(part_res[i]==PartOne)
			VerCount[0] ++;
		else if(part_res[i]==PartTwo)
			VerCount[1] ++;
	}
	map<int,bool> mapForInnerNodeCount;
	map<int,bool> mapForCutNodeCount;
	for(int i=0;i<pInitGraph->nhedges;i++)
	{
		bool cutedge = false;
		ArcNodeV *myvert = pInitGraph->pAllEdges[i].firstarc;
		//int part_num = part_res[myvert->node];
		int EdgeState = 0;
		while(myvert!=NULL)
		{
			if(part_res[myvert->node]==PartOne)
			{
				EdgeState |= 1;
				
			}
			else if(part_res[myvert->node]==PartTwo)
			{
				EdgeState |= 2;
			}
			else
				EdgeState |= 4;
			if(EdgeState%4==3)
			{
				cutedge = true;
				break;
			}
			myvert = myvert->nextarc;
		}
		if(cutedge==false&&EdgeState<4)
		{
			innerEdgeValue[EdgeState-1] += double(pInitGraph->pAllEdges[i].Hweight) * (1.0-double(1.0/pInitGraph->pAllEdges[i].nodeNum));
			ArcNodeV *pNode = pInitGraph->pAllEdges[i].firstarc;
			while(pNode!=NULL)
			{
				map<int,bool>::iterator myiter;
				myiter  = mapForInnerNodeCount.find(pNode->node);
				if(myiter==mapForInnerNodeCount.end())
				{
					mapForInnerNodeCount.insert(pair<int,bool>(pNode->node,true));
					innerVertCount[EdgeState-1] += pInitGraph->pAllVertices[pNode->node].Vweight;
				}
				pNode = pNode->nextarc;
			}
			//innerVertCount[part_num] += pInitGraph->pAllEdges[i].nodeNum;
		}
		else if(cutedge==true)
		{
			cutEdgeValue += double(pInitGraph->pAllEdges[i].Hweight);
			ArcNodeV *pNode = pInitGraph->pAllEdges[i].firstarc;
			while(pNode!=NULL)
			{
				map<int,bool>::iterator myiter;
				myiter  = mapForCutNodeCount.find(pNode->node);
				if(myiter==mapForCutNodeCount.end())
				{
					mapForCutNodeCount.insert(pair<int,bool>(pNode->node,true));
					cutVertCount ++;
				}
				pNode = pNode->nextarc;
			}
		}
	}
	double coupling = 0.0;
	double cohen = 0.0;
	if(VerCount[0]!=0&&VerCount[1]!=0)
	{
		coupling = (cutEdgeValue/VerCount[0]) + (cutEdgeValue/VerCount[1]); 
	}
	if(coupling<0.000001)
	{
		int a = 1;
	}
	if(VerCount[0]!=0)
		CohenList[PartOne] = (innerEdgeValue[0]/VerCount[0]);
	
	return coupling;
}
double CTestCaseCpp::ComputeTotalCompling(vector<int> part_res,CMygraph *pInitGraph,int PartOne)
{
	double innerEdgeValue[2];
	int innerVertCount[2];
	double cutEdgeValue = 0.0;
	int cutVertCount = 0;
	for(int i=0;i<2;i++)
	{
		innerVertCount[i]=0;
		innerEdgeValue[i] = 0.0;
	}
	int VerCount[2] = {0};
	for(int i=0;i<pInitGraph->nvetes;i++)
	{
		if(part_res[i]==PartOne)
			VerCount[0] ++;
		else
			VerCount[1] ++;
	}
	map<int,bool> mapForInnerNodeCount;
	map<int,bool> mapForCutNodeCount;
	for(int i=0;i<pInitGraph->nhedges;i++)
	{
		bool cutedge = false;
		ArcNodeV *myvert = pInitGraph->pAllEdges[i].firstarc;
		//int part_num = part_res[myvert->node];
		int EdgeState = 0;
		while(myvert!=NULL)
		{
			if(part_res[myvert->node]==PartOne)
			{
				EdgeState |= 1;
				
			}
			else
			{
				EdgeState |= 2;
			}
			if(EdgeState%4==3)
			{
				cutedge = true;
				break;
			}
			myvert = myvert->nextarc;
		}
		if(cutedge==false&&EdgeState<4)
		{
			innerEdgeValue[EdgeState-1] += double(pInitGraph->pAllEdges[i].Hweight) * (1.0-double(1.0/pInitGraph->pAllEdges[i].nodeNum));
			ArcNodeV *pNode = pInitGraph->pAllEdges[i].firstarc;
			while(pNode!=NULL)
			{
				map<int,bool>::iterator myiter;
				myiter  = mapForInnerNodeCount.find(pNode->node);
				if(myiter==mapForInnerNodeCount.end())
				{
					mapForInnerNodeCount.insert(pair<int,bool>(pNode->node,true));
					innerVertCount[EdgeState-1] += pInitGraph->pAllVertices[pNode->node].Vweight;
				}
				pNode = pNode->nextarc;
			}
			//innerVertCount[part_num] += pInitGraph->pAllEdges[i].nodeNum;
		}
		else if(cutedge==true)
		{
			cutEdgeValue += double(pInitGraph->pAllEdges[i].Hweight);
			ArcNodeV *pNode = pInitGraph->pAllEdges[i].firstarc;
			while(pNode!=NULL)
			{
				map<int,bool>::iterator myiter;
				myiter  = mapForCutNodeCount.find(pNode->node);
				if(myiter==mapForCutNodeCount.end())
				{
					mapForCutNodeCount.insert(pair<int,bool>(pNode->node,true));
					cutVertCount ++;
				}
				pNode = pNode->nextarc;
			}
		}
	}
	double coupling = 0.0;
	double cohen = 0.0;
	if(VerCount[0]!=0&&VerCount[1]!=0)
	{
		coupling = (cutEdgeValue/VerCount[0]) + (cutEdgeValue/VerCount[1]); 
	}
	
	return coupling;
}
int CTestCaseCpp::dfsForWriteParts(CHypergraph *G,int parts)
{
	if(G!=NULL)
	{
		CHypergraph *p=G->leftSon;
		CHypergraph *q=G->rightSon;
		CHypergraph *cur_Graph = G;
		if(p==NULL&&q==NULL)
		{
			for(int i=0;i<G->nvtxs;i++)
			{
				int node = i;
				cur_Graph = G;
				while(cur_Graph->fatherGraph!=NULL)
				{
					map<int,int>::iterator iter;
					iter = cur_Graph->mapForFather.find(node);
					node = iter->second;
					cur_Graph = cur_Graph->fatherGraph;
				}
				pHyper->part[node]=parts;
			}
			return 1;
		}
		int count = dfsForWriteParts(p,parts);
		count += dfsForWriteParts(q,parts+count);
		return count;
	}
	return 0;
}

double CTestCaseCpp::GetGlobalQ(CHypergraph *G,int part)
{
	double NumCnt = 0;
	double AllEdgeValue = 0;

	double *allc = new double[part];
	double *inc = new double[part];
	for(int i=0;i<part;i++)
	{
		allc[i] = 0.0;
		inc[i] = 0.0;
	}


	CMygraph *pG = new CMygraph(G);
	for(int i=0;i<pG->nhedges;i++)
	{
		NumCnt += pG->pAllEdges[i].nodeNum;
		AllEdgeValue += pG->pAllEdges[i].Hweight;
	}
	double a = NumCnt/pG->nhedges;
	int* part_res = G->part;

	for(int i=0;i<pG->nhedges;i++)
	{
		bool cutedge = false;
		ArcNodeV *myvert = pG->pAllEdges[i].firstarc;
		int part_num = part_res[myvert->node];
		int *cc = new int[part];
		memset(cc,0,sizeof(int)*part);
		cc[part_num] = 1;
		while(myvert!=NULL)
		{
			cc[part_res[myvert->node]] = 1;
			if(part_res[myvert->node]!=part_num)
			{
				cutedge = true;
			}
			myvert = myvert->nextarc;
		}
		if(cutedge==false)
		{
			inc[part_num] += double(pG->pAllEdges[i].Hweight);
			allc[part_num] += double(pG->pAllEdges[i].Hweight);
		}
		else
		{
			for(int i=0;i<part;i++)
			{
				if(cc[i]==1)
					allc[i] += double(pG->pAllEdges[i].Hweight);
			}
		}
	}

	double GlobalQ = 0.0;
	for(int i=0;i<part;i++)
	{
		GlobalQ += inc[i]/AllEdgeValue - pow(allc[i]/AllEdgeValue,a);
	}
	return GlobalQ;
}