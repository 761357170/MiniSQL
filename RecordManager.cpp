#include "RecordManager.h"
#include "BufferManager.h"
#include "IndexManager.h"
#include <set>


/**************************** ����ӿ� ****************************/
//�ͷ����ݿ���ڴ滺����
void Close_Database(CString DB_Name,bool closetype) {
	try {
		closeDatabase(DB_Name,closetype);
	} catch (...) {
		throw;
	}
}

//�ͷű���������ڴ滺����
void Close_File(CString DB_Name,CString filename,int filetype) {
	try {
		closeFile(DB_Name,filename,filetype);
	} catch (...) {
		throw;
	}
}

//�������������ڵ�
void Insert_Index_All(CString DB_Name,CString Table_Name,CString Index_Name,int length,int offset,int type) {
	try {
		blockInfo* block;
		index_info* index;
		fileInfo* file = get_file_info(DB_Name,Table_Name,0);   //ȡ�ļ��е�record
		int len = (int)floor(BLOCK_LEN/file->recordLength)*file->recordLength;
		for (int i=0; i<file->freeNum; ++i) {
			block = get_file_block(DB_Name,Table_Name,0,i);
			for (int j=0; j<len; j+=file->recordLength) {
				if (block->cBlock[j] == 'u') {           //��ʹ��
					index = new index_info();
					index->index_name= Index_Name;
					index->length = length;
					index->offset = i*len+j;
					index->type = type;
					int base_len = j+1+offset;   
					int off_len = 0;                                         //ȡֵ
					CString value="";
					char ch = block->cBlock[base_len+off_len];
					while(off_len<length && ch != ';') {
						value +=ch;
						++off_len;
						if ((base_len+off_len+1)>=BLOCK_LEN) break;
						ch = block->cBlock[base_len+off_len];
					}
					index->value=value;
					index->isInsert = 0;
					insert_one(DB_Name,Index_Name,index);
				}
			}
			block->lock = false;       // not pin
		}
	} catch (...) {
		throw;
	}
}

//����в���Ԫ��
bool Insert_Item(CString DB_Name,CString Table_Name,CString Attr,index_info* nodes,int num,attr_info* uniques, int uniqueNum) {
	try {
		blockInfo* block;
		fileInfo* file;
		for (int i=0; i<num; ++i) {                 //��֤ index  unique
			nodes[i].isInsert = 0;
			int return_value = search_one(DB_Name,nodes[i].index_name,&nodes[i],0);
			if ( return_value >=0 && nodes[i].isInsert!=-2) {
				std::cout << " This value in index " << nodes[i].index_name <<"  has already existed." << endl;
				return false;
			}
		}
		if (uniqueNum > 0) {   //��֤�� index  unique
			file = get_file_info(DB_Name,Table_Name,0);   //ȡ�ļ��е�record
			int len =  (int)floor(BLOCK_LEN/file->recordLength)*file->recordLength;
			for (int i=0; i<file->freeNum; ++i) {                
				block = get_file_block(DB_Name,Table_Name,0,i);
				for (int j=0; j<len; j+=file->recordLength) {
					if (block->cBlock[j] == 'u') {           //��ʹ��	
						for (int k=0; k<uniqueNum; ++k) {
							char ch;
							int base_len =j+1+uniques[k].offset;
							int off_len = 0;                         //ȡֵ
							CString result = "";
							ch = block->cBlock[base_len];
							while (off_len<uniques[k].length && ch != ';') {
								result+=ch;
								++off_len;
								if((base_len+off_len)>=BLOCK_LEN) break;
								ch = block->cBlock[base_len+off_len];
							}
							if (result.Compare(uniques[k].value) == 0) {
								cout << " This value in " << uniques[k].attr_name <<"  has already existed." << endl;
								return false;
							}
						}
					}
				}
				block->lock=false;
			}
		}
		file = get_file_info(DB_Name,Table_Name,0);   //ȡ�ļ��е�record
		int len =  (int)floor(BLOCK_LEN/file->recordLength)*file->recordLength;
		bool get = false;
		int i=0;
		while(true) {
			block = get_file_block(DB_Name,Table_Name,0,i);
			if(block->charNum >= file->recordLength) {            //��װ��record
				for (int j=0; j<len; j+=file->recordLength) {
					if (block->cBlock[j] != 'u') {           //δ��ʹ��	
						strcpy_s(&block->cBlock[j],file->recordLength+1,(char*)Attr.GetBuffer());
						block->dirtyBit = true;
						block->charNum -= file->recordLength;
						block->cBlock[j] = 'u';
						for (int k=0; k<num; ++k) {    //����index�ڵ��ƫ��ֵ
							nodes[k].offset=i*len+j;			
							nodes[k].isInsert = 0;
							insert_one(DB_Name,nodes[k].index_name,&nodes[k]);
						}
						get = true;
						break;
					}
				}
			}
			block->lock = false;
			if(get) break;
			++i;
		}
		if (i>=file->freeNum) file->freeNum +=1;    //����һ��block
		cout << "Query Ok." << endl;
		return true;
	} catch (...) {
		throw;
	}
}

