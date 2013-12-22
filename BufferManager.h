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


//�ļ��ṹ��(struct fileInfo)
//�����ļ��Ļ�����Ϣ;���ͣ������ļ��������ļ������ļ�������¼�����տ�š���¼���ȡ�ָ����һ���ļ���ָ�롢�ļ���ָ��ĵ�һ���顣
struct fileInfo  {
	int type;				     // 0-> data file�� 1 -> index file
	CString fileName;		// the name of the file
	int freeNum;			    // the block number a file needs
	int recordLength;		// the length of the record in the file      or n ���ڵ�
	fileInfo *next;			// the pointer points to the next file
	blockInfo* firstBlock;	// point to the first block within the file
	int recordAmount;
	long iTime;			// it indicate the age of the file in use 
};	
	
//����Ϣ�ṹ��(struct blockInfo)
//������Ļ�����Ϣ;��š���λ��ָ����һ�����ָ�롢���е��ַ����������Ϣ���ַ������顢���䣨����LRU�㷨��������
struct blockInfo  {
	int blockNum;	         // the block number of the block,
	bool dirtyBit;				// 0 -> flase�� 1 -> indicate dirty, write back
	blockInfo *next;  		// the pointer point to next block 
	fileInfo *file;				// the pointer point to the file, which the block belongs to
	int charNum;	     	// the number of chars not used in the block
	char cBlock[BLOCK_LEN];	   	// the array space for storing the records in the block in buffer
	long iTime;			// it indicate the age of the block in use 
	bool lock;			// prevent the block from replacing
};



/************************* ����ӿ� *******************************/
blockInfo * get_file_block(CString DB_Name,CString Table_Name,int fileType,int blockNum);
//�����ļ������ļ����Ͳ��Ҹ��ļ��Ƿ����ڴ棬
//����ǣ������ļ��Ŀ�ţ����ڴ��в�ѯ�ÿ飬
//��ÿ��Ѿ����ڴ棬���ظÿ��ָ�룬
//����ÿ�û�����ڴ棬�ж������������Ƿ��п���Ŀ飬
//���д����ҵ�һ�飨�����ͷ�������ÿ��blockNUM����Ϊ����blockNUM��
//���û�У��ж������ڴ��еĿ����Ƿ��Ѿ��ﵽ������������
//���û�У�Ϊ������·���һ�飬�����ӵ���Ӧ���ļ�ͷ��ָ������Ľ�β��
//����Ѵﵽ��ʹ��LRU�㷨���ҵ�һ���滻�飬���ո�����Ҫ����г�ʼ�������������ӵ�ָ���ļ�������Ľ�β��
//����ļ�û�����ڴ棬����get_file_info(CString DB_Name,CString fileName, int m_fileType)��Ϊ�ļ�����һ��ͷָ�롣Ȼ���blockHandle���·����ʹ��LRU�㷨�ҵ�һ���滻�飨�������������ƣ������ÿ鰴Ҫ���ʼ����


void closeDatabase(CString DB_Name,bool m_flag);
//����closeFile(DB_Name,filename,fileType,m_flag),����ر��ļ���


fileInfo* get_file_info(CString DB_Name,CString fileName,int m_fileType);
//���Ȳ��Ҹ��ļ��Ƿ��Ѿ����ڴ��д��ڣ�����ǣ����ظ��ļ�ͷ��
// ��������ڣ��ж��ڴ������е��ļ����Ƿ���������ļ�����
//������ǣ�Ϊ�����һ���ļ�ͷ��struct��������Ҫ����г�ʼ�����ļ�������һ������get_tablei_info(DB_Name,fileName,fileinfo->recordAmount,fileinfo->recordLength,fileinfo->freeNum)��get_index_info(DB_Name,fileName,fileinfo->recordAmount,fileinfo->recordLength,fileinfo->freeNum)�Ը��ļ�ͷ���г�ʼ���������ظ��ļ�ͷ��
//������ǣ�ѡȡ�ļ�ͷ����ĵ�һ���ļ�ͷ����Ϊ�滻����ȶ�ѡ�����ļ����йر��ļ�������ʹ���ڴ��е��ļ���Ŀ����һ��������Ϊ������������һ���ļ�ͷ������get_tablei_info(DB_Name,fileName,fileinfo->recordAmount,fileinfo->recordLength,fileinfo->freeNum)��get_index_info(DB_Name,fileName,fileinfo->recordAmount,fileinfo->recordLength,fileinfo->freeNum)�Ը��ļ�ͷ���г�ʼ�������ظ��ļ�ͷ��

void closeFile(CString DB_Name,CString m_fileName,int m_fileType);
//�����ļ������ļ����ͣ������ڴ��Ƿ�������ļ������û�к�������
//����ҵ����ļ�����1�����ڿ������е�ÿ���飬����dirtyλΪ1������writeBlock��DB_Name����ָ�룩�����ÿ������д�ش��̣����dirtyλΪ0����������Ȼ������������ӵ����������С��ͷ��ļ�ͷ��struct ��


void writeFile(CString DB_Name,CString index_name,int m_fileType);

/************************* ���ں��� *******************************/

blockInfo*	findBlock(CString DB_Name );  //DB_NameΪ���ݿ����ơ�����ʵ�ִ����ѷ�����ڴ浥Ԫ�л�ȡ�����滻���ڴ�顣
//�����ж���������blockHandle���Ƿ��п��õ��ڴ�飬����д������ͷȡһ�鷵�ء�
//��û�п��ÿ飬ʹ��LRU�㷨�ҵ�û��ʹ�õ�ʱ����Ŀ飨itime��󣩣���������λ��ȡ����itime���㣻�����ÿ鷵�ء�

void replace(fileInfo * m_fileInfo, blockInfo * m_blockInfo);
//ʵ�ְ�ָ��m_blockInfo ��ָ��Ŀ������ļ�ͷָ��m_fileInfo��ָ��Ŀ�����Ľ�β��ͬʱ��m_blockInfo��ָ��Ŀ��fileָ��ָ��m_fileInfo��


void writeBlock(CString DB_Name,blockInfo * block);
//��blockָ����ָ��Ŀ������д�ش��̡�

void readBlock(CString DB_Name,CString fileName, int m_blockNum, int fileType,blockInfo* &block,fileInfo* tmpFile);
//�Ӵ����ж�ȡ�ÿ�����ݵ��ڴ棬���ظÿ顣

fileInfo* Get_Table_Info(CString DB_Name,CString Table_Name);
//��ȡ���ļ���Ϣ��

fileInfo* Get_Index_Info(CString DB_Name,CString Index_Name);
// ��ȡ�����ļ���Ϣ��

//��ӿ鵽��������
void addToBlockHandle(blockInfo* block);


#endif