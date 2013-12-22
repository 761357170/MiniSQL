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


/**************************** 对外接口 ****************************/

//释放数据库的内存缓冲区
void Close_Database(CString DB_Name,bool closetype);

//释放表或索引的内存缓冲区
void Close_File(CString DB_Name,CString filename,int filetype);

//向表中插入元组
bool Insert_Item(CString DB_Name,CString Table_Name,CString Attr,index_info* nodes,int num,attr_info* uniques, int uniqueNum);

//选择语句（无where）
void Select_No_Where(CString DB_Name,CString Table_Name,attr_info print[32],int count);

//选择语句（有where）
void Select_With_Where(CString DB_Name,CString Table_Name,condition_info conds[10],int count,attr_info print[32],int Count);

//删除语句（有where）
void Delete_With_Where(CString DB_Name,CString Table_Name,condition_info conds[10],int count,index_info nodes[32],int num);

//插入所有索引节点
void Insert_Index_All(CString DB_Name,CString Table_Name,CString Index_Name,int length,int offset,int type);



/*********************** 对内函数 ****************************/

void Delete_Nodes(CString DB_Name,CString record,int len,index_info node);

//用大于或大于等于索引的选择语句
bool Select_With_Greater_Cond(CString DB_Name,condition_info conds,int op,set<int> &addr,int i);

//用小于或小于等于索引的选择语句
bool Select_With_Smaller_Cond(CString DB_Name,condition_info conds,int op,set<int> &addr,int i);

//判断是否符合某个指定条件
bool Confirm(CString record,int len,condition_info condition);

#endif