//ѡ����䣨��where��
void Select_No_Where(CString DB_Name,CString Table_Name,attr_info print[32],int count) {
	try {
		blockInfo* block;
		fileInfo* file = get_file_info(DB_Name,Table_Name,0);     //ȡ�ļ��е�record
		int len =  (int)floor(BLOCK_LEN/file->recordLength)*file->recordLength;
		CString resultHead="";                   // ��ӡ�ļ�ͷ��Ϣ
		for(int k=0; k<count; ++k) {
			resultHead+=print[k].attr_name;
			//for (int i=print[k].attr_name.GetLength(); i<print[k].length;++i)
			//	result+=' ';
			resultHead +='\t';
		}
		resultHead +='\n';
		CString result = "";
		for (int i=0; i<file->freeNum; ++i) {
			block = get_file_block(DB_Name,Table_Name,0,i);
			for (int j=0; j<len; j+=file->recordLength) {
				if (block->cBlock[j] == 'u') {           //��ʹ��
					for (int k=0; k<count; ++k) {
						char ch;
						int base_len =j+1+print[k].offset;
						int off_len = 0;                         //ȡֵ
						ch = block->cBlock[base_len];
						while (off_len<print[k].length && ch != ';') {
							result+=ch;
							++off_len;
							if ((base_len+off_len)>=BLOCK_LEN) break;
							ch = block->cBlock[base_len+off_len];
						}
					//	for (int p=off_len; p<print[k].length; ++p)   //�ո��
					//		result+=' ';
						result +='\t';
					}
					result+='\n';
				}
			}
			block->lock = false;
		}
		if (result == "") {
			cout << "No result." << endl;
		} else {
			cout << resultHead << result << endl;
		}
	} catch (...) {
		throw;
	}
}

