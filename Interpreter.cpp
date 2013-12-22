//////////////////////////////////////////////////////////
///----------------------------------------------------///
///       Module: Interpreter_Module                   ///
///       Produced by: ����   3110104509               ///
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
//��ȡ�û�����
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
	//�����û�����
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//1
//�������
CString get_attribute(CString temp,CString sql)
{
	int start=0,end,index;
	CString T,C;
	temp+=" ";
	//���������
	end=temp.Find(' ',start);
	T=temp.Mid(start,end-start);
	start=end+1;
	sql+=T+" ";
	//�����������
	while(temp.GetAt(start)==' ')
		start++;
	end=temp.Find(' ',start);
	T=temp.Mid(start,end-start);
	start=end+1;
	//���ޣ���ӡ������Ϣ
	if(T.IsEmpty())
	{
		cout<<"error : error in create table statement!"<<endl;
		sql="99";
		return sql;
	}
	//��Ϊint 
	else if(T=="int")
		sql+="+";
	//��Ϊfloat
	else if(T=="float")
		sql+="-";
	//����
	else
	{
		index=T.Find('(');
		C=T.Left(index);
		index++;
		//�����󣬴�ӡ������Ϣ
		if(C.IsEmpty())
		{
			cout<<"error: "<<T<<"---is not a valid data type definition!"<<endl;
			sql="99";
			return sql;
		}
		//��Ϊchar
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
		//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
		else
		{
			cout<<"error: "<<C<<"---is not a valid key word!"<<endl;
			sql="99";
			return sql;
		}
	}
	//�Ƿ��и�����Ϣ
	while(start<temp.GetLength()&&temp.GetAt(start)==' ')
		start++;
	if(start<temp.GetLength())
	{
		//��Ϊunique���壬������Ϣ
		end=temp.Find(' ',start);
		T=temp.Mid(start,end-start);
		if(T=="unique")
		{
			sql+=" 1,";
		}
		//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
		else
		{
			cout<<"error: "<<temp<<"---is not a valid key word!"<<endl;
			sql="99";
			return sql;
		}
	}
	//���޸�����Ϣ
	else
		sql+=" 0,";
	return sql;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//2
//�������
CString get_condition(CString temp,CString sql)
{
	int end=temp.GetLength();
	while(temp.GetAt(end-1)==' ')  //ȥ������Ŀո�
		end--;
	temp=temp.Left(end);
	CString t,c;
	CString tp=temp;
	int i=temp.FindOneOf("><="),j;  //Ѱ��op
	if(i<1)     //δ�ҵ�op��û����ֵ
	{
		cout<<"syntax error: no op or left operand in condition statement!"<<endl;
		return "99";   //���ش�����Ϣ
	}
	t=temp.SpanExcluding("><=");   //��ȡ��ֵ
	end=t.GetLength();
	while(t.GetAt(end-1)==' ')  //ȥ������Ŀո�
		end--;
	t=t.Left(end);
	int f=t.Find(',');
	if(f!=-1)  //��ֵ����
	{
		cout<<"syntax error: wrong left operand in condition statement!"<<endl;
		return "99";   //���ش�����Ϣ
	}
	sql+=t;
	sql+=" ";
	sql+=temp.GetAt(i);  //��ȡop
	temp.SetAt(i,' ');
	j=temp.FindOneOf("><=");
	if(j!=-1)
	{
		if(j!=i+1)
		{
			cout<<"syntax error:  wrong op for condition statement!"<<endl;   //op����
			return "99";
		}
		tp=tp.Mid(i,2);
		if(tp!=">="&&tp!="<="&&tp!="<>")
		{
			cout<<"syntax error:  wrong op for condition statement!"<<endl;   //op����
			return "99";
		}
		sql+=temp.GetAt(j);  //��ȡop
		temp.SetAt(j,' ');
	}
	i=temp.ReverseFind(' ');
	c=temp.Mid(i);  //��ȡ��ֵ
	if(c==" ")      //û����ֵ
	{
		cout<<"syntax error: no  right operand in condition statement!"<<endl;
		return "99";//���ش�����Ϣ
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
		else if(is_int(c))  //����
			sql=sql+" + "+c+",";
		else if(is_float(c)) //������
			sql=sql+" - "+c+",";
		else                    //�����ʽ����
		{
			cout<<"error: wrong data format in condition statement!"<<endl;
			return "99";
		}
	}
	return sql;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//3
//��֤create database����Ƿ���Ч
CString create_database(CString SQL,int start)
{
	CString temp;
	int index,end;
	//��ȡ����������
	while(SQL.GetAt(start)==' ')
		start++;
	index=start;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	//���ޣ���ӡ������Ϣ
	if(temp.IsEmpty())
	{
		cout<<"error: database name has not been specified!"<<endl;
		SQL="99";
	}
	else
	{	
		while(SQL.GetAt(start)==' ')
			start++;
		//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
		if(SQL.GetAt(start)!=';'||start!=SQL.GetLength()-1)
		{
			cout<<"error:"<<SQL.Mid(index,SQL.GetLength()-index-2)<<"---is not a valid database name!"<<endl;
			SQL="99";
		}
		//����create database�����ڲ���ʽ
		else
			SQL="00"+temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//4
//��֤create table����Ƿ���Ч
CString create_table(CString SQL,int start)
{
	CString temp,sql,T;
	int index,end,length,i=0,flag=0;
	//��ȡ����
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
	//���ޣ���ӡ������Ϣ
	if(temp.IsEmpty())
	{
		cout<<"error: error in create table statement!"<<endl;
		SQL="99";
		return SQL;
	}
	//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
	else if(temp.Find(' ')!=-1)
	{
		cout<<"error: "<<temp<<"---is not a valid table name!"<<endl;
		SQL="99";
		return SQL;
	}
	else 
	{
		CString tname=temp;
		//��ȡÿ������
		while((end=SQL.Find(',',start))!=-1)
		{
			temp=SQL.Mid(start,end-start);
			start=end+1;
			//���п����ԣ���ӡ������Ϣ
			if(temp.IsEmpty())
			{
				cout<<"error: error in create table statement!"<<endl;
				SQL="99";
				return SQL;
			}
			//��������
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
		//����������
		temp=SQL.Mid(start,SQL.GetLength()-start-1);
		length=temp.GetLength()-1;
		while(temp.GetAt(length)!=')'&&length>=0)
			length--;
		//���ޣ���ӡ������Ϣ
		if(length<1)
		{
			cout<<"error: error in create table statement!"<<endl;
			SQL="99";
			return SQL;
		}
		//�洢����
		else
		{
			temp=temp.Left(length);
			end=SQL.Find(' ',start);
			T=SQL.Mid(start,end-start);
			start=end+1;
			//��Ϊ��������
			if(T=="primary")
			{
				//�ж��Ƿ��йؼ���key
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
				//��Ϊ�գ���ӡ������Ϣ
				if(T.IsEmpty())
				{
					cout<<"syntax error: syntax error in create table statement!"<<endl;
					cout<<"\t missing key word key!"<<endl;
					SQL="99";
					return SQL;
				}
				//���У�������֤
				else if(T=="key")
				{
					//��ȡ����������
					while(SQL.GetAt(start)==' ')
						start++;
					end=SQL.Find(')',start);
					T=SQL.Mid(start,end-start);
					length=T.GetLength()-1;
					while(T.GetAt(length)==' ')
						length--;
					T=T.Left(length+1);
					//���ޣ���ӡ������Ϣ
					if(T.IsEmpty())
					{
						cout<<"error : missing primary key attribute name!"<<endl;
						SQL="99";
						return SQL;
					}
					//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
					else if(T.Find(' ')!=-1)
					{
						cout<<"error : "<<T<<"---is not a valid primary key attribute name!"<<endl;
						SQL="99";
						return SQL;
					}
					//��������
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
				//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
				else
				{
					cout<<"error : "<<T<<"---is not a valid key word!"<<endl;
					SQL="99";
					return SQL;
				}				
			}
			//��Ϊһ������
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
//��֤create index on����Ƿ���Ч
CString create_index_on(CString SQL,int start,CString sql)
{
	CString temp;
	int end,length;
	//��ȡ����
	while(SQL.GetAt(start)==' ')
		start++;
	end=SQL.Find('(',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	//���ޣ���ӡ������Ϣ
	if(temp.IsEmpty())
	{
		cout<<"syntax error: syntax error for create index statement!"<<endl;
		cout<<"\t missing ( !"<<endl;
		SQL="99";
		return SQL;
	}
	else
	{
		//�����Ƿ�Ϊ��Ч�ļ���
		length=temp.GetLength()-1;
		while(temp.GetAt(length)==' ')
			length--;
		temp=temp.Left(length+1);
		//��Ч
		if(temp.Find(' ')==-1)
		{
			sql+=" "+temp;
			//��ȡ������
			while(SQL.GetAt(start)==' ')
				start++;
			end=SQL.Find(')',start);
			temp=SQL.Mid(start,end-start);
			start=end+1;
			//���ޣ���ӡ������Ϣ
			if(temp.IsEmpty())
			{
				cout<<"syntax error: syntax error for create index statement!"<<endl;
				cout<<"\t missing ) !"<<endl;
				SQL="99";
				return SQL;
			}
			else
			{
				//�����Ƿ�Ϊ��Ч������
				length=temp.GetLength()-1;
				while(temp.GetAt(length)==' ')
					length--;
				temp=temp.Left(length+1);
				//��Ч
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
					//����create index�����ڲ���ʽ
					else
						SQL="02"+sql;
				}
				//��Ч,��ӡ������Ϣ
				else
				{
					cout<<"error:"<<" "<<temp<<"---is not a valid attribute name!"<<endl;
					SQL="99";
					return SQL;
				}
			}
		}
		//��Ч,��ӡ������Ϣ
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
//��֤create index����Ƿ���Ч
CString create_index(CString SQL,int start)
{
	CString temp,sql;
	int end;
	//��ȡ����������
	while(SQL.GetAt(start)==' ')
		start++;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	//���ޣ���ӡ������Ϣ
	if(temp.IsEmpty())
	{
		cout<<"syntax error: syntax error for create index statement!"<<endl;
		return "99";   //���ش�����Ϣ
	}
	else
	{
		sql=temp;
		//��ȡ���ĸ�����
		while(SQL.GetAt(start)==' ')
			start++;
		end=SQL.Find(' ',start);
		temp=SQL.Mid(start,end-start);
		start=end+1;
		//���ޣ���ӡ������Ϣ
		if(temp.IsEmpty())
		{
			cout<<"syntax error: syntax error for create index statement!"<<endl;
			return "99";   //���ش�����Ϣ
		}
		//��Ϊon,������֤
		else if(temp=="on")
			SQL=create_index_on(SQL,start,sql);
		//��Ϊ�Ƿ���Ϣ����ӡ�Ƿ���Ϣ
		else
		{
			cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
			return "99";   //���ش�����Ϣ
		}			
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//7
//��֤create����Ƿ���Ч
CString create_clause(CString SQL,int start)
{
	CString temp;
	int end;
	//��ȡ�ڶ�������
	while(SQL.GetAt(start)==' ')
		start++;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	//���ޣ���ӡ������Ϣ
	if(temp.IsEmpty())
	{
		cout<<"syntax error: syntax error for create statement!"<<endl;
		return "99";   //���ش�����Ϣ
	}
	//��Ϊdatabase,������֤
	else if(temp=="database")
		SQL=create_database(SQL,start);
	//��Ϊtable,������֤
	else if(temp=="table")
		SQL=create_table(SQL,start);
	//��Ϊindex,������֤
	else if(temp=="index")
		SQL=create_index(SQL,start);
	//��Ϊ������Ϣ����ӡ������Ϣ
	else
	{
		cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
		return "99";   //���ش�����Ϣ
	}
	//����create�����ڲ���ʽ
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//8
//��֤drop database����Ƿ���Ч
CString drop_database(CString SQL,int start)
{
	CString temp;
	int index,end;
	//��ȡ����������
	while(SQL.GetAt(start)==' ')
		start++;
	index=start;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	//���ޣ���ӡ������Ϣ
	if(temp.IsEmpty())
	{
		cout<<"error: database name has not been specified!"<<endl;
		return "99";   //���ش�����Ϣ
	}
	else
	{
		while(SQL.GetAt(start)==' ')
			start++;
		//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
		if(SQL.GetAt(start)!=';'||start!=SQL.GetLength()-1)
		{
			cout<<"error:"<<SQL.Mid(index,SQL.GetLength()-index-2)<<"---is not a valid database name!"<<endl;
			return "99";   //���ش�����Ϣ
		}
		//����drop database�����ڲ���ʽ
		else
			SQL="10"+temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//9
//��֤drop table����Ƿ���Ч
CString drop_table(CString SQL,int start)
{
	CString temp;
	int index,end;
	//��ȡ����������
	while(SQL.GetAt(start)==' ')
		start++;
	index=start;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	//���ޣ���ӡ������Ϣ
	if(temp.IsEmpty())
	{
		cout<<"error: table name has not been specified!"<<endl;
		return "99";   //���ش�����Ϣ
	}
	else
	{
		while(SQL.GetAt(start)==' ')
			start++;
		//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
		if(SQL.GetAt(start)!=';'||start!=SQL.GetLength()-1)
		{
			cout<<"error:"<<SQL.Mid(index,SQL.GetLength()-index-2)<<"---is not a valid table name!"<<endl;
			return "99";   //���ش�����Ϣ
		}
		//����drop table�����ڲ���ʽ
		else
			SQL="11"+temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//10
//��֤drop index����Ƿ���Ч
CString drop_index(CString SQL,int start)
{
	CString temp;
	int index,end;
	//��ȡ����������
	while(SQL.GetAt(start)==' ')
		start++;
	index=start;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	//���ޣ���ӡ������Ϣ
	if(temp.IsEmpty())
	{
		cout<<"error: index name has not been specified!"<<endl;
		return "99";   //���ش�����Ϣ
	}
	else
	{
		while(SQL.GetAt(start)==' ')
			start++;
		//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
		if(SQL.GetAt(start)!=';'||start!=SQL.GetLength()-1)
		{
			cout<<"error:"<<SQL.Mid(index,SQL.GetLength()-index-2)<<"---is not a valid index name!"<<endl;
			return "99";   //���ش�����Ϣ
		}
		//����drop index�����ڲ���ʽ
		else
			SQL="12"+temp;
	}
	return SQL;	
}

/////////////////////////////////////////////////////////////////////////////////////////////
//11
//��֤drop����Ƿ���Ч
CString drop_clause(CString SQL,int start)
{
	CString temp;
	int end;
	//��ȡ�ڶ�������
	while(SQL.GetAt(start)==' ')
		start++;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	//���ޣ���ӡ������Ϣ
	if(temp.IsEmpty())
	{
		cout<<"syntax error: syntax error for drop statement!"<<endl;
		return "99";   //���ش�����Ϣ
	}
	//��Ϊdatabase,������֤
	else if(temp=="database")
		SQL=drop_database(SQL,start);
	//��Ϊtable,������֤
	else if(temp=="table")
		SQL=drop_table(SQL,start);
	//��Ϊindex,������֤
	else if(temp=="index")
		SQL=drop_index(SQL,start);
	//��Ϊ������Ϣ����ӡ������Ϣ
	else
	{
		cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
		return "99";   //���ش�����Ϣ
	}
	//����drop�����ڲ���ʽ
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//12
//��֤ select from where ����Ƿ���Ч
CString selete_from_where(CString SQL,int start)
{
	CString temp,sql;
	int end;
	int count=0;
	while(SQL.GetAt(start)==' ')
		start++;
	while((end=SQL.Find(" and ",start))!=-1)//Ѱ�ҡ�and���ؼ���
	{
		temp=SQL.Mid(start,end-start);
		start=end+5;
		while(SQL.GetAt(start)==' ')
			start++;
		//��Ϊ�գ���ӡ������Ϣ
		if(temp.IsEmpty())
		{
			cout<<"error: error in  condition statement!"<<endl;
			SQL="99";
			return SQL;
		}
		//��������
		else
		{
			sql=get_condition(temp,sql);
			if(sql=="99")
				return sql;
		}
		count++;
	}
	//��ȡ���һ������
	end=SQL.Find(" ;",start);
	temp=SQL.Mid(start,end-start);
	//��Ϊ�գ���ӡ������Ϣ
	if(temp.IsEmpty())
	{
		cout<<"error: error in  condition statement!"<<endl;
		SQL="99";
		return SQL;
	}
	//��������
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
//��֤select ����Ƿ���Ч
CString select_clause(CString SQL,int start)
{
	CString temp;
	int end;
	//��ȡ�ڶ�������
	while(SQL.GetAt(start)==' ')
		start++;
	if((end=SQL.Find(" from",start))==-1)
	{
		cout<<"syntax error: missing key word \"from\"!"<<endl;
		return "99";   //���ش�����Ϣ
	}
	temp=SQL.Mid(start,end-start);
	int ijk=temp.GetLength();
	while(temp.GetAt(ijk-1)==' ')
		ijk--;
	temp.Left(ijk);
	start=end+5;
	//���ޣ���ӡ������Ϣ
	if(temp.IsEmpty())
	{
		cout<<"syntax error: syntax error for select statement!"<<endl;
		return "99";   //���ش�����Ϣ
	}
	//��Ϊ��*����������֤
	else if(temp=="*")
	{
		//��ȡ���ĸ�����
		while(SQL.GetAt(start)==' ')
			start++;
		end=SQL.Find(' ',start);
		temp=SQL.Mid(start,end-start);
		start=end+1;
		//���ޣ���ӡ������Ϣ
		if(temp.IsEmpty())
		{
			cout<<"syntax error: syntax error for select statement!"<<endl;
			return "99";   //���ش�����Ϣ
		}
		//��Ϊ������������֤
		else
		{
			CString tname=temp;
			//��ȡ���������
			while(SQL.GetAt(start)==' ')
			start++;
			end=SQL.Find(' ',start);
			temp=SQL.Mid(start,end-start);
			start=end+1;
			//����,����select *�����ڲ���ʽ
			if(temp.IsEmpty())
				SQL="20"+tname;
			//��Ϊ"where"��������֤
			else if(temp="where")
			{
				CString it=selete_from_where(SQL,start);
				if(it=="99")
					return it;
				SQL="22"+tname+","+it;
			}
			//��Ϊ������Ϣ����ӡ������Ϣ
			else
			{
				cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
				return "99";   //���ش�����Ϣ
			}
		}
	}
	//��Ϊ����,������֤
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
		//��ȡ���ĸ�����
		while(SQL.GetAt(start)==' ')
		start++;
		end=SQL.Find(' ',start);
		temp=SQL.Mid(start,end-start);
		start=end+1;
		//���ޣ���ӡ������Ϣ
		if(temp.IsEmpty())
		{
			cout<<"syntax error: syntax error for select statement!"<<endl;
			return "99";   //���ش�����Ϣ
		}
		//��Ϊ������������֤
		else
		{
			CString tname=temp;
			//��ȡ���������
			while(SQL.GetAt(start)==' ')
			start++;
			end=SQL.Find(' ',start);
			temp=SQL.Mid(start,end-start);
			start=end+1;
			//����,����select ����������...�����ڲ���ʽ
			if(temp.IsEmpty())
				SQL="21"+tname+","+cc+","+cname+",";
			//��Ϊ"where"��������֤
			else if(temp="where")
			{
				CString it=selete_from_where(SQL,start);
				if(it=="99")
					return it;
				SQL="23"+tname+","+cc+","+cname+","+selete_from_where(SQL,start);
			}
			//��Ϊ������Ϣ����ӡ������Ϣ
			else
			{
				cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
				return "99";   //���ش�����Ϣ
			}
		}
	}
	//����select�����ڲ���ʽ
	return SQL;
}


/////////////////////////////////////////////////////////////////////////////////////////////
//14
//��֤insert into values����Ƿ���Ч
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
		//��Ϊ��ֵ����ӡ������Ϣ
		if(temp.IsEmpty())
		{
			cout<<"error: error in insert statement!"<<endl;
			SQL="99";
			return SQL;
		}
		//����ֵ
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
			else if(is_int(temp))  //����
			{
				sql=sql+"+ "+temp+",";
				i++;
			}
			else if(is_float(temp)) //������
			{
				sql=sql+"- "+temp+",";
				i++;
			}
			else                    //�����ʽ����
			{
				cout<<"error: wrong data format in insert statement!"<<endl;
			    SQL="99";
				return SQL;
			}
		}
		while(SQL.GetAt(start)==' ')
			start++;
	}
	//ȡ���ֵ
	end=SQL.Find(')',start);
	temp=SQL.Mid(start,end-start);
	length=temp.GetLength()-1;
	while(temp.GetAt(length)==' ')
		length--;
	temp=temp.Left(length+1);
	//��Ϊ��ֵ����ӡ������Ϣ
	if(temp.IsEmpty())
	{
		cout<<"error: error in insert statement!"<<endl;
		SQL="99";
		return SQL;
	}
	//����ֵ
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
		else if(is_int(temp))  //����
		{
			sql=sql+"+ "+temp+",";
			i++;
		}
		else if(is_float(temp)) //������
		{
			sql=sql+"- "+temp+",";
			i++;
		}
		else                    //�����ʽ����
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
//��֤insert ����Ƿ���Ч
CString insert_clause(CString SQL,int start)
{
	CString temp;
	int end,length;
	//��ȡ�ڶ�������
	while(SQL.GetAt(start)==' ')
		start++;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	//���ޣ���ӡ������Ϣ
	if(temp.IsEmpty())
	{
		cout<<"syntax error: syntax error for insert statement!"<<endl;
		return "99";   //���ش�����Ϣ
	}
	//��Ϊ��into����������֤
	else if(temp=="into")
	{
		//��ȡ����
		while(SQL.GetAt(start)==' ')
			start++;
		end=SQL.Find(' ',start);
		temp=SQL.Mid(start,end-start);
		start=end+1;
		//���ޣ���ӡ������Ϣ
		if(temp.IsEmpty())
		{
			cout<<"syntax error: syntax error for insert statement!"<<endl;
			return "99";   //���ش�����Ϣ
		}
		//��Ϊ������������֤
		else
		{
			CString tname=temp;
			//��ȡ���ĸ�����
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
			//���ޣ���ӡ������Ϣ
			if(temp.IsEmpty())
			{
				cout<<"syntax error: syntax error for insert statement!"<<endl;
				return "99";   //���ش�����Ϣ
			}
			//��Ϊ"values"��������֤
			else if(temp=="values")
			{
				CString it=insert_into_values(SQL,start);
				if(it=="99")
					return it;
				SQL="30"+tname+","+it;
			}
			//��Ϊ������Ϣ����ӡ������Ϣ
			else
			{
				cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
				return "99";   //���ش�����Ϣ
			}
		}
	}
	//��Ϊ������Ϣ����ӡ������Ϣ
	else
	{
		cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
		return "99";   //���ش�����Ϣ
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//16
//��֤ delete from where ����Ƿ���Ч
CString delete_from_where(CString SQL,int start)
{
	CString temp,sql;
	int end;
	int count=0;
	while(SQL.GetAt(start)==' ')
		start++;
	while((end=SQL.Find(" and ",start))!=-1)//Ѱ�ҡ�and���ؼ���
	{
		temp=SQL.Mid(start,end-start);
		start=end+5;
		while(SQL.GetAt(start)==' ')
			start++;
		//��Ϊ�գ���ӡ������Ϣ
		if(temp.IsEmpty())
		{
			cout<<"error: error in  condition statement!"<<endl;
			SQL="99";
			return SQL;
		}
		//��������
		else
		{
			sql=get_condition(temp,sql);
			if(sql=="99")
				return sql;
		}
		count++;
	}
	//��ȡ���һ������
	end=SQL.Find(" ;",start);
	temp=SQL.Mid(start,end-start);
	//��Ϊ�գ���ӡ������Ϣ
	if(temp.IsEmpty())
	{
		cout<<"error: error in  condition statement!"<<endl;
		SQL="99";
		return SQL;
	}
	//��������
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
//��֤delete����Ƿ���Ч
CString delete_clause(CString SQL,int start)
{
	CString temp;
	int end;
	//��ȡ�ڶ�������
	while(SQL.GetAt(start)==' ')
		start++;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	//���ޣ���ӡ������Ϣ
	if(temp.IsEmpty())
	{
		cout<<"syntax error: syntax error for delect statement!"<<endl;
		return "99";   //���ش�����Ϣ
	}
	//��Ϊ��from����������֤
	else if(temp=="from")
	{
		//��ȡ����������
		while(SQL.GetAt(start)==' ')
			start++;
		end=SQL.Find(' ',start);
		temp=SQL.Mid(start,end-start);
		start=end+1;
		//���ޣ���ӡ������Ϣ
		if(temp.IsEmpty())
		{
			cout<<"syntax error: syntax error for delect statement!"<<endl;
			return "99";   //���ش�����Ϣ
		}
		//��Ϊ������������֤
		else
		{
			CString tname=temp;
			//��ȡ���ĸ�����
			while(SQL.GetAt(start)==' ')
				start++;
			end=SQL.Find(' ',start);
			temp=SQL.Mid(start,end-start);
			start=end+1;
			//���ޣ�����delete�ڲ���ʽ
			if(temp.IsEmpty())
				SQL="40"+tname;
			//��Ϊ"where"��������֤
			else if(temp=="where")
			{
				CString it=delete_from_where(SQL,start);
				if(it=="99")
					return it;
				SQL="41"+tname+","+it;
			}
			//��Ϊ������Ϣ����ӡ������Ϣ
			else
			{
				cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
				return "99";   //���ش�����Ϣ
			}
		}
	}
	//��Ϊ������Ϣ����ӡ������Ϣ
	else
	{
		cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
		return "99";   //���ش�����Ϣ
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//18
//�����������ļ����ÿһ��
CString get_part(CString temp,CString sql,CString kind)
{
	return "";
}

/////////////////////////////////////////////////////////////////////////////////////////////
//19
//�����ʽת��Ϊ�ڲ���ʽ
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
//��ȡ���ʽ���ÿ�����ʽ
CString get_each(CString sql,int &begin)
{
	CString temp;
	int start=begin;
	begin=sql.Find(';',begin)+1;     //SQL����ԡ�;���ָ�
	temp=sql.Mid(start,begin-start);
	if((temp=get_expression(temp))=="99")
		return "99";
	else
		return temp+";";
}

/////////////////////////////////////////////////////////////////////////////////////////////
//21
//��֤use����Ƿ���Ч
CString use_clause(CString SQL,int start)
{
	CString temp;
	int index,end;
	//��ȡ�ڶ�������
	while(SQL.GetAt(start)==' ')
		start++;
	index=start;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	//���ޣ���ӡ������Ϣ
	if(temp.IsEmpty())
	{
		cout<<"error: database name has not been specified!"<<endl;
		return "99";   //���ش�����Ϣ
	}
	else
	{	
		while(SQL.GetAt(start)==' ')
			start++;
		//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
		if(SQL.GetAt(start)!=';'||start!=SQL.GetLength()-1)
		{
			cout<<"error:"<<SQL.Mid(index,SQL.GetLength()-index-2)<<"---is not a valid database name!"<<endl;
			return "99";   //���ش�����Ϣ
		}
		//����use database�����ڲ���ʽ
		else
			SQL="50"+temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//22
//��֤execfile����Ƿ���Ч
CString execfile_clause(CString SQL,int start)
{
	CString temp;
	int end;
	int count=0;
	//��ȡ�ڶ�������
	while(SQL.GetAt(start)==' ')
		start++;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	string sql;
	CString tp;
	//���ޣ���ӡ������Ϣ
	if(temp.IsEmpty())
	{
		cout<<"syntax error: syntax error for execfile statement!"<<endl;
		SQL="99";
		return SQL;
	}
	//��Ϊ�ļ�����������֤
	else
	{
		CString fname=temp;
		//��ȡ����������
		while(SQL.GetAt(start)==' ')
			start++;
		end=SQL.Find(' ',start);
		temp=SQL.Mid(start,end-start);
		start=end+1;
		
		//���ж��൥�ʣ���ӡ������Ϣ
		if(!temp.IsEmpty())
		{
			cout<<"syntax error: syntax error for execfile statement!"<<endl;
			SQL="99";
			return SQL;
		}
		else
		{
			if((_access(fname,0))==-1)        //�ļ�������
			{
				cout<<"error: file does not exist!"<<endl;
				SQL="99";
				return SQL;
			}
			else 
			{
				SQL="";
				fstream in(fname);             //���ļ�
				string s;
				CString ss;
				while(getline(in,s))            //���ļ�
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
				for(i=0;i<sql.length();i++)     //ͳ��SQL�������
					if(sql[i]==';')
						count++;
				/*int begin=0;
				for(i=0;i<count;i++)            //SQL���ת��Ϊ�ڲ�����
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
//��֤quit����Ƿ���Ч
CString quit_clause(CString SQL,int start)
{
	CString temp;
	int index,end;
	//�ж�quit֮���Ƿ�����������
	while(SQL.GetAt(start)==' ')
		start++;
	index=start;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	//���ޣ�����quit�����ڲ���ʽ
	if(temp.IsEmpty())
		SQL="70";
	else
	{	//�������������Ϣ
		cout<<"syntax error: in quit_clause, there should be no other words after \"quit\" !"<<endl;
		SQL="99";
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//24
//��ȡ�û����룬������������Ч�Լ�飬����ȷ�����������ڲ���ʾ��ʽ
CString Interpreter(CString SQL)
{
	CString temp;
	//CString sql;
	int start=0,end;
	if(SQL=="")
		SQL=read_input();//��ȡ�û�����;
	//��ȡ����ĵ�һ������
	while(SQL.GetAt(start)==' ')
		start++;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	//�������룬��ӡ������Ϣ
	if(temp.IsEmpty())
	{
		cout<<"syntax error: empty statement!"<<endl;
		return "99";   //���ش�����Ϣ
	}
	//��Ϊcreate���
	else if(temp=="create")
		SQL=create_clause(SQL,start);
	//��Ϊdrop���
	else if(temp=="drop")
		SQL=drop_clause(SQL,start);
	//��Ϊselect���
	else if(temp=="select")
		SQL=select_clause(SQL,start);
	//��Ϊinsert���
	else if(temp=="insert")
		SQL=insert_clause(SQL,start);
	//��Ϊdelete���
	else if(temp=="delete")
		SQL=delete_clause(SQL,start);
	//��Ϊuse���
	else if(temp=="use")
		SQL=use_clause(SQL,start);
	//��Ϊexecfile���
	else if(temp=="execfile")
		SQL=execfile_clause(SQL,start);
	//��Ϊquit���
	else if(temp=="quit")
		SQL=quit_clause(SQL,start);
	//��ȡ����
	else if(temp=="help")
		SQL="80";
	//��Ϊ�Ƿ����
	else
	{
		cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
		return "99";   //���ش�����Ϣ
	}
	//�������������ڲ���ʽ
	//cout<<SQL<<endl;
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//25
//�ж��Ƿ�Ϊ�ַ���
bool is_string(CString temp)
{
	if(temp.GetAt(0)=='\''&&temp.GetAt(temp.GetLength()-1)=='\'')
		return true;
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//26
//�ж��Ƿ�Ϊ������
bool is_float(CString temp)
{
	int flag=0;
	if(temp.GetAt(0)=='.'||temp.GetAt(temp.GetLength()-1)=='.')  //�ԡ�.����ͷ���β
		return false;
	for(int i=0;i<temp.GetLength();i++)                          //�зǷ��ַ�
		if(temp.GetAt(i)<'0'||temp.GetAt(i)>'9')
			if(temp.GetAt(i)!='.')
				return false;
	for(int i=1;i<temp.GetLength()-1;i++)                        //ͳ�ơ�.������
		if(temp.GetAt(i)=='.')
			flag++;
	if(flag==1)                                               
		return true;
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//27
//�ж��Ƿ�Ϊ����
bool is_int(CString temp)
{
	for(int i=0;i<temp.GetLength();i++)
		if(temp.GetAt(i)<'0'||temp.GetAt(i)>'9')
			return false;
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//28
//�жϸ�ʽ�Ƿ���ȷ
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