//////////////////////////////////////////////////////////
///----------------------------------------------------///
///       Module: CatalogManager                       ///
///       Produced by: 刘欢                            ///
///       Description: Maintain the basic information  ///
///                    of the database including tables///
///                    and indices                     ///
///       date: 2013/10/24                             ///
///----------------------------------------------------///
//////////////////////////////////////////////////////////

#include"MiniSQL.h"

#define INT_LENGTH 10
#define FLOAT_LENGTH 40
#define SQL_MAX_NUM  4000

//创建数据库
bool Create_Database(CString DB_Name);                                                                //0

//创建表
bool Create_Table(CString Table_Name,CString Attr,CString DB_Name,CString & Attr_Name);               //1

//创建索引
bool Create_Index(CString Index_Name,CString Table_Name,CString Attr_Name,
	              CString DB_Name,int & length,int & offset,int & type);                              //2

//删除数据库
bool Drop_Database(CString DB_Name);                                                                  //3

//删除表
bool Drop_Table(CString Table_Name,CString DB_Name,CString index_name[32],int & count);               //4

//删除索引
bool Drop_Index(CString Index_Name,CString DB_Name);                                                  //5

//获取显示属性信息
bool Get_Attr_Info(CString DB_Name,CString Table_Name,attr_info print[32],int & count,CString Attr);  //6   

//获取所有属性的信息
bool Get_Attr_Info_All(CString DB_Name,CString Table_Name,attr_info print[32],int & count);           //7

//转换条件组
bool Get_Condition(CString DB_Name,CString Table_Name,CString Condition,                              //8
	condition_info conds[32],int & count,index_info nodes[32],int & num);                 

//使用数据库
bool Use_Database(CString DB_Name);                                                                   //9

//执行指定文件
void Exect_File(CString SQL,CString SQLS[SQL_MAX_NUM],int &num);                                      //10

//验证各属性值
bool verify_attr_value(CString Attr_Name, CString DB_Name ,CString Table_Name,CString & Attr,         //11
	                   index_info  nodes[32],int & num,attr_info  uniques[32],int & uniqueNum);                   

//获取一张表中所有的索引信息
bool Get_All_Index(CString DB_Name, CString Table_Name, CString index_name[32],int & num);            //12

//补全插入值
CString MakeValue(CString value,int length);                                                          //13

//得到属性的长度
int FindLen(CString str);                                                                             //14

//删除目录及子文件
bool DeleteDirectory(char *DirName);                                                                  //15

//清空表文件
bool Clear_Table(CString DB_Name,CString Table_Name);                                                 //16

//清空索引文件
bool Clear_Index(CString DB_Name,CString Index_Name);                                                 //17