//ѡ����䣨��where��
void Select_With_Where(CString DB_Name,CString Table_Name,condition_info conds[10],int count,attr_info print[32],int attr_num) {
	try {
		index_info* index;
		set<int> addr,newAddr;
		int notEqual[11],notEqualNum=-1;           // ���11������
		int i;
		for (i=0; i<count; ++i) {
			if(conds[i].left_index_name !="") {               // ������
				switch(conds[i].op) {
				case 1:  {                                     //  >		     						
								if (!Select_With_Greater_Cond(DB_Name,conds[i],1,addr,i))
									return;
								break;
							}
				case 2:  {                  //  >=		       
								if(!Select_With_Greater_Cond(DB_Name,conds[i],2,addr,i))
									return;
								break;
							}
				case 3: {                    //  <	 						
								if(!Select_With_Smaller_Cond(DB_Name,conds[i],3,addr,i))
									return;
								break; 
						   }
				case 4: {                   //  <=	       
								if(!Select_With_Smaller_Cond(DB_Name,conds[i],4,addr,i))
									return;
								break; 
							}
				case 5:  {                  // =
								index = new index_info();
								index->index_name = conds[i].left_index_name;
								index->type = conds[i].left_type;
								index->length = conds[i].left_len;
								index->value  = conds[i].const_data ;
								index->isInsert = 0;
								int return_value = search_one(DB_Name,conds[i].left_index_name,index,0);    //  =
								if (return_value <0 || (return_value>=0 && index->isInsert==-2)) {
									cout << "No such result." << endl;
									return;
								}
								blockInfo* block = block = get_file_block(DB_Name,conds[i].left_index_name,1,return_value);
								CString record=""; 
								char ch;
								int base_len = index->offset;           //addr ��ʼλ��
								int off_len=0;
								ch = block->cBlock[base_len];
								while(off_len < INDEX_OFFSET && ch != ';') {
									record += ch;
									++off_len;
									if((base_len+off_len)>=BLOCK_LEN) break;
									ch =  block->cBlock[base_len+off_len];
								}
								int record_addr = _ttoi(record);
								if (i==0) {
									addr.insert(record_addr);
								} else {
									set<int >::iterator it;								
									it = addr.find(record_addr);
									if(it == addr.end()) {
											cout << "No such result." << endl;    
											return;
									} else {
										addr.erase(addr.begin(),addr.end());
										addr.insert(record_addr);
									}
								}
								block->lock = false;
								delete index;
								break; 
							}
				case 6: {
								index = new index_info();
								index->index_name = conds[i].left_index_name;
								index->type = conds[i].left_type;
								index->length = conds[i].left_len;
								index->value  = conds[i].const_data ;
								index->isInsert = 0;
								int return_value=search_one(DB_Name,conds[i].left_index_name,index,0);    // <>
								if (return_value >= 0 && index->isInsert!=-2) {                //�ҵ� = ��ֵ	
								   blockInfo* block = block = get_file_block(DB_Name,conds[i].left_index_name,1,return_value);
								   CString record=""; 
									char ch;
									int base_len = index->offset;           //addr ��ʼλ��
									int off_len=0;
									ch = block->cBlock[base_len];
									while(off_len < INDEX_OFFSET && ch != ';') {
										record += ch;
										++off_len;
										if((base_len+off_len)>=BLOCK_LEN) break;
										ch =  block->cBlock[base_len+off_len];
									}
									int record_addr = _ttoi(record);
									notEqual[++notEqualNum] = record_addr;
									block->lock = 0;
								}	
								delete index;
								break; 
							}
				}
			}
			else break;
		}
		CString resultHead="";                   // ��ӡѡ���ļ�ͷ��Ϣ 
		for(int k=0; k<attr_num; ++k) {                  
			resultHead+=print[k].attr_name;
			//for (int i=print[k].attr_name.GetLength(); i<=print[k].length;++i)
			//	result+=' ';
			resultHead +='\t';
		}
		resultHead+='\n';
		CString result = "";
		fileInfo* file = get_file_info(DB_Name,Table_Name,0);      
		int len = (int)floor(BLOCK_LEN/file->recordLength)*file->recordLength;
		set<int>::iterator it;
		if (i==0 || (addr.size() == 0 && notEqualNum>=0)) {
			for (int ip=0; ip<file->freeNum; ++ip) {
				blockInfo* block = get_file_block(DB_Name,Table_Name,0,ip);
				for (int j=0; j<len; j+=file->recordLength) {
					if (block->cBlock[j] == 'u') {           //��ʹ��
						bool notE = false;
						for (int k=0; k<= notEqualNum; ++k) {                // ���<>���� 
							if ((int)ceil(notEqual[k]/len)==ip && notEqual[k]%len==j) {
								notE = true;
								break;
							}
						}
						if (notE) continue;
						CString record= "";
						int countNum = 0;
						while(countNum < file->recordLength) {
							record += block->cBlock[j+countNum];
							++countNum;
						}
						bool isSelect = true;
						for(int k=i; k<count; ++k) {              //��֤�Ƿ����ÿһ����
							if (!Confirm(record,file->recordLength,conds[k])) {           //��������������result
								isSelect = false;
								break;
							}
						}
						if (isSelect) {
							for (int k=0; k<attr_num; ++k) {               // ��ȡ������
								char ch;
								int base_len =1+print[k].offset;
								int off_len = 0;                         //ȡֵ
								ch = record[base_len];
								while (off_len<print[k].length && ch != ';') {
									result+=ch;
									++off_len;
									if (base_len+off_len >= file->recordLength) break;
									ch = record[base_len+off_len];
								}
								//for (int p=off_len; p<print[k].length; ++p)   //�ո��
								//	result+=' ';
								result +='\t';
							}
							result+='\n';
						}
					}
				}
				block->lock = false;
			}
		} else {
			for (it=addr.begin(); it != addr.end(); ++it) {              //���ݵ�ַȡ��record
				bool notE = false;
				for (int j=0; j<= notEqualNum; ++j) {                // ���<>���� 
					if (*it == notEqual[j]) {
						notE = true;
						break;
					}
				}
				if (notE) continue;
				int block_n = (int)floor(*it / len);        // block��0��ʼ������+1
				int block_o = *it%len;
	/*			if (block_o != 0) {
					block_o = (block_o -1) * file->recordLength;		
				}
				else {
					block_o = len - file->recordLength;
					--block_n;
				}*/
				blockInfo* block = get_file_block(DB_Name,Table_Name,0,block_n);
				CString record= "";
				int countNum = 0;
				while(countNum < file->recordLength) {
					record += block->cBlock[block_o+countNum];
					++countNum;
				}
				bool isSelect = true;
				for(int j=i; j<count; ++j) {              //��֤�Ƿ����ÿһ����
					if (!Confirm(record,file->recordLength,conds[j])) {           //��������������result
						isSelect = false;
						break;
					}
				}
				if (isSelect) {
					for (int k=0; k<attr_num; ++k) {               // ��ȡ������
						char ch;
						int base_len =1+print[k].offset;
						int off_len = 0;                         //ȡֵ
						ch = record[base_len];
						while (off_len<print[k].length && ch != ';') {
							result+=ch;
							++off_len;
							if (base_len+off_len >= file->recordLength) break;
							ch = record[base_len+off_len];
						}
						//for (int p=off_len; p<print[k].length; ++p)   //�ո��
						//	result+=' ';
						result +='\t';
					}
					result +='\n';
				}
				block->lock = false;
			}
		}
		if (result == "") {
			cout << "No such result." << endl;
		} else {
			cout << resultHead << result << endl;
		}
	} catch (...) {
		throw;
	}
}

