//////////////////////////////////////////////////////////
///----------------------------------------------------///
///       Module: Interpreter                          ///
///       Produced by: 刘欢                            ///
///       Description: Read user's input and change it ///
///                    to another format that can be   ///
///                    recognized by Module API        ///
///       date: 2013/10/24                             ///
///----------------------------------------------------///
//////////////////////////////////////////////////////////
#if !defined(_INTERPRETER_H_)
#define _INTERPRETER_H_

#include"MiniSQL.h"

//读取用户输入
CString read_input();                                          //0

//获得属性
CString get_attribute(CString temp,CString sql);               //1

//获得条件
CString get_condition(CString temp,CString sql);               //2

//验证create database语句是否有效
CString create_database(CString SQL,int start);                //3   00

//验证create table语句是否有效
CString create_table(CString SQL,int start);                   //4   01

//验证create index on语句是否有效
CString create_index_on(CString SQL,int start,CString sql);    //5   02

//验证create index语句是否有效
CString create_index(CString SQL,int start);                   //6   

//验证create语句是否有效
CString create_clause(CString SQL,int start);                  //7

//验证drop database语句是否有效
CString drop_database(CString SQL,int start);                  //8   10

//验证drop table语句是否有效
CString drop_table(CString SQl,int start);                     //9   11

//验证drop index语句是否有效
CString drop_index(CString SQL,int start);                     //10  12

//验证drop语句是否有效
CString drop_clause(CString SQL,int start);                    //11

//验证 select from where 语句是否有效
CString selete_from_where(CString SQL,int start);              //12  22，23

//验证select 语句是否有效
CString select_clause(CString SQL,int start);                  //13  20，21

//验证insert into values语句是否有效
CString insert_into_values(CString SQL,int start);             //14  30

//验证insert 语句是否有效
CString insert_clause(CString SQL,int start);                  //15

//验证 delete from where 语句是否有效
CString delete_from_where(CString SQL,int start);              //16  41

//验证delete语句是否有效
CString delete_clause(CString SQL,int start);                  //17  40

//获得属性组或文件组的每一项
CString get_part(CString temp,CString sql,CString kind);       //18

//将表达式转化为内部形式
CString get_expression(CString temp);                          //19

//获取表达式组的每个表达式
CString get_each(CString sql,int &begin);                      //20

//验证use语句是否有效
CString use_clause(CString SQL,int start);                     //21  50

//验证execfile语句是否有效
CString execfile_clause(CString SQL,int start);                //22  60

//验证quit语句是否有效
CString quit_clause(CString SQL,int start);                    //23  70

//获取用户输入，并对输入作有效性检查，若正确，返回语句的内部表示形式
CString Interpreter(CString SQL);                              //24

//判断是否为字符串
bool is_string(CString temp);                                  //25

//判断是否为浮点数
bool is_float(CString temp);                                   //26

//判断是否为整数
bool is_int(CString temp);                                     //27

//判断格式是否正确
bool is_correct(CString temp);                                 //28

#endif