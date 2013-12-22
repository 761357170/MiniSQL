#include "IndexManager.h"
#include "BufferManager.h"


void insert_one(CString DB_Name, CString  index_name, index_info* inform)
{
	try {
		int blockNum = search_one(DB_Name, index_name, inform,0);
		fileInfo* file = get_file_info(DB_Name,index_name,1);
		if(blockNum == -7 || blockNum == -8){           //indexΪ��
			blockNum = 0;
			blockInfo * block = get_file_block(DB_Name,index_name,1, blockNum);
			block->cBlock[0] = '!';
			block->cBlock[1] = '0';
			block->cBlock[2] = '0';
			block->cBlock[3] = '0';
			block->cBlock[4] = '1';
			CString tostr;
			tostr.Format(_T("%ld"),inform->offset);
			int length = tostr.GetLength();
			while((INDEX_OFFSET - length)>0){
				tostr = '0' + tostr;
				length++;
			}
			for(int i = 0; i < INDEX_OFFSET; i++){
				block->cBlock[i + 5] = tostr[i];
			}
			CString insertvalue;
			switch(inform->type){
				case 0:
					insertvalue = int_to_str(_ttoi(inform->value));
					break;
				case 1:
					insertvalue = float_to_str((float)_ttof(inform->value));
					break;
				case 2:
					insertvalue = str_to_str(inform->value, inform->length);
					break;
				default:
					break;
			}
			for(int k = 0; k < inform->length; k++)
				block->cBlock[k + 5+INDEX_OFFSET] = insertvalue[k];
			block->cBlock[5+INDEX_OFFSET + inform->length] = '#';
			block->dirtyBit = true;
			block->lock = false;
			return;
		}
		if(inform->isInsert != -2)            // ���иýڵ�
			return;
		else{
			CString soffset;
			soffset.Format(_T("%ld"), inform->offset);
			int offsetlength = soffset.GetLength();
			while((INDEX_OFFSET - offsetlength)>0){
				soffset = '0' + soffset;
				offsetlength++;
			}
			blockInfo * block = get_file_block(DB_Name,index_name,1, blockNum);
			CString snum = "";
			int num, value, L;
			for(int i = 1; i <= 4; i++)	
				snum += block->cBlock[i];
			num = _ttoi(snum);
			if(num <INDEX_N - 1){           //�ڵ�δ��			
				switch(inform -> type){
					case 0:{                //int
						L = num * (10 + INDEX_OFFSET) + 5;
						if(block->cBlock[L] == '#')
							block->cBlock[L + 10 + INDEX_OFFSET] = block->cBlock[L];
						else{
							for(int i = L; i < L + 3; i++)
								block->cBlock[i + 10 + INDEX_OFFSET] = block->cBlock[i];
						}	
						while(num--){
							CString svalue = "";
							for(int j = L - 10; j < L; j++){
								if(block->cBlock[j] == ';')
									break;
								svalue += block->cBlock[j];
							}
							value = _ttoi(svalue);
							if(_ttoi(inform->value) < value){
								for(int k = L - 10 - INDEX_OFFSET; k < L; k++)
									block->cBlock[k + 10 + INDEX_OFFSET] = block->cBlock[k];
								L -= 10 + INDEX_OFFSET;
								svalue = "";
							}
							else{
								CString insvalue = int_to_str(_ttoi(inform->value));
								char*   insertvalue = new char[10];
								memcpy(insertvalue, (LPCSTR)insvalue, 10);
								//for(int i = 0; i < 10; i++)
									//insertvalue[i] = insvalue[i];
								for(int k = L; k < L+INDEX_OFFSET; k++)
									block->cBlock[k] = soffset[k - L];
								L += INDEX_OFFSET;
								int j = 0;							
								for(int k = L; k < L + 10; k++){							
									block->cBlock[k] = insertvalue[j];
									j++;
								}
								break;
							}
						}
						break;
					}				
					case 1:{               //float
						L = num * (40 + INDEX_OFFSET) + 5;
						if(block->cBlock[L] == '#')
							block->cBlock[L + 40 + INDEX_OFFSET] = block->cBlock[L];
						else{
							for(int i = L; i < L + 3; i++)
								block->cBlock[i + 40 + INDEX_OFFSET] = block->cBlock[i];
						}	
						while(num--){
							CString svalue = "";
							for(int j = L - 40; j < L; j++){
								if(block->cBlock[j] == ';')
									break;
								svalue += block->cBlock[j];
							}
							value = _ttoi(svalue);
							if(_ttoi(inform->value) < value){
								for(int k = L - 40 - INDEX_OFFSET; k < L; k++)
									block->cBlock[k + 40 + INDEX_OFFSET] = block->cBlock[k];
								L -= 40 + INDEX_OFFSET;
								svalue = "";
							}
							else{
								CString insvalue = float_to_str((float)_ttof(inform->value));
								char*   insertvalue = new char[40]; 
								memcpy(insertvalue, (LPCSTR)insvalue, 40);
								for(int k = L; k < L+INDEX_OFFSET; k++)
									block->cBlock[k] = soffset[k - L];
								L += INDEX_OFFSET;
								int j = 0;
								for(int k = L; k < L + 40; k++){							
									block->cBlock[k] = insertvalue[j]; 
									j++;
								}
								break;
							}
						}
						break;
					}
					case 2:{               //char
						L = num * (inform->length + INDEX_OFFSET) + 5;
						if(block->cBlock[L] == '#')
							block->cBlock[L + inform->length + INDEX_OFFSET] = block->cBlock[L];
						else{
							for(int i = L; i < L + 3; i++)
								block->cBlock[i + inform->length + INDEX_OFFSET] = block->cBlock[i];
						}	
						bool isInsert = false;
						while(num--){
							CString svalue = "";
							for(int j = L - inform->length; j < L; j++) {
								if(block->cBlock[j] == ';')
									break;
								svalue += block->cBlock[j];
							}
							if(inform->value.Compare(svalue) <= 0){
								for(int k = L - inform->length - INDEX_OFFSET; k < L; k++)
									block->cBlock[k + inform->length + INDEX_OFFSET] = block->cBlock[k];
								L -= inform->length + INDEX_OFFSET;
								svalue = "";
							}
							else{
								CString insertvalue = str_to_str(inform -> value, inform -> length);
								for(int k = L; k < L+INDEX_OFFSET; k++)
									block->cBlock[k] = soffset[k - L];
								L += INDEX_OFFSET;
								int j = 0;
								for(int k = L; k < L + inform->length; k++){							
									block->cBlock[k] = insertvalue[j]; 
									j++;
								}
								isInsert=true;
								break;
							}
						}	
						if(!isInsert) { 
							CString insertvalue = str_to_str(inform -> value, inform -> length);
								for(int k = L; k < L+INDEX_OFFSET; k++)
									block->cBlock[k] = soffset[k - L];
								L += INDEX_OFFSET;
								int j = 0;
								for(int k = L; k < L + inform->length; k++){							
									block->cBlock[k] = insertvalue[j]; 
									j++;
								}
								isInsert=true;
						}
						break;
					}			
				}
				num = _ttoi(snum);
				num++;
				CString finalnum;
				finalnum.Format(_T("%d"), num);
				int numlength = finalnum.GetLength();
				switch(numlength){
					case 1:
						finalnum = "000" + finalnum;
						break;
					case 2:
						finalnum = "00" + finalnum;
						break;
					case 3:
						finalnum = "0" + finalnum;
						break;
					default:
						break;
				}
				for(int i = 0; i < 4; i++)
					block->cBlock[i+1] = finalnum[i];
				block->dirtyBit = true;
				block->lock=false;
			}
			else{
				int leaf1 = blockNum;
				int leaf2 = get_new_freeblocknum(DB_Name, index_name);
				block->lock=false;
				if (leaf2 == -1) {
					cout << "The index nodes of this index are too many, we refuse to give it more memory." << endl;
					return;
				}
				insert_split(DB_Name, index_name, inform, leaf1 , leaf2);
			}
		}
	}
	catch(...){
		throw;
	}
}