//ɾ����䣨��where��
void Delete_With_Where(CString DB_Name,CString Table_Name,condition_info conds[10],int count,index_info nodes[32],int num) {
	try {
		index_info* index;
		set<int> addr,newAddr;
		int notEqual[11],notEqualNum=-1;           // ���11������
		int i;
		for (i=0; i<count; ++i) {
			if(conds[i].left_index_name !="") {               // ������
				switch(conds[i].op) {
				case 1:  {                                     //  >		     						
								if (!Select_With_Greater_Cond(DB_Name,conds[i],1,addr,i))
									return;
								break;
							}
				case 2:  {                  //  >=		       
								if(!Select_With_Greater_Cond(DB_Name,conds[i],2,addr,i))
									return;
								break;
							}
				case 3: {                    //  <	 						
								if(!Select_With_Smaller_Cond(DB_Name,conds[i],3,addr,i))
									return;
								break; 
						   }
				case 4: {                   //  <=	       
								if(!Select_With_Smaller_Cond(DB_Name,conds[i],4,addr,i))
									return;
								break; 
							}
				case 5:  {                  // =
								index = new index_info();
								index->index_name = conds[i].left_index_name;
								index->type = conds[i].left_type;
								index->length = conds[i].left_len;
								index->value  = conds[i].const_data ;
								index->isInsert = 0;
								int return_value = search_one(DB_Name,conds[i].left_index_name,index,0);    //  =
								if (return_value <0 || (return_value>=0 && index->isInsert==-2))  {
									cout << "No such result." << endl;
									return;
								}
								blockInfo* block = block = get_file_block(DB_Name,conds[i].left_index_name,1,return_value);
								CString record=""; 
								char ch;
								int base_len = index->offset;           //addr ��ʼλ��
								int off_len=0;
								ch = block->cBlock[base_len];
								while(off_len < INDEX_OFFSET && ch != ';') {
									record += ch;
									++off_len;
									if((base_len+off_len)>=BLOCK_LEN) break;
									ch =  block->cBlock[base_len+off_len];
								}
								int record_addr = _ttoi(record);
								if (i==0) {
									addr.insert(record_addr);
								} else {
									set<int >::iterator it;								
									it = addr.find(record_addr);
									if(it == addr.end()) {
											cout << "No such result." << endl;    
											return;
									} else {
										addr.erase(addr.begin(),addr.end());
										addr.insert(record_addr);
									}
								}
								block->lock = false;
								delete index;
								break; 
							}
				case 6: {
								index = new index_info();
								index->index_name = conds[i].left_index_name;
								index->type = conds[i].left_type;
								index->length = conds[i].left_len;
								index->value  = conds[i].const_data ;
								index->isInsert = 0;
								int return_value=search_one(DB_Name,conds[i].left_index_name,index,0);    // <>
								if (return_value >= 0 && index->isInsert != -2) {                //�ҵ� = ��ֵ	
								   blockInfo* block = block = get_file_block(DB_Name,conds[i].left_index_name,1,return_value);
								   CString record=""; 
									char ch;
									int base_len = index->offset;           //addr ��ʼλ��
									int off_len=0;
									ch = block->cBlock[base_len];
									while(off_len < INDEX_OFFSET && ch != ';') {
										record += ch;
										++off_len;
										if((base_len+off_len)>=BLOCK_LEN) break;
										ch =  block->cBlock[base_len+off_len];
									}
									int record_addr = _ttoi(record);
									notEqual[++notEqualNum] = record_addr;
									block->lock = 0;
								}	
								delete index;
								break; 
							}
				}
			}
			else break;
		}
		fileInfo* file = get_file_info(DB_Name,Table_Name,0);      
		int len = (int)floor(BLOCK_LEN/file->recordLength)*file->recordLength;
		if (i==0 || (addr.size() == 0 && notEqualNum>=0)) {
			for (int ip=0; ip<file->freeNum; ++ip) {
				blockInfo* block = get_file_block(DB_Name,Table_Name,0,ip);
				for (int j=0; j<len; j+=file->recordLength) {
					if (block->cBlock[j] == 'u') {           //��ʹ��
						bool notE = false;
						for (int k=0; k<= notEqualNum; ++k) {                // ���<>���� 
							if ((int)ceil(notEqual[k]/len)==ip && notEqual[k]%len==j) {
								notE = true;
								break;
							}
						}
						if (notE) continue;
						CString record= "";
						int countNum = 0;
						while(countNum < file->recordLength) {
							record += block->cBlock[j+countNum];
							++countNum;
						}
						bool isDelete = true;
						for(int k=i; k<count; ++k) {              //��֤�Ƿ����ÿһ����
							if (!Confirm(record,file->recordLength,conds[k])) {           //��������������ɾ��
								isDelete = false;
								break;			
							}
						}
						if (isDelete) {    // ���������ϣ�ɾ��
							block->cBlock[j] = 'n';
							block->charNum += file->recordLength;
							block->dirtyBit = true;          
							for (int k=0; k<num; ++k)
								Delete_Nodes(DB_Name,record,file->recordLength,nodes[k]);  // Ѱ�Ҵ˼�¼�ĸ�index�ڵ㣬ɾ��
						}
					}
				}
				block-> lock = false;
			}
		} else {
			set<int>::iterator it;
			for (it=addr.begin(); it != addr.end(); ++it) {              //���ݵ�ַȡ��record
				bool notE = false;
				for (int j=0; j<= notEqualNum; ++j) {                // ���<>���� 
					if (*it == notEqual[j]) {
						notE = true;
						break;
					}
				}
				if (notE) continue;
				int block_n = (int)floor(*it / len);        // block��0��ʼ������+1
				int block_o = *it%len;
	/*			if (block_o != 0) {
					block_o = (block_o -1) * file->recordLength;		
				}
				else {
					block_o = len - file->recordLength;
					--block_n;
				}*/
				blockInfo* block = get_file_block(DB_Name,Table_Name,0,block_n);
				CString record= "";
				int countNum = 0;
				while(countNum < file->recordLength) {
					record += block->cBlock[block_o+countNum];
					++countNum;
				}
				bool isDelete = true;
				for(int j=i; j<count; ++j) {              //��֤�Ƿ����ÿһ����
					if (!Confirm(record,file->recordLength,conds[j])) {           //��������������ɾ��
						isDelete = false;
						break;			
					}
				}
				if (isDelete) {    // ���������ϣ�ɾ��
					block->cBlock[block_o] = 'n';
					block->charNum += file->recordLength;
					block->dirtyBit = true;          
					for (int j=0; j<num; ++j)
						Delete_Nodes(DB_Name,record,file->recordLength,nodes[j]);  // Ѱ�Ҵ˼�¼�ĸ�index�ڵ㣬ɾ��
				}
				block-> lock = false;
			}
		}
		cout << "Query ok." << endl;
	} catch (...) {
		throw;
	}
}



