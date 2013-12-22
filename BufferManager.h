/////////////////////////////////////////////////////////////
///------------------------------------------------------ ///
///       Module: BufferManager                                      ///
///       Produced by:Zhang Xuewen                               ///
///       date: 2013/10/29                                             ///
///----------------------------------------------------///
/////////////////////////////////////////////////////////

#if !defined(_BUFFERMANAGER_H_)
#define _BUFFERMANAGER_H_

#include"MiniSQL.h"
#include <fstream>
#include <map>
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

#define BLOCK_LEN			4096	// the size of one block
#define MAX_FILE_ACTIVE 	5		// limit the active files in the buffer
#define MAX_BLOCK			80	// the max number of the blocks


struct blockInfo;


//文件结构体(struct fileInfo)
//包含文件的基本信息;类型（数据文件或索引文件）、文件名、记录数、空块号、记录长度、指向下一个文件的指针、文件所指向的第一个块。
struct fileInfo  {
	int type;				     // 0-> data file， 1 -> index file
	CString fileName;		// the name of the file
	int freeNum;			    // the block number a file needs
	int recordLength;		// the length of the record in the file      or n 个节点
	fileInfo *next;			// the pointer points to the next file
	blockInfo* firstBlock;	// point to the first block within the file
	int recordAmount;
	long iTime;			// it indicate the age of the file in use 
};	
	
//块信息结构体(struct blockInfo)
//包含块的基本信息;块号、脏位、指向下一个块的指针、块中的字符数、存放信息的字符型数组、年龄（用于LRU算法）、锁。
struct blockInfo  {
	int blockNum;	         // the block number of the block,
	bool dirtyBit;				// 0 -> flase， 1 -> indicate dirty, write back
	blockInfo *next;  		// the pointer point to next block 
	fileInfo *file;				// the pointer point to the file, which the block belongs to
	int charNum;	     	// the number of chars not used in the block
	char cBlock[BLOCK_LEN];	   	// the array space for storing the records in the block in buffer
	long iTime;			// it indicate the age of the block in use 
	bool lock;			// prevent the block from replacing
};



/************************* 对外接口 *******************************/
blockInfo * get_file_block(CString DB_Name,CString Table_Name,int fileType,int blockNum);
//根据文件名，文件类型查找该文件是否在内存，
//如果是，根据文件的块号，从内存中查询该块，
//如该块已经在内存，返回该块的指针，
//如果该块没有在内存，判断垃圾链表中是否有空余的块，
//如有从中找到一块（链表的头），将该块的blockNUM设置为参数blockNUM，
//如果没有，判断现在内存中的块数是否已经达到了最大块数限制
//如果没有，为其分配新分配一块，并链接到对应的文件头所指的链表的结尾；
//如果已达到，使用LRU算法，找到一个替换块，按照给定的要求进行初始化，并将其链接到指定文件块链表的结尾。
//如果文件没有在内存，调用get_file_info(CString DB_Name,CString fileName, int m_fileType)来为文件分配一个头指针。然后从blockHandle或新分配或使用LRU算法找到一个替换块（方法与上面类似），将该块按要求初始化。


void closeDatabase(CString DB_Name,bool m_flag);
//调用closeFile(DB_Name,filename,fileType,m_flag),逐个关闭文件。


fileInfo* get_file_info(CString DB_Name,CString fileName,int m_fileType);
//首先查找该文件是否已经在内存中存在，如果是，返回该文件头；
// 如果不存在，判断内存中已有的文件数是否等于最大的文件数，
//如果不是，为其分配一个文件头的struct，并根据要求进行初始化，文件个数加一，调用get_tablei_info(DB_Name,fileName,fileinfo->recordAmount,fileinfo->recordLength,fileinfo->freeNum)或get_index_info(DB_Name,fileName,fileinfo->recordAmount,fileinfo->recordLength,fileinfo->freeNum)对给文件头进行初始化，并返回该文件头；
//如果不是，选取文件头链表的第一个文件头，作为替换项，首先对选定的文件进行关闭文件操作，使得内存中的文件数目减少一个，进而为可以重新申请一个文件头，调用get_tablei_info(DB_Name,fileName,fileinfo->recordAmount,fileinfo->recordLength,fileinfo->freeNum)或get_index_info(DB_Name,fileName,fileinfo->recordAmount,fileinfo->recordLength,fileinfo->freeNum)对给文件头进行初始化，返回该文件头。

void closeFile(CString DB_Name,CString m_fileName,int m_fileType);
//根据文件名和文件类型，查找内存是否有这个文件，如果没有函数结束
//如果找到，文件数减1，对于块链表中的每个块，而且dirty位为1，调用writeBlock（DB_Name，块指针），将该块的内容写回磁盘，如果dirty位为0，不作处理。然后把真个块链表加到垃圾链表中。释放文件头的struct 。


void writeFile(CString DB_Name,CString index_name,int m_fileType);

/************************* 对内函数 *******************************/

blockInfo*	findBlock(CString DB_Name );  //DB_Name为数据库名称。函数实现从现已分配的内存单元中获取用于替换的内存块。
//首先判断垃圾链表blockHandle中是否有可用的内存块，如果有从链表的头取一块返回。
//如没有可用块，使用LRU算法找到没有使用的时间最长的块（itime最大），从其现在位置取出，itime置零；并将该块返回。

void replace(fileInfo * m_fileInfo, blockInfo * m_blockInfo);
//实现把指针m_blockInfo 所指向的块连到文件头指针m_fileInfo所指向的块链表的结尾，同时将m_blockInfo所指向的块的file指针指向m_fileInfo。


void writeBlock(CString DB_Name,blockInfo * block);
//把block指针所指向的块的内容写回磁盘。

void readBlock(CString DB_Name,CString fileName, int m_blockNum, int fileType,blockInfo* &block,fileInfo* tmpFile);
//从磁盘中读取该块的内容到内存，返回该块。

fileInfo* Get_Table_Info(CString DB_Name,CString Table_Name);
//读取表文件信息。

fileInfo* Get_Index_Info(CString DB_Name,CString Index_Name);
// 读取索引文件信息。

//添加块到垃圾链表
void addToBlockHandle(blockInfo* block);


#endif