void delete_one(CString DB_Name, CString  index_name, index_info* inform,int flag)
{
	try 
	{
		if(flag==1) //ɾ����Ҷ�ӽڵ�Ԫ��
		{
			int blockNum = search_one(DB_Name, index_name, inform,1);
			fileInfo* file = get_file_info(DB_Name,index_name,1);
			if(blockNum<0 || inform->isInsert == -2)   //δ�ҵ�
				return;
			else
			{
				blockInfo * block = get_file_block(DB_Name,index_name,1, blockNum);
				CString snum = "";
				CString svalue = "";
				char ch[4096];   //��ʱ����
				memset(ch,'u',sizeof(char)*4096);
				int num, value, L,num1;
				float fvalue;
				for(int i = 1; i <= 4; i++)	
					snum += block->cBlock[i];
				num = _ttoi(snum);	
				num1=num;
				switch(inform -> type)
				{
					case 0:                     //int
					{                
						//ɾ��Ԫ��
						while(--num)
						{
							L = (num+1) * (10 + 3) + 5+3;
							for(int j = L - 10-3; j < L-3; j++)  //ȡԪ��ֵ
								svalue += block->cBlock[j];
							int end=svalue.ReverseFind(';');
							if(end==-1)
								end=svalue.GetLength();
							value = _ttoi(svalue.Left(end));
							if(_ttoi(inform->value) < value){   //���Ǹ�Ԫ��
								for(int k = L - 10 - 3; k < L; k++)
									ch[k-(10 + 3)] = block->cBlock[k];
							svalue = "";
							}
							else  //�Ǹ�Ԫ��
								break;
						}
						if(num==0)  //Ԫ���ǵ�һ��
						{
							for(int i=0;i<8;i++)
								ch[i]=block->cBlock[i];
							for(int i=0;i<4096;i++)
								block->cBlock[i]=ch[i];
							CString cc;
							cc.Format("%04d",num1-1);
							for(int i=1;i<5;i++)
								block->cBlock[i]=cc.GetAt(i-1);
							block->dirtyBit=true;
							//д��
						}
						else       //Ԫ�����м���βλ��
						{
							for(int i=0;i<num* (10 + 3) +8;i++)
								ch[i]=block->cBlock[i];
							for(int i=0;i<4096;i++)
								block->cBlock[i]=ch[i];
							CString cc;
							cc.Format("%04d",num1-1);
							for(int i=1;i<5;i++)
								block->cBlock[i]=cc.GetAt(i-1);
							block->dirtyBit=true;
							//д��
						}
						break;
						}				
					case 1:
					{               //float
						//ɾ��Ԫ��
						while(--num)
						{
							L = (num+1) * (40 + 3) + 5+3;
							for(int j = L - 40-3; j < L-3; j++)  //ȡԪ��ֵ
								svalue += block->cBlock[j];
							int end=svalue.ReverseFind(';');
							if(end==-1)
								end=svalue.GetLength();
							fvalue = (float)_ttof(svalue.Left(end));
							if(_ttof(inform->value) < fvalue){   //���Ǹ�Ԫ��
								for(int k = L - 40 - 3; k < L; k++)
									ch[k-(40 + 3)] = block->cBlock[k];
								svalue = "";
							}
							else  //�Ǹ�Ԫ��
								break;
						}
						if(num==0)  //Ԫ���ǵ�һ��
						{
							for(int i=0;i<8;i++)
								ch[i]=block->cBlock[i];
							for(int i=0;i<4096;i++)
								block->cBlock[i]=ch[i];
							CString cc;
							cc.Format("%04d",num1-1);
							for(int i=1;i<5;i++)
								block->cBlock[i]=cc.GetAt(i-1);
							block->dirtyBit=true;
							//д��
						}
						else       //Ԫ�����м���βλ��
						{
							for(int i=0;i<num* (40 + 3) +8;i++)
								ch[i]=block->cBlock[i];
							for(int i=0;i<4096;i++)
								block->cBlock[i]=ch[i];
							CString cc;
							cc.Format("%04d",num1-1);
							for(int i=1;i<5;i++)
								block->cBlock[i]=cc.GetAt(i-1);
							block->dirtyBit=true;
							//д��
						}
						break;
					}
					case 2:
					{               //char
						//ɾ��Ԫ��
						while(--num)
						{
							L = (num+1) * (inform->length + 3) + 8;
							for(int j = L - inform->length-3; j < L-3; j++)  //ȡԪ��ֵ
								svalue += block->cBlock[j];
							int end=svalue.ReverseFind(';');
							if(end==-1)
								end=svalue.GetLength();
							svalue=svalue.Left(end);
							if(inform->value.Compare(svalue)<0){   //���Ǹ�Ԫ��
								for(int k = L - inform->length - 3; k < L; k++)
									ch[k-(inform->length + 3)] = block->cBlock[k];
								svalue = "";
							}
							else  //�Ǹ�Ԫ��
								break;
						}
						if(num==0)  //Ԫ���ǵ�һ��
						{
							for(int i=0;i<8;i++)
								ch[i]=block->cBlock[i];
							for(int i=0;i<4096;i++)
								block->cBlock[i]=ch[i];
							CString cc;
							cc.Format("%04d",num1-1);
							for(int i=1;i<5;i++)
								block->cBlock[i]=cc.GetAt(i-1);
							block->dirtyBit=true;
							//д��
						}
						else       //Ԫ�����м���βλ��
						{
							for(int i=0;i<num* (inform->length + 3) +8;i++)
								ch[i]=block->cBlock[i];
							for(int i=0;i<4096;i++)
								block->cBlock[i]=ch[i];
							CString cc;
							cc.Format("%04d",num1-1);
							for(int i=1;i<5;i++)
								block->cBlock[i]=cc.GetAt(i-1);
							block->dirtyBit=true;
							//д��
						}
						break;
					}								
				}
				if(blockNum!=0&&num1<=(INDEX_N - 1)/2) //�Ǹ��ڵ���Ԫ�ز�����һ�룬ɾ����Ҫ���¸��ڵ�
				{
					num1--;
					int leftsiblingblocknum=find_prev_sibling(DB_Name,index_name,inform,blockNum);
					if(leftsiblingblocknum<=0)  //û�����ֵ�
					{
						int rightsiblingblocknum=find_next_sibling(DB_Name,index_name,inform,blockNum);
						//fileInfo* nfile = get_file_info(DB_Name,index_name,1);
						blockInfo * nblock = get_file_block(DB_Name,index_name,1,rightsiblingblocknum);
						CString strnum = "";
						CString strvalue = "";
						char chr1[256],chr[4096];        //��ʱ����
						int nn;
						for(int i = 1; i <= 4; i++)	
							strnum += nblock->cBlock[i];
						nn = _ttoi(strnum);
						if(nn>(INDEX_N - 1)/2)      //���ֵ�Ԫ�ش���һ��,��һ��
						{
							CString c;
							for(int i=0;i<inform->length+3;i++)   //ȡ�ҽڵ��һ��Ԫ��
								chr1[i]=nblock->cBlock[i+5];
							L = nn * (inform->length + 3) + 5 + 3;
							c.Format("%04d",nn-1);
							for(int i=1;i<5;i++)
								nblock->cBlock[i]=c.GetAt(i-1);
							for(int i=5;i<L-(inform->length + 3);i++)
								nblock->cBlock[i]=nblock->cBlock[i+(inform->length + 3)];
							nblock->dirtyBit=true;
							//д��
							c=find_merge_value_left(DB_Name, index_name,inform,rightsiblingblocknum);
							L= (num1+1) * (inform->length+ 3) + 8;
							for(int i=L-(inform->length +3);i<L-3;i++)
								block->cBlock[i]=c.GetAt(i-L+(inform->length +3));
							for(int i=0;i<3;i++)
								block->cBlock[i+L-3]=chr1[i];
							c.Format("%04d",num1+1);
							for(int i=1;i<5;i++)
								block->cBlock[i]=c.GetAt(i-1);
							block->dirtyBit=true;
								//д��
							int fa=find_father(DB_Name, index_name,inform,blockNum);
							blockInfo * fablock = get_file_block(DB_Name,index_name,1, fa);
							for(int i=0;i<inform->length;i++)
								fablock->cBlock[i+5+3]=chr1[i+3];
							fablock->dirtyBit=true;
							block->lock = false;
							nblock->lock = false;
							fablock->lock = false;
								//д��
						}
						else                        //���ֵ�Ԫ������һ�룬�鲢
						{
							int fa=find_father(DB_Name, index_name,inform,blockNum);
							blockInfo * fablock = get_file_block(DB_Name,index_name,1, fa);
							CString c;
							int count=nn+num1+1;
							c.Format("%04d",count);
							for(int i=1;i<5;i++)
								block->cBlock[i]=c.GetAt(i-1);
							L=num1 * (inform->length+ 3) + 8;
							for(int i=L;i<L+nn*(inform->length + 3)+3;i++)
								block->cBlock[i+inform->length]=nblock->cBlock[i-L+5];
							c=find_merge_value_left(DB_Name, index_name,inform,rightsiblingblocknum);
							for(int i=L;i<L+inform->length;i++)
								block->cBlock[i]=c.GetAt(i-L);
							if(fa==0)  //���ڵ��Ǹ�
							{
								CString sn = "";
								int nu;
								for(int i = 1; i <= 4; i++)	
									sn += fablock->cBlock[i];
								nu = _ttoi(sn);
								if(nu==1)  //ֻ��һ��ֵ
								{
									for(int i=0;i<4096;i++)
										fablock->cBlock[i]=block->cBlock[i];  //���ýڵ㿽�������ڵ㲢�ÿ�
									fablock->dirtyBit=true;
									//д��
									block->cBlock[0]='0';
									block->dirtyBit=true;
									//д��
									nblock->cBlock[0]='0';
									nblock->dirtyBit=true;
									//д��
									block->lock = false;
									nblock->lock = false;
									fablock->lock = false;
									return ;
								}
							}
							block->dirtyBit=true;
							//д��
							nblock->cBlock[0]='0';
							nblock->dirtyBit=true;
							//д��
							//int fa=find_father(DB_Name, index_name,inform,blockNum);
							//blockInfo * fablock = get_file_block(DB_Name,index_name,1, fa);
							index_info inf=*inform;
							CString sn = "",cn="";
							int nu;
							for(int i = 1; i <= 4; i++)	
								sn += fablock->cBlock[i];
							nu = _ttoi(sn);
							char str[4];
							int tmp;
							for(int i=0;i<nu;i++)
							{
								L=i*(3+inform->length)+5;
								for(int j=0;j<3;j++)
									str[j]=fablock->cBlock[j+L];
								str[3]='\0';
								tmp=_ttoi(str);
								if(tmp==blockNum)
								{
									for(int j=0;j<inform->length;j++)
										cn+=fablock->cBlock[j+3+L];
									break;
								}
							}
							int end=cn.ReverseFind(';');
							if(end==-1)
								end=cn.GetLength();
							inf.value=cn.Left(end);
							block->lock = false;
							nblock->lock = false;
							fablock->lock = false;
							delete_one(DB_Name,index_name,&inf,1);
						}
					}
					else   //�����ֵ�
					{
						blockInfo * nblock = get_file_block(DB_Name,index_name,1,leftsiblingblocknum);
						CString strnum = "";
						CString strvalue = "";
						char chr1[256],chr[4096];        //��ʱ����
						int nn;
						for(int i = 1; i <= 4; i++)	
							strnum += nblock->cBlock[i];
						nn = _ttoi(strnum);
						if(nn>(INDEX_N - 1)/2)      //���ֵ�Ԫ�ش���һ��,��һ��
						{
							CString c;
							L = (nn-1) * (inform->length + 3) + 8;
							for(int i=0;i<inform->length+3;i++)  //ȡ��ڵ����һ��Ԫ��
								chr[i]=nblock->cBlock[i+L];
							c.Format("%04d",nn-1);
							for(int i=1;i<5;i++)
								nblock->cBlock[i]=c.GetAt(i-1);
							nblock->dirtyBit=true;
							//д��
							c=find_merge_value_left(DB_Name, index_name,inform,blockNum);
							L= (num1+1) * (inform->length+ 3) + 5+3;
							for(int i=L-1;i>=5+(inform->length+ 3);i--)
								block->cBlock[i]=block->cBlock[i-(inform->length + 3)];
							for(int i=0;i<inform->length;i++)
								block->cBlock[i+8]=c.GetAt(i);
							for(int i=0;i<3;i++)
								block->cBlock[i+5]=chr[i+inform->length];
							c.Format("%04d",num1+1);
							for(int i=1;i<5;i++)
								block->cBlock[i]=c.GetAt(i-1);
							block->dirtyBit=true;
								//д��
							int fa=find_father(DB_Name, index_name,inform,blockNum);
							blockInfo * fablock = get_file_block(DB_Name,index_name,1, fa);
							CString sn = "";
							int nu;
							for(int i = 1; i <= 4; i++)	
								sn += fablock->cBlock[i];
							nu = _ttoi(sn);
							char str[4];
							int tmp;
							for(int i=0;i<nu;i++)
							{
								L=i*(3+inform->length)+5;
								for(int j=0;j<3;j++)
									str[j]=fablock->cBlock[j+L];
								str[3]='\0';
								tmp=_ttoi(str);
								if(tmp==leftsiblingblocknum)
								{
									for(int j=0;j<inform->length;j++)
										fablock->cBlock[j+3+L]=chr[j];
									break;
								}
							}
							fablock->dirtyBit=true;
							block->lock = false;
							nblock->lock = false;
							fablock->lock = false;
								//д��
						}
						else                        //���ֵ�Ԫ������һ�룬�鲢
						{
							int fa=find_father(DB_Name, index_name,inform,blockNum);
							blockInfo * fablock = get_file_block(DB_Name,index_name,1, fa);
							CString c;
							int count=nn+num1+1;
							c.Format("%04d",count);
							for(int i=1;i<5;i++)
								nblock->cBlock[i]=c.GetAt(i-1);
							L=nn * (inform->length+ 3) + 8;
							for(int i=L;i<L+num1 * (inform->length + 3)+3;i++)
								nblock->cBlock[i+inform->length]=block->cBlock[i-L+5];
							c=find_merge_value_left(DB_Name, index_name,inform,blockNum);
							for(int i=0;i<inform->length;i++)
								nblock->cBlock[i+L]=c.GetAt(i);
							if(fa==0)  //���ڵ��Ǹ�
							{
								CString sn = "";
								int nu;
								for(int i = 1; i <= 4; i++)	
									sn += fablock->cBlock[i];
								nu = _ttoi(sn);
								if(nu==1)  //ֻ��һ��ֵ
								{
									for(int i=0;i<4096;i++)
										fablock->cBlock[i]=nblock->cBlock[i];  //���ýڵ㿽�������ڵ㲢�ÿ�
									fablock->dirtyBit=true;
									//д��
									block->cBlock[0]='0';
									block->dirtyBit=true;
									//д��
									nblock->cBlock[0]='0';
									nblock->dirtyBit=true;
									//д��
									block->lock = false;
									nblock->lock = false;
									fablock->lock = false;
									return ;
								}
							}
							nblock->dirtyBit=true;
							//д��
							block->cBlock[0]='0';
							block->dirtyBit=true;
							//д��
							//int fa=find_father(DB_Name, index_name,inform,blockNum);
							//blockInfo * fablock = get_file_block(DB_Name,index_name,1, fa);
							index_info inf=*inform;
							CString sn = "",cn="";
							int nu;
							for(int i = 1; i <= 4; i++)	
								sn += fablock->cBlock[i];
							nu = _ttoi(sn);
							char str[4];
							int tmp;
							for(int i=0;i<nu;i++)
							{
								L=i*(3+inform->length)+5;
								for(int j=0;j<3;j++)
									str[j]=fablock->cBlock[j+L];
								str[3]='\0';
								tmp=_ttoi(str);
								if(tmp==leftsiblingblocknum)
								{
									for(int j=0;j<inform->length;j++)
										cn+=fablock->cBlock[j+3+L];
									break;
								}
							}
							int end=cn.ReverseFind(';');
							if(end==-1)
								end=cn.GetLength();
							inf.value=cn.Left(end);
							block->lock = false;
							nblock->lock = false;
							fablock->lock = false;
							delete_one(DB_Name,index_name,&inf,1);
						}
					}
				}
				block->lock=false;
				return ;
			}
		}
		//ɾ��Ҷ�ӽڵ�Ԫ��
		int blockNum = search_one(DB_Name, index_name, inform,0);
		fileInfo* file = get_file_info(DB_Name,index_name,1);	
		if(blockNum<0 || inform->isInsert ==-2)   //δ�ҵ�
			return;
		else
		{
			blockInfo * block = get_file_block(DB_Name,index_name,1, blockNum);
			CString snum = "";
			CString svalue = "";
			CString w="";
			char ch[4096];   //��ʱ����
			memset(ch,'u',sizeof(char)*4096);
			int num, value, L,num1;
			float fvalue;
			for(int i = 1; i <= 4; i++)	
				snum += block->cBlock[i];
			num = _ttoi(snum);	
			num1=num;
			switch(inform -> type)
			{
				case 0:                     //int
				{              
					//ȡ�����ֵܽڵ�Ŀ��
					L = num * (10 + INDEX_OFFSET) + 5;  
					if(block->cBlock[L] =='#')
						ch[L-10-INDEX_OFFSET]= block->cBlock[L];
					else
						for(int i = L; i < L + 3; i++)
							ch[i-10-INDEX_OFFSET]= block->cBlock[i];
					//ɾ��Ԫ��
					while(--num)
					{
						L = (num+1) * (10 + INDEX_OFFSET) + 5;
						for(int j = L - 10; j < L; j++)  //ȡԪ��ֵ
							svalue += block->cBlock[j];
						int end=svalue.ReverseFind(';');
						if(end==-1)
							end=svalue.GetLength();
						value = _ttoi(svalue.Left(end));
						if(_ttoi(inform->value) < value){   //���Ǹ�Ԫ��
							for(int k = L - 10 - INDEX_OFFSET; k < L; k++)
								ch[k-(10 + INDEX_OFFSET)] = block->cBlock[k];
						svalue = "";
						}
						else  //�Ǹ�Ԫ��
							break;
					}
					if(num==0)  //Ԫ���ǵ�һ��
					{
						for(int i=0;i<5;i++)
							ch[i]=block->cBlock[i];
						for(int i=0;i<inform->length;i++)
							w+=block->cBlock[i+5+INDEX_OFFSET+(inform->length+INDEX_OFFSET)];
						update_block(DB_Name,index_name,blockNum,inform,w);
						for(int i=0;i<4096;i++)
								block->cBlock[i]=ch[i];
						CString cc;
						cc.Format("%04d",num1-1);
						for(int i=1;i<5;i++)
							block->cBlock[i]=cc.GetAt(i-1);
						block->dirtyBit=true;
						//д��
					}
					else       //Ԫ�����м���βλ��
					{
						for(int i=0;i<num* (10 + INDEX_OFFSET) +5;i++)
							ch[i]=block->cBlock[i];
						for(int i=0;i<4096;i++)
								block->cBlock[i]=ch[i];
						CString cc;
						cc.Format("%04d",num1-1);
						for(int i=1;i<5;i++)
							block->cBlock[i]=cc.GetAt(i-1);
						block->dirtyBit=true;
						//д��
					}
					break;
					}				
				case 1:
				{               //float
						//ȡ�����ֵܽڵ�Ŀ��
					L = num * (40 + INDEX_OFFSET) + 5;  
					if(block->cBlock[L] =='#')
						ch[L-40-INDEX_OFFSET]= block->cBlock[L];
					else
						for(int i = L; i < L + 3; i++)
							ch[i-40-INDEX_OFFSET]= block->cBlock[i];
					//ɾ��Ԫ��
					while(--num)
					{
						L = (num+1) * (40 + INDEX_OFFSET) + 5;
						for(int j = L - 40; j < L; j++)  //ȡԪ��ֵ
							svalue += block->cBlock[j];
						int end=svalue.ReverseFind(';');
						if(end==-1)
							end=svalue.GetLength();
						fvalue = (float)_ttof(svalue.Left(end));
						if(_ttof(inform->value) < fvalue){   //���Ǹ�Ԫ��
							for(int k = L - 40 - INDEX_OFFSET; k < L; k++)
								ch[k-(40 + INDEX_OFFSET)] = block->cBlock[k];
							svalue = "";
						}
						else  //�Ǹ�Ԫ��
							break;
					}
					if(num==0)  //Ԫ���ǵ�һ��
					{
						for(int i=0;i<5;i++)
							ch[i]=block->cBlock[i];
						for(int i=0;i<inform->length;i++)
							w+=block->cBlock[i+5+INDEX_OFFSET+(inform->length+INDEX_OFFSET)];
						update_block(DB_Name,index_name,blockNum,inform,w);
						for(int i=0;i<4096;i++)
							block->cBlock[i]=ch[i];
						CString cc;
						cc.Format("%04d",num1-1);
						for(int i=1;i<5;i++)
							block->cBlock[i]=cc.GetAt(i-1);
						block->dirtyBit=true;
						//д��
					}
					else       //Ԫ�����м���βλ��
					{
						for(int i=0;i<num* (40 + INDEX_OFFSET) +5;i++)
							ch[i]=block->cBlock[i];
						for(int i=0;i<4096;i++)
								block->cBlock[i]=ch[i];
						CString cc;
						cc.Format("%04d",num1-1);
						for(int i=1;i<5;i++)
							block->cBlock[i]=cc.GetAt(i-1);
						block->dirtyBit=true;
						//д��
					}
					break;
				}
				case 2:
				{               //char
						//ȡ�����ֵܽڵ�Ŀ��
					L = num * (inform->length + INDEX_OFFSET) + 5;  
					if(block->cBlock[L] =='#')
						ch[L-inform->length-INDEX_OFFSET]= block->cBlock[L];
					else
						for(int i = L; i < L + 3; i++)
							ch[i-inform->length-INDEX_OFFSET]= block->cBlock[i];
					//ɾ��Ԫ��
					while(--num)
					{
						L = (num+1) * (inform->length + INDEX_OFFSET) + 5;
						for(int j = L - inform->length; j < L; j++)  //ȡԪ��ֵ
							svalue += block->cBlock[j];
						int end=svalue.ReverseFind(';');
						if(end==-1)
							end=svalue.GetLength();
						svalue=svalue.Left(end);
						if(inform->value.Compare(svalue)<0){   //���Ǹ�Ԫ��
							for(int k = L - inform->length - INDEX_OFFSET; k < L; k++)
								ch[k-(inform->length + INDEX_OFFSET)] = block->cBlock[k];
							svalue = "";
						}
						else  //�Ǹ�Ԫ��
							break;
					}
					if(num==0)  //Ԫ���ǵ�һ��
					{
						for(int i=0;i<5;i++)
							ch[i]=block->cBlock[i];
						for(int i=0;i<inform->length;i++)
							w+=block->cBlock[i+5+INDEX_OFFSET+(inform->length+INDEX_OFFSET)];
						update_block(DB_Name,index_name,blockNum,inform,w);
						for(int i=0;i<4096;i++)
							block->cBlock[i]=ch[i];
						CString cc;
						cc.Format("%04d",num1-1);
						for(int i=1;i<5;i++)
							block->cBlock[i]=cc.GetAt(i-1);
						block->dirtyBit=true;
						//д��
					}
					else       //Ԫ�����м���βλ��
					{
						for(int i=0;i<num* (inform->length + INDEX_OFFSET) +5;i++)
							ch[i]=block->cBlock[i];
						for(int i=0;i<4096;i++)
							block->cBlock[i]=ch[i];
						CString cc;
						cc.Format("%04d",num1-1);
						for(int i=1;i<5;i++)
							block->cBlock[i]=cc.GetAt(i-1);
						block->dirtyBit=true;
						//д��
					}
					break;
				}								
			}
			if(blockNum!=0&&num1<=(INDEX_N - 1)/2) //�Ǹ��ڵ���Ԫ�ز�����һ�룬ɾ����Ҫ���¸��ڵ�
			{
				num1--;
				int leftsiblingblocknum=find_prev_sibling(DB_Name,index_name,inform,blockNum);
				if(leftsiblingblocknum<=0)  //������
				{
					int rightsiblingblocknum=find_next_sibling(DB_Name,index_name,inform,blockNum);
					//fileInfo* nfile = get_file_info(DB_Name,index_name,1);
					blockInfo * nblock = get_file_block(DB_Name,index_name,1,rightsiblingblocknum);
					CString strnum = "";
					CString strvalue = "";
					char chr1[256],chr[4096];        //��ʱ����
					int nn;
					for(int i = 1; i <= 4; i++)	
						strnum += nblock->cBlock[i];
					nn = _ttoi(strnum);
					if(nn>(INDEX_N - 1)/2)      //���ֵ�Ԫ�ش���һ��,��һ��
					{
						CString c;
						for(int i=0;i<inform->length;i++)   //ȡ�ҽڵ�ڶ���Ԫ��
							chr1[i]=nblock->cBlock[i+5+(inform->length + INDEX_OFFSET)+INDEX_OFFSET];
						L = nn * (inform->length + INDEX_OFFSET) + 5 + 4;
						for(int i=0;i<inform->length+INDEX_OFFSET;i++)  //ȡ�ҽڵ��һ��Ԫ��
							chr[i]=nblock->cBlock[i+5];
						c.Format("%04d",nn-1);
						for(int i=1;i<5;i++)
							nblock->cBlock[i]=c.GetAt(i-1);
						for(int i=5;i<L-(inform->length + INDEX_OFFSET);i++)
							nblock->cBlock[i]=nblock->cBlock[i+(inform->length + INDEX_OFFSET)];
						nblock->dirtyBit=true;
						//д��
						L= (num1+1) * (inform->length+ INDEX_OFFSET) + 5;
						for(int i=L;i<L+4;i++)
							block->cBlock[i]=block->cBlock[i-(inform->length + INDEX_OFFSET)];
						for(int i=0;i<(inform->length+ INDEX_OFFSET);i++)
							block->cBlock[i+L-(inform->length + INDEX_OFFSET)]=chr[i];
						c.Format("%04d",num1+1);
						for(int i=1;i<5;i++)
							block->cBlock[i]=c.GetAt(i-1);
						block->dirtyBit=true;
							//д��
						int fa=find_father(DB_Name, index_name,inform,blockNum);
						blockInfo * fablock = get_file_block(DB_Name,index_name,1, fa);
						for(int i=0;i<inform->length;i++)
							fablock->cBlock[i+5+3]=chr1[i];
						fablock->dirtyBit=true;
						block->lock = false;
						nblock->lock = false;
						fablock->lock = false;
							//д��
					}
					else                        //���ֵ�Ԫ������һ�룬�鲢
					{
						int fa=find_father(DB_Name, index_name,inform,blockNum);
						blockInfo * fablock = get_file_block(DB_Name,index_name,1, fa);
						CString c;
						int count=nn+num1;
						c.Format("%04d",count);
						for(int i=1;i<5;i++)
							block->cBlock[i]=c.GetAt(i-1);
						L=num1 * (inform->length+ INDEX_OFFSET) + 5;
						for(int i=L;i<L+nn * (inform->length + INDEX_OFFSET) + 4;i++)
							block->cBlock[i]=nblock->cBlock[i-L+5];
						if(fa==0)  //���ڵ��Ǹ�
						{
							CString sn = "";
							int nu;
							for(int i = 1; i <= 4; i++)	
								sn += fablock->cBlock[i];
							nu = _ttoi(sn);
							if(nu==1)  //ֻ��һ��ֵ
							{
								for(int i=0;i<4096;i++)
									fablock->cBlock[i]=block->cBlock[i];  //���ýڵ㿽�������ڵ㲢�ÿ�
								fablock->dirtyBit=true;
								//д��
								block->cBlock[0]='0';
								block->dirtyBit=true;
								//д��
								nblock->cBlock[0]='0';
								nblock->dirtyBit=true;
								//д��
								block->lock = false;
								nblock->lock = false;
								fablock->lock = false;
								return ;
							}
						}
						block->dirtyBit=true;
						//д��
						nblock->cBlock[0]='0';
						nblock->dirtyBit=true;
						//д��
						//int fa=find_father(DB_Name, index_name,inform,blockNum);
						//blockInfo * fablock = get_file_block(DB_Name,index_name,1, fa);
						index_info inf=*inform;
						CString sn = "",cn="";
						int nu;
						for(int i = 1; i <= 4; i++)	
							sn += fablock->cBlock[i];
						nu = _ttoi(sn);
						char str[4];
						int tmp;
						for(int i=0;i<nu;i++)
						{
							L=i*(3+inform->length)+5;
							for(int j=0;j<3;j++)
								str[j]=fablock->cBlock[j+L];
							str[3]='\0';
							tmp=_ttoi(str);
							if(tmp==blockNum)
							{
								for(int j=0;j<inform->length;j++)
									cn+=fablock->cBlock[j+3+L];
								break;
							}
						}
						int end=cn.ReverseFind(';');
						if(end==-1)
							end=cn.GetLength();
						inf.value=cn.Left(end);
						block->lock = false;
						nblock->lock = false;
						fablock->lock = false;
						delete_one(DB_Name,index_name,&inf,1);
					}
				}
				else   //�����ֵ�
				{
					blockInfo * nblock = get_file_block(DB_Name,index_name,1,leftsiblingblocknum);
					CString strnum = "";
					CString strvalue = "";
					char chr1[256],chr[4096];        //��ʱ����
					int nn;
					for(int i = 1; i <= 4; i++)	
						strnum += nblock->cBlock[i];
					nn = _ttoi(strnum);
					if(nn>(INDEX_N - 1)/2)      //���ֵ�Ԫ�ش���һ��,��һ��
					{
						CString c;
						L = (nn-1) * (inform->length + INDEX_OFFSET) + 5;
						for(int i=0;i<inform->length+INDEX_OFFSET;i++)  //ȡ��ڵ����һ��Ԫ��
							chr[i]=nblock->cBlock[i+L];
						c.Format("%04d",nn-1);
						for(int i=1;i<5;i++)
							nblock->cBlock[i]=c.GetAt(i-1);
						for(int i=0;i<3;i++)
							nblock->cBlock[i+L]=nblock->cBlock[i+L+(inform->length+INDEX_OFFSET)];
						nblock->dirtyBit=true;
						//д��
						L= (num1+1) * (inform->length+ INDEX_OFFSET) + 5+3;
						for(int i=L-1;i>=5+(inform->length+ INDEX_OFFSET);i--)
							block->cBlock[i]=block->cBlock[i-(inform->length + INDEX_OFFSET)];
						for(int i=0;i<(inform->length+ INDEX_OFFSET);i++)
							block->cBlock[i+5]=chr[i];
						c.Format("%04d",num1+1);
						for(int i=1;i<5;i++)
							block->cBlock[i]=c.GetAt(i-1);
						block->dirtyBit=true;
							//д��
						int fa=find_father(DB_Name, index_name,inform,blockNum);
						blockInfo * fablock = get_file_block(DB_Name,index_name,1, fa);
						CString sn = "";
						int nu;
						for(int i = 1; i <= 4; i++)	
							sn += fablock->cBlock[i];
						nu = _ttoi(sn);
						char str[4];
						int tmp;
						for(int i=0;i<nu;i++)
						{
							L=i*(3+inform->length)+5;
							for(int j=0;j<3;j++)
								str[j]=fablock->cBlock[j+L];
							str[3]='\0';
							tmp=_ttoi(str);
							if(tmp==leftsiblingblocknum)
							{
								for(int j=0;j<inform->length;j++)
									fablock->cBlock[j+3+L]=chr[j+INDEX_OFFSET];
								break;
							}
						}
						fablock->dirtyBit=true;
						block->lock = false;
						nblock->lock = false;
						fablock->lock = false;
							//д��
					}
					else                        //���ֵ�Ԫ������һ�룬�鲢
					{
						int fa=find_father(DB_Name, index_name,inform,blockNum);
						blockInfo * fablock = get_file_block(DB_Name,index_name,1, fa);
						CString c;
						int count=nn+num1;
						c.Format("%04d",count);
						for(int i=1;i<5;i++)
							nblock->cBlock[i]=c.GetAt(i-1);
						L=nn * (inform->length+ INDEX_OFFSET) + 5;
						for(int i=L;i<L+num1 * (inform->length + INDEX_OFFSET) + 4;i++)
							nblock->cBlock[i]=block->cBlock[i-L+5];
						if(fa==0)  //���ڵ��Ǹ�
						{
							CString sn = "";
							int nu;
							for(int i = 1; i <= 4; i++)	
								sn += fablock->cBlock[i];
							nu = _ttoi(sn);
							if(nu==1)  //ֻ��һ��ֵ
							{
								for(int i=0;i<4096;i++)
									fablock->cBlock[i]=nblock->cBlock[i];  //���ýڵ㿽�������ڵ㲢�ÿ�
								fablock->dirtyBit=true;
								//д��
								block->cBlock[0]='0';
								block->dirtyBit=true;
								//д��
								nblock->cBlock[0]='0';
								nblock->dirtyBit=true;
								//д��
								block->lock = false;
								nblock->lock = false;
								fablock->lock = false;
								return ;
							}
						}
						nblock->dirtyBit=true;
						//д��
						block->cBlock[0]='0';
						block->dirtyBit=true;
						//д��
						//int fa=find_father(DB_Name, index_name,inform,blockNum);
						//blockInfo * fablock = get_file_block(DB_Name,index_name,1, fa);
						index_info inf=*inform;
						CString sn = "",cn="";
						int nu;
						for(int i = 1; i <= 4; i++)	
							sn += fablock->cBlock[i];
						nu = _ttoi(sn);
						char str[4];
						int tmp;
						for(int i=0;i<nu;i++)
						{
							L=i*(3+inform->length)+5;
							for(int j=0;j<3;j++)
								str[j]=fablock->cBlock[j+L];
							str[3]='\0';
							tmp=_ttoi(str);
							if(tmp==leftsiblingblocknum)
							{
								for(int j=0;j<inform->length;j++)
									cn+=fablock->cBlock[j+3+L];
								break;
							}
						}
						int end=cn.ReverseFind(';');
						if(end==-1)
							end=cn.GetLength();
						inf.value=cn.Left(end);
						block->lock = false;
						nblock->lock = false;
						fablock->lock = false;
						delete_one(DB_Name,index_name,&inf,1);
					}
				}
			}
			block->lock=false;
			return ;
		}	
	}
	catch(...){
		throw;
	}
}

	 


