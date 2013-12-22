#include "BufferManager.h"

fileInfo* fileHandle=NULL;
blockInfo* blockHandle=NULL;

int blockNum = 0;
int fileNum = 0;

/******************* ����ӿ� ******************************/

//�����ļ������ļ����Ͳ��Ҹ��ļ��Ƿ����ڴ棬
//����ǣ������ļ��Ŀ�ţ����ڴ��в�ѯ�ÿ飬
//��ÿ��Ѿ����ڴ棬���ظÿ��ָ�룬
//����ÿ�û�����ڴ棬�ж������������Ƿ��п���Ŀ飬
//���д����ҵ�һ�飨�����ͷ�������ÿ��blockNUM����Ϊ����blockNUM��
//���û�У��ж������ڴ��еĿ����Ƿ��Ѿ��ﵽ������������
//���û�У�Ϊ������·���һ�飬�����ӵ���Ӧ���ļ�ͷ��ָ������Ľ�β��
//����Ѵﵽ��ʹ��LRU�㷨���ҵ�һ���滻�飬���ո�����Ҫ����г�ʼ�������������ӵ�ָ���ļ�������Ľ�β��
//����ļ�û�����ڴ棬����get_file_info(CString DB_Name,CString fileName, int m_fileType)��Ϊ�ļ�����һ��ͷָ�롣
//Ȼ���blockHandle���·����ʹ��LRU�㷨�ҵ�һ���滻�飨�������������ƣ������ÿ鰴Ҫ���ʼ����
blockInfo * get_file_block(CString DB_Name,CString Table_Name,int fileType,int blockNum) {
	try {
		CString fileName = Table_Name;
		fileInfo* tmpFile = fileHandle;
		blockInfo* tmpBlock;
		blockInfo* tmp=NULL;
		while (tmpFile != NULL) {         //���ڴ��в�ѯ��
			if (tmpFile->fileName.Compare(fileName)==0 && tmpFile->type==fileType) {
				tmpBlock = tmpFile->firstBlock;
				while (tmpBlock != NULL) {
					if (tmpBlock->blockNum == blockNum) {
						fileInfo* files = fileHandle;           // ����ʱ��
						while ( files != NULL) {
							blockInfo* block = files->firstBlock;
							while(block!=NULL) {
								block->iTime++;
								block = block->next;
							}
							files = files->next;
						}
						tmpBlock->iTime = 0;
						return  tmpBlock;
					}
					tmpBlock = tmpBlock->next;
				}		                 
				tmp = findBlock(DB_Name);   //�ÿ�û�����ڴ�
				tmp->file = tmpFile;
				readBlock(DB_Name,fileName, blockNum,fileType,tmp,tmpFile);
				replace(tmpFile,tmp);
				break;
			}
			tmpFile = tmpFile->next;
		}
		if (tmp == NULL) {           //�ļ�û�����ڴ�
			fileInfo* newFile=get_file_info(DB_Name,fileName,fileType);
			tmp = findBlock(DB_Name);
			tmp->file = newFile;
			readBlock(DB_Name,fileName, blockNum,fileType,tmp,newFile);
			replace(newFile,tmp);
		}
		fileInfo* files = fileHandle;          //����ʱ��
		while ( files != NULL) {
			blockInfo* block = files->firstBlock;
			while(block!=NULL) {
				block->iTime++;
				block = block->next;
			}
			files = files->next;
		}
		tmp->iTime = 0;
	//	tmp->lock = true;     // pin block
		tmp->blockNum =blockNum;
		return tmp;
	} catch (...) {
		throw;
	}
}


//����ر��ļ���ture:quit �ͷ�block      false:close db
void closeDatabase(CString DB_Name,bool m_flag) {
	try {
		fileInfo* tmpFile;
		while(fileHandle != NULL) {
			tmpFile = fileHandle;
			fileHandle = fileHandle->next;
			--fileNum;
			blockInfo* tmpBlock = tmpFile->firstBlock;
			while(tmpBlock != NULL) {
				if (!tmpBlock->lock && tmpBlock->dirtyBit) 
					writeBlock(DB_Name,tmpBlock);
				blockInfo* tmp = tmpBlock;
				tmpBlock = tmpBlock->next;
				if (m_flag) {		
					--blockNum;
					free(tmp);
				}
			}
			 if(!m_flag) addToBlockHandle(tmpFile->firstBlock);
			free(tmpFile);
		}
		if(m_flag) {                // �ͷ�blockHandle
			blockInfo* tmpBlock;
			while(blockHandle != NULL) {
				tmpBlock = blockHandle;
				blockHandle = blockHandle->next;
				free(tmpBlock);
			}
		 }
	} catch (...) {
		throw;
	}
}


