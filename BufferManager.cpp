#include "BufferManager.h"

fileInfo* fileHandle=NULL;
blockInfo* blockHandle=NULL;

int blockNum = 0;
int fileNum = 0;

/******************* 对外接口 ******************************/

//根据文件名，文件类型查找该文件是否在内存，
//如果是，根据文件的块号，从内存中查询该块，
//如该块已经在内存，返回该块的指针，
//如果该块没有在内存，判断垃圾链表中是否有空余的块，
//如有从中找到一块（链表的头），将该块的blockNUM设置为参数blockNUM，
//如果没有，判断现在内存中的块数是否已经达到了最大块数限制
//如果没有，为其分配新分配一块，并链接到对应的文件头所指的链表的结尾；
//如果已达到，使用LRU算法，找到一个替换块，按照给定的要求进行初始化，并将其链接到指定文件块链表的结尾。
//如果文件没有在内存，调用get_file_info(CString DB_Name,CString fileName, int m_fileType)来为文件分配一个头指针。
//然后从blockHandle或新分配或使用LRU算法找到一个替换块（方法与上面类似），将该块按要求初始化。
blockInfo * get_file_block(CString DB_Name,CString Table_Name,int fileType,int blockNum) {
	try {
		CString fileName = Table_Name;
		fileInfo* tmpFile = fileHandle;
		blockInfo* tmpBlock;
		blockInfo* tmp=NULL;
		while (tmpFile != NULL) {         //从内存中查询块
			if (tmpFile->fileName.Compare(fileName)==0 && tmpFile->type==fileType) {
				tmpBlock = tmpFile->firstBlock;
				while (tmpBlock != NULL) {
					if (tmpBlock->blockNum == blockNum) {
						fileInfo* files = fileHandle;           // 更新时间
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
				tmp = findBlock(DB_Name);   //该块没有在内存
				tmp->file = tmpFile;
				readBlock(DB_Name,fileName, blockNum,fileType,tmp,tmpFile);
				replace(tmpFile,tmp);
				break;
			}
			tmpFile = tmpFile->next;
		}
		if (tmp == NULL) {           //文件没有在内存
			fileInfo* newFile=get_file_info(DB_Name,fileName,fileType);
			tmp = findBlock(DB_Name);
			tmp->file = newFile;
			readBlock(DB_Name,fileName, blockNum,fileType,tmp,newFile);
			replace(newFile,tmp);
		}
		fileInfo* files = fileHandle;          //更新时间
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


//逐个关闭文件。ture:quit 释放block      false:close db
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
		if(m_flag) {                // 释放blockHandle
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


//首先查找该文件是否已经在内存中存在，如果是，返回该文件头；
// 如果不存在，判断内存中已有的文件数是否等于最大的文件数，
//如果不是，为其分配一个文件头的struct，并根据要求进行初始化，文件个数加一，调用get_tablei_info(DB_Name,fileName,fileinfo->recordAmount,fileinfo->recordLength,fileinfo->freeNum)或get_index_info(DB_Name,fileName,fileinfo->recordAmount,fileinfo->recordLength,fileinfo->freeNum)对给文件头进行初始化，并返回该文件头；
//如果不是，选取文件头链表的第一个文件头，作为替换项，首先对选定的文件进行关闭文件操作，使得内存中的文件数目减少一个，进而为可以重新申请一个文件头，调用get_tablei_info(DB_Name,fileName,fileinfo->recordAmount,fileinfo->recordLength,fileinfo->freeNum)或get_index_info(DB_Name,fileName,fileinfo->recordAmount,fileinfo->recordLength,fileinfo->freeNum)对给文件头进行初始化，返回该文件头。
fileInfo* get_file_info(CString DB_Name,CString fileName, int m_fileType) {
	try {
		fileInfo* tmpFile=fileHandle;
		while(tmpFile!=NULL) {
			if((tmpFile->fileName.Compare (fileName)== 0) && tmpFile->type ==m_fileType) {
				fileInfo* files = fileHandle;          //更新时间
				while ( files != NULL) {
					files->iTime++;
					files= files->next;
				}
				tmpFile->iTime = 0;
				return tmpFile;
			}
			tmpFile = tmpFile->next;
		}
		if (fileNum < MAX_FILE_ACTIVE) {          //新建文件
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
			fileInfo* files = fileHandle;          //更新时间
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
			fileInfo* files = fileHandle;          //更新时间
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


//根据文件名和文件类型，查找内存是否有这个文件，如果没有函数结束
//如果找到，文件数减1，对于块链表中的每个块，而且dirty位为1，调用writeBlock（DB_Name，块指针），将该块的内容写回磁盘，如果dirty位为0，不作处理。然后把真个块链表加到垃圾链表中。释放文件头的struct 。
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


/************************ 对内函数 **************************/

//DB_Name为数据库名称。函数实现从现已分配的内存单元中获取用于替换的内存块。
//首先判断垃圾链表blockHandle中是否有可用的内存块，如果有从链表的头取一块返回。
//如果没有，判断现在内存中的块数是否已经达到了最大块数限制
//如果没有，为其分配新分配一块，并链接到对应的文件头所指的链表的结尾；
//如没有可用块，使用LRU算法找到没有使用的时间最长的块（itime最大），从其现在位置取出，itime置零；并将该块返回。
blockInfo* findBlock(CString DB_Name ) {
	try {
		blockInfo* tmp=NULL;
		if (blockHandle != NULL) {           //垃圾链表blockHandle中有可用的内存块
			tmp = blockHandle;
			blockHandle = blockHandle->next;
		} else {		
			if ( blockNum < MAX_BLOCK) {     //判断现在内存中的块数是否已经达到了最大块数限制
				tmp = new blockInfo();
				++blockNum;
			} else {
				fileInfo* fileTmp=fileHandle;
				blockInfo* tmpBlock;
				fileInfo* anotherFile=fileHandle;
			    long time = -1;   //获取现在的时间
				while(fileTmp != NULL) {                        //LRU算法
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
				if(tmp && !tmp->lock && tmp->dirtyBit) writeBlock(DB_Name,tmp);   // block已更改，写回
				tmpBlock = anotherFile->firstBlock;          //把要置换的block从文件中删去
				if (tmpBlock == tmp) {
					anotherFile->firstBlock = tmp->next;
				} else {
					while(tmpBlock->next != tmp) 
						tmpBlock = tmpBlock->next;
					tmpBlock->next = tmp->next;
				}		
			}
		}
		memset(tmp->cBlock,'n',sizeof(char)*BLOCK_LEN);  //清除 block数据
		fileInfo* files = fileHandle;          //更新时间
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

//实现把指针m_blockInfo 所指向的块连到文件头指针m_fileInfo所指向的块链表的结尾，同时将m_blockInfo所指向的块的file指针指向m_fileInfo。
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

//从磁盘中读取该块的内容到内存，返回该块。
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
			if (length<= (m_blockNum*len)) {        // 重新申请一个新块
				block->charNum = BLOCK_LEN;
				memset(block->cBlock,'n',sizeof(char)*BLOCK_LEN);
				return;
			}
			if (length < (m_blockNum+1)*len) {
				fin.seekp(m_blockNum*len, fin.beg);    //防止被截断数据
				fin.read(block->cBlock, length -m_blockNum*len+1);
			} else {
				fin.seekp(m_blockNum*len, fin.beg);    //防止被截断数据
				fin.read(block->cBlock, len);
			}
			fin.close();
			int size=0;            // 设置空闲字节数
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
			if (length<= (m_blockNum*BLOCK_LEN)) {        // 重新申请一个新块
				block->charNum = BLOCK_LEN;
				memset(block->cBlock,'n',sizeof(char)*BLOCK_LEN);
				return;
			} else {
				fin.seekp(m_blockNum*BLOCK_LEN, fin.beg);    //防止被截断数据
				fin.read(block->cBlock, BLOCK_LEN);
			}
			fin.close();
		}
	} catch (...) {
		throw;
	}
}

//把block指针所指向的块的内容写回磁盘。
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


//添加块到垃圾块链表
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

//读取表文件信息。
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

// 读取索引文件信息。
fileInfo* Get_Index_Info(CString DB_Name,CString Index_Name) {
	try {
		fileInfo* newFile = new fileInfo();
		CString	path2= DB_Name+"/"+Index_Name+".idx";
		CFile file(path2,CFile::modeRead);
		newFile->freeNum = 0;
		newFile->recordAmount = (int)ceil(file.GetLength() / BLOCK_LEN);        //块个数
		newFile->recordLength=BLOCK_LEN;
		file.Close();
		return newFile;
	} catch (...) {
		throw;
	}
}