//����һ��ֵ����inform . value(informΪinfo�ṹ��)�ļ�¼��table���еļ�¼�ţ�
/*����˵����
database�����ݿ�����cstring�ͣ�
table_name������ļ�����cstring��
inform��info�ṹ�壬�����á�
������������ʱ����¼�ű�����inform .offset�У���û�У�inform .offset��0��
����ֵ��
   -7    inform .offset=0    �����ļ�Ϊ��
	   -8    inform .offset=0    �����޼�¼
		 -1    inform .offset=0    ����ֵ�����ʹ���int��float��char(n)��
		 -3    inform .offset=0    �����쳣��

		 num�������ļ���Ѱ�ҽڵ�Ŀ��
		����  Ҷ�ӿ�ţ�   ��inform .offset>=0����¼���ڱ��ļ��У���¼��Ϊinform .offset  
����  Ҷ�ӿ�� ����inform .offset��-1����¼���ڱ��ļ��У����ص�Ҷ�ӿ���ں���insert_one��Ҫ�ã���Ϊ��ֵ����Ŀ�͹��ˣ�
˼·������b������ԭ�������м��Ҷ�ӽڵ㣬ֱ���ҵ���ֵ�����ء�*/
int search_one(CString DB_Name, CString  index_name,index_info * inform,int flag) {
	try {
		fileInfo* file = get_file_info(DB_Name,index_name,1);
		int next = 0;                // ��һ��Ӧ��ȥ�Ŀ�
		while (true) {	
			blockInfo* block = get_file_block(DB_Name,index_name,1,next);
			if (block->cBlock[0] != '?' && block->cBlock[0] != '!') {          // �����ļ�Ϊ��
				inform->offset = 0;
				block->lock = false;
				return -7;
			}
			int i = 1;
			if (block->cBlock[0] == '?') {    // ����Ҷ�ӽڵ�
				bool isNext = false;
				int num;
				CString snum = "";
				for (; i<=4; ++i)                  // �ڵ���Ŀ
					snum += block->cBlock[i] ;
				num = _ttoi(snum);
				while(num -- ) {
					CString svalue="",sblock_n="";
					int cx = 3;
					while(cx--) {                     // ���ӽڵ���
						sblock_n += block->cBlock[i] ;
						++i;
					}
					int block_n = _ttoi(sblock_n);
					cx = inform->length;
					bool isValue = false;
					while (cx-- ) {
						if (block->cBlock[i] == ';') isValue = true;
						if(!isValue) svalue += block->cBlock[i] ;
						++i;
					}
					switch (inform->type) {
					case 0: {
									int value = _ttoi(svalue);              // int
									if (flag == 1) {
										if (_ttoi(inform->value) == value) {
											block->lock = false;
											return next;
										}
									}
									if (_ttoi(inform->value) < value)  {
										next = block_n;
										isNext = true;
									}
									break;
								}
					case 1: {
									float value = (float) _ttof(svalue);              // float
									if (flag == 1) {
										if ((float)_ttof(inform->value) == value) {
											block->lock = false;
											return next;
										}
									}
									if ((float)_ttof(inform->value) < value)  {
										next = block_n;
										isNext = true;
									}
									break;
								}
					case 2: {                     // char
									if (flag == 1) {
										if (inform->value.Compare(svalue) == 0) {
											block->lock = false;
											return next;
										}
									}
									if (inform->value.Compare(svalue) < 0)  {
										next = block_n;
										isNext = true;
									}
									break;
								}
					}
					if (isNext) break;
				}
				if (!isNext) {
					int cx = 3;
					CString sblock_n = "";
					while(cx--) {                     // ���ӽڵ���
						sblock_n += block->cBlock[i] ;
						++i;
					}
					next = _ttoi(sblock_n);
				}
				block->lock = false;
			} else {                     // ΪҶ�ӽڵ�
				if (flag==1) return -1;
				int num;
				CString snum = "";
				for (; i<=4; ++i)                  // �ڵ���Ŀ
					snum += block->cBlock[i] ;
				num = _ttoi(snum);
				while(num -- ) {
					CString svalue="",soffset="";
					int cx = INDEX_OFFSET;
					while(cx--) {                     // ��.db�ļ��е�offset
						soffset += block->cBlock[i] ;
						++i;
					}
					long offset = _ttol(soffset);
					cx = inform->length;
					bool isValue = false;
					while (cx-- ) {
						if (block->cBlock[i] == ';') isValue = true;
						if(!isValue) svalue += block->cBlock[i] ;
						++i;
					}
					switch (inform->type) {
					case 0: {
									int value = _ttoi(svalue);              // int
									if (_ttoi(inform->value) == value)  {
										inform->offset = i-inform->length-INDEX_OFFSET;
										block->lock = false;
										return next;
									}
									break;
								}
					case 1: {
									float value = (float) _ttof(svalue);              // float
									if ((float)_ttof(inform->value) == value)  {
										inform->offset = i-inform->length-INDEX_OFFSET;
										block->lock = false;
										return next;
									}
									break;
								}
					case 2: {                     // char
									if (inform->value.Compare(svalue) == 0)  {
										inform->offset = i-inform->length-INDEX_OFFSET;
										block->lock = false;
										return next;
									}
									break;
								}
					}
				}                                        //δ�ҵ��ýڵ�
				inform ->isInsert= -2;
				block->lock = false;
				return next;            // ���ظÿ��
			}
		} 
	} catch (...) {
		throw;
	}
}