//���Ȳ��Ҹ��ļ��Ƿ��Ѿ����ڴ��д��ڣ�����ǣ����ظ��ļ�ͷ��
// ��������ڣ��ж��ڴ������е��ļ����Ƿ���������ļ�����
//������ǣ�Ϊ�����һ���ļ�ͷ��struct��������Ҫ����г�ʼ�����ļ�������һ������get_tablei_info(DB_Name,fileName,fileinfo->recordAmount,fileinfo->recordLength,fileinfo->freeNum)��get_index_info(DB_Name,fileName,fileinfo->recordAmount,fileinfo->recordLength,fileinfo->freeNum)�Ը��ļ�ͷ���г�ʼ���������ظ��ļ�ͷ��
//������ǣ�ѡȡ�ļ�ͷ����ĵ�һ���ļ�ͷ����Ϊ�滻����ȶ�ѡ�����ļ����йر��ļ�������ʹ���ڴ��е��ļ���Ŀ����һ��������Ϊ������������һ���ļ�ͷ������get_tablei_info(DB_Name,fileName,fileinfo->recordAmount,fileinfo->recordLength,fileinfo->freeNum)��get_index_info(DB_Name,fileName,fileinfo->recordAmount,fileinfo->recordLength,fileinfo->freeNum)�Ը��ļ�ͷ���г�ʼ�������ظ��ļ�ͷ��
fileInfo* get_file_info(CString DB_Name,CString fileName, int m_fileType) {
	try {
		fileInfo* tmpFile=fileHandle;
		while(tmpFile!=NULL) {
			if((tmpFile->fileName.Compare (fileName)== 0) && tmpFile->type ==m_fileType) {
				fileInfo* files = fileHandle;          //����ʱ��
				while ( files != NULL) {
					files->iTime++;
					files= files->next;
				}
				tmpFile->iTime = 0;
				return tmpFile;
			}
			tmpFile = tmpFile->next;
		}
		if (fileNum < MAX_FILE_ACTIVE) {          //�½��ļ�
			fileInfo* newFile = NULL;
			++fileNum;
			if (m_fileType == 0)
				newFile = Get_Table_Info(DB_Name,fileName);
			else 
				newFile = Get_Index_Info(DB_Name,fileName);
			newFile->fileName=fileName;
			newFile->type=m_fileType;
			newFile->next=fileHandle;
			fileHandle=newFile;
			newFile->firstBlock =NULL;
			fileInfo* files = fileHandle;          //����ʱ��
			while ( files != NULL) {
				files->iTime++;
				files= files->next;
			}
			newFile->iTime = 0;
			return newFile;
		} else {
			tmpFile=fileHandle;
			fileInfo* newFile = NULL;
			long time=-1;
			while(tmpFile != NULL) {                   // LRU file
				 if (tmpFile ->iTime > time) {
					 newFile = tmpFile;
					 time = tmpFile->iTime;
				 }
				 tmpFile = tmpFile->next;
			}
			closeFile(DB_Name,newFile->fileName,newFile->type);
			if (m_fileType == 0)
				newFile = Get_Table_Info(DB_Name,fileName);
			else 
				newFile =Get_Index_Info(DB_Name,fileName);
			newFile->fileName=fileName;
			newFile->type=m_fileType;
			newFile->next=fileHandle;
			fileHandle=newFile;
			newFile->firstBlock =NULL;
			fileInfo* files = fileHandle;          //����ʱ��
			while ( files != NULL) {
				files->iTime++;
				files= files->next;
			}
			newFile->iTime = 0;
			return newFile;
		}
	} catch (...) {
		throw;
	}
}


//�����ļ������ļ����ͣ������ڴ��Ƿ�������ļ������û�к�������
//����ҵ����ļ�����1�����ڿ������е�ÿ���飬����dirtyλΪ1������writeBlock��DB_Name����ָ�룩�����ÿ������д�ش��̣����dirtyλΪ0����������Ȼ������������ӵ����������С��ͷ��ļ�ͷ��struct ��
void closeFile(CString DB_Name,CString m_fileName,int m_fileType) {
	try {
		fileInfo* tmpFile=fileHandle;
		fileInfo* prevFile=NULL;
		while(tmpFile != NULL) {
			if ((tmpFile->fileName.Compare(m_fileName) == 0) && tmpFile->type == m_fileType) {
				--fileNum;
				blockInfo* tmpBlock = tmpFile->firstBlock;
				while(tmpBlock != NULL) {
					if (!tmpBlock->lock  && tmpBlock->dirtyBit) writeBlock(DB_Name,tmpBlock);
					tmpBlock = tmpBlock->next;
				}
				addToBlockHandle(tmpFile->firstBlock);
				if (prevFile == NULL) {
					fileHandle = tmpFile->next;
				} else {
					prevFile->next = tmpFile->next;
				}
				free(tmpFile);
				break;
			}
			prevFile = tmpFile;
			tmpFile = tmpFile->next;
		}
	} catch (...) {
		throw;
	}
}