/*********************** ���ں��� ****************************/

void Delete_Nodes(CString DB_Name,CString record,int len,index_info node) {
	try {
		CString attr="";
		int base_len = node.offset+1;
		int off_len = 0;
		char ch = record[base_len];                 // ȡ��attr
		while(off_len < node.length &&  ch != ';') {
			attr +=ch;
			++ off_len;
			if (base_len + off_len >= len) break;
			ch = record[base_len+off_len];
		}
		node.value=attr;
		node.isInsert=0;
		delete_one(DB_Name,node.index_name,&node,0);
	} catch (...) {
		throw;
	}
}

//�ô��ڻ���ڵ���������ѡ�����          i:������index
bool Select_With_Greater_Cond(CString DB_Name,condition_info conds,int op,set<int> &addr,int i) {
	try {
		index_info* index = new index_info();
		index->index_name = conds.left_index_name;
		index->type = conds.left_type;
		index->length = conds.left_len;
		index->value  = conds.const_data ;
		int start=0,end=0;
		CString record="";
		int record_addr;
		set<int> newAddr;
		search_many(DB_Name,conds.left_index_name,start,end,op,index);
		if (start == -10) {     //�޷��ϼ�¼
			cout << "No such result." << endl;
			return false;
		}
		if ( i == 0) {         // ��һ����Ϣ
			fileInfo* file = get_file_info(DB_Name,conds.left_index_name,1);      //ȡindex�ļ�
			int j = start;
			while(true) {                        //index �ļ��ṹ�洢�ṹ   ȡ��ַ��set
				blockInfo* block = get_file_block(DB_Name,conds.left_index_name,1,j);
				if(block->cBlock[0] == '!') {
					int num,k=1;
					CString snum = "";
					for (; k<=4; ++k)                  // �ڵ���Ŀ
						snum += block->cBlock[k] ;
					num = _ttoi(snum);
					int len =num*(INDEX_OFFSET+conds.left_len)+4+1;
					if (j == start) k=index->offset;
					for ( ; k< len; k+=INDEX_OFFSET+conds.left_len) {
						record=""; 
						char ch;
						int base_len = k;           //addr ��ʼλ��
						int off_len = 0;  
						ch = block->cBlock[base_len];
						while(off_len < INDEX_OFFSET && ch != ';') {
							record += ch;
							++off_len;
							if((base_len+off_len)>=BLOCK_LEN) break;
							ch =  block->cBlock[base_len+off_len];
						}
						record_addr = _ttoi(record);
						addr.insert(record_addr);
					}
					snum = "";
					if (block->cBlock[k]== '#') {
						block->lock = false;
						break;
					} else {
						snum =  snum + block->cBlock[k]  + block->cBlock[k+1] + block->cBlock[k+2];
						j = _ttoi(snum);
						block->lock = false;
					}
				} else break;
			}	
		} else {               //��֤ȡ���ĵ�ַ�����set���У����棬û�У�����  Ҫ������
			fileInfo* file = get_file_info(DB_Name,conds.left_index_name,1);      //ȡindex�ļ�
			int j=start;
			while(true) {                        //index �ļ��ṹ�洢�ṹ   ȡ��ַ��set
				blockInfo* block = get_file_block(DB_Name,conds.left_index_name,1,j);
				if(block->cBlock[0] == '!') {
					int num,k=1;
					CString snum = "";
					for (; k<=4; ++k)                  // �ڵ���Ŀ
						snum += block->cBlock[k] ;
					num = _ttoi(snum);
					int len =num*(INDEX_OFFSET+conds.left_len)+4+1;
					if (j == start) k=index->offset;
					for ( ; k< len; k+=INDEX_OFFSET+conds.left_len) {
						record=""; 
						char ch;
						int base_len = k;           //addr ��ʼλ��
						int off_len = 0;  
						ch = block->cBlock[base_len];
						while(off_len < INDEX_OFFSET && ch != ';') {
							record += ch;
							++off_len;
							if((base_len+off_len)>=BLOCK_LEN) break;
							ch =  block->cBlock[base_len+off_len];
						}
						record_addr = _ttoi(record);
						set<int >::iterator it;									
						it = addr.find(record_addr);
						if(it != addr.end()) 
							newAddr.insert(record_addr);
					}
					snum = "";
					if (block->cBlock[k]== '#') {
						block->lock = false;
						
						break;
					} else {
						snum = snum + block->cBlock[k]  + block->cBlock[k+1] + block->cBlock[k+2];
						j = _ttoi(snum);
						block->lock = false;
					
					}
				} else break;
			}	
			addr.erase(addr.begin(),addr.end());
			addr = newAddr;
		}
		delete index;                //�ͷ�index
		return true;
	} catch (...) {
		throw;
	}
}