//����һ��>,  >=,  <, <=inform . value(informΪinfo�ṹ��)�ļ�¼��table���еļ�¼�ţ�
/*����˵����
database�����ݿ�����cstring�ͣ�
table_name������ļ�����cstring��

inform��info�ṹ�壬�����ã�
type�� 3 �� �ҳ����� < inform .value �ļ�¼;
		4 �� �ҳ����� <= inform .value �ļ�¼;
	  1 �� �ҳ����� > inform .value; �ļ�¼�� 
2 �� �ҳ����� >= inform .value �ļ�¼;
��������ʱ��
��û�У�start��-10��  end = -10;
���� start�������ļ��������¼���ڵ�Ҷ�ӽڵ����ʼ���
	   end�������ļ��������¼���ڵ�Ҷ�ӽڵ����ֹ���
	   inform . offset �������ļ�start��ʼ���еĵ�һ������ֵ��ƫ������>,>=��   
���������ļ�end ��ֹ���е����һ������ֵ��ƫ������<,<=��
˼·�������࿼�ǣ�����b������ԭ�������м��Ҷ�ӽڵ㣬ֱ���ҵ���ʼֵ����ֵֹ���ڵĿ�ʱ�����ء�
*/
void search_many(CString DB_Name, CString  index_name,int& start,int& end,int type,index_info* inform) {
	try {
		fileInfo* file = get_file_info(DB_Name,index_name,1);
		bool isGet = false;
		int next = file->freeNum;                // ��һ��Ӧ��ȥ�Ŀ�
		while (!isGet) {	
			blockInfo* block = get_file_block(DB_Name,index_name,1,next);
			if (block->cBlock[0] != '?' && block->cBlock[0] != '!') {          // �����ļ�Ϊ��
				inform->offset = 0;
				start = -10;
				end = -10;
				block->lock = false;
				return;
			}
			int i = 1;
			if (block->cBlock[0] == '?') {    // ����Ҷ�ӽڵ�
				bool isNext = false;
				int num;
				CString snum = "";
				for (; i<=4; ++i)                  // �ڵ���Ŀ
					snum += block->cBlock[i] ;
				num = _ttoi(snum);
				while(num -- ) {
					CString svalue="",sblock_n="";
					int cx = 3;
					while(cx--) {                     // ���ӽڵ���
						sblock_n += block->cBlock[i] ;
						++i;
					}
					int block_n = _ttoi(sblock_n);
					cx = inform->length; 
					bool isValue = false;
					while (cx-- ) {
						if (block->cBlock[i] == ';') isValue = true;
						if(!isValue) svalue += block->cBlock[i] ;
						++i;
					}
					switch (inform->type) {
					case 0: {
									int value = _ttoi(svalue);              // int
									if (_ttoi(inform->value) < value || (type == 3 && _ttoi(inform->value) == value))  {
										next = block_n;
										isNext = true;
									} 
									break;
								}
					case 1: {
									float value = (float) _ttof(svalue);              // float
									if ((float)_ttof(inform->value) < value || (type == 3 && (float)_ttof(inform->value)== value))  {
										next = block_n;
										isNext = true;
									}
									break;
								}
					case 2: {                     // char
									if (inform->value.Compare(svalue) < 0  || (type == 3 && inform->value.Compare(svalue) == 0))  {
										next = block_n;
										isNext = true;
									}
									break;
								}
					}
					if (isNext) break;
				}
				if (!isNext) {
					int cx = 3;
					CString sblock_n = "";
					while(cx--) {                     // ���ӽڵ���
						sblock_n += block->cBlock[i] ;
						++i;
					}
					next = _ttoi(sblock_n);
				}
				block->lock = false;
			} else {                     // ΪҶ�ӽڵ�
				int num;
				CString snum = "";
				for (; i<=4; ++i)                  // �ڵ���Ŀ
					snum += block->cBlock[i] ;
				num = _ttoi(snum);
				while(num -- ) {
					CString svalue="",soffset="";
					int cx = INDEX_OFFSET;
					while(cx--) {                     // ��.db�ļ��е�offset
						soffset += block->cBlock[i] ;
						++i;
					}
					long offset = _ttol(soffset);
					cx = inform->length;
					bool isValue = false;
					while (cx-- ) {
						if (block->cBlock[i] == ';') isValue = true;
						if(!isValue) svalue += block->cBlock[i] ;
						++i;
					}
					switch (inform->type) {
					case 0: {
									int value = _ttoi(svalue);              // int
									switch (type) {
										case 1: 	{                // >
														if (_ttoi(inform->value) < value)  {
															inform->offset = i-inform->length-INDEX_OFFSET;
															start = next;
															block->lock = false;
															return;
														}
														break;
													}
										case 2: {                // >=
														if (_ttoi(inform->value) <= value)  {
															inform->offset = i-inform->length-INDEX_OFFSET;
															start = next;
															block->lock = false;
															return;
														}
														break;
													}
										case 3: {                // <
														if (_ttoi(inform->value) <= value)  {
															inform->offset = i-inform->length-INDEX_OFFSET-inform->length-INDEX_OFFSET;
															end = next;
															block->lock = false;
															return;
														}
														break;
													}
										case 4: {                // <=
														if (_ttoi(inform->value) == value)  {
															inform->offset = i-inform->length-INDEX_OFFSET;
															end = next;
															block->lock = false;
															return;
														} else if (_ttoi(inform->value) < value) {
															inform->offset = i-inform->length-INDEX_OFFSET-inform->length-INDEX_OFFSET;
															end = next;
															block->lock = false;
															return;
														}
														break;
													}
									}
									break;
								}
					case 1: {
									float value = (float) _ttof(svalue);              // float
									switch (type) {
										case 1: 	{                // >
														if ((float)_ttof(inform->value) < value)  {
															inform->offset = i-inform->length-INDEX_OFFSET;
															start = next;
															block->lock = false;
															return;
														}
														break;
													}
										case 2: {                // >=
														if ((float)_ttof(inform->value) <= value)  {
															inform->offset = i-inform->length-INDEX_OFFSET;
															start = next;
															block->lock = false;
															return;
														}
														break;
													}
										case 3: {                // <
														if ((float)_ttof(inform->value) <= value)  {
															inform->offset = i-inform->length-INDEX_OFFSET-inform->length-INDEX_OFFSET;
															end = next;
															block->lock = false;
															return;
														}
														break;
													}
										case 4: {                // <=
														if ((float)_ttof(inform->value) == value) {
															inform->offset = i-inform->length-INDEX_OFFSET;
															end = next;
															block->lock = false;
															return;
														} else if ((float)_ttof(inform->value) < value) {
															inform->offset = i-inform->length-INDEX_OFFSET-inform->length-INDEX_OFFSET;
															end = next;
															block->lock = false;
															return;
														}
														break;
													}
									}
									break;
								}
					case 2: {                     // char
									switch (type) {
										case 1: 	{                // >
														if (inform->value.Compare(svalue) < 0)  {
															inform->offset = i-inform->length-INDEX_OFFSET;
															start = next;
															block->lock = false;
															return;
														}
														break;
													}
										case 2: {                // >=
														if (inform->value.Compare(svalue) <= 0)  {
															inform->offset = i-inform->length-INDEX_OFFSET;
															start = next;
															block->lock = false;
															return;
														}
														break;
													}
										case 3: {                // <
														if (inform->value.Compare(svalue) <= 0)  {
															inform->offset = i-inform->length-INDEX_OFFSET-inform->length-INDEX_OFFSET;
															end = next;
															block->lock = false;
															return;
														}
														break;
													}
										case 4: {                // <=
														if (inform->value.Compare(svalue) == 0) {
															inform->offset = i-inform->length-INDEX_OFFSET;
															end = next;
															block->lock = false;
															return;
														} else if (inform->value.Compare(svalue) < 0) {
															inform->offset = i-inform->length-INDEX_OFFSET-inform->length-INDEX_OFFSET;
															end = next;
															block->lock = false;
															return;
														}
														break;
													}
									}
									break;
								}
					}
				}
				if(block->cBlock[i] == '#' && (type==3 || type == 4)) {
					inform->offset = i-inform->length-INDEX_OFFSET;
					end = next;
					block->lock = false;
					return;
				}
				block->lock = false;
				start = end = -10;            // not find
				return;
			}
		}
	}  catch (...) {
		throw;
	}
}


