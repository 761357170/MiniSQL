//////////////////////////////////////////////////////////////
///------------------------------------------------------  ///
///       Module: RecordManager                                     ///
///       Produced by:Zhang Xuewen                               ///
///       date: 2013/10/29                                             ///
///----------------------------------------------------///
/////////////////////////////////////////////////////////

#if !defined(_RECORDMANAGER_H_)
#define _RECORDMANAGER_H_

#include"MiniSQL.h"
#include <set>

#define MAX_RECORD_NUM 1000;


/**************************** ����ӿ� ****************************/

//�ͷ����ݿ���ڴ滺����
void Close_Database(CString DB_Name,bool closetype);

//�ͷű���������ڴ滺����
void Close_File(CString DB_Name,CString filename,int filetype);

//����в���Ԫ��
bool Insert_Item(CString DB_Name,CString Table_Name,CString Attr,index_info* nodes,int num,attr_info* uniques, int uniqueNum);

//ѡ����䣨��where��
void Select_No_Where(CString DB_Name,CString Table_Name,attr_info print[32],int count);

//ѡ����䣨��where��
void Select_With_Where(CString DB_Name,CString Table_Name,condition_info conds[10],int count,attr_info print[32],int Count);

//ɾ����䣨��where��
void Delete_With_Where(CString DB_Name,CString Table_Name,condition_info conds[10],int count,index_info nodes[32],int num);

//�������������ڵ�
void Insert_Index_All(CString DB_Name,CString Table_Name,CString Index_Name,int length,int offset,int type);



/*********************** ���ں��� ****************************/

void Delete_Nodes(CString DB_Name,CString record,int len,index_info node);

//�ô��ڻ���ڵ���������ѡ�����
bool Select_With_Greater_Cond(CString DB_Name,condition_info conds,int op,set<int> &addr,int i);

//��С�ڻ�С�ڵ���������ѡ�����
bool Select_With_Smaller_Cond(CString DB_Name,condition_info conds,int op,set<int> &addr,int i);

//�ж��Ƿ����ĳ��ָ������
bool Confirm(CString record,int len,condition_info condition);

#endif