void writeFile(CString DB_Name,CString index_name,int m_fileType) {
	fileInfo* tmpFile = get_file_info(DB_Name,index_name,m_fileType);
	blockInfo* tmpBlock = tmpFile->firstBlock;
	while(tmpBlock != NULL) {
		if (!tmpBlock->lock  && tmpBlock->dirtyBit) writeBlock(DB_Name,tmpBlock);
		tmpBlock = tmpBlock->next;
	}
}


/************************ ���ں��� **************************/

//DB_NameΪ���ݿ����ơ�����ʵ�ִ����ѷ�����ڴ浥Ԫ�л�ȡ�����滻���ڴ�顣
//�����ж���������blockHandle���Ƿ��п��õ��ڴ�飬����д������ͷȡһ�鷵�ء�
//���û�У��ж������ڴ��еĿ����Ƿ��Ѿ��ﵽ������������
//���û�У�Ϊ������·���һ�飬�����ӵ���Ӧ���ļ�ͷ��ָ������Ľ�β��
//��û�п��ÿ飬ʹ��LRU�㷨�ҵ�û��ʹ�õ�ʱ����Ŀ飨itime��󣩣���������λ��ȡ����itime���㣻�����ÿ鷵�ء�
blockInfo* findBlock(CString DB_Name ) {
	try {
		blockInfo* tmp=NULL;
		if (blockHandle != NULL) {           //��������blockHandle���п��õ��ڴ��
			tmp = blockHandle;
			blockHandle = blockHandle->next;
		} else {		
			if ( blockNum < MAX_BLOCK) {     //�ж������ڴ��еĿ����Ƿ��Ѿ��ﵽ������������
				tmp = new blockInfo();
				++blockNum;
			} else {
				fileInfo* fileTmp=fileHandle;
				blockInfo* tmpBlock;
				fileInfo* anotherFile=fileHandle;
			    long time = -1;   //��ȡ���ڵ�ʱ��
				while(fileTmp != NULL) {                        //LRU�㷨
					tmpBlock= fileTmp->firstBlock;
					while(tmpBlock != NULL) { 
						if (!tmpBlock->lock && tmpBlock->iTime > time) {
							tmp = tmpBlock;
							anotherFile = fileTmp;
							time = tmpBlock->iTime;
						}
						tmpBlock = tmpBlock->next;
					}
					fileTmp = fileTmp->next;
				}
				if(tmp && !tmp->lock && tmp->dirtyBit) writeBlock(DB_Name,tmp);   // block�Ѹ��ģ�д��
				tmpBlock = anotherFile->firstBlock;          //��Ҫ�û���block���ļ���ɾȥ
				if (tmpBlock == tmp) {
					anotherFile->firstBlock = tmp->next;
				} else {
					while(tmpBlock->next != tmp) 
						tmpBlock = tmpBlock->next;
					tmpBlock->next = tmp->next;
				}		
			}
		}
		memset(tmp->cBlock,'n',sizeof(char)*BLOCK_LEN);  //��� block����
		fileInfo* files = fileHandle;          //����ʱ��
		while ( files != NULL) {
			blockInfo* block = files->firstBlock;
			while(block!=NULL) {
				block->iTime++;
				block = block->next;
			}
			files = files->next;
		}
		tmp->iTime = 0;
		tmp->charNum = BLOCK_LEN;
		tmp->dirtyBit = false;
		tmp->lock = 0;
		tmp->next = NULL;
		return tmp;
	} catch (...) {
		throw;
	}
}

//ʵ�ְ�ָ��m_blockInfo ��ָ��Ŀ������ļ�ͷָ��m_fileInfo��ָ��Ŀ�����Ľ�β��ͬʱ��m_blockInfo��ָ��Ŀ��fileָ��ָ��m_fileInfo��
void replace(fileInfo * m_fileInfo, blockInfo * m_blockInfo) {
	try {
		blockInfo* tmp = m_fileInfo->firstBlock;
		m_blockInfo->file = m_fileInfo;
		if (tmp == NULL) m_fileInfo->firstBlock = m_blockInfo;
		else {
			while(tmp->next != NULL) {
				tmp = tmp->next;
			}
		   tmp->next = m_blockInfo;
		}
	} catch (...) {
		throw;
	}
}