//��Ҷ�ӿ���ʱ���ú�����
void insert_split(CString DB_Name, CString  index_name, struct index_info* inform, int leaf1,int leaf2)
{
	int cx = inform->length;
	bool isFinish = false;
	CString soffset;
	soffset.Format(_T("%d"),inform->offset);
	int offsetlength = soffset.GetLength();
	while((INDEX_OFFSET - offsetlength)>0){
		soffset = '0' + soffset;
		offsetlength++;
	}
	CString insertvalue;
	switch(inform->type){
		case 0:
			insertvalue = int_to_str(_ttoi(inform->value));
			break;
		case 1:
			insertvalue = float_to_str((float)_ttof(inform->value));
			break;
		case 2:
			insertvalue = str_to_str(inform->value, inform->length);
			break;
		default:
			break;
	}
	CString childnum;
	while(!isFinish){							
		blockInfo* Left = get_file_block(DB_Name, index_name, 1, leaf1);
		blockInfo* Right = get_file_block(DB_Name, index_name, 1, leaf2);				
		CString snum="";
		for(int i = 1; i <= 4; i++)	
				snum += Left->cBlock[i];
		int num = _ttoi(snum);
		CString rightnum;           //��Right�Ŀ�ű�Ϊ�ַ���
		rightnum.Format(_T("%d"), leaf2);
		switch(rightnum.GetLength()){
			case 1:
				rightnum = "00" + rightnum;
				break;
			case 2:
				rightnum = '0' + rightnum;
				break;
			default:
				break;
		}
		CString leftnum = "";           //��Right�Ŀ�ű�Ϊ�ַ���
		leftnum.Format(_T("%d"), leaf1);
		switch(leftnum.GetLength()){
			case 1:
				leftnum = "00" + leftnum;
				break;
			case 2:
				leftnum = '0' + leftnum;
				break;
			default:
				break;
		}
		int count = 0;
		if(Left->cBlock[0] == '!')
			offsetlength = INDEX_OFFSET;
		else if(Left->cBlock[0] == '?')
			offsetlength = 3;
		for(count = 0; count < num; count++){
			CString svalue = "";
			CString sinsvalue = "";
			if(inform->type == 0){            //int
				int insvalue;
				for(int i = 0; i < cx; i++){
					if(insertvalue[i] == ';')
						break;
					sinsvalue += insertvalue[i];
				}
				insvalue = _ttoi(sinsvalue);
				for(int i = 5 + count * (offsetlength + cx) + offsetlength; i < 5 + (count + 1) * (offsetlength + cx); i++){
					if(Left->cBlock[i] == ';')
						break;
					svalue += Left->cBlock[i];
				}
				int value = _ttoi(svalue);
				if(insvalue < value)
					break;
			} 
			else if(inform->type == 1){            //float
				float insvalue;
				for(int i = 0; i < cx; i++){
					if(insertvalue[i] == ';')
						break;
					sinsvalue += insertvalue[i];
				}
				insvalue = (float)_ttof(sinsvalue);
				for(int i = 5 + count * (offsetlength + cx) + offsetlength; i < 5 + (count + 1) * (offsetlength + cx); i++){
					if(Left->cBlock[i] == ';')
						break;
					svalue += Left->cBlock[i];
				}
					float value = (float)_ttof(svalue);
				if(insvalue < value)
					break;
			}
			else{            //char
				for(int i = 5 + count * (offsetlength + cx) + offsetlength; i < 5 + (count + 1) * (offsetlength + cx); i++){
					if(Left->cBlock[i] == ';')
						break;
					svalue += Left->cBlock[i];
				}
				CString sinsvalue;
				for(int i = 0; i < cx; i++){
					if(insertvalue[i] == ';')
						break;
					sinsvalue += insertvalue[i];
				}
				if(sinsvalue.Compare(svalue) < 0){
					break;
				}
			}

		}
		if(leaf1 == 0){               //��
			if(Left->cBlock[0] == '!'){        //indexֻ�и�����ڵ�
				int newnum = get_new_freeblocknum(DB_Name, index_name);
				blockInfo* newLeft = get_file_block(DB_Name, index_name, 1, newnum);  
				if(count < (int)ceil(INDEX_N/2.0)){
					for(int i = 0; i < 5 + count * (INDEX_OFFSET + cx); i++)            
						newLeft->cBlock[i] = Left->cBlock[i];
					for(int i = 0; i < INDEX_OFFSET; i++)
						newLeft->cBlock[i + 5 + count * (INDEX_OFFSET + cx)] = soffset[i];
					for(int i = INDEX_OFFSET; i < INDEX_OFFSET + cx; i++)
						newLeft->cBlock[i + 5 + count * (INDEX_OFFSET + cx) ] = insertvalue[i];
					for(int i = 5 + (count + 1) * (INDEX_OFFSET + cx); i < 5 + (int)ceil(INDEX_N/2.0) * (INDEX_OFFSET + cx); i++)
						newLeft->cBlock[i] = Left->cBlock[i-cx-INDEX_OFFSET];
					for(int i = 5; i < 5 + (int)floor(INDEX_N/2.0)  * (INDEX_OFFSET + cx); i++)
						Right->cBlock[i] = Left->cBlock[i + (int)floor(INDEX_N/2.0) * (INDEX_OFFSET + cx)];
				}
				else{
					for(int i = 0; i < 5 + (int)ceil(INDEX_N/2.0) * (INDEX_OFFSET + cx); i++)            
						newLeft->cBlock[i] = Left->cBlock[i];
					for(int i = 5; i < 5 + (count - (int)ceil(INDEX_N/2.0)) * (INDEX_OFFSET + cx); i++)
						Right->cBlock[i] = Left->cBlock[(int)ceil(INDEX_N/2.0) * (INDEX_OFFSET + cx) + i];
					for(int i = 0; i < INDEX_OFFSET; i++)
						Right->cBlock[i + 5 + (count - (int)ceil(INDEX_N/2.0)) * (INDEX_OFFSET + cx)] = soffset[i];
					for(int i = 0; i < cx; i++)
						Right->cBlock[i + 5 + (count - (int)ceil(INDEX_N/2.0)) * (INDEX_OFFSET +cx) + INDEX_OFFSET] = insertvalue[i];
					for(int i = 5 + (count - (int)ceil(INDEX_N/2.0) + 1) * (INDEX_OFFSET + cx); i < 5 + (int)floor(INDEX_N/2.0) * (INDEX_OFFSET + cx); i++)
						Right->cBlock[i] = Left->cBlock[i + (int)ceil(INDEX_N/2.0) * (INDEX_OFFSET + cx)];
				}
				CString rightmin = "";
				for(int i = 5 + INDEX_OFFSET; i < 5 + INDEX_OFFSET + cx; i++)
					rightmin += Right->cBlock[i];
				Right->cBlock[0] = '!';
				Right->cBlock[1] = '0';
				Right->cBlock[2] = '0';
				Right->cBlock[3] = '0';
				Right->cBlock[4] = '7';		
				newLeft->cBlock[0] = '!';
				newLeft->cBlock[1] = '0';
				newLeft->cBlock[2] = '0';
				newLeft->cBlock[3] = '0';
				newLeft->cBlock[4] = '8';
				newLeft->cBlock[5 + (int)ceil(INDEX_N/2.0) * (INDEX_OFFSET + cx)] = rightnum[0];
				newLeft->cBlock[5 + (int)ceil(INDEX_N/2.0) * (INDEX_OFFSET + cx) + 1] = rightnum[1];
				newLeft->cBlock[5 + (int)ceil(INDEX_N/2.0) * (INDEX_OFFSET + cx) + 2] = rightnum[2];
				Right->cBlock[5 + (int)floor(INDEX_N/2.0) * (INDEX_OFFSET + cx)] = '#';
				CString leftnum;                     //��newLeft�Ŀ�ű�Ϊ�ַ���
				leftnum.Format(_T("%d"), newnum);       
				switch(leftnum.GetLength()){
					case 1:
						leftnum = "00" + leftnum;
						break;
					case 2:
						leftnum = '0' + leftnum;
						break;
					default:
						break;
				}	
				Left->cBlock[0] = '?';
				Left->cBlock[1] = '0';
				Left->cBlock[2] = '0';
				Left->cBlock[3] = '0';
				Left->cBlock[4] = '1';
				Left->cBlock[5] = leftnum[0];
				Left->cBlock[6] = leftnum[1];
				Left->cBlock[7] = leftnum[2];
				for(int i = 0; i < cx; i++)
					Left->cBlock[8 + i] = Right->cBlock[5 + INDEX_OFFSET + i];
				Left->cBlock[8 + cx] = rightnum[0];
				Left->cBlock[9 + cx] = rightnum[1];
				Left->cBlock[10 + cx] = rightnum[2];
				Left->cBlock[11 + cx] = 'n';
				//Left->blockNum = 0;                 //���ָ��ڵ�Ŀ��Ϊ0
				Left->dirtyBit = true;
				newLeft->dirtyBit = true;
				Right ->dirtyBit = true;
				isFinish = true;
				Left->lock = false;
				newLeft->lock =false;
				Right->lock = false;
			}//end if(Left->cBlock[0] == '!')
			else{                        //���Ƿ�Ҷ�ӽڵ�
				CString rightmin = "";
				Left->cBlock[1] = '0';
				Left->cBlock[2] = '0';
				Left->cBlock[3] = '0';				
				Left->cBlock[4] = '1';
				Right->cBlock[0] = '?';
				Right->cBlock[1] = '0';
				Right->cBlock[2] = '0';
				Right->cBlock[3] = '0';
				Right->cBlock[4] = '7';
				int newnum = get_new_freeblocknum(DB_Name, index_name);
				blockInfo* newLeft = get_file_block(DB_Name, index_name, 1, newnum);
				for(int i = 0; i < 5 + (int)floor(INDEX_N/2.0) * (3 + cx) + 3; i++)
					newLeft->cBlock[i] = Left->cBlock[i];
				newLeft->cBlock[4] = '7';
				if(count < (int)floor(INDEX_N/2.0)){
					for(int i = 5; i < 5 + ((int)ceil(INDEX_N/2.0) - 1) * (cx + 3) + 3; i++)
						Right->cBlock[i] = Left->cBlock[i + (int)floor(INDEX_N/2.0) * (cx + 3)];
					for(int i = 0; i < cx; i++)
						rightmin += Left->cBlock[i + (INDEX_N - 1 - (int)ceil(INDEX_N/2.0)) * (3 + cx) + 3];
					for(int i = 5 + (int)floor(INDEX_N/2.0) * (INDEX_OFFSET + cx) + 3 - 1; i >= 5 + (count + 1) * (3 + cx) + 3; i--)
						newLeft->cBlock[i] = Left->cBlock[i - 3 - cx];
					for(int i = 0; i < 3; i++)
						newLeft->cBlock[i + (count + 1) * (cx + 3) + 3] = childnum[i];
					for(int i = 0; i < cx; i++)
						newLeft->cBlock[i + count * cx + 5 + 3] = insertvalue[i];
				}
				else if(count == (int)floor(INDEX_N/2.0)){
					for(int i = 0; i < 3; i++)
						Right->cBlock[i + 5] = childnum[i];
					rightmin = insertvalue;
					for(int i = 0; i < (int)floor(INDEX_N/2.0) * (cx + 3); i++)
						Right->cBlock[i + 5 + 3] = Left->cBlock[i + 5 + (int)floor(INDEX_N/2.0) * (cx + 3) + 3];
				}
				else{
					for(int i = 0; i < cx; i++)
						rightmin += Left->cBlock[ i + (int)floor(INDEX_N/2.0) * (cx + 3) + 3 + 5];
					for(int i = 5; i < (count - (int)ceil(INDEX_N/2.0)) * (cx + 3) + 5 + 3; i++)
						Right->cBlock[i] = Left->cBlock[i + (int)ceil(INDEX_N/2.0) * (cx + 3)];
					for(int i = 0; i < cx; i++)
						Right->cBlock[5 + (count - (int)ceil(INDEX_N/2.0)) * (cx + 3) + 3 + i] = insertvalue[i];
					for(int i = 0; i < 3; i++)
						Right->cBlock[i + (count - (int)ceil(INDEX_N/2.0)) * (cx + 3) + 5 + 3 + cx] = childnum[i];
					for(int i = 5 + (count - (int)ceil(INDEX_N/2.0) + 1) * (cx + 3) + 3; i < 5 + (int)(ceil(INDEX_N/2.0) - 1) * (3 + cx) + 3; i++)
						Right->cBlock[i] = Left->cBlock[i + (int)ceil(INDEX_N/2.0) * (cx + 3)];
					Right->cBlock[5 + ((int)ceil(INDEX_N/2.0) - 1) * (cx + 3) + 3] = 'n';
				}				
				
				for(int i = 0; i < cx; i++)
					Left->cBlock[5 + 3 + i] = rightmin[i];
				CString leftnum;                     //��newLeft�Ŀ�ű�Ϊ�ַ���
				leftnum.Format(_T("%d"), newnum);       
				switch(leftnum.GetLength()){
					case 1:
						leftnum = "00" + leftnum;
						break;
					case 2:
						leftnum = '0' + leftnum;
						break;
					default:
						break;
				}
				for(int i = 0; i < 3; i++){
					Left->cBlock[i + 5] = leftnum[i];
					Left->cBlock[i + 5 + cx + 3] = rightnum[i];
				}
				Left->cBlock[5 + 3 + cx + 3] = 'n';
				Left->blockNum = 0;
				isFinish = true;
				Left->dirtyBit = true;
				Right->dirtyBit = true;
				newLeft->dirtyBit = true;
				Left->lock = false;
				Right->lock = false;
				newLeft->lock = false;
			}
		}//end if(leaf1 == 0)
		else if(Left->cBlock[0] == '!'){           //Ҷ�ӽڵ�
			CString rightmin = "";
			if(count < (int)ceil(INDEX_N/2.0)){				
				for(int i = 5; i < 5 + (int)floor(INDEX_N/2.0) * (INDEX_OFFSET + cx)+3; i++)
					Right->cBlock[i] = Left->cBlock[i + (int)floor(INDEX_N/2.0) * (INDEX_OFFSET + cx)];
				for(int i = 0; i < INDEX_OFFSET; i++)
					Left->cBlock[i + 5 + count * (INDEX_OFFSET + cx)] = soffset[i];
				for(int i = 0; i < cx; i++)
					Left->cBlock[i + 5 + count * (INDEX_OFFSET + cx) + INDEX_OFFSET] = insertvalue[i];
				for(int i = 5 + (int)ceil(INDEX_N/2.0) * (INDEX_OFFSET + cx) - 1; i >= 5 + (count + 1) * (INDEX_OFFSET + cx); i--)
					Left->cBlock[i] = Left->cBlock[i - INDEX_OFFSET - cx];
				for(int i = 5 + INDEX_OFFSET; i < 5 + INDEX_OFFSET + cx; i++)
					rightmin += Right->cBlock[i];
				Left->cBlock[5 + (int)ceil(INDEX_N/2.0) * (INDEX_OFFSET + cx)] = rightnum[0];
				Left->cBlock[5 + (int)ceil(INDEX_N/2.0) * (INDEX_OFFSET + cx) + 1] = rightnum[1];
				Left->cBlock[5 + (int)ceil(INDEX_N/2.0) * (INDEX_OFFSET + cx) + 2] = rightnum[2];
				Left->cBlock[5 + (int)ceil(INDEX_N/2.0) * (INDEX_OFFSET + cx) + 3] = 'n';
			}
			else{              //count>= (int)ceil(INDEX_N/2.0)
				for(int i = 0; i <cx; i++)
					rightmin += Left->cBlock[5 + (int)ceil(INDEX_N/2.0) * (INDEX_OFFSET + cx) + INDEX_OFFSET + i];
				for(int i = 5; i < 5 + (count - (int)ceil(INDEX_N/2.0)) * (INDEX_OFFSET + cx); i++)
					Right->cBlock[i] = Left->cBlock[i + (int)ceil(INDEX_N/2.0) * (INDEX_OFFSET + cx)];
				for(int i = 0; i < INDEX_OFFSET; i++)
					Right->cBlock[i + 5 + (count - (int)ceil(INDEX_N/2.0)) * (INDEX_OFFSET + cx)] = soffset[i];
				for(int i = 0; i < cx; i++)
					Right->cBlock[i + 5 + (count - (int)ceil(INDEX_N/2.0)) * (INDEX_OFFSET + cx) + INDEX_OFFSET] = insertvalue[i];
				for(int i = 5 + (count - (int)ceil(INDEX_N/2.0) + 1) * (INDEX_OFFSET + cx); i < (int)floor(INDEX_N/2.0) * (INDEX_OFFSET + cx) + 3; i++)
					Right->cBlock[i] = Left->cBlock[i + (int)ceil(INDEX_N/2.0) * (INDEX_N + cx)];
				for(int i = 0; i < 3; i++)
					Right->cBlock[i + 5 + (int)floor(INDEX_N/2.0) * (INDEX_OFFSET + cx)] = Left->cBlock[i + (INDEX_N - 1) * (INDEX_OFFSET + cx) + 5];
				Left->cBlock[5 + (int)ceil(INDEX_N/2.0) * (INDEX_OFFSET + cx)] = rightnum[0];
				Left->cBlock[5 + (int)ceil(INDEX_N/2.0) * (INDEX_OFFSET + cx) + 1] = rightnum[1];
				Left->cBlock[5 + (int)ceil(INDEX_N/2.0) * (INDEX_OFFSET + cx) + 2] = rightnum[2];
				Left->cBlock[5 + (int)ceil(INDEX_N/2.0) * (INDEX_OFFSET + cx) + 3] = 'n';
			}
			Left->cBlock[1] = '0';
			Left->cBlock[2] = '0';
			Left->cBlock[3] = '0';
			Left->cBlock[4] = '8';
			Right->cBlock[0] = '!';
			Right->cBlock[1] = '0';
			Right->cBlock[2] = '0';
			Right->cBlock[3] = '0';
			Right->cBlock[4] = '7';
			int fatherNum = find_father(DB_Name, index_name, inform, leaf1);
			blockInfo* father = get_file_block(DB_Name, index_name, 1, fatherNum);
			CString stempnum = "";
			for (int i = 1; i <= 4; i++)
				stempnum += father->cBlock[i];
			int tempnum = _ttoi(stempnum);
			if(tempnum < INDEX_N -1){       //���׽ڵ�δ��
				int counter = 0;
				for(int i = 5; i < tempnum * (3 + cx); i++){						
					CString stempchild = "";
					for(int j = 0; j < 3; j++)
						stempchild += father->cBlock[5 + counter * (3 + cx) + j]; 
					int tempchild = _ttoi(stempchild);
					if(tempchild == leaf1)
						break;
					else
						counter++;
				}			
				while(tempnum - counter){
					for(int i = 0; i < 3 + cx; i++)
						father->cBlock[i + tempnum * (3 + cx) + 5 + 3] = father->cBlock[5 + 3 + i + (tempnum - 1) * (3 + cx)];
					tempnum--;
				}
				for(int i = 0; i < cx; i++)
					father->cBlock[counter * (3 + cx) + 5 + 3 + i] = rightmin[i];
				for(int i = 0; i < 3; i++)
					father->cBlock[(counter + 1) * (3 + cx) + 5 + i] = rightnum[i];
				father->cBlock[(tempnum + 1) * (3 + cx) + 5 + 3] = 'n';
				tempnum = _ttoi(stempnum) + 1;
				CString finalnum = "";
				finalnum.Format(_T("%d"), tempnum);
				int finallength = finalnum.GetLength();
				for( ; finallength < 4; finallength++){
					finalnum = '0' + finalnum;					
				}
				for(int i = 0; i < 4; i++)
					father->cBlock[i + 1] = finalnum[i];
				father->dirtyBit = true;
				Left->dirtyBit = true;
				Right->dirtyBit = true;
				father->lock = false;
				Left->lock = false;
				Right->lock = false;
				isFinish = true;
			}
			else{             //���ڵ�����
					leaf1 = fatherNum;
					leaf2 = get_new_freeblocknum(DB_Name, index_name);
					Left->dirtyBit = true;
					Right->dirtyBit = true;
					father->dirtyBit = true;
					Left->lock = false;
					Right->lock = false;
					father->lock = false;
					if (leaf2 == -1) {
						cout << "The index nodes of this index are too many, we refuse to give it more memory." << endl;
						return;
					}
					insertvalue = rightmin;
					childnum = rightnum;
			}
		}
		else{               //�Ǹ���Ҷ�ӽڵ�
			CString rightmin = "";
			Left->cBlock[1] = '0';
			Left->cBlock[2] = '0';
			Left->cBlock[3] = '0';
			Left->cBlock[4] = '7';
			Right->cBlock[0] = '?';
			Right->cBlock[1] = '0';
			Right->cBlock[2] = '0';
			Right->cBlock[3] = '0';
			Right->cBlock[4] = '7';
			if(count < (int)floor(INDEX_N/2.0)){
				for(int i = 0; i < cx; i++)
					rightmin += Left->cBlock[i + ((int)floor(INDEX_N/2.0) - 1) * (3 + cx) + 3];
				for(int i = 5; i < 5 + (int)floor(INDEX_N/2.0) * (cx + 3) + 3; i++)
					Right->cBlock[i] = Left->cBlock[i + (int)floor(INDEX_N/2.0) * (cx + 3)];
				for(int i = 5 + (int)floor(INDEX_N/2.0) * (3 + cx) + 3 - 1; i >= 5 + (count + 1) * (3 + cx) + 3; i--)
					Left->cBlock[i] = Left->cBlock[i - 3 - cx];
				for(int i = 0; i < 3; i++)
					Left->cBlock[i + (count + 1) * (cx + 3) + 5] = childnum[i];
				for(int i = 0; i < cx; i++)
					Left->cBlock[i + count * (cx + 3)+ 5 + 3] = insertvalue[i];
			}
			else if(count == (int)floor(INDEX_N/2.0)){
				for(int i = 0; i < cx; i++)
					rightmin = insertvalue;
				for(int i = 0; i < 3; i++)
					Right->cBlock[i + 5] = rightnum[i];
				for(int i = 0; i < (int)floor(INDEX_N/2.0) * (cx + 3); i++)
					Right->cBlock[i + 5 + 3] = Left->cBlock[i + 5 + (int)floor(INDEX_N/2.0) * (cx + 3) + 3];
			}
			else{
				for(int i = 0; i < cx; i++)
					rightmin += Left->cBlock[ i + (int)floor(INDEX_N/2.0) * (cx + 3) + 3 + 5];
				for(int i = 5; i < (count - (int)ceil(INDEX_N/2.0)) * (cx + 3) + 5 + 3; i++)
					Right->cBlock[i] = Left->cBlock[i + (int)ceil(INDEX_N/2.0) * (cx + 3)];
				for(int i = 0; i < cx; i++)
					Right->cBlock[5 + (count - (int)ceil(INDEX_N/2.0)) * (cx + 3) + 3 + i] = insertvalue[i];
				for(int i = 5 + (count - (int)floor(INDEX_N/2.0)) * (cx + 3); i < 5 + (int)floor(INDEX_N/2.0) * (3 + cx) + 3; i++)
					Right->cBlock[i] = Left->cBlock[i + (int)ceil(INDEX_N/2.0) * (cx + 3)];
				for(int i = 0; i < 3; i++)
					Right->cBlock[i + (count - (int)floor(INDEX_N/2.0)) * (cx + 3) + 5] = childnum[i];
			}			
			int fatherNum = find_father(DB_Name, index_name, inform, leaf1);
			blockInfo* father = get_file_block(DB_Name, index_name, 1, fatherNum);
			CString stempnum = "";
			for (int i = 1; i <= 4; i++)
				stempnum += father->cBlock[i];
			int tempnum = _ttoi(stempnum);
			if(tempnum < INDEX_N -1){
				int counter = 0;
				for(int i = 5; i < tempnum * (3 + cx); i++){						
					CString stempchild = "";
					for(int j = 0; j < 3; j++)
						stempchild += father->cBlock[5 + counter * (3 + cx) + j]; 
					int tempchild = _ttoi(stempchild);
					if(tempchild == leaf1)
						break;
					else
						counter++;
				}
				father->cBlock[(tempnum + 1) * (cx + 3) + 5 + 3] = 'n';
				while(tempnum - counter){
					for(int i = 0; i < 3 + cx; i++)
						father->cBlock[i + tempnum * (3 + cx) + 5 + 3] = father->cBlock[5 + 3 + i + (tempnum - 1) * (3 + cx)];
					tempnum--;
				}					
				for(int i = 0; i < cx; i++)
					father->cBlock[counter * (3 + cx) + 5 + 3 + i] = rightmin[i];
				for(int i = 0; i < 3; i++)
					father->cBlock[(counter + 1) * (3 + cx) + 5 + i] = rightnum[i];
				tempnum = _ttoi(stempnum) + 1;
				CString finalnum = "";
				finalnum.Format(_T("%d"), tempnum);
				int finallength = finalnum.GetLength();
				for( ; finallength < 4; finallength++){
					finalnum = '0' + finalnum;					
				}
				for(int i = 0; i < 4; i++)
					father->cBlock[i + 1] = finalnum[i];
				father->dirtyBit = true;
				Left->dirtyBit = true;
				Right->dirtyBit = true;
				Left->lock = false;
				Right->lock = false;
				father->lock = false;
				isFinish = true;
			}
			else{             //���ڵ�����
					leaf1 = fatherNum;
					leaf2 = get_new_freeblocknum(DB_Name, index_name);
					Left->dirtyBit = true;
					Right->dirtyBit = true;
					father->dirtyBit = true;
					Left->lock = false;
					Right->lock = false;
					father->lock = false;
					if (leaf2 == -1) {
						cout << "The index nodes of this index are too many, we refuse to give it more memory." << endl;
						return;
					}
					insertvalue = rightmin;
					childnum = rightnum;
			}
		}
	}
}
	


