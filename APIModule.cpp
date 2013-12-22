#include"APIModule.h"
#include"Interpreter.h"
#include"CatalogManager.h"
#include"RecordManager.h"
#include <iostream>

using namespace std;

CString DB_Name;

void APIModule(CString SQL)
{
	CString Type,Attr,Index_Name,Table_Name,Attr_Name,Condition,index_name[32];
	int index,end,length,offset,type,count,num;
	index_info nodes[32];
	condition_info conds[10];
	attr_info print[32];
	bool ok;
	Type=SQL.Left(2);
	SQL=SQL.Mid(2,SQL.GetLength()-2);

	try {
		/////////////////////////////////////////////////////////////////////////////////////////
		//创建数据库
		if(Type=="00")
			Create_Database(SQL);
	
		/////////////////////////////////////////////////////////////////////////////////////////
		//创建数据表
		else if(Type=="01")
		{
			if(DB_Name.IsEmpty())
				cout<<"error: you have not specified the database to be used!"<<endl;
			else
			{
				index=SQL.Find(',');
				Table_Name=SQL.Left(index);
				Attr=SQL.Right(SQL.GetLength()-index-1);
				Attr_Name.Empty();
				//创建数据表
				if (Create_Table(Table_Name,Attr,DB_Name,Attr_Name)) {
					//判断是否创建主键索引
					if(!Attr_Name.IsEmpty())
						Create_Index(Attr_Name,Table_Name,Attr_Name,DB_Name,length,offset,type);           //主键索引 主键.idx   
				}
			}		
		}

		/////////////////////////////////////////////////////////////////////////////////////////
		//创建索引
		else if(Type=="02")
		{
			if(DB_Name.IsEmpty())
				cout<<"error: you have not specified the database to be used!"<<endl;
			else
			{
				index=SQL.Find(' ');
				//获取索引名
				Index_Name=SQL.Left(index);
				index++;
				end=SQL.Find(' ',index);
				//获取表名
				Table_Name=SQL.Mid(index,end-index);
				//获取属性名
				Attr=SQL.Right(SQL.GetLength()-end-1);
				if(Create_Index(Index_Name,Table_Name,Attr,DB_Name,length,offset,type)) {        //offset Attr的偏移
					//插入所有索引节点
					if(length!=-1)
						Insert_Index_All(DB_Name,Table_Name,Index_Name,length,offset,type);	
				}
			}
		}

		/////////////////////////////////////////////////////////////////////////////////////////
		//删除数据库
		else if(Type=="10")
		{
			if(SQL==DB_Name)
			{
				//释放数据库的内存
				Close_Database(DB_Name,false);
				DB_Name.Empty();
			}
			Drop_Database(SQL);
		}

		/////////////////////////////////////////////////////////////////////////////////////////
		//删除数据表
		else if(Type=="11")
		{
			if(DB_Name.IsEmpty())
				cout<<"error: you have not specified the database to be used!"<<endl;
			else
			{
				Table_Name=SQL;
				//释放表的内存
				Close_File(DB_Name,Table_Name,0);
				//删除表文件
				if (Drop_Table(Table_Name,DB_Name,index_name,count)) {
					//释放表中所有索引的内存
					for(index=0;index<count;index++) {
						Close_File(DB_Name,index_name[index],1);
						Drop_Index(index_name[index],DB_Name);
					}
				}
			}
		}

		/////////////////////////////////////////////////////////////////////////////////////////
		//删除索引
		else if(Type=="12")
		{
			if(DB_Name.IsEmpty())
				cout<<"error: you have not specified the database to be used!"<<endl;
			else
			{
				Index_Name=SQL;
				//删除索引的内存
				Close_File(DB_Name,Index_Name,1);
				Drop_Index(Index_Name,DB_Name);
			}
		}

		/////////////////////////////////////////////////////////////////////////////////////////
		//选择语句(无where子句)
		else if(Type=="20")
		{
			if(DB_Name.IsEmpty())
				cout<<"error: you have not specified the database to be used!"<<endl;
			else
			{
				//获取表名
				Table_Name=SQL;
				if(Table_Name.Find(',')!=-1)
					cout<<"error: can not select from more than one table!"<<endl;
				else
				{
					//获取显示记录格式
					if (Get_Attr_Info_All(DB_Name,Table_Name,print,count)) {            //count:  ATTR个数
						if(count!=0)
							Select_No_Where(DB_Name,Table_Name,print,count);
					}
				}
			}		
		}

		/////////////////////////////////////////////////////////////////////////////////////////
		//选择列语句(无where子句)
		else if(Type=="21")
		{
			if(DB_Name.IsEmpty())
				cout<<"error: you have not specified the database to be used!"<<endl;
			else
			{
				//获取选择属性组
				index=SQL.Find(',');
				Table_Name = SQL.Left(index);
				if(Table_Name.Find(',')!=-1)
					cout<<"error: can not select from more than one table!"<<endl;
				else
				{
					Attr=SQL.Right(SQL.GetLength()-index-1);
					Get_Attr_Info(DB_Name,Table_Name,print,count,Attr);	
					if(count!=0)
						Select_No_Where(DB_Name,Table_Name,print,count);
				}
			}		
		}

		/////////////////////////////////////////////////////////////////////////////////////////
		//选择语句(有where子句)
		else if(Type=="22")
		{
			if(DB_Name.IsEmpty())
				cout<<"error: you have not specified the database to be used!"<<endl;
			else
			{
				//获取表名
				index=SQL.Find(',');
				Table_Name = SQL.Left(index);
				Condition=SQL.Right(SQL.GetLength()-index-1);
				if(Table_Name.Find(',')!=-1)
					cout<<"error: can not select from more than one table!"<<endl;
				else
				{
					//获取显示记录格式
					int counts=0;
					if(Get_Attr_Info_All(DB_Name,Table_Name,print,counts)) {
						if (Get_Condition(DB_Name,Table_Name,Condition,conds,count,nodes,num)) {  //count :条件个数   num:属性个数  conds:有索引的条件在前面
							if(count!=0)
								Select_With_Where(DB_Name,Table_Name,conds,count,print,counts);   
						}
					}
				}
			}
		}

		/////////////////////////////////////////////////////////////////////////////////////////
		//选择列语句(有where子句)
		else if(Type=="23") {
			if(DB_Name.IsEmpty())
				cout<<"error: you have not specified the database to be used!"<<endl;
			else
			{
				//获取表名
				index=SQL.Find(',');
				Table_Name = SQL.Left(index);
				SQL=SQL.Right(SQL.GetLength()-index-1);
				index=SQL.Find(',');
				int start=index+1;
				CString AttrNum = SQL.Left(index);
				int attrNum = _ttoi(AttrNum);
				for(int i=0;i<attrNum; ++i)
				{
					index=SQL.Find(',',start);
					start=index+1;
				}
				Attr = SQL.Left(start);
				Condition=SQL.Right(SQL.GetLength()-index-1);
				if(Table_Name.Find(',')!=-1)
					cout<<"error: can not select from more than one table!"<<endl;
				else
				{
					//获取显示记录格式
					int counts =0;
					if(Get_Attr_Info(DB_Name,Table_Name,print,counts,Attr)) {
						if(Get_Condition(DB_Name,Table_Name,Condition,conds,count,nodes,num)) {  //count :条件个数   num:属性个数   conds : 有索引放在前面
							if(count!=0)
								Select_With_Where(DB_Name,Table_Name,conds,count,print,counts);
						}
					}
				}
			}
		}

		/////////////////////////////////////////////////////////////////////////////////////////
		//insert语句
		else if(Type=="30")
		{
			if(DB_Name.IsEmpty())
				cout<<"error: you have not specified the database to be used!"<<endl;
			else
			{
				//获取表名
				index=SQL.Find(',');
				Table_Name = SQL.Left(index);
				if(Table_Name.Find(',')!=-1)
					cout<<"error: can not select from more than one table!"<<endl;
				else
				{
					Attr_Name=SQL.Right(SQL.GetLength()-index-1);
					attr_info uniques[32];
					int uniqueNum;
					if (verify_attr_value(Attr_Name,DB_Name,Table_Name,Attr,nodes,num,uniques,uniqueNum) ) {   //attr_info* uniques, int uniqueNum
						ok = Insert_Item(DB_Name,Table_Name,Attr,nodes,num,uniques,uniqueNum);        
					}
				} 
			}
		}

		/////////////////////////////////////////////////////////////////////////////////////////
		//delete语句(无where子句)
		else if(Type=="40")
		{
			if(DB_Name.IsEmpty())
				cout<<"error: you have not specified the database to be used!"<<endl;
			else
			{
				//获取表名
				Table_Name = SQL;
				if(Table_Name.Find(',')!=-1)
					cout<<"error: can not select from more than one table!"<<endl;
				else
				{
					Close_File(DB_Name,Table_Name,0);
					if(Clear_Table(DB_Name,Table_Name)) {     //清空table内容
						CString indices[32];
						if(Get_All_Index(DB_Name,Table_Name,indices,num)) {
							for(int i =0; i<num; i++) {
								Close_File(DB_Name,indices[i],1);
								Clear_Index(DB_Name,indices[i]);          //删除索引的所有节点，即把索引清空
							}
						}
					}
				}
			}
		}

		/////////////////////////////////////////////////////////////////////////////////////////
		//delete语句(有where子句)
		else if(Type=="41")
		{
			if(DB_Name.IsEmpty())
				cout<<"error: you have not specified the database to be used!"<<endl;
			else
			{
				//获取表名
				index=SQL.Find(',');
				Table_Name = SQL.Left(index);
				Condition=SQL.Right(SQL.GetLength()-index-1);
				if(Table_Name.Find(',')!=-1)
					cout<<"error: can not select from more than one table!"<<endl;
				else
				{
					//获取显示记录格式
					if (Get_Condition(DB_Name,Table_Name,Condition,conds,count,nodes,num)) {              //count :条件个数   num:属性个数   conds : 有索引放在前面
						if(count!=0)
							Delete_With_Where(DB_Name,Table_Name,conds,count,nodes,num);
					}
				}
			}
		}

		/////////////////////////////////////////////////////////////////////////////////////////
		//use语句
		else if(Type=="50")
		{
			if (Use_Database(SQL) ) {
				if(!DB_Name.IsEmpty())
					Close_Database(DB_Name,false);
				DB_Name = SQL;
				cout << "Database has changed to " + DB_Name << endl;
			} else {
				cout << "No database " + DB_Name << endl;
			}
			
		}

		/////////////////////////////////////////////////////////////////////////////////////////
		//execfile语句
		else if(Type=="60")
		{
			CString SQLS[SQL_MAX_NUM];
			int num;
			Exect_File(SQL,SQLS,num);
			for (int i=0; i<num; ++i) {
				cout <<i+1<<endl;
				APIModule(Interpreter(SQLS[i]));	    
			}
		}

		/////////////////////////////////////////////////////////////////////////////////////////
		//quit语句
		else if(Type=="70")
		{
			if( ! DB_Name.IsEmpty())
				Close_Database(DB_Name,true);
			cout << "Byebye" << endl;
			exit(0);
		}

		/////////////////////////////////////////////////////////////////////////////////////////
		//help语句
		else if(Type=="80")
		{
			cout<<"List of all our MiniSQL commands:\nNote that all text commands must be first on line and end with ';'\n1.CREATE; \n2.DROP;  \n3.SELECT; \n4.DELETE; \n5.INSERT; \n6.USE;\n7.EXECFILE; \n8.HELP;\n9.QUIT.\n\n";
		}

		/////////////////////////////////////////////////////////////////////////////////////////
		//错误语句
		else if(Type=="99")
		{
		}
	} catch (...) {
		cout << "Execution failed." << endl;
	}
}
