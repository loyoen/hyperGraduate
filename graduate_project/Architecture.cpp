// CArchitecture.cpp : Defines the class behaviors for the application.
/*
 * Project------ CArchitecture
 * Class Name--- CArchitecture.cpp
 * Author------- wxf891201@gmail.com
 * Date--------- 2013-4-8
 * Edition------ 1.0

 * Description-- MFC��APP, ʵ��ͬʱ��ָ���Ķ����׺����ͬ���ļ�

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
	 * �˴�testcases_menu�ļ���ÿ��ǰ�������path: ��Ϊ�����ڸ��к������file����Ŀ���ֿ���
	 * ���β�����û�п��Ƿֿ����������ļ���


	*/

	extern CLog *dataLog;
	dataLog=new CLog();

	fstream			pFileTxt;					// ���ڲ����������в����ļ�·�����Ǹ��ı��ļ�
	string			strFileName;				// �����ļ�(�������в����ļ���·�����ļ���)
	string			testFilePath;				// ÿһ�������ļ���·����
	string			testFileName;
	int			posPath;
	LPTESTCASE		pTestCase;
	
	fstream			pFileDir;
	string			fileName;
	string			fileNameTmp;
	string			filePath;

	vector < string >	ary_fileName;	// testcase�ļ���

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
			testFileName	= testFilePath.substr ( posPath + 5 );		// ��path:֮��ʼ����β���Ӵ���Ϊ·����
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

	//ÿ������֮ǰ�����漸�п������recordForExcel
	ofstream excelFile;
	string	filePathName = "..//recordsForExcel.txt";
	excelFile.open(filePathName.c_str());
	excelFile.close();



	excelFile.open ( filePathName.c_str(), ios::app );   //׷�ӷ�ʽ��
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



