#include"MiniSQL.h"
#include"Interpreter.h"
#include"APIModule.h"


void main()
{
	CString SQL;
	//打印软件信息
	cout<<endl;
	cout<<"\t\t***********************************************"<<endl;
	cout<<"\t\t             欢迎使用 MiniSQL !"<<endl;
	cout<<"\t\t               Version (1.0)  "<<endl;
	cout<<"\t\t            "<<endl;      
	cout<<"\t\t        copyright(2013) all right reserved !"<<endl;
	cout<<"\t\t***********************************************"<<endl;
	cout<<endl<<endl;
	while(1)
	{
		cout<<"MiniSQL-->> ";
		SQL.Empty();
		SQL=Interpreter(SQL);
		APIModule(SQL);
	}
}