//�Ӵ����ж�ȡ�ÿ�����ݵ��ڴ棬���ظÿ顣
void readBlock(CString DB_Name,CString fileName, int m_blockNum, int fileType,blockInfo* &block,fileInfo* tmpFile) {
	try {
		CString path;
		block->blockNum = m_blockNum;
		if (fileType==0) {
			path= DB_Name+"/"+fileName+".db";
			fstream fin(path,ios::in|ios::out);
			fin.seekg (0, fin.end);
			int length = fin.tellg();
			int len = (int)floor(BLOCK_LEN/tmpFile->recordLength)*tmpFile->recordLength;
			if (length<= (m_blockNum*len)) {        // ��������һ���¿�
				block->charNum = BLOCK_LEN;
				memset(block->cBlock,'n',sizeof(char)*BLOCK_LEN);
				return;
			}
			if (length < (m_blockNum+1)*len) {
				fin.seekp(m_blockNum*len, fin.beg);    //��ֹ���ض�����
				fin.read(block->cBlock, length -m_blockNum*len+1);
			} else {
				fin.seekp(m_blockNum*len, fin.beg);    //��ֹ���ض�����
				fin.read(block->cBlock, len);
			}
			fin.close();
			int size=0;            // ���ÿ����ֽ���
			for (int i=0; i<len; i+=len) {
				if (block->cBlock[i] == 'u') {
					size += tmpFile->recordLength;
				}
			}
			block->charNum = BLOCK_LEN-size;
		}
		else  {
			path= DB_Name+"/"+fileName+".idx";
			fstream fin(path,ios::in|ios::out);
			fin.seekg (0, fin.end);
			int length = fin.tellg();
			if (length<= (m_blockNum*BLOCK_LEN)) {        // ��������һ���¿�
				block->charNum = BLOCK_LEN;
				memset(block->cBlock,'n',sizeof(char)*BLOCK_LEN);
				return;
			} else {
				fin.seekp(m_blockNum*BLOCK_LEN, fin.beg);    //��ֹ���ض�����
				fin.read(block->cBlock, BLOCK_LEN);
			}
			fin.close();
		}
	} catch (...) {
		throw;
	}
}

//��blockָ����ָ��Ŀ������д�ش��̡�
void writeBlock(CString DB_Name,blockInfo * block) {
	try {
		fileInfo* tmpFile=block->file;
		CString path;
		if (tmpFile->type==0) {
			path= DB_Name+"/"+tmpFile->fileName+".db";
			fstream fout(path,ios::in|ios::out);
			fout.seekp(block->blockNum*floor(BLOCK_LEN/tmpFile->recordLength)*tmpFile->recordLength, fout.beg);
			fout.write(block->cBlock, floor(BLOCK_LEN/tmpFile->recordLength)*tmpFile->recordLength);
			fout.close();
		} else  {
			path= DB_Name+"/"+tmpFile->fileName+".idx";
			fstream fout(path,ios::in|ios::out);
			fout.seekp(block->blockNum*BLOCK_LEN, fout.beg);
			fout.write(block->cBlock, BLOCK_LEN);
			fout.close();
		}
		
	} catch (...) {
		throw;
	}
}


//��ӿ鵽����������
void addToBlockHandle(blockInfo* block) {
	try {
		if (block != NULL) {
			blockInfo* tmpBlock=blockHandle;
			blockHandle = block;
			while (block->next != NULL) 
				block = block->next;
			block->next = tmpBlock;
		}
	} catch (...) {
		throw;
	}
}

//��ȡ���ļ���Ϣ��
fileInfo* Get_Table_Info(CString DB_Name,CString Table_Name) {
	try {
		fileInfo* newFile = new fileInfo();
		CString	path1= DB_Name+"/head.all";
		CString	path2= DB_Name+"/"+Table_Name+".db";
		ifstream fin(path1);
		CString name;
		string record;
		CString len;	
		newFile->recordLength = 0;
		while(getline(fin,record)) {
			name = "";
			int i=0;
			while (record[i] != ' ') {
				name+=record[i];
				++i;
			}
			if (name.Compare(Table_Name) == 0) {
				len = "";
				++i;
				while (i<record.length() && record[i] != ' ' && record[i] != '\n') {
					len+=record[i];
					++i;
				}
				newFile->recordLength = _ttoi(len);
				break;
			}
		}
		fin.close();
		CFile file(path2,CFile::modeRead);
		if (newFile->recordLength != 0) {
			newFile->freeNum = (int)ceil(file.GetLength() / (floor(BLOCK_LEN/newFile->recordLength)*newFile->recordLength));
		} else {
			newFile->freeNum = 0;
		}
		file.Close();
		return newFile;
	} catch (...) {
		throw;
	}
}

// ��ȡ�����ļ���Ϣ��
fileInfo* Get_Index_Info(CString DB_Name,CString Index_Name) {
	try {
		fileInfo* newFile = new fileInfo();
		CString	path2= DB_Name+"/"+Index_Name+".idx";
		CFile file(path2,CFile::modeRead);
		newFile->freeNum = 0;
		newFile->recordAmount = (int)ceil(file.GetLength() / BLOCK_LEN);        //�����
		newFile->recordLength=BLOCK_LEN;
		file.Close();
		return newFile;
	} catch (...) {
		throw;
	}
}


