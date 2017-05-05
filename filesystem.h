#pragma once

#include <bits/stdc++.h>

using namespace std;

/*参数设置¨*/
static const unsigned int IALLOC = 0x8000;
static const unsigned int IFMT = 0x6000;
static const unsigned int IFDIR = 0x4000;
static const unsigned int IFCHR = 0x2000;
static const unsigned int IFBLK = 0x6000;
static const unsigned int ILARG = 0x1000;
static const unsigned int ISUID = 0x800;
static const unsigned int ISGID = 0x400;
static const unsigned int ISVTX = 0x200;
static const unsigned int IREAD = 0x100;
static const unsigned int IWRITE = 0x80;
static const unsigned int IEXEC = 0x40;
static const unsigned int ITEXT = 0x20;
static const unsigned int IWANT = 0x10;
static const unsigned int IMOUNT = 0x8;
static const unsigned int IACC = 0x4;
static const unsigned int IUPD = 0x2;
static const unsigned int ILOCK = 0x1;

static const char *DISKFILENAME = "fs.img";
static const int DISKINODECAPACITY = 2048;
static const int BLOCKSIZE = 512;
static const int DATABLOCKCOUNT = 32768;

struct inode_d_type{
	unsigned int d_mode;
	int d_nlink;
	short d_uid;
	short d_gid;
	int d_size;
	int d_addr[10];
	int d_atime;
	int d_mtime;
};

struct fbl_type{
	int n;
	int blkno[110];
};

struct fil_type{
	int n;
	int ino[110];
};

struct sb_type{
	int s_isize;
	int s_fsize;

	fbl_type freebl;
	fil_type freeil;

	int s_flock;
	int s_ilock;

	int s_fmod;
	int s_ronly;
	int s_time;
	int padding[27];
};

static const int DIRENTRYSIZE = 32;

struct de_type{
	int m_ino;
	char m_name[DIRENTRYSIZE - sizeof(int)];
};

struct inode_type{
	unsigned int i_flag;
	unsigned int i_mode;
	int i_size;
	int i_addr[10];
	int i_number;
	int i_count;
	int i_offset;
	int i_nlink;
	short i_dev;
	short i_uid;
	short i_gid;
	int i_lastr;
};

inode_type getRootInode();
inode_type getInode(int ino);
void setInode(inode_type inode);

const int BUFCOUNT = 23;
void readDisk(void *p, int offset, int size);//读取硬盘的数据，offset 为整个硬盘的绝对位移
void writeDisk(void *p, int offset, int size);//写入硬盘的数据，offset 为整个硬盘的绝对位移
int readBlock(void *p, int blkno, int offset, int size);//根据 blkno 读取某一块的内容，不一定是数据块
int writeBlock(void *p, int blkno, int offset, int size);//根据 blkno 写入某一块的内容，不一定是数据块

enum BufFlag{
	B_WRITE = 0x1,
	B_READ = 0x2,
	B_DONE = 0x4,
	B_ERROR = 0x8,
	B_BUSY = 0x10,
	B_WANTED = 0x20,
	B_ASYNC = 0x40,
	B_DELWRI = 0x80
};

struct buf_type{
	unsigned int b_flags;
	int b_blkno;
	buf_type *b_forw;
	buf_type *b_back;
	buf_type *av_forw;
	buf_type *av_back;
	char data[BLOCKSIZE];
	short b_dev;
	int b_wcount;
	unsigned char* b_addr;
	int b_error;
	int b_resid;
};

buf_type *getBuffer(int blkno, int mode);//获取缓存块
void releaseBuffers();//释放所有缓存块

const int FILECOUNT = 20;

void readFile(int ino, void *p, int offset, int size);//从 Inode 编号为 ino 的文件中读取数据
void writeFile(inode_type &is, void *p, int offset, int size);//向 is 的文件中写入数据
int openFile(int ino, int mode);//打开文件，返回描述符
int getBlockNum(inode_type *node);//计算某个 offset 对应的数据块编号
int fseek(int fd, int offset);//移动文件指针
int fread(int fd, void *buffer, int len);//从文件中读取
int fwrite(int fd, void *buffer, int len);//向文件中写入
void fclose(int fd);//关闭文件
bool ck(char *name);//查看路径是否存在

struct kernel_type{
	FILE *diskfile;
	sb_type superBlock;
	buf_type buffers[BUFCOUNT];
	buf_type *freeBufHead;
	buf_type *freeBufTail;
	int freeBufCount;
	inode_type openFiles[FILECOUNT];
	int openFileCount;
};
extern kernel_type kernel;

inode_type getDirInode(char *name, char *end);
void ls(char *name); //列出 name 路径下的所有文件
int fopen(char *name, int mode);//以mode模式打开 name 文件
void fcreat(char *name, int mode); //以mode模式创建 name 文件
void fdelete(char *name);//删除 name 文件
void releaseDataBlocks(inode_type inode);
void pushFreeBlockList(int blkno);

void loadSuperBlock();
int alloc();
int ialloc();

void IORecv(void *p, int offset, int size);//发送 IO 读取请求，模拟设备读取
void IOSend(void *p, int offset, int size);//发送 IO 写入请求，模拟设备写入


