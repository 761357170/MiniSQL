#if !defined(_MINISQL_H_)
#define _MINISQL_H_

#include<iostream>
#include<afx.h>
#include<stdlib.h>
#include<math.h>
#include<direct.h>

using namespace std;

#define FILE_COUNTER 1
#define FILE_INFO_LENGTH 671
#define ATTR_INFO_LENGTH 20
#define FILE_HEAD_LENGTH 31
#define INDEX_OFFSET        7      // the max length of  index::offset in table

//�����������ڵ���Ϣ
struct index_info
{
	CString index_name;
	int length;       // ����  char(n):n      int:10         float:40
	int type;      //0---int,1---float,2----char(n)  
	long offset;       //��table�ļ����ƫ��λ��
	CString value;  // ֵ
	int isInsert;
};

//����delete ����������Ϣ
struct condition_info             // �� op ֵ
{
	int left_type;        //0---int,1---float,2----char(n)  
	int left_len;        
	long left_offset;
	CString left_index_name;   //û�еĻ���""
	CString const_data;          // ���г���      //�о���д  ֱ��ֵ�ͺã����ü� ���� '  '
	int op; // 1 :>   2:>=    3:<     4:<=   5:=  6:<>
};	

//����select �����ʾ��������Ϣ
struct attr_info
{
	CString attr_name;
	int offset;           //�����������е�ƫ�Ƴ���
	int length;         //�̶�����
	int type;
	CString value;       //�о���д  ֱ��ֵ�ͺã����ü� ���� '  '
};

#endif