//��С�ڻ�С�ڵ���������ѡ�����
bool Select_With_Smaller_Cond(CString DB_Name,condition_info conds,int op,set<int> &addr,int i) {
	try {
		index_info* index = new index_info();
		index->index_name = conds.left_index_name;
		index->type = conds.left_type;
		index->length = conds.left_len;
		index->value  = conds.const_data ;
		int start=0,end=0;
		CString record="";
		int record_addr;
		set<int> newAddr;
		search_many(DB_Name,conds.left_index_name,start,end,op,index);
		if (start == -10) {     //�޷��ϼ�¼
			cout << "No such result." << endl;
			return false;
		}
		if ( i == 0) {
			fileInfo* file = get_file_info(DB_Name,conds.left_index_name,1);      //ȡindex�ļ�
			int j = find_left_child (DB_Name,conds.left_index_name);
			while(true) {                        //index �ļ��ṹ�洢�ṹ   ȡ��ַ��set
				blockInfo* block = get_file_block(DB_Name,conds.left_index_name,1,j);
				if(block->cBlock[0] == '!') {
					int num,k=1;
					CString snum = "";
					for (; k<=4; ++k)                  // �ڵ���Ŀ
						snum += block->cBlock[k] ;
					num = _ttoi(snum);
					int len =num*(INDEX_OFFSET+conds.left_len)+4+1;
					if (j == end) len=index->offset+1;
					for ( ; k<len; k+=INDEX_OFFSET+conds.left_len) {
						record=""; 
						char ch;
						int base_len = k;           //addr ��ʼλ��
						int off_len = 0;  
						ch = block->cBlock[base_len];
						while(off_len < INDEX_OFFSET && ch != ';') {
							record += ch;
							++off_len;
							if((base_len+off_len)>=BLOCK_LEN) break;
							ch =  block->cBlock[base_len+off_len];
						}
						record_addr = _ttoi(record);
						addr.insert(record_addr);
					}
					snum = "";
					if (j == end) {
						block->lock = false;
						break;
					} else {
						snum = snum + block->cBlock[k]  + block->cBlock[k+1] + block->cBlock[k+2];
						j = _ttoi(snum);
						block->lock = false;
					}
				} else break;
			}
		} else {               //��֤ȡ���ĵ�ַ�����set���У����棬û�У�����  Ҫ������
			fileInfo* file = get_file_info(DB_Name,conds.left_index_name,1);      //ȡindex�ļ�
			int j = find_left_child (DB_Name,conds.left_index_name);
			while(true) {                        //index �ļ��ṹ�洢�ṹ   ȡ��ַ��set
				blockInfo* block = get_file_block(DB_Name,conds.left_index_name,1,j);
				if(block->cBlock[0] == '!') {
					int num,k=1;
					CString snum = "";
					for (; k<=4; ++k)                  // �ڵ���Ŀ
						snum += block->cBlock[k] ;
					num = _ttoi(snum);
					int len =num*(INDEX_OFFSET+conds.left_len)+4+1;
					if (j == end) len=index->offset+1;
					for ( ; k<len; k+=INDEX_OFFSET+conds.left_len)  {
						record=""; 
						char ch;
						int base_len = k;           //addr ��ʼλ��
						int off_len = 0;  
						ch = block->cBlock[base_len];
						while(off_len < INDEX_OFFSET && ch != ';') {
							record += ch;
							++off_len;
							if((base_len+off_len)>=BLOCK_LEN) break;
							ch =  block->cBlock[base_len+off_len];
						}
						record_addr = _ttoi(record);
						set<int >::iterator it;								
						it = addr.find(record_addr);
						if(it != addr.end()) 
							newAddr.insert(record_addr);
					}
					snum = "";
					if (j == end) {
						block->lock = false;
						break;
					} else {
						snum = snum + block->cBlock[k]  + block->cBlock[k+1] + block->cBlock[k+2];
						j = _ttoi(snum);
						block->lock = false;
					}
				} else break;
			}
			addr.erase(addr.begin(),addr.end());
			addr = newAddr;
		}
		delete index;
		return true;
	} catch (...) {
		throw;
	}
}