//int ����ֵת����10�ֽڵ�string���� ��
CString int_to_str(int value)
{
	CString tostr;
	tostr.Format(_T("%d"),value);
	int length = tostr.GetLength();
	switch (length)
	{
		case 1:
			tostr += ";00000000";
			break;
		case 2:
			tostr += ";0000000";
			break;
		case 3:
			tostr += ";000000";
			break;
		case 4:
			tostr += ";00000";
			break;
		case 5:
			tostr += ";0000";
			break;
		case 6:
			tostr += ";000";
			break;
		case 7:
			tostr += ";00";
			break;
		case 8:
			tostr += ";0";
			break;
		case 9:
			tostr += ";";
			break;
		default:
			break;
	
	}
	return tostr;
}

CString float_to_str(float value)
{
	CString tostr;
	tostr.Format(_T("%f"),value);
	tostr += ';';
	int length = tostr.GetLength();
	while(40 - length){
		tostr += '0';
		length++;
	}
	return tostr;
}

//�ҳ�����Ҷ�ӿ��ǰһ���ֵ�Ҷ�ӿ飺
int find_prev_sibling(CString DB_Name, CString  index_name, struct index_info *inform, int nodenum)
{
	int fanum=find_father( DB_Name, index_name, inform, nodenum);
	blockInfo * block = get_file_block(DB_Name,index_name,1,fanum);
	CString snum = "";
	CString svalue = "";
	char ch[4096];   //��ʱ����
	int num, value, L,num1;
	for(int i = 1; i <= 4; i++)	
		snum += block->cBlock[i];
	num = _ttoi(snum);
	char str[4];
	int tmp;
	for(int i=0;i<num+1;i++)
	{
		//char str[4];
		L=i*(3+inform->length)+5;
		for(int j=0;j<3;j++)
			str[j]=block->cBlock[j+L];
		str[3]='\0';
		tmp=_ttoi(str);
		if(tmp==nodenum)
		{
			if(i==0)  //û�����ֵ�
				return 0;
			for(int j=0;j<3;j++)
				str[j]=block->cBlock[j+L-(3+inform->length)];
			return _ttoi(str);
		}
	}
	return -1;
}
//����˵����
//database�����ݿ�����cstring�ͣ�
//table_name������ļ�����cstring��
//inform��info�ṹ�壬�������ã�
//nodenum������Ҷ�ӿ���
//����ֵ��
// -1      �����ж�������飻
//0       û��ǰһ���ֵ�Ҷ�ӿ飨���˿�Ϊ�����ӣ�
//>0      ǰһ���ֵ�Ҷ�ӿ�Ŀ��
//˼·������find_left_child�������ҳ������ӣ����ж��Ƿ���nodenum��ȣ�����ȵĻ���
//�������к��ӣ��ҵ�nodenum��ʱ����������ǰ��ڵ㣬���򣬷���0��

