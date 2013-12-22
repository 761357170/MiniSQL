//////////////////////////////////////////////////////////
///----------------------------------------------------///
///       Module: Interpreter                          ///
///       Produced by: ����                            ///
///       Description: Read user's input and change it ///
///                    to another format that can be   ///
///                    recognized by Module API        ///
///       date: 2013/10/24                             ///
///----------------------------------------------------///
//////////////////////////////////////////////////////////
#if !defined(_INTERPRETER_H_)
#define _INTERPRETER_H_

#include"MiniSQL.h"

//��ȡ�û�����
CString read_input();                                          //0

//�������
CString get_attribute(CString temp,CString sql);               //1

//�������
CString get_condition(CString temp,CString sql);               //2

//��֤create database����Ƿ���Ч
CString create_database(CString SQL,int start);                //3   00

//��֤create table����Ƿ���Ч
CString create_table(CString SQL,int start);                   //4   01

//��֤create index on����Ƿ���Ч
CString create_index_on(CString SQL,int start,CString sql);    //5   02

//��֤create index����Ƿ���Ч
CString create_index(CString SQL,int start);                   //6   

//��֤create����Ƿ���Ч
CString create_clause(CString SQL,int start);                  //7

//��֤drop database����Ƿ���Ч
CString drop_database(CString SQL,int start);                  //8   10

//��֤drop table����Ƿ���Ч
CString drop_table(CString SQl,int start);                     //9   11

//��֤drop index����Ƿ���Ч
CString drop_index(CString SQL,int start);                     //10  12

//��֤drop����Ƿ���Ч
CString drop_clause(CString SQL,int start);                    //11

//��֤ select from where ����Ƿ���Ч
CString selete_from_where(CString SQL,int start);              //12  22��23

//��֤select ����Ƿ���Ч
CString select_clause(CString SQL,int start);                  //13  20��21

//��֤insert into values����Ƿ���Ч
CString insert_into_values(CString SQL,int start);             //14  30

//��֤insert ����Ƿ���Ч
CString insert_clause(CString SQL,int start);                  //15

//��֤ delete from where ����Ƿ���Ч
CString delete_from_where(CString SQL,int start);              //16  41

//��֤delete����Ƿ���Ч
CString delete_clause(CString SQL,int start);                  //17  40

//�����������ļ����ÿһ��
CString get_part(CString temp,CString sql,CString kind);       //18

//�����ʽת��Ϊ�ڲ���ʽ
CString get_expression(CString temp);                          //19

//��ȡ���ʽ���ÿ�����ʽ
CString get_each(CString sql,int &begin);                      //20

//��֤use����Ƿ���Ч
CString use_clause(CString SQL,int start);                     //21  50

//��֤execfile����Ƿ���Ч
CString execfile_clause(CString SQL,int start);                //22  60

//��֤quit����Ƿ���Ч
CString quit_clause(CString SQL,int start);                    //23  70

//��ȡ�û����룬������������Ч�Լ�飬����ȷ�����������ڲ���ʾ��ʽ
CString Interpreter(CString SQL);                              //24

//�ж��Ƿ�Ϊ�ַ���
bool is_string(CString temp);                                  //25

//�ж��Ƿ�Ϊ������
bool is_float(CString temp);                                   //26

//�ж��Ƿ�Ϊ����
bool is_int(CString temp);                                     //27

//�жϸ�ʽ�Ƿ���ȷ
bool is_correct(CString temp);                                 //28

#endif