//�ж��Ƿ����ĳ��ָ������
bool Confirm(CString record,int len,condition_info condition) {
	try {
		CString attr="";
		int base_len = condition.left_offset+1;
		int off_len = 0;
		char ch = record[base_len];                   // ȡ��attr
		while(off_len < condition.left_len &&  ch != ';') {
			attr +=ch;
			++ off_len;
			if (base_len + off_len >= len) break;
			ch = record[base_len+off_len];
		}
		float fleft,fright;
		int left,right;
		if(condition.left_type == 1) {            //float
			fleft = (float)_ttof(attr); 
			fright =  (float)_ttof(condition.const_data);
		} else if(condition.left_type == 0) {   //int
			left = _ttoi(attr);
			right = _ttoi(condition.const_data);
		}
		switch(condition.op) {
		case 1: 	if(condition.left_type == 1) {            //> float
						return (fleft > fright);
					} else if (condition.left_type == 0) { 
						return (left > right);
					} else {
						return (attr.Compare(condition.const_data) > 0);
					}
					break;
		case 2: if(condition.left_type == 1) {            //>= float
						return (fleft >= fright);
					} else if (condition.left_type == 0) { 
						return (left >= right);
					} else {
						return (attr.Compare(condition.const_data) >= 0);
					}
					break;
		case 3: if(condition.left_type == 1) {            //< float
						return (fleft < fright);
					} else if (condition.left_type == 0) { 
						return (left < right);
					} else {
						return (attr.Compare(condition.const_data) < 0);
					}
					break;
		case 4: if(condition.left_type == 1) {            //<= float
						return (fleft <= fright);
					} else if (condition.left_type == 0) { 
						return (left <= right);
					} else {
						return (attr.Compare(condition.const_data) <= 0);
					}
					break;
		case 5: if(condition.left_type == 1) {            //= float
						return (fleft == fright);
					} else if (condition.left_type == 0) { 
						return (left == right);
					} else {
						return (attr.Compare(condition.const_data) == 0) ;
					}
					break;
		case 6: if(condition.left_type == 1) {            //<> float
						return (fleft != fright);
					} else if (condition.left_type == 0) { 
						return (left != right);
					} else {
						return (attr.Compare(condition.const_data) != 0) ;
					}
					break;
		default: return false;
		}
	} catch (...) {
		throw;
	}
}			
