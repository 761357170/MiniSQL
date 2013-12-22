//////////////////////////////////////////////////////////
///----------------------------------------------------///
///       Module: CatalogManager                       ///
///       Produced by: ����                            ///
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

//�������ݿ�
bool Create_Database(CString DB_Name);                                                                //0

//������
bool Create_Table(CString Table_Name,CString Attr,CString DB_Name,CString & Attr_Name);               //1

//��������
bool Create_Index(CString Index_Name,CString Table_Name,CString Attr_Name,
	              CString DB_Name,int & length,int & offset,int & type);                              //2

//ɾ�����ݿ�
bool Drop_Database(CString DB_Name);                                                                  //3

//ɾ����
bool Drop_Table(CString Table_Name,CString DB_Name,CString index_name[32],int & count);               //4

//ɾ������
bool Drop_Index(CString Index_Name,CString DB_Name);                                                  //5

//��ȡ��ʾ������Ϣ
bool Get_Attr_Info(CString DB_Name,CString Table_Name,attr_info print[32],int & count,CString Attr);  //6   

//��ȡ�������Ե���Ϣ
bool Get_Attr_Info_All(CString DB_Name,CString Table_Name,attr_info print[32],int & count);           //7

//ת��������
bool Get_Condition(CString DB_Name,CString Table_Name,CString Condition,                              //8
	condition_info conds[32],int & count,index_info nodes[32],int & num);                 

//ʹ�����ݿ�
bool Use_Database(CString DB_Name);                                                                   //9

//ִ��ָ���ļ�
void Exect_File(CString SQL,CString SQLS[SQL_MAX_NUM],int &num);                                      //10

//��֤������ֵ
bool verify_attr_value(CString Attr_Name, CString DB_Name ,CString Table_Name,CString & Attr,         //11
	                   index_info  nodes[32],int & num,attr_info  uniques[32],int & uniqueNum);                   

//��ȡһ�ű������е�������Ϣ
bool Get_All_Index(CString DB_Name, CString Table_Name, CString index_name[32],int & num);            //12

//��ȫ����ֵ
CString MakeValue(CString value,int length);                                                          //13

//�õ����Եĳ���
int FindLen(CString str);                                                                             //14

//ɾ��Ŀ¼�����ļ�
bool DeleteDirectory(char *DirName);                                                                  //15

//��ձ��ļ�
bool Clear_Table(CString DB_Name,CString Table_Name);                                                 //16

//��������ļ�
bool Clear_Index(CString DB_Name,CString Index_Name);                                                 //17