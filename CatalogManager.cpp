//////////////////////////////////////////////////////////
///----------------------------------------------------///
///       Module: CatalogManager                       ///
///       Produced by: ����                            ///
///       Description: Produced to offer basic         ///
///                    information for other modules   ///
///                    about database                  ///
///       date: 2013/10/24                             ///
///----------------------------------------------------///
//////////////////////////////////////////////////////////

#include<iostream>
#include<fstream>
#include<io.h>
#include<direct.h>
#include<string>
#include<cstdlib>
#include"CatalogManager.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////////
//0
//�������ݿ�
bool Create_Database(CString DB_Name)
{
	if((_access(DB_Name,0))!=-1)        //���ݿ��Ѵ���
	{
		cout<<"error: database -- "<<DB_Name<<" already exists!"<<endl;
		return false;
	}
	else if(_mkdir(DB_Name)==0)        //�½����ݿ�
	{
		ofstream fout(DB_Name+"\\"+DB_Name+".cata");   //���������ֵ�
		if(fout)
		{
			fout<<0<<endl;
			fout.close();
			cout<<"create database -- "<<DB_Name<<" success"<<endl;
			/*ofstream indexall(DB_Name+"\\index.all");   //��������Ŀ¼
			indexall.close();*/
			ofstream headall(DB_Name+"\\head.all");     //������ͷĿ¼
			headall.close();
			return true;
		}
		else                                           //�½������ֵ�ʧ��
		{
			_rmdir(DB_Name);
			cout<<"error: create table catalog fail"<<endl;
			return false;
		}
	}
	else                             //�½����ݿ�ʧ��
	{
		cout<<"error: create database -- "<<DB_Name<<" fail"<<endl;
		return false;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
//1
//������
bool Create_Table(CString Table_Name,CString Attr,CString DB_Name,CString & Attr_Name)
{
	if((_access(DB_Name+"\\"+Table_Name+".db",0))!=-1)        //���Ѵ���
	{
		cout<<"error: table -- "<<Table_Name<<" already exists!"<<endl;
		return false;
	}
	else                                                      //�½���
	{
		ofstream fout(DB_Name+"\\"+Table_Name+".db");         
		if(fout)                                             
		{
			fout.close();
			//ͳ�����Ը���
			int index=Attr.Find(','),end; 
			end=index;
			CString tmp=Attr.Left(index);
			index=end+1;
			int count=atoi((LPCTSTR)tmp);
			//�ж��Ƿ�������
			index=Attr.Find(',',index);
			tmp=Attr.Mid(end+1,index-end-1);
			end=index;
			index=end+1;
			int flag=0,key=atoi((LPCTSTR)tmp);
			CString t[32][3];
			int len[32];
			//��ȡÿ������
			for(int i=0;i<count;i++)
			{
				index=Attr.Find(',',index);
				tmp=Attr.Mid(end+1,index-end-1);
				end=index;
				index=end+1;
				int a,b;
				a=tmp.Find(' ');
				t[i][0]=tmp.Left(a);
				b=a;
				a=b+1;
				a=tmp.Find(' ',a);
				t[i][1]=tmp.Mid(b+1,a-b-1);
				t[i][2]=tmp.Right(tmp.GetLength()-a-1);
				//cout<<t[0]<<" "<<t[1]<<" "<<t[2]<<endl;
			}
			for(int i=0;i<count;i++)
			{
				len[i]=FindLen(t[i][1]);
				if(len[i]>255||len[i]<1)    //���ݳ��ȷǷ�
				{
					cout<<"error: illegal data length for atrtibute -- "<<t[i][0]<<endl;
					DeleteFile(DB_Name+"\\"+Table_Name+".db");
					return false;
				}
			}
			for(int i=1;i<count;i++)
				len[i]+=len[i-1];
			if(key==1)        //������
			{
				index=Attr.Find(',',index);
				tmp=Attr.Mid(end+1,index-end-1);
				index=tmp.Find(' ');
				Attr_Name=tmp.Left(index);
				for(int i=0;i<count;i++)
					if(t[i][0]==Attr_Name)
					{
						flag=1;
						t[i][2]="1";
					}
				if(flag==0)    //����������֪����
				{
					cout<<"error: can not find such key -- "<<Attr_Name<<endl;
					DeleteFile(DB_Name+"\\"+Table_Name+".db");
					return false;
				}
				else
				{
					ifstream headfile(DB_Name+"\\head.all"); //���±�ͷĿ¼
					ofstream file(DB_Name+"\\temp");
					string ss;
					while(getline(headfile,ss))  //��������
						file<<ss<<endl;
					file<<Table_Name<<" "<<len[count-1]+1<<endl;//����
					headfile.close();
					file.close();
					DeleteFile(DB_Name+"\\head.all");
					rename(DB_Name+"\\temp",DB_Name+"\\head.all");
					ofstream hfile(DB_Name+"\\"+Table_Name+".head"); //�½���ͷ
					hfile<<len[count-1]+1<<endl<<count<<endl<<t[0][0]<<" "<<0<<endl;
					for(int i=1;i<count;i++)
						hfile<<t[i][0]<<" "<<len[i-1]<<endl;
					hfile.close();
					fstream catafile(DB_Name+"\\"+DB_Name+".cata");    //����Ϣд�������ֵ�
					ofstream tmp(DB_Name+"\\tmp");               //��ʱ�ļ�
					int n;
					catafile>>n;
					n++;
					tmp<<n<<endl; //������һ
					string str;
					getline(catafile,str);
					while(getline(catafile,str))
						tmp<<str<<endl;
					catafile.close();
					DeleteFile(DB_Name+"\\"+DB_Name+".cata");
					//�������ֵ���׷������
					tmp<<Table_Name<<" "<<Attr_Name<<" "<<count<<" "<<0<<endl;
					tmp<<t[0][0]<<" "<<t[0][1]<<" "<<len[0]<<" "<<t[0][2]<<" !"<<endl;
					for(int i=1;i<count;i++)
						tmp<<t[i][0]<<" "<<t[i][1]<<" "<<len[i]-len[i-1]<<" "<<t[i][2]<<" !"<<endl;
					tmp.close();
					rename(DB_Name+"\\tmp",DB_Name+"\\"+DB_Name+".cata");
					cout<<"create table -- "<<Table_Name<<" success!"<<endl;
					return true;
				}
			}
			else //û����
			{
				ifstream headfile(DB_Name+"\\head.all"); //���±�ͷĿ¼
				ofstream file(DB_Name+"\\temp");
				string ss;
				while(getline(headfile,ss))  //��������
					file<<ss<<endl;
				file<<Table_Name<<" "<<len[count-1]+1<<endl;//����
				headfile.close();
				file.close();
				DeleteFile(DB_Name+"\\head.all");
				rename(DB_Name+"\\temp",DB_Name+"\\head.all");
				ofstream hfile(DB_Name+"\\"+Table_Name+".head"); //�½���ͷ
				hfile<<len[count-1]+1<<endl<<count<<endl<<t[0][0]<<" "<<0<<endl;
				for(int i=1;i<count;i++)
					hfile<<t[i][0]<<" "<<len[i-1]<<endl;
				hfile.close();
				fstream catafile(DB_Name+"\\"+DB_Name+".cata");    //����Ϣд�������ֵ�
				ofstream tmp(DB_Name+"\\tmp");               //��ʱ�ļ�
				int n;
				catafile>>n;
				n++;
				tmp<<n<<endl; //������һ
				string str;
				getline(catafile,str);
				while(getline(catafile,str))
					tmp<<str<<endl;
				catafile.close();
				DeleteFile(DB_Name+"\\"+DB_Name+".cata");
				//�������ֵ���׷������
				tmp<<Table_Name<<" "<<"!"<<" "<<count<<" "<<0<<endl;
				tmp<<t[0][0]<<" "<<t[0][1]<<" "<<len[0]<<" "<<t[0][2]<<" !"<<endl;
				for(int i=1;i<count;i++)
					tmp<<t[i][0]<<" "<<t[i][1]<<" "<<len[i]-len[i-1]<<" "<<t[i][2]<<" !"<<endl;
				tmp.close();
				rename(DB_Name+"\\tmp",DB_Name+"\\"+DB_Name+".cata");
				cout<<"create table -- "<<Table_Name<<" success!"<<endl;
				return true;
			}
		}
		else                                                  //����ʧ��
		{
			cout<<"error: create table -- "<<Table_Name<<" fail"<<endl;
			return false;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
//2
//��������
bool Create_Index(CString Index_Name,CString Table_Name,CString Attr_Name,
				  CString DB_Name,int & length,int & offset,int & type)
{
	if((_access(DB_Name+"\\"+Index_Name+".idx",0))!=-1)        //�����Ѵ���
	{
		cout<<"error: index -- "<<Index_Name<<" already exists!"<<endl;
		return false;
	}
	int count,flag=0,num=0;
	CString TName,AName;
	fstream catafile(DB_Name+"\\"+DB_Name+".cata");  //�����ֵ��в���
	catafile>>count;   //����
	string str;
	getline(catafile,str);
	for(int i=0;i<count;i++)
	{
		getline(catafile,str);
		num++;
		CString tmp=str.c_str(),ss;
		int end=tmp.Find(' ');
		int start=end+1;
		TName=tmp.Left(end);  //����
		end=tmp.Find(' ',start);
		start=end+1;
		end=tmp.Find(' ',start);
		ss=tmp.Mid(start,end-start);
		int cc=atoi((LPCTSTR)ss);
		if(TName.Compare(Table_Name)==0)  //�ҵ��ñ�
		{
			flag=1;
			for(int j=0;j<cc;j++)
			{
				getline(catafile,str);
				num++;
				tmp=str.c_str();
				end=tmp.Find(' ');
				start=end+1;
				AName=tmp.Left(end);
				if(AName.Compare(Attr_Name)==0) //�ҵ�������
				{
					end=tmp.Find(' ',start);
					ss=tmp.Mid(start,end-start);
					if(ss.Compare("+")==0)
						type=0;
					else if(ss.Compare("-")==0)
						type=1;
					else
						type=2;
					start=end+1;
					end=tmp.Find(' ',start);
					ss=tmp.Mid(start,end-start);
					length=atoi((LPCTSTR)ss);  //���Գ���
					start=end+1;
					end=tmp.Find(' ',start);
					ss=tmp.Mid(start,end-start);
					if(ss.Compare("0")==0)    //���Էǵ�ֵ
					{
						cout<<"error: attribute -- "<<Attr_Name<<" is not unique!"<<endl;
						catafile.close();
						return false;
					}
					ofstream indexfile(DB_Name+"\\"+Index_Name+".idx"); //�½�����
					indexfile.close();
					catafile.close();
					//���������ֵ�
					fstream in(DB_Name+"\\"+DB_Name+".cata");
					ofstream temp(DB_Name+"\\tmp"); //��ʱ�ļ�
					int ii=0;
					string st;
					while(getline(in,st))
					{
						if(ii==num)
						{
							CString cs=st.c_str();
							int end=cs.ReverseFind(' ');
							cs=cs.Left(end)+" "+Index_Name;
							temp<<cs<<endl;
						}
						else
							temp<<st<<endl;
						ii++;
					}
					in.close();
					temp.close();
					DeleteFile(DB_Name+"\\"+DB_Name+".cata");
					rename(DB_Name+"\\tmp",DB_Name+"\\"+DB_Name+".cata");
					/*ifstream indexall(DB_Name+"\\index.all");  //����Ϣд������Ŀ¼
					ofstream tp(DB_Name+"\\tmp"); //��ʱ�ļ�
					while(getline(indexall,str))
						tp<<str<<endl;
					tp<<Index_Name<<" "<<(1024*4-1-4-3)/(length+INDEX_OFFSET)+1<<endl;
					indexall.close();
					tp.close();
					DeleteFile(DB_Name+"\\index.all");
					rename(DB_Name+"\\tmp",DB_Name+"\\index.all");*/
					fstream input(DB_Name+"\\"+Table_Name+".head");//���ƫ����
					getline(input,str);
					getline(input,str);
					while(getline(input,str))
					{
						CString cs=str.c_str();
						int end=cs.Find(' ');
						CString name,off;
						name=cs.Left(end);
						if(name.Compare(Attr_Name)==0)
						{
							off=cs.Right(cs.GetLength()-1-end);
							offset=atoi((LPCTSTR)off);
							break;
						}
						else
							continue;
					}
					input.close();
					cout<<"create index -- "<<Index_Name<<" success!"<<endl;
					return true;
				}
				else
					continue;
			}
			break;   //δ�ҵ�������
		}
		else   //׼����֤��һ�ű�
			for(int j=0;j<cc;j++)
			{
				getline(catafile,str);
				num++;
			}
	}
	catafile.close();
	if(flag==1)    //δ�ҵ�������
	{
		cout<<"error: can not find such attribute -- "<<Attr_Name<<endl;
		return false;
	}
	else    //δ�ҵ��ñ�
	{
		cout<<"error: can not find such table -- "<<Table_Name<<endl;
		return false;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
//3
//ɾ�����ݿ�
bool Drop_Database(CString DB_Name)
{
	if((_access(DB_Name,0))!=-1)        //���ݿ����
	{
		char *p=DB_Name.GetBuffer(DB_Name.GetLength());
		if(DeleteDirectory(p))
		{
			cout<<"drop database -- "<<DB_Name<<" success!"<<endl;
			return true;
		}
		else
		{
			cout<<"error: drop database -- "<<DB_Name<<" fail!"<<endl;
			return true;
		}
	}
	else
	{
		cout<<"error: database -- "<<DB_Name<<" does not exist!"<<endl;
		return false;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
//4
//ɾ����
bool Drop_Table(CString Table_Name,CString DB_Name,CString index_name[32],int & count)
{
	if((_access(DB_Name+"\\"+Table_Name+".db",0))!=-1)        //�����
	{
		DeleteFile(DB_Name+"\\"+Table_Name+".db");   //ɾ�����ļ�
		DeleteFile(DB_Name+"\\"+Table_Name+".head"); //ɾ����ͷ�ļ�
		//���±�ͷĿ¼
		ifstream headfile(DB_Name+"\\head.all");
		ofstream file(DB_Name+"\\temp");
		string ss;
		while(getline(headfile,ss))  //��������
		{
			CString tmp=ss.c_str(),tabname;
			int end=tmp.Find(' ');
			tabname=tmp.Left(end);
			if(tabname.Compare(Table_Name)==0) //ɾȥ�ü�¼
				continue;
			file<<ss<<endl;
		}
		headfile.close();
		file.close();
		DeleteFile(DB_Name+"\\head.all");
		rename(DB_Name+"\\temp",DB_Name+"\\head.all");
		//���������ֵ�
		count=0;
		int count1,cc,num=0;
		CString TName;
		fstream catafile(DB_Name+"\\"+DB_Name+".cata");  //�����ֵ��в���
		catafile>>count1;   //����
		string str;
		getline(catafile,str);
		for(int i=0;i<count1;i++)
		{
			getline(catafile,str);
			num++;
			CString tmp=str.c_str(),ss;
			int end=tmp.Find(' ');
			int start=end+1;
			TName=tmp.Left(end);  //����
			end=tmp.Find(' ',start);
			start=end+1;
			end=tmp.Find(' ',start);
			ss=tmp.Mid(start,end-start);
			cc=atoi((LPCTSTR)ss);
			if(TName.Compare(Table_Name)==0)  //�ҵ��ñ�
				break;
			else               //׼����֤��һ�ű�
				for(int j=0;j<cc;j++)
				{
					getline(catafile,str);
					num++;
				}
		}
		catafile.close();
		ifstream in(DB_Name+"\\"+DB_Name+".cata");
		ofstream temp(DB_Name+"\\tmp"); //��ʱ�ļ�
		int ii=0;
		string st;
		while(getline(in,st))
		{
			if(ii==0)
			{
				CString cstr=st.c_str();
				int a=atoi((LPCTSTR)cstr);
				a--;
				temp<<a<<endl;
			}
			else if(ii>num&&ii<=num+cc)
			{
				CString cs=st.c_str();
				int end=cs.ReverseFind(' ');
				cs=cs.Right(cs.GetLength()-end-1);
				if(cs.Compare("!")!=0)
					index_name[count++]=cs; //��ñ�������
			}
			else if(ii==num)
			{
				ii++;
				continue;
			}
			else
				temp<<st<<endl;
			ii++;
		}
		in.close();
		temp.close();
		DeleteFile(DB_Name+"\\"+DB_Name+".cata");
		rename(DB_Name+"\\tmp",DB_Name+"\\"+DB_Name+".cata");
		cout<<"drop table -- "<<Table_Name<<" success!"<<endl;
		return true;
	}
	else
	{
		cout<<"error: table -- "<<Table_Name<<" does not exist!"<<endl;
		return false;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
//5
//ɾ������
bool Drop_Index(CString Index_Name,CString DB_Name)
{
	if((_access(DB_Name+"\\"+Index_Name+".idx",0))!=-1)        //��������
	{
		DeleteFile(DB_Name+"\\"+Index_Name+".idx"); //ɾ�������ļ�
		/*//����Ŀ¼����
		ifstream indexall(DB_Name+"\\index.all");
		ofstream tp(DB_Name+"\\tmp"); //��ʱ�ļ�
		while(getline(indexall,str))
		{
			CString cs=str.c_str();
			int end=cs.Find(' ');
			cs=cs.Left(end);
			if(cs.Compare(Index_Name)!=0)
				tp<<str<<endl;
		}
		indexall.close();
		tp.close();
		DeleteFile(DB_Name+"\\index.all");
		rename(DB_Name+"\\tmp",DB_Name+"\\index.all");*/
		//���������ֵ�
		string str;
		int count,flag=0,num=0;
		fstream catafile(DB_Name+"\\"+DB_Name+".cata");  //�����ֵ��в���
		catafile>>count;   //����
		getline(catafile,str);
		for(int i=0;i<count;i++)
		{
			getline(catafile,str);
			num++;
			CString tmp=str.c_str(),ss;
			int end=tmp.Find(' ');
			int start=end+1;
			end=tmp.Find(' ',start);
			start=end+1;
			end=tmp.Find(' ',start);
			ss=tmp.Mid(start,end-start); //������
			int cc=atoi((LPCTSTR)ss);
			for(int j=0;j<cc;j++)
			{
				getline(catafile,str);
				num++;
				tmp=str.c_str();
				end=tmp.ReverseFind(' ');
				ss=tmp.Right(tmp.GetLength()-end-1); //������
				if(ss.Compare(Index_Name)==0)
				{
					flag=1;
					break;
				}
			}
			if(flag==1)
				break;
		}
		if(flag==0)  //�Ѵ������ֵ���ɾ��
		{
			cout<<"drop index -- "<<Index_Name<<" success!"<<endl;
			return true;
		}
		catafile.close();
		ifstream in(DB_Name+"\\"+DB_Name+".cata");
		ofstream temp(DB_Name+"\\tmp"); //��ʱ�ļ�
		int ii=0;
		string st;
		while(getline(in,st))
		{
			if(ii==num)
			{
				CString cs=st.c_str();
				int end=cs.ReverseFind(' ');
				cs=cs.Left(end)+" !";
				temp<<cs<<endl;
			}
			else
				temp<<st<<endl;
			ii++;
		}
		in.close();
		temp.close();
		DeleteFile(DB_Name+"\\"+DB_Name+".cata");
		rename(DB_Name+"\\tmp",DB_Name+"\\"+DB_Name+".cata");
		cout<<"drop index -- "<<Index_Name<<" success!"<<endl;
		return true;
	}
	else
	{
		cout<<"error: index -- "<<Index_Name<<" does not exist!"<<endl;
		return false;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
//6
//��ȡ��ʾ������Ϣ
bool Get_Attr_Info(CString DB_Name,CString Table_Name,attr_info print[32],int & count,CString Attr)
{
	if((_access(DB_Name+"\\"+Table_Name+".db",0))==-1)        //������
	{
		cout<<"error: table -- "<<Table_Name<<" does not exist!"<<endl;
		return false;
	}
	int end=Attr.Find(',');
	int start=end+1;
	CString cstr=Attr.Left(end);
	count=atoi((LPCTSTR)cstr);
	for(int i=0;i<count;i++)
	{
		end=Attr.Find(',',start);
		print[i].attr_name=Attr.Mid(start,end-start);
		start=end+1;
	}
	int count1,cc,num=0;
	CString TName;
	fstream catafile(DB_Name+"\\"+DB_Name+".cata");  //�����ֵ��в���
	catafile>>count1;   //����
	string str;
	getline(catafile,str);
	for(int i=0;i<count1;i++)
	{
		getline(catafile,str);
		CString tmp=str.c_str(),ss;
		int end=tmp.Find(' ');
		int start=end+1;
		TName=tmp.Left(end);  //����
		end=tmp.Find(' ',start);
		start=end+1;
		end=tmp.Find(' ',start);
		ss=tmp.Mid(start,end-start);
		cc=atoi((LPCTSTR)ss);  //������
		if(TName.Compare(Table_Name)==0)  //�ҵ��ñ�
		{
			for(int j=0;j<cc;j++)
			{
				int flag=0;
				getline(catafile,str);
				tmp=str.c_str();
				end=tmp.Find(' ');
				start=end+1;
				CString aname=tmp.Left(end);  //������
				end=tmp.Find(' ',start);
				CString atype=tmp.Mid(start,end-start);
				start=end+1;
				end=tmp.Find(' ',start);
				CString alen=tmp.Mid(start,end-start);
				int aa=atoi((LPCTSTR)alen);
				for(int k=0;k<count;k++)
					if(print[k].attr_name.Compare(aname)==0)
					{
						print[k].length=aa;
						if(atype.Compare("+")==0)
							print[k].type=0;
						else if(atype.Compare("-")==0)
							print[k].type=1;
						else
							print[k].type=2;
						flag++;
						num++;
					}
				if(flag>1)
				{
					cout<<"error: two or more identical attributes are selected !"<<endl;
					return false;
				}
			}
			break;
		}
		else               //׼����֤��һ�ű�
			for(int j=0;j<cc;j++)
				getline(catafile,str);
	}
	catafile.close();
	if(num<count)
	{
		cout<<"error: illegal attribute in the statement!"<<endl;
		return false;
	}
	ifstream headfile(DB_Name+"\\"+Table_Name+".head"); //��ȡ��ͷ�ļ�������ƫ����
	getline(headfile,str);
	getline(headfile,str);
	while(getline(headfile,str))
	{
		CString tmp=str.c_str(),ss;
		int end=tmp.Find(' ');
		ss=tmp.Left(end); //������
		for(int i=0;i<count;i++)
			if(print[i].attr_name.Compare(ss)==0)
			{
				ss=tmp.Right(tmp.GetLength()-end-1);
				print[i].offset=atoi((LPCTSTR)ss); //ƫ����
				break;
			}
	}
	headfile.close();
	//cout<<"Get_Attr_Info"<<endl;
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//7
//��ȡ�������Ե���Ϣ
bool Get_Attr_Info_All(CString DB_Name,CString Table_Name,attr_info print[32],int & count)
{
	if((_access(DB_Name+"\\"+Table_Name+".db",0))==-1)        //������
	{
		cout<<"error: table -- "<<Table_Name<<" does not exist!"<<endl;
		return false;
	}
	int count1,cc;
	CString TName;
	fstream catafile(DB_Name+"\\"+DB_Name+".cata");  //�����ֵ��в���
	catafile>>count1;   //����
	string str;
	getline(catafile,str);
	for(int i=0;i<count1;i++)
	{
		getline(catafile,str);
		CString tmp=str.c_str(),ss;
		int end=tmp.Find(' ');
		int start=end+1;
		TName=tmp.Left(end);  //����
		end=tmp.Find(' ',start);
		start=end+1;
		end=tmp.Find(' ',start);
		ss=tmp.Mid(start,end-start);
		cc=atoi((LPCTSTR)ss);  //������
		if(TName.Compare(Table_Name)==0)  //�ҵ��ñ�
		{
			count=cc;
			for(int j=0;j<cc;j++)
			{
				getline(catafile,str);
				tmp=str.c_str();
				end=tmp.Find(' ');
				start=end+1;
				print[j].attr_name=tmp.Left(end);  //������
				end=tmp.Find(' ',start);
				CString atype=tmp.Mid(start,end-start);
				start=end+1;
				end=tmp.Find(' ',start);
				CString alen=tmp.Mid(start,end-start);
				print[j].length=atoi((LPCTSTR)alen);
				if(atype.Compare("+")==0)
					print[j].type=0;
				else if(atype.Compare("-")==0)
					print[j].type=1;
				else
					print[j].type=2;
			}
			break;
		}
		else               //׼����֤��һ�ű�
			for(int j=0;j<cc;j++)
				getline(catafile,str);
	}
	catafile.close();
	ifstream headfile(DB_Name+"\\"+Table_Name+".head"); //��ȡ��ͷ�ļ�������ƫ����
	getline(headfile,str);
	getline(headfile,str);
	int tt=0;
	while(getline(headfile,str))
	{
		CString tmp=str.c_str(),ss;
		int end=tmp.Find(' ');
		ss=tmp.Right(tmp.GetLength()-end-1);
		print[tt++].offset=atoi((LPCTSTR)ss); //ƫ����
	}
	headfile.close();
	//cout<<"Get_Attr_Info_All"<<endl;
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//8
//ת��������
bool Get_Condition(CString DB_Name,CString Table_Name,CString Condition,                              
	condition_info conds[32],int & count,index_info nodes[32],int & num)
{
	if((_access(DB_Name+"\\"+Table_Name+".db",0))==-1)        //������
	{
		cout<<"error: table -- "<<Table_Name<<" does not exist!"<<endl;
		return false;
	}
	num=0;
	CString name[32];
	CString types[32];
	CString nm[32];
	int end=Condition.Find(',');
	int start=end+1;
	CString cstr=Condition.Left(end);
	count=atoi((LPCTSTR)cstr);     //������
	for(int i=0;i<count;i++)       //���������Ϣ
	{
		end=Condition.Find(',',start);
		CString each=Condition.Mid(start,end-start);
		int back=each.Find(' ');
		int front=back+1;
		name[i]=each.Left(back);
		back=each.Find(' ',front);
		CString opt=each.Mid(front,back-front);  //�ж�op
		if(opt.Compare(">")==0)
			conds[i].op=1;
		else if(opt.Compare(">=")==0)
			conds[i].op=2;
		else if(opt.Compare("<")==0)
			conds[i].op=3;
		else if(opt.Compare("<=")==0)
			conds[i].op=4;
		else if(opt.Compare("=")==0)
			conds[i].op=5;
		else
			conds[i].op=6;
		front=back+1;
		back=each.Find(' ',front);
		types[i]=each.Mid(front,back-front);  //��ֵ����
		conds[i].const_data=each.Right(each.GetLength()-back-1); //��ֵ 
		//cout<<name[i]+" "<<conds[i].op<<" "+types[i]+" "+conds[i].const_data<<endl;
		start=end+1;
	}
	int count1,cc,flag=count;
	CString TName;
	fstream catafile(DB_Name+"\\"+DB_Name+".cata");  //�����ֵ��в���
	catafile>>count1;   //����
	string str;
	getline(catafile,str);
	for(int i=0;i<count1;i++)
	{
		getline(catafile,str);
		CString tmp=str.c_str(),ss;
		int end=tmp.Find(' ');
		int start=end+1;
		TName=tmp.Left(end);  //����
		end=tmp.Find(' ',start);
		start=end+1;
		end=tmp.Find(' ',start);
		ss=tmp.Mid(start,end-start);
		cc=atoi((LPCTSTR)ss);  //������
		if(TName.Compare(Table_Name)==0)  //�ҵ��ñ�
		{
			for(int j=0;j<cc;j++)
			{
				getline(catafile,str);
				tmp=str.c_str();
				end=tmp.Find(' ');
				start=end+1;
				CString aname=tmp.Left(end);  //������
				end=tmp.Find(' ',start);
				CString atype=tmp.Mid(start,end-start);
				start=end+1;
				end=tmp.Find(' ',start);
				CString alen=tmp.Mid(start,end-start);
				int aa=atoi((LPCTSTR)alen);
				start=end+1;
				end=tmp.Find(' ',start);
				CString iname=tmp.Right(tmp.GetLength()-end-1);
				if(iname.Compare("!")!=0)
				{
					nm[num]=aname;
					nodes[num].length=aa;
					if(atype.Compare("+")==0)  //��������Ϊ����
						nodes[num].type=0;
					else if(atype.Compare("-")==0)  //��������Ϊ������
						nodes[num].type=1;
					else                     //��������Ϊchar
						nodes[num].type=2;
					nodes[num++].index_name=iname;  //ȡ��������
				}
				for(int k=0;k<count;k++)
					if(name[k].Compare(aname)==0)  //�ҵ�������ֵ��Ӧ����
					{
						//�ж����������Ƿ�ƥ��
						if(atype.Compare("+")==0)  //��������Ϊ����
							if(types[k].Compare("+")!=0) //��ֵ���Ͳ���
							{
								cout<<"error: data type does not match!"<<endl;
								return false;
							}
							else
								conds[k].left_type=0;
						else if(atype.Compare("-")==0)  //��������Ϊ������
							if(types[k].Compare("+")!=0&&types[k].Compare("-")!=0) //��ֵ���Ͳ���
							{
								cout<<"error: data type does not match!"<<endl;
								return false;
							}
							else
								conds[k].left_type=1;
						else                       //��������Ϊchar
							if(types[k].Compare("+")==0||types[k].Compare("-")==0) //��ֵ���Ͳ���
							{
								cout<<"error: data type does not match!"<<endl;
								return false;
							}
							else
							{
								int a=atoi((LPCTSTR)types[k]);
								int b=atoi((LPCTSTR)atype);
								if(a>b)   //��ֵ���ݳ��ȳ������ֵ
								{
									cout<<"error: data length is too long!"<<endl;
									return false;
								}
								conds[k].left_type=2;
							}
						conds[k].left_len=aa;
						if(iname.Compare("!")==0) //������������
							conds[k].left_index_name="";
						else
							conds[k].left_index_name=iname;
						flag--;
					}
			}
			break;
		}
		else               //׼����֤��һ�ű�
			for(int j=0;j<cc;j++)
				getline(catafile,str);
	}
	catafile.close();
	if(flag!=0)
	{
		cout<<"error: some attribute in conditon statement is illegal!"<<endl;
		return false;
	}
	ifstream headfile(DB_Name+"\\"+Table_Name+".head"); //��ȡ��ͷ�ļ�������ƫ����
	getline(headfile,str);
	getline(headfile,str);
	int tt=0;
	while(getline(headfile,str))
	{
		CString tmp=str.c_str(),ss;
		int end=tmp.Find(' ');
		CString at=tmp.Left(end);
		ss=tmp.Right(tmp.GetLength()-end-1);
		int p=atoi((LPCTSTR)ss); //ƫ����
		for(int i=0;i<count;i++)
			if(name[i].Compare(at)==0)
				conds[i].left_offset=p;
		for(int i=0;i<num;i++)
			if(nm[i].Compare(at)==0)
				nodes[i].offset=p;
	}
	headfile.close();
	//������������������ǰ��
	condition_info yes[32],no[32];
	int x=0,y=0;
	for(int i=0;i<count;i++)
		if(conds[i].left_index_name.IsEmpty())
			no[y++]=conds[i];
		else
			yes[x++]=conds[i];
	int order;
	for(order=0;order<x;order++)
		conds[order]=yes[order];
	for(;order<count;order++)
		conds[order]=no[order-x];
	//cout<<"Get_Condition"<<endl;
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//9
//ʹ�����ݿ�
bool Use_Database(CString  DB_Name)
{
	if((_access(DB_Name,0))!=-1)        //���ݿ����
		return true;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//10
//ִ��ָ���ļ�
void Exect_File(CString SQL,CString SQLS[SQL_MAX_NUM],int &num)                                     
{
	int end=SQL.Find(',');
	int start=end+1;
	end=SQL.Find(',',start);
	CString cstr=SQL.Mid(start,end-start);
	num=atoi((LPCTSTR)cstr);  //SQL�������
	start=end+1;
	cout<<num<<endl;
	for(int i=0;i<num;i++)   //��ȡÿ��SQL���
	{
		end=SQL.Find(';',start);
		SQLS[i]=SQL.Mid(start,end-start)+" ;";
		start=end+1;
	}
	cout<<"execfile success!"<<endl;
	return ;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//11
//��֤������ֵ
bool verify_attr_value(CString Attr_Name, CString DB_Name ,CString Table_Name,CString &Attr,           
					   index_info  nodes[32],int & num,attr_info  uniques[32],int & uniqueNum)
{
	if((_access(DB_Name+"\\"+Table_Name+".db",0))==-1)        //������
	{
		cout<<"error: table -- "<<Table_Name<<" does not exist!"<<endl;
		return false;
	}
	Attr="u";
	num=uniqueNum=0;
	CString values[32];
	CString types[32];
	CString index_attr[32];
	int end=Attr_Name.Find(',');
	int start=end+1;
	CString cstr=Attr_Name.Left(end);
	int count=atoi((LPCTSTR)cstr);       //������ֵ����
	for(int i=0;i<count;i++)         //�����ֵ��Ϣ
	{
		end=Attr_Name.Find(',',start);
		CString each=Attr_Name.Mid(start,end-start);
		int back=each.Find(' ');
		types[i]=each.Left(back);
		values[i]=each.Right(each.GetLength()-back-1);
		//cout<<types[i]+" "+values[i]<<endl;
		start=end+1;
	}
	int count1,cc;
	CString TName;
	fstream catafile(DB_Name+"\\"+DB_Name+".cata");  //�����ֵ��в���
	catafile>>count1;   //����
	string str;
	getline(catafile,str);
	for(int i=0;i<count1;i++)
	{
		getline(catafile,str);
		CString tmp=str.c_str(),ss;
		int end=tmp.Find(' ');
		int start=end+1;
		TName=tmp.Left(end);  //����
		end=tmp.Find(' ',start);
		start=end+1;
		end=tmp.Find(' ',start);
		ss=tmp.Mid(start,end-start);
		cc=atoi((LPCTSTR)ss);  //������
		if(TName.Compare(Table_Name)==0)  //�ҵ��ñ�
		{
			//��֤ÿ������ֵ�Ƿ�Ϸ�
			for(int j=0;j<cc;j++)
			{
				getline(catafile,str);
				tmp=str.c_str();
				end=tmp.Find(' ');
				start=end+1;
				CString aname=tmp.Left(end);  //������
				end=tmp.Find(' ',start);
				CString atype=tmp.Mid(start,end-start);
				start=end+1;
				end=tmp.Find(' ',start);
				CString alen=tmp.Mid(start,end-start);
				int aa=atoi((LPCTSTR)alen);
				start=end+1;
				end=tmp.Find(' ',start);
				CString uniq=tmp.Mid(start,end-start);
				CString iname=tmp.Right(tmp.GetLength()-end-1);
				if(atype.Compare("+")==0)  //��������Ϊ����
				{
					if(types[j].Compare("+")!=0) //���Ͳ���
					{
						cout<<"error: data type does not match!"<<endl;
						return false;
					}
				}
				else if(atype.Compare("-")==0)  //��������Ϊ������
				{
					if(types[j].Compare("+")!=0&&types[j].Compare("-")!=0) //���Ͳ���
					{
						cout<<"error: data type does not match!"<<endl;
						return false;
					}
				}
				else                         //��������Ϊchar
				{
					if(types[j].Compare("+")==0||types[j].Compare("-")==0) //���Ͳ���
					{
						cout<<"error: data type does not match!"<<endl;
						return false;
					}
					else
					{
						int a=atoi((LPCTSTR)types[j]);
						int b=atoi((LPCTSTR)atype);
						if(a>b)   //���ݳ��ȳ������ֵ
						{
							cout<<"error: data length is too long!"<<endl;
							return false;
						}
					}
				}
				//�жϸ����ݶ�Ӧ�������Ƿ��������Լ��Ƿ�Ϊ��ֵ
				if(uniq.Compare("1")==0)
					if(iname.Compare("!")==0) //��ֵ��������
					{
						uniques[uniqueNum].attr_name=aname;
						uniques[uniqueNum].length=aa;
						uniques[uniqueNum].value=values[j];
						if(atype.Compare("+")==0)  //��������Ϊ����
							uniques[uniqueNum].type=0;
						else if(atype.Compare("-")==0)  //��������Ϊ������
							uniques[uniqueNum].type=1;
						else                         //��������Ϊchar
							uniques[uniqueNum].type=2;
						uniqueNum++;
					}
					else   //������
					{
						nodes[num].index_name=iname;
						nodes[num].length=aa;
						nodes[num].value=values[j];
						if(atype.Compare("+")==0)  //��������Ϊ����
							nodes[num].type=0;
						else if(atype.Compare("-")==0)  //��������Ϊ������
							nodes[num].type=1;
						else                         //��������Ϊchar
							nodes[num].type=2;
						index_attr[num]=aname;
						num++;
					}
				Attr+=MakeValue(values[j],aa);
			}
			break;
		}
		else               //׼����֤��һ�ű�
			for(int j=0;j<cc;j++)
				getline(catafile,str);
	}
	catafile.close();
	ifstream headfile(DB_Name+"\\"+Table_Name+".head"); //��ȡ��ͷ�ļ�������ƫ����
	getline(headfile,str);
	getline(headfile,str);
	int tt=0;
	while(getline(headfile,str))
	{
		CString tmp=str.c_str(),ss;
		int end=tmp.Find(' ');
		CString at=tmp.Left(end);
		ss=tmp.Right(tmp.GetLength()-end-1);
		int p=atoi((LPCTSTR)ss); //ƫ����
		for(int i=0;i<uniqueNum;i++)
			if(uniques[i].attr_name.Compare(at)==0)
				uniques[i].offset=p;
		for(int i=0;i<num;i++)
			if(index_attr[i].Compare(at)==0)
				nodes[i].offset=p;
	}
	headfile.close();
	//cout<<"verify_attr_value"<<endl;
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//12
//��ȡһ�ű������е�������Ϣ
bool Get_All_Index(CString DB_Name, CString Table_Name, CString index_name[32],int & num)
{
	if((_access(DB_Name+"\\"+Table_Name+".db",0))==-1)        //������
	{
		cout<<"error: table -- "<<Table_Name<<" does not exist!"<<endl;
		return false;
	}
	num=0;
	int count1,cc;
	CString TName;
	fstream catafile(DB_Name+"\\"+DB_Name+".cata");  //�����ֵ��в���
	catafile>>count1;   //����
	string str;
	getline(catafile,str);
	for(int i=0;i<count1;i++)
	{
		getline(catafile,str);
		CString tmp=str.c_str(),ss;
		int end=tmp.Find(' ');
		int start=end+1;
		TName=tmp.Left(end);  //����
		end=tmp.Find(' ',start);
		start=end+1;
		end=tmp.Find(' ',start);
		ss=tmp.Mid(start,end-start);
		cc=atoi((LPCTSTR)ss);  //������
		if(TName.Compare(Table_Name)==0)  //�ҵ��ñ�
		{
			for(int j=0;j<cc;j++)
			{
				getline(catafile,str);
				CString cs=str.c_str();
				int end=cs.ReverseFind(' ');
				cs=cs.Right(cs.GetLength()-end-1);
				if(cs.Compare("!")!=0)
					index_name[num++]=cs; //��ñ�������
			}
			break;
		}
		else               //׼����֤��һ�ű�
			for(int j=0;j<cc;j++)
				getline(catafile,str);
	}
	catafile.close();
	//cout<<"Get_All_Index"<<endl;
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//13
//��ȫ����ֵ
CString MakeValue(CString value,int length)
{
	if(value.GetLength()==length)
		return value;
	value+=";";
	for(int i=value.GetLength();i<length;i++)
		value+=" ";
	return value;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//14
//�õ����Եĳ���
int FindLen(CString str)
{
	if(str=="+")    //int
		return INT_LENGTH;
	else if(str=="-")  //float
		return FLOAT_LENGTH;
	else    //char
		return atoi((LPCTSTR)str);
}

/////////////////////////////////////////////////////////////////////////////////////////////
//15
//ɾ��Ŀ¼�����ļ�
bool DeleteDirectory(char *DirName)
{
	CFileFind tempFind; 
	char tempFileFind[200]; 
	sprintf_s(tempFileFind,"%s\\*.*",DirName); 
	BOOL IsFinded=(BOOL)tempFind.FindFile(tempFileFind); 
	while(IsFinded) 
	{	 
		IsFinded=(BOOL)tempFind.FindNextFile(); 
		if(!tempFind.IsDots()) 
		{ 
			char foundFileName[200]; 
			strcpy_s(foundFileName,tempFind.GetFileName().GetBuffer(200)); 
			if(tempFind.IsDirectory()) 
			{ 
				char tempDir[200]; 
				sprintf_s(tempDir,"%s\\%s",DirName,foundFileName); 
				DeleteDirectory(tempDir); 
			} 
			else 
			{ 
				char tempFileName[200]; 
				sprintf_s(tempFileName,"%s\\%s",DirName,foundFileName); 
				DeleteFile(tempFileName); 
			} 
		} 
	} 
	tempFind.Close(); 
	if(!RemoveDirectory(DirName)) 
		return FALSE; 
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//16
//��ձ��ļ�
bool Clear_Table(CString DB_Name,CString Table_Name)
{
	if((_access(DB_Name+"\\"+Table_Name+".db",0))==-1)        //������
	{
		cout<<"error: table -- "<<Table_Name<<" does not exist!"<<endl;
		return false;
	}
	else
	{
		ofstream out(DB_Name+"\\"+Table_Name+".db");
		out<<""; //���
		out.close();
		return true;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
//17
//��������ļ�
bool Clear_Index(CString DB_Name,CString Index_Name)
{
	if((_access(DB_Name+"\\"+Index_Name+".idx",0))==-1)        //����������
	{
		cout<<"error: index -- "<<Index_Name<<" does not exist!"<<endl;
		return false;
	}
	else
	{
		ofstream out(DB_Name+"\\"+Index_Name+".idx");
		out<<"";   //���
		out.close();
		return true;
	}
}