//�ҳ�����Ҷ�ӿ�ĺ�һ���ֵ�Ҷ�ӿ飺
int find_next_sibling(CString DB_Name, CString  index_name,struct index_info *inform,int nodenum)
{
	int fanum=find_father( DB_Name, index_name, inform, nodenum);
	blockInfo * block = get_file_block(DB_Name,index_name,1,fanum);
	CString snum = "";
	CString svalue = "";
	char ch[4096];   //��ʱ����
	int num, value, L,num1;
	for(int i = 1; i <= 4; i++)	
		snum += block->cBlock[i];
	num = _ttoi(snum);
	char str[4];
	int tmp;
	for(int i=0;i<num+1;i++)
	{
		//char str[4];
		L=i*(3+inform->length)+5;
		for(int j=0;j<3;j++)
			str[j]=block->cBlock[j+L];
		str[3]='\0';
		tmp=_ttoi(str);
		if(tmp==nodenum)
		{
			if(i==num)  //û�����ֵ�
				return 0;
			for(int j=0;j<3;j++)
				str[j]=block->cBlock[j+L+(3+inform->length)];
			return _ttoi(str);
		}
	}
	return -1;
}

//�ҳ�����Ҷ�ӿ�Ŀ�ţ�
//����˵����
//database�����ݿ�����cstring�ͣ� 
//inform��info�ṹ�壬�������ã�
//����ֵ��
//-1      �����ж�������飻
//0       �����ļ���
//>0      ����Ҷ�ӿ�Ŀ��
//˼·������B������ԭ��ÿ�β��������ӣ�ֱ���ҵ�Ҷ�ӽڵ㡣
int find_left_child (CString DB_Name,CString index_name) {
	try {
		fileInfo* file = get_file_info(DB_Name,index_name,1);
		int next = file->freeNum;
		bool isGet = false;
		while(true) {
			blockInfo* block = get_file_block(DB_Name,index_name,1,next);
			if (block->cBlock[0] == '#') return 0;
			if (block->cBlock[0] == '?') {
					int i = 5;
					int cx = 3;
					CString sblock_n="";
					while(cx--) {                     // ���ӽڵ���
						sblock_n += block->cBlock[i] ;
						++i;
					}
					next = _ttoi(sblock_n);
			} else {
				return next;
			}
		}
	} catch (...) {
		throw;
	}
}

