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

//定义表的索引节点信息
struct index_info
{
	CString index_name;
	int length;       // 长度  char(n):n      int:10         float:40
	int type;      //0---int,1---float,2----char(n)  
	long offset;       //在table文件里的偏移位置
	CString value;  // 值
	int isInsert;
};

//定义delete 语句的条件信息
struct condition_info             // 列 op 值
{
	int left_type;        //0---int,1---float,2----char(n)  
	int left_len;        
	long left_offset;
	CString left_index_name;   //没有的话，""
	CString const_data;          // 若有常数      //有就填写  直接值就好，不用加 ；或 '  '
	int op; // 1 :>   2:>=    3:<     4:<=   5:=  6:<>
};	

//定义select 语句显示各属性信息
struct attr_info
{
	CString attr_name;
	int offset;           //在所有属性中的偏移长度
	int length;         //固定长度
	int type;
	CString value;       //有就填写  直接值就好，不用加 ；或 '  '
};

#endif
