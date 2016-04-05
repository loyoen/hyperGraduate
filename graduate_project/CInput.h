#ifndef CINPUT_H
#define CINPUT_H
#include <iostream>
#define themax 10000000
using namespace std;
class CInput
{
public:

	int *part;
	int nvtxs;  //number of vertices
    int nhedges;  //number of hyperedges
	char fp[1024];
	char fp_fixfile[1024];
	int npart;
	int ubfactor;
	int nruns;
	int ctype;
	int rtype;
	int vcycle;
	int reconst;
	int dbglvl;
	int type;    //ȷ����ͼ�ļ�����   ������Ȩ��  ����Ȩ��
	int eind[themax];
	int *eptr;
	int *weight;
	int *vertice;//����Ȩ��
	int option[9];
	bool swit; //���ֵ����ĸ����캯��
	int switOption6;//����������û��fixfile
	int eind_length;
public:
	CInput(const char _fp[],int _npart,int _ubfactor);
	CInput(char _fp[],int _npart,int _ubfactor,int _nruns,int _ctype,int _rtype,int _vcycle,int _reconst,int _dbglvl);
	CInput(char _fp[],char _FixFile[],int _npart,int _ubfactor);
	CInput(char _fp[],char _FixFile[],int _npart,int _ubfactor,int _nruns,int _ctype,int _rtype,int _vcycle,int _reconst,int _dbglvl);
    ~CInput();
	bool CInput_readfile();
    bool CInput_check();
	bool CInput_readFixFile();
	bool belongto(int require,int min,int max);
	void MakeWeight();
	void print(void);
	
	int get_nvtxs();
	int get_nhedges();
	int* get_vwgts();
	int* get_eptr();
	int* get_eind();
	int* get_hewgts();
	int get_nparts();
	int get_ubfactor();
	int* get_part();
	int get_eind_length();
	int* get_option();
};
bool beblogto(int requair,int min,int max);
#endif

