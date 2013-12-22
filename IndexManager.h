#ifndef _INDEXMANAGER_H_
#define _INDEXMANAGER_H_
#pragma once
#include "MiniSQL.h"
#include "BufferManager.h"
#include <list>
#include <string>
#include <vector>
#include "stdlib.h"
#define INDEX_N   15
using namespace std;



void delete_one(CString DB_Name, CString  index_name, index_info* inform,int flag); 
void insert_one(CString DB_Name, CString  index_name, index_info* inform);
int search_one(CString DB_Name, CString index_name,index_info * inform,int flag);
void search_many(CString DB_Name,CString index_name,int& start,int& end,int type, struct index_info* inform);
void insert_split(CString DB_Name, CString  index_name,struct index_info* inform, int leaf1,int leaf2);
CString int_to_str(int value);
CString float_to_str(float value);
int find_prev_sibling(CString DB_Name, CString  index_name, struct index_info *inform, int nodenum);
int find_next_sibling(CString DB_Name, CString  index_name,struct index_info* inform,int nodenum);
int find_left_child (CString DB_Name,CString index_name);
int find_right_child(CString DB_Name,CString index_name);
int get_new_freeblocknum(CString DB_Name, CString  index_name);
int find_father(CString DB_Name, CString  index_name,index_info* inform, int num);
//找出delete操作时merge两个节点所需要添加的索引值，即右节点的最左边的叶节点值
CString find_merge_value_left(CString DB_Name, CString  index_name,index_info* inform,int num);
//找出delete操作时merge两个节点所需要添加的索引值，即左节点的最右边的叶节点值
CString find_merge_value_right(CString DB_Name, CString  index_name,index_info* inform,int num);
CString str_to_str(CString value, int len);
void update_block(CString DB_Name,CString index_name,int blockNum,index_info* inform,CString w);
#endif