//�ҳ�delete����ʱmerge�����ڵ�����Ҫ��ӵ�����ֵ�����ҽڵ������ߵ�Ҷ�ڵ�ֵ
CString find_merge_value_left(CString DB_Name, CString  index_name,index_info* inform,int num)
{
	blockInfo * block = get_file_block(DB_Name,index_name,1,num);
	if(block->cBlock[0]=='!')  //�ҵ�
	{
		CString cs="";
		for(int i=0;i<inform->length;i++)
			cs+=block->cBlock[i+5+INDEX_OFFSET];
		return cs;
	}
	else
	{
		CString cs="";
		for(int i=0;i<3;i++)
			cs+=block->cBlock[i+5];
		return find_merge_value_left(DB_Name, index_name,inform,_ttoi(cs));
	}

}

//�ҳ�delete����ʱmerge�����ڵ�����Ҫ��ӵ�����ֵ������ڵ�����ұߵ�Ҷ�ڵ�ֵ
CString find_merge_value_right(CString DB_Name, CString  index_name,index_info* inform,int num)
{
	blockInfo * block = get_file_block(DB_Name,index_name,1,num);
	if(block->cBlock[0]=='!')  //�ҵ�
	{
		CString cs="";
		int nn;
		for(int i = 1; i <= 4; i++)	
			cs += block->cBlock[i];
		nn = _ttoi(cs);
		int L=5+(inform->length+INDEX_OFFSET)*(nn-1)+INDEX_OFFSET;
		for(int i=0;i<inform->length;i++)
			cs+=block->cBlock[i+L];
		return cs;
	}
	else
	{
		CString cs="";
		int nn;
		for(int i = 1; i <= 4; i++)	
			cs += block->cBlock[i];
		nn = _ttoi(cs);
		int L=5+(inform->length+3)*nn;
		for(int i=0;i<3;i++)
			cs+=block->cBlock[i+L];
		return find_merge_value_right(DB_Name, index_name,inform,_ttoi(cs));
	}

}



//��ȡһ���տ�Ŀ�ţ���ʹ�ܿ�����һ
int get_new_freeblocknum(CString DB_Name, CString  index_name)
{
	try {
		blockInfo* block;
		writeFile(DB_Name,index_name,1);
		fileInfo* file = get_file_info(DB_Name,index_name,1);
		bool isGetfreeblock = false;
		int blockNum = 0;               
		if(file->recordAmount > 256)
			return -1;
		else{
			while (!isGetfreeblock) {	
				block = get_file_block(DB_Name,index_name,1, blockNum);			
				if(block->cBlock[0] == 'n'){         //blockδʹ�ù�
					isGetfreeblock = true;
					block->cBlock[0] = 'w';
					block->lock=false;
					file->recordAmount++;
					return blockNum;
				}
				if (block->cBlock[0] == '0') {          // blockΪ��
					isGetfreeblock = true;
					block->cBlock[0] = 'f';
					block->lock=false;
					return blockNum;				
				}
				else{
					block->lock=false;
					blockNum++;
				}
			}
		}
	} 
	catch (...) {
		throw;
	}
	
}

//�ҿ�num�ĸ��׿�Ŀ�ţ��ڲ���inform . value��·���ϣ�
int find_father(CString DB_Name, CString  index_name,index_info* inform, int num)
{
	int fatherNum;
	try {
		fileInfo* file = get_file_info(DB_Name, index_name, 1);
		int next = 0;
		int l = file->recordAmount;
		if(num == 0){              //û�и��׿飨���˿�Ϊ����
			fatherNum = -1;
			return fatherNum;
		}
		else {
			CString sminnum = "";
			int cx = inform->length;
			int tempnum;
			for(int i = 0; i < l; i++){
				blockInfo* block = get_file_block(DB_Name, index_name, 1, i);
				CString snodenum = "";
				if (block->cBlock[0] == '0')  {     // �տ�
					block->lock = false;
					continue;      
				}
				for(cx = 1; cx <= 4; cx++){
					snodenum += block->cBlock[cx];
				}
				int nodenum = _ttoi(snodenum);
				if(block->cBlock[0] == '?'){      //�м�ڵ�	
					int len = 5;
					for(int j = 0; j <nodenum; j++){
						CString stempnum = "";						
						for(int k = len; k < len + 3; k++){   //ȡ�����ӿ��
							stempnum += block->cBlock[k];
						}
						tempnum = _ttoi(stempnum);
						if(tempnum == num){
							fatherNum = i;
							block->lock = false;
							return fatherNum;
						}
						else
							len += 3 + inform->length;
					}
					CString stempnum = "";						
					for(int k = len; k < len + 3; k++){   //ȡ�����ӿ��
						stempnum += block->cBlock[k];
					}
					tempnum = _ttoi(stempnum);
					if(tempnum == num){
						fatherNum = i;
						block->lock = false;
						return fatherNum;
					}
				}
				block->lock = false;
			}
			return -1;
		}
	} 
	catch (...) {
		throw;
	}
}


CString str_to_str(CString value, int len){
	int length = value.GetLength();
	if(length < len){
		value += ';';
		length++;
	}
	while(len - length){
		value += '0';
		length++;
	}
	return value;
}

void update_block(CString DB_Name,CString index_name,int blockNum,index_info* inform,CString w)
{
	int fa=find_father(DB_Name, index_name,inform,blockNum);
	blockInfo * fablock = get_file_block(DB_Name,index_name,1, fa);
	CString cs="";
	for(int i=0;i<3;i++)
		cs+=fablock->cBlock[i+5];
	int t=_ttoi(cs),L;
	char str[4];
	if(fa==0)
	{
		if(t==blockNum)
		{
			
			fablock->dirtyBit=false;
			fablock->lock=false;
			return ;
		}
		else
		{
			
			CString cn="";
			for(int i=0;i<4;i++)
				cn+=fablock->cBlock[i+1];
			int count=_ttoi(cn);
			for(int i=0;i<count+1;i++)
			{
				L=i*(3+inform->length)+5;
				for(int j=0;j<3;j++)
					str[j]=fablock->cBlock[j+L];
				str[3]='\0';
				int tmp=_ttoi(str);
				if(tmp==blockNum)
				{
					for(int j=0;j<inform->length;j++)
						fablock->cBlock[j+L-inform->length]=w.GetAt(j);
					fablock->dirtyBit=true;
					fablock->lock=false;
					//writeFile(DB_Name,index_name,1);
					return ;
				}
			}
		}
	}
	else if(t==blockNum)
	{
		
		fablock->dirtyBit=false;
		fablock->lock=false;
		update_block(DB_Name, index_name,fa,inform,w);
	}
	else
	{
		
		CString cn="";
		for(int i=0;i<4;i++)
			cn+=fablock->cBlock[i+1];
		int count=_ttoi(cn);
		for(int i=0;i<count+1;i++)
		{
			L=i*(3+inform->length)+5;
			for(int j=0;j<3;j++)
				str[j]=fablock->cBlock[j+L];
			str[3]='\0';
			int tmp=_ttoi(str);
			if(tmp==blockNum)
			{
				for(int j=0;j<inform->length;j++)
					fablock->cBlock[j+L-inform->length]=w.GetAt(j);
				fablock->dirtyBit=true;
				fablock->lock=false;
				//writeFile(DB_Name,index_name,1);
				return ;
			}
		}
	}
}
