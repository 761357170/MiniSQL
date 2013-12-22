//////////////////////////////////////////////////////////
///----------------------------------------------------///
///       Module: Interpreter_Module                   ///
///       Produced by: 刘欢   3110104509               ///
///       Description: Produced to deal with SQL parse ///
///       date: 2013/10/24                             ///
///----------------------------------------------------///
//////////////////////////////////////////////////////////

#include"CatalogManager.h"
#include"Interpreter.h"
#include<iostream>
#include<string>
#include<fstream>
#include<io.h>

using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////////
//0
//读取用户输入
CString read_input()
{
	CString SQL;
	CString temp;
	char str[100];
	bool finish=false;
	while(!finish)
	{
		cin>>str;
		temp=str;
		SQL=SQL+" "+temp;
		if(SQL.GetAt(SQL.GetLength()-1)==';')
		{
			SQL.SetAt(SQL.GetLength()-1,' ');
			SQL+=";";
			finish=true;
		}
	}
	//返回用户输入
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//1
//获得属性
CString get_attribute(CString temp,CString sql)
{
	int start=0,end,index;
	CString T,C;
	temp+=" ";
	//获得属性名
	end=temp.Find(' ',start);
	T=temp.Mid(start,end-start);
	start=end+1;
	sql+=T+" ";
	//获得数据类型
	while(temp.GetAt(start)==' ')
		start++;
	end=temp.Find(' ',start);
	T=temp.Mid(start,end-start);
	start=end+1;
	//若无，打印出错信息
	if(T.IsEmpty())
	{
		cout<<"error : error in create table statement!"<<endl;
		sql="99";
		return sql;
	}
	//若为int 
	else if(T=="int")
		sql+="+";
	//若为float
	else if(T=="float")
		sql+="-";
	//其他
	else
	{
		index=T.Find('(');
		C=T.Left(index);
		index++;
		//若有误，打印出错信息
		if(C.IsEmpty())
		{
			cout<<"error: "<<T<<"---is not a valid data type definition!"<<endl;
			sql="99";
			return sql;
		}
		//若为char
		else if(C=="char")
		{
			C=T.Mid(index,T.GetLength()-index-1);
			if(C.IsEmpty())
			{
				cout<<"error: the length of the data type char has not been specified!"<<endl;
				sql="99";
				return sql;
			}
			else
				sql+=C;
		}
		//若为非法信息，打印出错信息
		else
		{
			cout<<"error: "<<C<<"---is not a valid key word!"<<endl;
			sql="99";
			return sql;
		}
	}
	//是否有附加信息
	while(start<temp.GetLength()&&temp.GetAt(start)==' ')
		start++;
	if(start<temp.GetLength())
	{
		//若为unique定义，保存信息
		end=temp.Find(' ',start);
		T=temp.Mid(start,end-start);
		if(T=="unique")
		{
			sql+=" 1,";
		}
		//若为非法信息，打印出错信息
		else
		{
			cout<<"error: "<<temp<<"---is not a valid key word!"<<endl;
			sql="99";
			return sql;
		}
	}
	//若无附加信息
	else
		sql+=" 0,";
	return sql;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//2
//获得条件
CString get_condition(CString temp,CString sql)
{
	int end=temp.GetLength();
	while(temp.GetAt(end-1)==' ')  //去除后面的空格
		end--;
	temp=temp.Left(end);
	CString t,c;
	CString tp=temp;
	int i=temp.FindOneOf("><="),j;  //寻找op
	if(i<1)     //未找到op或没有左值
	{
		cout<<"syntax error: no op or left operand in condition statement!"<<endl;
		return "99";   //返回错误信息
	}
	t=temp.SpanExcluding("><=");   //获取左值
	end=t.GetLength();
	while(t.GetAt(end-1)==' ')  //去除后面的空格
		end--;
	t=t.Left(end);
	int f=t.Find(',');
	if(f!=-1)  //左值错误
	{
		cout<<"syntax error: wrong left operand in condition statement!"<<endl;
		return "99";   //返回错误信息
	}
	sql+=t;
	sql+=" ";
	sql+=temp.GetAt(i);  //获取op
	temp.SetAt(i,' ');
	j=temp.FindOneOf("><=");
	if(j!=-1)
	{
		if(j!=i+1)
		{
			cout<<"syntax error:  wrong op for condition statement!"<<endl;   //op错误
			return "99";
		}
		tp=tp.Mid(i,2);
		if(tp!=">="&&tp!="<="&&tp!="<>")
		{
			cout<<"syntax error:  wrong op for condition statement!"<<endl;   //op错误
			return "99";
		}
		sql+=temp.GetAt(j);  //获取op
		temp.SetAt(j,' ');
	}
	i=temp.ReverseFind(' ');
	c=temp.Mid(i);  //获取右值
	if(c==" ")      //没有右值
	{
		cout<<"syntax error: no  right operand in condition statement!"<<endl;
		return "99";//返回错误信息
	}
	else
	{
		c=c.Mid(1);
		if(is_string(c))    //string
		{
			CString cs;
			cs.Format("%d",c.GetLength()-2);
			c=c.Mid(1,c.GetLength()-2);
			sql=sql+" "+cs+" "+c+",";
		}
		else if(is_int(c))  //整数
			sql=sql+" + "+c+",";
		else if(is_float(c)) //浮点数
			sql=sql+" - "+c+",";
		else                    //错误格式数据
		{
			cout<<"error: wrong data format in condition statement!"<<endl;
			return "99";
		}
	}
	return sql;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//3
//验证create database语句是否有效
CString create_database(CString SQL,int start)
{
	CString temp;
	int index,end;
	//获取第三个单词
	while(SQL.GetAt(start)==' ')
		start++;
	index=start;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	//若无，打印出错信息
	if(temp.IsEmpty())
	{
		cout<<"error: database name has not been specified!"<<endl;
		SQL="99";
	}
	else
	{	
		while(SQL.GetAt(start)==' ')
			start++;
		//若为非法信息，打印出错信息
		if(SQL.GetAt(start)!=';'||start!=SQL.GetLength()-1)
		{
			cout<<"error:"<<SQL.Mid(index,SQL.GetLength()-index-2)<<"---is not a valid database name!"<<endl;
			SQL="99";
		}
		//返回create database语句的内部形式
		else
			SQL="00"+temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//4
//验证create table语句是否有效
CString create_table(CString SQL,int start)
{
	CString temp,sql,T;
	int index,end,length,i=0,flag=0;
	//获取表名
	end=SQL.GetLength()-2;
	while(SQL.GetAt(end)==' ')
		end--;
	if(SQL.GetAt(end)!=')')
	{
		cout<<"syntax error: syntax error in create table statement!"<<endl;
		cout<<"\t missing ) or redundant words after ) !"<<endl;
		SQL="99";
		return SQL;
	}
	while(SQL.GetAt(start)==' ')
		start++;
	index=start;
	if((end=SQL.Find('(',start))==-1)
	{
		cout<<"error: missing ( in the statement!"<<endl;
		SQL="99";
		return SQL;
	}
	temp=SQL.Mid(start,end-start);
	start=end+1;
	if(!temp.IsEmpty())
	{
		while(SQL.GetAt(start)==' ')
			start++;
		length=temp.GetLength()-1;
		while(temp.GetAt(length)==' ')
			length--;
		temp=temp.Left(length+1);
	}
	//若无，打印出错信息
	if(temp.IsEmpty())
	{
		cout<<"error: error in create table statement!"<<endl;
		SQL="99";
		return SQL;
	}
	//若为非法信息，打印出错信息
	else if(temp.Find(' ')!=-1)
	{
		cout<<"error: "<<temp<<"---is not a valid table name!"<<endl;
		SQL="99";
		return SQL;
	}
	else 
	{
		CString tname=temp;
		//获取每个属性
		while((end=SQL.Find(',',start))!=-1)
		{
			temp=SQL.Mid(start,end-start);
			start=end+1;
			//若有空属性，打印出错信息
			if(temp.IsEmpty())
			{
				cout<<"error: error in create table statement!"<<endl;
				SQL="99";
				return SQL;
			}
			//保存属性
			else
			{
				sql=get_attribute(temp,sql);
				if(sql=="99")
					return sql;
				i++;
			}
			while(SQL.GetAt(start)==' ')
				start++;
		}
		//获得最后属性
		temp=SQL.Mid(start,SQL.GetLength()-start-1);
		length=temp.GetLength()-1;
		while(temp.GetAt(length)!=')'&&length>=0)
			length--;
		//若无，打印出错信息
		if(length<1)
		{
			cout<<"error: error in create table statement!"<<endl;
			SQL="99";
			return SQL;
		}
		//存储属性
		else
		{
			temp=temp.Left(length);
			end=SQL.Find(' ',start);
			T=SQL.Mid(start,end-start);
			start=end+1;
			//若为主键定义
			if(T=="primary")
			{
				//判断是否有关键字key
				temp+=")";
				while(SQL.GetAt(start)==' ')
					start++;
				end=SQL.Find('(',start);
				T=SQL.Mid(start,end-start);
				start=end+1;
				length=T.GetLength()-1;
				while(T.GetAt(length)==' ')
					length--;
				T=T.Left(length+1);
				//若为空，打印出错信息
				if(T.IsEmpty())
				{
					cout<<"syntax error: syntax error in create table statement!"<<endl;
					cout<<"\t missing key word key!"<<endl;
					SQL="99";
					return SQL;
				}
				//若有，继续验证
				else if(T=="key")
				{
					//获取主键属性名
					while(SQL.GetAt(start)==' ')
						start++;
					end=SQL.Find(')',start);
					T=SQL.Mid(start,end-start);
					length=T.GetLength()-1;
					while(T.GetAt(length)==' ')
						length--;
					T=T.Left(length+1);
					//若无，打印出错信息
					if(T.IsEmpty())
					{
						cout<<"error : missing primary key attribute name!"<<endl;
						SQL="99";
						return SQL;
					}
					//若为非法信息，打印出错信息
					else if(T.Find(' ')!=-1)
					{
						cout<<"error : "<<T<<"---is not a valid primary key attribute name!"<<endl;
						SQL="99";
						return SQL;
					}
					//保存主键
					else
					{
						sql+=T+" #,";
						flag=1;
						CString s1,s2;
						s1.Format("%d",i);
						s2.Format("%d",flag);
						SQL="01"+tname+","+s1+","+s2+","+sql;
					}
				}
				//若为非法信息，打印出错信息
				else
				{
					cout<<"error : "<<T<<"---is not a valid key word!"<<endl;
					SQL="99";
					return SQL;
				}				
			}
			//若为一般属性
			else
			{
				sql=get_attribute(temp,sql);
				if(sql=="99")
				{
					SQL="99";
					return SQL;
				}
				else
				{
					CString s1,s2;
					i++;
					s1.Format("%d",i);
					s2.Format("%d",flag);
					SQL="01"+tname+","+s1+","+s2+","+sql;
				}
			}
		}
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//5
//验证create index on语句是否有效
CString create_index_on(CString SQL,int start,CString sql)
{
	CString temp;
	int end,length;
	//获取表名
	while(SQL.GetAt(start)==' ')
		start++;
	end=SQL.Find('(',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	//若无，打印出错信息
	if(temp.IsEmpty())
	{
		cout<<"syntax error: syntax error for create index statement!"<<endl;
		cout<<"\t missing ( !"<<endl;
		SQL="99";
		return SQL;
	}
	else
	{
		//检验是否为有效文件名
		length=temp.GetLength()-1;
		while(temp.GetAt(length)==' ')
			length--;
		temp=temp.Left(length+1);
		//有效
		if(temp.Find(' ')==-1)
		{
			sql+=" "+temp;
			//获取属性名
			while(SQL.GetAt(start)==' ')
				start++;
			end=SQL.Find(')',start);
			temp=SQL.Mid(start,end-start);
			start=end+1;
			//若无，打印出错信息
			if(temp.IsEmpty())
			{
				cout<<"syntax error: syntax error for create index statement!"<<endl;
				cout<<"\t missing ) !"<<endl;
				SQL="99";
				return SQL;
			}
			else
			{
				//检验是否为有效属性名
				length=temp.GetLength()-1;
				while(temp.GetAt(length)==' ')
					length--;
				temp=temp.Left(length+1);
				//有效
				if(temp.Find(' ')==-1)
				{
					sql+=" "+temp;
					while(SQL.GetAt(start)==' ')
						start++;
					if(SQL.GetAt(start)!=';'||start!=SQL.GetLength()-1)
					{
						cout<<"syntax error: syntax error for quit!"<<endl;
						SQL="99";
						return SQL;
					}
					//返回create index语句的内部形式
					else
						SQL="02"+sql;
				}
				//无效,打印出错信息
				else
				{
					cout<<"error:"<<" "<<temp<<"---is not a valid attribute name!"<<endl;
					SQL="99";
					return SQL;
				}
			}
		}
		//无效,打印出错信息
		else
		{
			cout<<"error:"<<" "<<temp<<"---is not a valid table name!"<<endl;
			SQL="99";
			return SQL;
		}
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//6
//验证create index语句是否有效
CString create_index(CString SQL,int start)
{
	CString temp,sql;
	int end;
	//获取第三个单词
	while(SQL.GetAt(start)==' ')
		start++;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	//若无，打印出错信息
	if(temp.IsEmpty())
	{
		cout<<"syntax error: syntax error for create index statement!"<<endl;
		return "99";   //返回错误信息
	}
	else
	{
		sql=temp;
		//获取第四个单词
		while(SQL.GetAt(start)==' ')
			start++;
		end=SQL.Find(' ',start);
		temp=SQL.Mid(start,end-start);
		start=end+1;
		//若无，打印出错信息
		if(temp.IsEmpty())
		{
			cout<<"syntax error: syntax error for create index statement!"<<endl;
			return "99";   //返回错误信息
		}
		//若为on,继续验证
		else if(temp=="on")
			SQL=create_index_on(SQL,start,sql);
		//若为非法信息，打印非法信息
		else
		{
			cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
			return "99";   //返回错误信息
		}			
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//7
//验证create语句是否有效
CString create_clause(CString SQL,int start)
{
	CString temp;
	int end;
	//获取第二个单词
	while(SQL.GetAt(start)==' ')
		start++;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	//若无，打印出错信息
	if(temp.IsEmpty())
	{
		cout<<"syntax error: syntax error for create statement!"<<endl;
		return "99";   //返回错误信息
	}
	//若为database,继续验证
	else if(temp=="database")
		SQL=create_database(SQL,start);
	//若为table,继续验证
	else if(temp=="table")
		SQL=create_table(SQL,start);
	//若为index,继续验证
	else if(temp=="index")
		SQL=create_index(SQL,start);
	//若为错误信息，打印出错信息
	else
	{
		cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
		return "99";   //返回错误信息
	}
	//返回create语句的内部形式
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//8
//验证drop database语句是否有效
CString drop_database(CString SQL,int start)
{
	CString temp;
	int index,end;
	//获取第三个单词
	while(SQL.GetAt(start)==' ')
		start++;
	index=start;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	//若无，打印出错信息
	if(temp.IsEmpty())
	{
		cout<<"error: database name has not been specified!"<<endl;
		return "99";   //返回错误信息
	}
	else
	{
		while(SQL.GetAt(start)==' ')
			start++;
		//若为非法信息，打印出错信息
		if(SQL.GetAt(start)!=';'||start!=SQL.GetLength()-1)
		{
			cout<<"error:"<<SQL.Mid(index,SQL.GetLength()-index-2)<<"---is not a valid database name!"<<endl;
			return "99";   //返回错误信息
		}
		//返回drop database语句的内部形式
		else
			SQL="10"+temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//9
//验证drop table语句是否有效
CString drop_table(CString SQL,int start)
{
	CString temp;
	int index,end;
	//获取第三个单词
	while(SQL.GetAt(start)==' ')
		start++;
	index=start;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	//若无，打印出错信息
	if(temp.IsEmpty())
	{
		cout<<"error: table name has not been specified!"<<endl;
		return "99";   //返回错误信息
	}
	else
	{
		while(SQL.GetAt(start)==' ')
			start++;
		//若为非法信息，打印出错信息
		if(SQL.GetAt(start)!=';'||start!=SQL.GetLength()-1)
		{
			cout<<"error:"<<SQL.Mid(index,SQL.GetLength()-index-2)<<"---is not a valid table name!"<<endl;
			return "99";   //返回错误信息
		}
		//返回drop table语句的内部形式
		else
			SQL="11"+temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//10
//验证drop index语句是否有效
CString drop_index(CString SQL,int start)
{
	CString temp;
	int index,end;
	//获取第三个单词
	while(SQL.GetAt(start)==' ')
		start++;
	index=start;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	//若无，打印出错信息
	if(temp.IsEmpty())
	{
		cout<<"error: index name has not been specified!"<<endl;
		return "99";   //返回错误信息
	}
	else
	{
		while(SQL.GetAt(start)==' ')
			start++;
		//若为非法信息，打印出错信息
		if(SQL.GetAt(start)!=';'||start!=SQL.GetLength()-1)
		{
			cout<<"error:"<<SQL.Mid(index,SQL.GetLength()-index-2)<<"---is not a valid index name!"<<endl;
			return "99";   //返回错误信息
		}
		//返回drop index语句的内部形式
		else
			SQL="12"+temp;
	}
	return SQL;	
}

/////////////////////////////////////////////////////////////////////////////////////////////
//11
//验证drop语句是否有效
CString drop_clause(CString SQL,int start)
{
	CString temp;
	int end;
	//获取第二个单词
	while(SQL.GetAt(start)==' ')
		start++;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	//若无，打印出错信息
	if(temp.IsEmpty())
	{
		cout<<"syntax error: syntax error for drop statement!"<<endl;
		return "99";   //返回错误信息
	}
	//若为database,继续验证
	else if(temp=="database")
		SQL=drop_database(SQL,start);
	//若为table,继续验证
	else if(temp=="table")
		SQL=drop_table(SQL,start);
	//若为index,继续验证
	else if(temp=="index")
		SQL=drop_index(SQL,start);
	//若为错误信息，打印出错信息
	else
	{
		cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
		return "99";   //返回错误信息
	}
	//返回drop语句的内部形式
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//12
//验证 select from where 语句是否有效
CString selete_from_where(CString SQL,int start)
{
	CString temp,sql;
	int end;
	int count=0;
	while(SQL.GetAt(start)==' ')
		start++;
	while((end=SQL.Find(" and ",start))!=-1)//寻找”and“关键字
	{
		temp=SQL.Mid(start,end-start);
		start=end+5;
		while(SQL.GetAt(start)==' ')
			start++;
		//若为空，打印出错信息
		if(temp.IsEmpty())
		{
			cout<<"error: error in  condition statement!"<<endl;
			SQL="99";
			return SQL;
		}
		//保存属性
		else
		{
			sql=get_condition(temp,sql);
			if(sql=="99")
				return sql;
		}
		count++;
	}
	//获取最后一个条件
	end=SQL.Find(" ;",start);
	temp=SQL.Mid(start,end-start);
	//若为空，打印出错信息
	if(temp.IsEmpty())
	{
		cout<<"error: error in  condition statement!"<<endl;
		SQL="99";
		return SQL;
	}
	//保存属性
	else
	{
		sql=get_condition(temp,sql);
		if(sql=="99")
			return sql;
	}
	count++;
	CString ss;
	ss.Format("%d",count);
	return ss+","+sql;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//13
//验证select 语句是否有效
CString select_clause(CString SQL,int start)
{
	CString temp;
	int end;
	//获取第二个单词
	while(SQL.GetAt(start)==' ')
		start++;
	if((end=SQL.Find(" from",start))==-1)
	{
		cout<<"syntax error: missing key word \"from\"!"<<endl;
		return "99";   //返回错误信息
	}
	temp=SQL.Mid(start,end-start);
	int ijk=temp.GetLength();
	while(temp.GetAt(ijk-1)==' ')
		ijk--;
	temp.Left(ijk);
	start=end+5;
	//若无，打印出错信息
	if(temp.IsEmpty())
	{
		cout<<"syntax error: syntax error for select statement!"<<endl;
		return "99";   //返回错误信息
	}
	//若为“*”，继续验证
	else if(temp=="*")
	{
		//获取第四个单词
		while(SQL.GetAt(start)==' ')
			start++;
		end=SQL.Find(' ',start);
		temp=SQL.Mid(start,end-start);
		start=end+1;
		//若无，打印出错信息
		if(temp.IsEmpty())
		{
			cout<<"syntax error: syntax error for select statement!"<<endl;
			return "99";   //返回错误信息
		}
		//若为表名，继续验证
		else
		{
			CString tname=temp;
			//获取第五个单词
			while(SQL.GetAt(start)==' ')
			start++;
			end=SQL.Find(' ',start);
			temp=SQL.Mid(start,end-start);
			start=end+1;
			//若无,返回select *语句的内部形式
			if(temp.IsEmpty())
				SQL="20"+tname;
			//若为"where"，继续验证
			else if(temp="where")
			{
				CString it=selete_from_where(SQL,start);
				if(it=="99")
					return it;
				SQL="22"+tname+","+it;
			}
			//若为错误信息，打印出错信息
			else
			{
				cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
				return "99";   //返回错误信息
			}
		}
	}
	//若为列名,继续验证
	else
	{
		temp.Replace(" ","");
		string ss(temp);
		int count=1;
		for(int i=0;i<ss.length();i++)
			if(ss[i]==',')
				count++;
		CString cc,cname=temp;
		cc.Format("%d",count);
		//获取第四个单词
		while(SQL.GetAt(start)==' ')
		start++;
		end=SQL.Find(' ',start);
		temp=SQL.Mid(start,end-start);
		start=end+1;
		//若无，打印出错信息
		if(temp.IsEmpty())
		{
			cout<<"syntax error: syntax error for select statement!"<<endl;
			return "99";   //返回错误信息
		}
		//若为表名，继续验证
		else
		{
			CString tname=temp;
			//获取第五个单词
			while(SQL.GetAt(start)==' ')
			start++;
			end=SQL.Find(' ',start);
			temp=SQL.Mid(start,end-start);
			start=end+1;
			//若无,返回select 列名，列名...语句的内部形式
			if(temp.IsEmpty())
				SQL="21"+tname+","+cc+","+cname+",";
			//若为"where"，继续验证
			else if(temp="where")
			{
				CString it=selete_from_where(SQL,start);
				if(it=="99")
					return it;
				SQL="23"+tname+","+cc+","+cname+","+selete_from_where(SQL,start);
			}
			//若为错误信息，打印出错信息
			else
			{
				cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
				return "99";   //返回错误信息
			}
		}
	}
	//返回select语句的内部形式
	return SQL;
}


/////////////////////////////////////////////////////////////////////////////////////////////
//14
//验证insert into values语句是否有效
CString insert_into_values(CString SQL,int start)
{
	CString temp,sql,T;
	int index,end,length,i=0;
	end=SQL.GetLength()-2;
	while(SQL.GetAt(end)==' ')
		end--;
	if(SQL.GetAt(end)!=')')
	{
		cout<<"syntax error: syntax error in insert statement!"<<endl;
		cout<<"\t missing ) or redundant words after ) !"<<endl;
		SQL="99";
		return SQL;
	}
	while(SQL.GetAt(start)==' ')
		start++;
	index=start;
	while((end=SQL.Find(',',start))!=-1)
	{
		temp=SQL.Mid(start,end-start);
		start=end+1;
		length=temp.GetLength()-1;
		while(temp.GetAt(length)==' ')
			length--;
		temp=temp.Left(length+1);
		//若为空值，打印出错信息
		if(temp.IsEmpty())
		{
			cout<<"error: error in insert statement!"<<endl;
			SQL="99";
			return SQL;
		}
		//保存值
		else
		{
			if(is_string(temp))    //string
			{
				CString cs;
				cs.Format("%d",temp.GetLength()-2);
				temp=temp.Mid(1,temp.GetLength()-2);
				sql=sql+cs+" "+temp+",";
			    i++;
			}
			else if(is_int(temp))  //整数
			{
				sql=sql+"+ "+temp+",";
				i++;
			}
			else if(is_float(temp)) //浮点数
			{
				sql=sql+"- "+temp+",";
				i++;
			}
			else                    //错误格式数据
			{
				cout<<"error: wrong data format in insert statement!"<<endl;
			    SQL="99";
				return SQL;
			}
		}
		while(SQL.GetAt(start)==' ')
			start++;
	}
	//取最后值
	end=SQL.Find(')',start);
	temp=SQL.Mid(start,end-start);
	length=temp.GetLength()-1;
	while(temp.GetAt(length)==' ')
		length--;
	temp=temp.Left(length+1);
	//若为空值，打印出错信息
	if(temp.IsEmpty())
	{
		cout<<"error: error in insert statement!"<<endl;
		SQL="99";
		return SQL;
	}
	//保存值
	else
	{
		if(is_string(temp))    //string
		{
			CString cs;
			cs.Format("%d",temp.GetLength()-2);
			temp=temp.Mid(1,temp.GetLength()-2);
			sql=sql+cs+" "+temp+",";
			i++;
		}
		else if(is_int(temp))  //整数
		{
			sql=sql+"+ "+temp+",";
			i++;
		}
		else if(is_float(temp)) //浮点数
		{
			sql=sql+"- "+temp+",";
			i++;
		}
		else                    //错误格式数据
		{
			cout<<"error: wrong data format in insert statement!"<<endl;
			SQL="99";
			return SQL;
		}
	}
	CString st;
	st.Format("%d",i);
	return st+","+sql;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//15
//验证insert 语句是否有效
CString insert_clause(CString SQL,int start)
{
	CString temp;
	int end,length;
	//获取第二个单词
	while(SQL.GetAt(start)==' ')
		start++;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	//若无，打印出错信息
	if(temp.IsEmpty())
	{
		cout<<"syntax error: syntax error for insert statement!"<<endl;
		return "99";   //返回错误信息
	}
	//若为“into”，继续验证
	else if(temp=="into")
	{
		//获取表名
		while(SQL.GetAt(start)==' ')
			start++;
		end=SQL.Find(' ',start);
		temp=SQL.Mid(start,end-start);
		start=end+1;
		//若无，打印出错信息
		if(temp.IsEmpty())
		{
			cout<<"syntax error: syntax error for insert statement!"<<endl;
			return "99";   //返回错误信息
		}
		//若为表名，继续验证
		else
		{
			CString tname=temp;
			//获取第四个单词
			while(SQL.GetAt(start)==' ')
				start++;
			if((end=SQL.Find('(',start))==-1)
			{
				cout<<"syntax error: syntax error in insert statement!"<<endl;
				cout<<"\t missing ( !"<<endl;
				SQL="99";
				return SQL;
			}
			temp=SQL.Mid(start,end-start);
			start=end+1;
			if(!temp.IsEmpty())
			{
				while(SQL.GetAt(start)==' ')
					start++;
				length=temp.GetLength()-1;
				while(temp.GetAt(length)==' ')
					length--;
				temp=temp.Left(length+1);
			}
			//若无，打印出错信息
			if(temp.IsEmpty())
			{
				cout<<"syntax error: syntax error for insert statement!"<<endl;
				return "99";   //返回错误信息
			}
			//若为"values"，继续验证
			else if(temp=="values")
			{
				CString it=insert_into_values(SQL,start);
				if(it=="99")
					return it;
				SQL="30"+tname+","+it;
			}
			//若为错误信息，打印出错信息
			else
			{
				cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
				return "99";   //返回错误信息
			}
		}
	}
	//若为错误信息，打印出错信息
	else
	{
		cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
		return "99";   //返回错误信息
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//16
//验证 delete from where 语句是否有效
CString delete_from_where(CString SQL,int start)
{
	CString temp,sql;
	int end;
	int count=0;
	while(SQL.GetAt(start)==' ')
		start++;
	while((end=SQL.Find(" and ",start))!=-1)//寻找”and“关键字
	{
		temp=SQL.Mid(start,end-start);
		start=end+5;
		while(SQL.GetAt(start)==' ')
			start++;
		//若为空，打印出错信息
		if(temp.IsEmpty())
		{
			cout<<"error: error in  condition statement!"<<endl;
			SQL="99";
			return SQL;
		}
		//保存属性
		else
		{
			sql=get_condition(temp,sql);
			if(sql=="99")
				return sql;
		}
		count++;
	}
	//获取最后一个条件
	end=SQL.Find(" ;",start);
	temp=SQL.Mid(start,end-start);
	//若为空，打印出错信息
	if(temp.IsEmpty())
	{
		cout<<"error: error in  condition statement!"<<endl;
		SQL="99";
		return SQL;
	}
	//保存属性
	else
	{
		sql=get_condition(temp,sql);
		if(sql=="99")
			return sql;
	}
	count++;
	CString ss;
	ss.Format("%d",count);
	return ss+","+sql;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//17
//验证delete语句是否有效
CString delete_clause(CString SQL,int start)
{
	CString temp;
	int end;
	//获取第二个单词
	while(SQL.GetAt(start)==' ')
		start++;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	//若无，打印出错信息
	if(temp.IsEmpty())
	{
		cout<<"syntax error: syntax error for delect statement!"<<endl;
		return "99";   //返回错误信息
	}
	//若为“from”，继续验证
	else if(temp=="from")
	{
		//获取第三个单词
		while(SQL.GetAt(start)==' ')
			start++;
		end=SQL.Find(' ',start);
		temp=SQL.Mid(start,end-start);
		start=end+1;
		//若无，打印出错信息
		if(temp.IsEmpty())
		{
			cout<<"syntax error: syntax error for delect statement!"<<endl;
			return "99";   //返回错误信息
		}
		//若为表名，继续验证
		else
		{
			CString tname=temp;
			//获取第四个单词
			while(SQL.GetAt(start)==' ')
				start++;
			end=SQL.Find(' ',start);
			temp=SQL.Mid(start,end-start);
			start=end+1;
			//若无，返回delete内部形式
			if(temp.IsEmpty())
				SQL="40"+tname;
			//若为"where"，继续验证
			else if(temp=="where")
			{
				CString it=delete_from_where(SQL,start);
				if(it=="99")
					return it;
				SQL="41"+tname+","+it;
			}
			//若为错误信息，打印出错信息
			else
			{
				cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
				return "99";   //返回错误信息
			}
		}
	}
	//若为错误信息，打印出错信息
	else
	{
		cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
		return "99";   //返回错误信息
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//18
//获得属性组或文件组的每一项
CString get_part(CString temp,CString sql,CString kind)
{
	return "";
}

/////////////////////////////////////////////////////////////////////////////////////////////
//19
//将表达式转化为内部形式
CString get_expression(CString temp)
{
	temp.SetAt(temp.GetLength()-1,' ');
	temp+=";";
	if((temp=Interpreter(temp))=="99")
		return "99";
	else 
		return temp;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//20
//获取表达式组的每个表达式
CString get_each(CString sql,int &begin)
{
	CString temp;
	int start=begin;
	begin=sql.Find(';',begin)+1;     //SQL语句以“;”分隔
	temp=sql.Mid(start,begin-start);
	if((temp=get_expression(temp))=="99")
		return "99";
	else
		return temp+";";
}

/////////////////////////////////////////////////////////////////////////////////////////////
//21
//验证use语句是否有效
CString use_clause(CString SQL,int start)
{
	CString temp;
	int index,end;
	//获取第二个单词
	while(SQL.GetAt(start)==' ')
		start++;
	index=start;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	//若无，打印出错信息
	if(temp.IsEmpty())
	{
		cout<<"error: database name has not been specified!"<<endl;
		return "99";   //返回错误信息
	}
	else
	{	
		while(SQL.GetAt(start)==' ')
			start++;
		//若为非法信息，打印出错信息
		if(SQL.GetAt(start)!=';'||start!=SQL.GetLength()-1)
		{
			cout<<"error:"<<SQL.Mid(index,SQL.GetLength()-index-2)<<"---is not a valid database name!"<<endl;
			return "99";   //返回错误信息
		}
		//返回use database语句的内部形式
		else
			SQL="50"+temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//22
//验证execfile语句是否有效
CString execfile_clause(CString SQL,int start)
{
	CString temp;
	int end;
	int count=0;
	//获取第二个单词
	while(SQL.GetAt(start)==' ')
		start++;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	string sql;
	CString tp;
	//若无，打印出错信息
	if(temp.IsEmpty())
	{
		cout<<"syntax error: syntax error for execfile statement!"<<endl;
		SQL="99";
		return SQL;
	}
	//若为文件名，继续验证
	else
	{
		CString fname=temp;
		//获取第三个单词
		while(SQL.GetAt(start)==' ')
			start++;
		end=SQL.Find(' ',start);
		temp=SQL.Mid(start,end-start);
		start=end+1;
		
		//若有多余单词，打印出错信息
		if(!temp.IsEmpty())
		{
			cout<<"syntax error: syntax error for execfile statement!"<<endl;
			SQL="99";
			return SQL;
		}
		else
		{
			if((_access(fname,0))==-1)        //文件不存在
			{
				cout<<"error: file does not exist!"<<endl;
				SQL="99";
				return SQL;
			}
			else 
			{
				SQL="";
				fstream in(fname);             //打开文件
				string s;
				CString ss;
				while(getline(in,s))            //读文件
					sql+=s;
			   tp=sql.c_str();
				int i=sql.length()-1;
				if(sql[i]==' ')
					i--;
				if(sql[i]!=';')
				{
					cout<<"error: missing \";\" at the end of file!"<<endl;
					return "99";
				}
				for(i=0;i<sql.length();i++)     //统计SQL语句条数
					if(sql[i]==';')
						count++;
				/*int begin=0;
				for(i=0;i<count;i++)            //SQL语句转化为内部命令
					if((ss=get_each(tp,begin))=="99")
						SQL=SQL+ss+";";
					else
						SQL+=ss;*/
			}
		}
	}
	CString st;
	st.Format("%d",count);
	return "60,"+st+","+tp;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//23
//验证quit语句是否有效
CString quit_clause(CString SQL,int start)
{
	CString temp;
	int index,end;
	//判断quit之后是否还有其他单词
	while(SQL.GetAt(start)==' ')
		start++;
	index=start;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	//若无，返回quit语句的内部形式
	if(temp.IsEmpty())
		SQL="70";
	else
	{	//否则输出出错信息
		cout<<"syntax error: in quit_clause, there should be no other words after \"quit\" !"<<endl;
		SQL="99";
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//24
//获取用户输入，并对输入作有效性检查，若正确，返回语句的内部表示形式
CString Interpreter(CString SQL)
{
	CString temp;
	//CString sql;
	int start=0,end;
	if(SQL=="")
		SQL=read_input();//获取用户输入;
	//获取输入的第一个单词
	while(SQL.GetAt(start)==' ')
		start++;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	//若无输入，打印出错信息
	if(temp.IsEmpty())
	{
		cout<<"syntax error: empty statement!"<<endl;
		return "99";   //返回错误信息
	}
	//若为create语句
	else if(temp=="create")
		SQL=create_clause(SQL,start);
	//若为drop语句
	else if(temp=="drop")
		SQL=drop_clause(SQL,start);
	//若为select语句
	else if(temp=="select")
		SQL=select_clause(SQL,start);
	//若为insert语句
	else if(temp=="insert")
		SQL=insert_clause(SQL,start);
	//若为delete语句
	else if(temp=="delete")
		SQL=delete_clause(SQL,start);
	//若为use语句
	else if(temp=="use")
		SQL=use_clause(SQL,start);
	//若为execfile语句
	else if(temp=="execfile")
		SQL=execfile_clause(SQL,start);
	//若为quit语句
	else if(temp=="quit")
		SQL=quit_clause(SQL,start);
	//获取帮助
	else if(temp=="help")
		SQL="80";
	//若为非法语句
	else
	{
		cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
		return "99";   //返回错误信息
	}
	//返回输入语句的内部形式
	//cout<<SQL<<endl;
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//25
//判断是否为字符串
bool is_string(CString temp)
{
	if(temp.GetAt(0)=='\''&&temp.GetAt(temp.GetLength()-1)=='\'')
		return true;
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//26
//判断是否为浮点数
bool is_float(CString temp)
{
	int flag=0;
	if(temp.GetAt(0)=='.'||temp.GetAt(temp.GetLength()-1)=='.')  //以‘.’开头或结尾
		return false;
	for(int i=0;i<temp.GetLength();i++)                          //有非法字符
		if(temp.GetAt(i)<'0'||temp.GetAt(i)>'9')
			if(temp.GetAt(i)!='.')
				return false;
	for(int i=1;i<temp.GetLength()-1;i++)                        //统计‘.’个数
		if(temp.GetAt(i)=='.')
			flag++;
	if(flag==1)                                               
		return true;
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//27
//判断是否为整数
bool is_int(CString temp)
{
	for(int i=0;i<temp.GetLength();i++)
		if(temp.GetAt(i)<'0'||temp.GetAt(i)>'9')
			return false;
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//28
//判断格式是否正确
bool is_correct(CString temp)
{
	if(is_string(temp))
		return true;
	else if(is_int(temp))
		return true;
	else if(is_float(temp))
		return true;
	else
		return false;
}

/*
int main(void)
{
	CString tmp;
	while(tmp!="70")
	{
		tmp=Interpreter("");
		cout<<tmp<<endl;
		/*
		Create_Database(tmp.Mid(2));
		
		/*
		CString DB_Name="db";
		int index=tmp.Find(',');
		CString Table_Name=tmp.Mid(2,index-2);
		CString Attr=tmp.Right(tmp.GetLength()-index-1);
		CString Attr_Name;
		Attr_Name.Empty();
		Create_Table(Table_Name,Attr,DB_Name,Attr_Name);
		
		
		CString DB_Name="db";
		int length,offset,type;
		int index=tmp.Find(' ');
		CString Index_Name=tmp.Mid(2,index-2);
		int end=index;
		index=end+1;
		index=tmp.Find(' ',index);
		CString Table_Name=tmp.Mid(end+1,index-end-1);
		CString Attr_Name=tmp.Right(tmp.GetLength()-index-1);
		Create_Index(Index_Name,Table_Name,Attr_Name,DB_Name,length,offset,type);
		cout<<length<<" "<<offset<<" "<<type<<endl;
	}
}
*/