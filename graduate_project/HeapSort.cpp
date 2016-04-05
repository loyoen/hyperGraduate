
#include "HeapSort.h"
CHeapSort::CHeapSort()
{
	renew = true;
}
void CHeapSort::AdjustHeap(ModStruct *A, int hLen, int i,int *Node_Rank)
{
     int left = 2*i+1;  //�ڵ�i������
     int right = 2*i+2; //�ڵ�i���Һ��ӽڵ�
     int largest = i;
     double tempMod;
	 int tempNode;
  
     while(left < hLen || right < hLen)
     {
		if (left < hLen && A[largest].dMod <= A[left].dMod)
        {
			if(A[largest].dMod == A[left].dMod)
			{
				if(Node_Rank[A[largest].node]>Node_Rank[A[left].node])
					largest = left;
			}
			else
				largest = left;
        }
         
		if (right < hLen && A[largest].dMod <= A[right].dMod)
        {
			if(A[largest].dMod == A[right].dMod)
			{
				if(Node_Rank[A[largest].node]>Node_Rank[A[right].node])
					largest = right;
			}
			else
				largest = right;
        }
        if (i != largest)   //������ֵ���Ǹ��ڵ�
        {
			 tempMod = A[largest].dMod; //�������ڵ�ͺ�ӵ�����ֵ���ӽڵ㽻��
			 tempNode = A[largest].node;
			 A[largest].dMod = A[i].dMod;
			 A[largest].node = A[i].node;
			 A[i].dMod = tempMod;
			 A[i].node = tempNode;
             i = largest;         //�µĸ��ڵ㣬�Ա���������
             left = 2*i+1;  //�µ��ӽڵ�
             right = 2*i+2;
         }
         else
         {
            break;
         }
     }
}
void CHeapSort::BuildHeap(ModStruct *A, int hLen,int *Node_Rank)
{
	int i;
    int begin = hLen/2 - 1;  //���һ����Ҷ�ӽڵ�
    for (i = begin; i >= 0; i--)
    {
        AdjustHeap(A, hLen, i,Node_Rank);  
    }
}
void CHeapSort::HeapSort(ModStruct *A, int aLen,int *Node_Rank)
{
	/*
	int hLen = aLen;
    int temp;
    BuildHeap(A, hLen);      //���� 
    while (hLen > 1)
    {
        temp = A[hLen-1];    //�����ѵĵ�һ��Ԫ�غͶѵ����һ��Ԫ��
        A[hLen-1] = A[0];
        A[0] = temp;
        hLen--;        //�ѵĴ�С��һ
        AdjustHeap(A, hLen, 0);  //����
	}
	*/
}
void CHeapSort::getMax(ModStruct *A, int hLen,int *Node_Rank)
{
	double tempMod;
	int tempNode;
	if(renew==true)
	{
		BuildHeap(A, hLen,Node_Rank);      //���� 
		tempMod = A[hLen-1].dMod;    //�����ѵĵ�һ��Ԫ�غͶѵ����һ��Ԫ��
		tempNode = A[hLen-1].node;
		A[hLen-1].dMod = A[0].dMod;
		A[hLen-1].node = A[0].node;
		A[0].dMod = tempMod;
		A[0].node = tempNode;
		//hLen--;        //�ѵĴ�С��һ
		//AdjustHeap(A, hLen, 0,Node_Rank);  //����
		
	}
	else
	{
		/*
		tempMod = A[hLen-1].dMod;    //�����ѵĵ�һ��Ԫ�غͶѵ����һ��Ԫ��
		tempNode = A[hLen-1].node;
		A[hLen-1].dMod = A[0].dMod;
		A[hLen-1].node = A[0].node;
		A[0].dMod = tempMod;
		A[0].node = tempNode;
		hLen--;        //�ѵĴ�С��һ
		*/
		AdjustHeap(A, hLen, 0,Node_Rank);  //����
	}
}