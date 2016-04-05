// CArchitecture.cpp : Defines the class behaviors for the application.
/*
 * Project------ CArchitecture
 * Class Name--- CArchitecture.cpp
 * Author------- wxf891201@gmail.com
 * Date--------- 2013-4-8
 * Edition------ 1.0

 * Description-- MFC的APP, 实现同时打开指定的多个后缀名相同的文件

 * Change Log:
 *		Date-----
 *		Staff----
 *		Edition--
 *		Content--
 */


#include "Architecture.h"
#include <iostream>

#include "CLog.h"

#ifdef WIN32
	#ifdef __cplusplus
	extern "C" 
	#endif
	FILE _iob[3] = {__iob_func()[0], __iob_func()[1], __iob_func()[2]};
#endif


CArchitectureApp::CArchitectureApp()
{
	m_pMgrJob = NULL;

}

// CArchitectureApp message handlers
void* loopJob(void* pParam)
{
	CMgrJob* loopjob;
	loopjob = (CMgrJob*)pParam;
//	CLogNote("MGRJOB THREAD LOOPJOB");
	return loopjob->dwLoopWork();
}

CLog *dataLog;
void *CLogThread(void* pParam)
{
	CLog *CLogwork;
	CLogwork = (CLog *)pParam;
	return CLogwork->loop();

}

void* StopThread(void* pParam)
{
	CMgrJob* stopjob;
	stopjob = (CMgrJob*)pParam;
	
	if(stopjob!=NULL)
	{
		stopjob->setLoopWorking();
	}
	return NULL;
}

void* IOThread(void* pParam)
{
	CMgrJob* IOjob;
	IOjob = (CMgrJob*)pParam;
//	CLogNote("MGRJOB THREAD LOOPJOB");
	return IOjob->dwLoopIOWork();
}

void CArchitectureApp::stopThreads()
{
	CThread*			cpStopThread;
	cpStopThread		= new CThread( StopThread, m_pMgrJob );
}
void CArchitectureApp::OnParserOpenfiles()
{
	/*
	 * 此处testcases_menu文件中每行前面加上了path: 是为了与在该行后面加上file等条目区分开，
	 * 本次测试中没有考虑分开给出测试文件名


	*/

	extern CLog *dataLog;
	dataLog=new CLog();

	fstream			pFileTxt;					// 用于操作存有所有测试文件路径的那个文本文件
	string			strFileName;				// 配置文件(包含所有测试文件的路径及文件名)
	string			testFilePath;				// 每一个测试文件的路径及
	string			testFileName;
	int			posPath;
	LPTESTCASE		pTestCase;
	
	fstream			pFileDir;
	string			fileName;
	string			fileNameTmp;
	string			filePath;

	vector < string >	ary_fileName;	// testcase文件名

	strFileName		= "..//testcases_menu.txt";
	testFilePath	= "";
	testFileName	= "";
	posPath			= 0;
	pTestCase		= NULL;
	fileNameTmp		= "";
	filePath		= "";
	//_CrtSetBreakAlloc(4577);
	pFileTxt.open ( strFileName.c_str() ) ;
	while ( getline ( pFileTxt, testFilePath ) )
	{
		posPath	= int(testFilePath.find ( "path:" ));
		if ( -1 != posPath )
		{
			testFileName	= testFilePath.substr ( posPath + 5 );		// 从path:之后开始到结尾的子串均为路径名
			ary_fileName.push_back(testFileName);
			//pTestCase		= new CTestCase ( m_TestIndex, testFileName );
			//pTestCase->doParsing ( pTestCase );
			//m_pMgrJob->addJob(pTestCase);
			//m_pTestcase->doModeling ( m_pTestcase ); 

			//m_pTestcase->m_pNext_test	= m_pTestHead;
			//m_pTestHead					= m_pTestcase;

			//m_TestIndex	++;
		}		
	}
	pFileTxt.close ( );
	m_pMgrJob	= new CMgrJob ( );
	for ( int i=0; i < int(ary_fileName.size()); i++ )
	{
		pTestCase		= new CTestCaseCpp ( int(i), ary_fileName.at(i) );
		m_pMgrJob->addIOJob(pTestCase);
	}


// added by loyoen 

	//每次运行之前，下面几行可以清空recordForExcel
	ofstream excelFile;
	string	filePathName = "..//recordsForExcel.txt";
	excelFile.open(filePathName.c_str());
	excelFile.close();



	excelFile.open ( filePathName.c_str(), ios::app );   //追加方式打开
	excelFile.write ( "file name \t hcut \t coupling \t cohesion \t time(\\ms) \n", int(strlen("file name \t hcut \t coupling \t cohesion \t time(\\ms) \n")) );
	excelFile.close();
	CThread*			cpInputJobThread;
	CThread*			cpDealThread;
	CThread*			cpDealThread2;
	CThread*			cpCLogThread;
	//CThread*			cpStopThread;
	
	cpCLogThread = new CThread(CLogThread,dataLog);
	cpInputJobThread	= new CThread ( IOThread, m_pMgrJob );
	cpDealThread		= new CThread ( loopJob, m_pMgrJob );
	cpDealThread2		= new CThread ( loopJob, m_pMgrJob );
	//cpStopThread		= new CThread( StopThread, m_pMgrJob );

	cpInputJobThread->wait4ThisThread();
	cpDealThread->wait4ThisThread();
	cpDealThread2->wait4ThisThread();

	delete cpInputJobThread;
	delete cpDealThread;
	delete cpDealThread2;
	delete m_pMgrJob;
    sleep(2);
#ifndef WIN32
    cpDealThread->wait4ThisThread();
    sleep(2);
#endif

}



