#include "filesystem.h"

using namespace std;

inode_d_type diskinodes[DISKINODECAPACITY];
de_type rootdirentry[5];
de_type emptydirentry;
char emptydata[(DATABLOCKCOUNT * BLOCKSIZE) - sizeof(de_type)];
FILE *outf;

void superblock_init(sb_type &sb){
	sb.s_isize = DISKINODECAPACITY * sizeof(inode_d_type) / BLOCKSIZE;
	sb.s_fsize = DATABLOCKCOUNT;
	int nextBlkno = ((sizeof(sb_type) + sizeof(diskinodes))) / BLOCKSIZE + 1;
	sb.freeil.n = 0;
	sb.freeil.ino[0] = -1;
	for (int i = 6; i < DISKINODECAPACITY; ++i){
		++sb.freeil.n;
		sb.freeil.ino[sb.freeil.n] = i;
		if (sb.freeil.n == 99){
			fseek(outf, nextBlkno * BLOCKSIZE, 0);
			fwrite(&sb.freeil, sizeof(sb.freeil), 1, outf);
			sb.freeil.n = 0;
			sb.freeil.ino[0] = nextBlkno;
			++nextBlkno;
		}
	}//生成 free inode list
	sb.freebl.n = 0;
	sb.freebl.blkno[0] = -1;
	int lastBlkno = (sizeof(sb_type) + sizeof(diskinodes)) / BLOCKSIZE + DATABLOCKCOUNT;
	for (int i = nextBlkno; i < lastBlkno; ++i){
		++sb.freebl.n;
		sb.freebl.blkno[sb.freebl.n] = i;
		if (sb.freebl.n == 99){
			fseek(outf,  lastBlkno * BLOCKSIZE, 0);
			fwrite(&sb.freebl, sizeof(sb.freebl), 1, outf);
			sb.freebl.n = 0;
			sb.freebl.blkno[0] = lastBlkno;
			--lastBlkno;
		}
	}//生成 free block list;
	sb.s_flock = 12;
	sb.s_ilock = 12;
	sb.s_fmod = 0;
	sb.s_ronly = 0;
	sb.s_time = 123;
}

void root_dir_init(){
	//根目录
	rootdirentry[0].m_ino = 1;
	strcpy(rootdirentry[0].m_name, "home");
	rootdirentry[1].m_ino = 2;
	strcpy(rootdirentry[1].m_name, "usr");
	rootdirentry[2].m_ino = 3;
	strcpy(rootdirentry[2].m_name, "A");
	rootdirentry[3].m_ino = 4;
	strcpy(rootdirentry[3].m_name, "B");
	rootdirentry[4].m_ino = 5;
	strcpy(rootdirentry[4].m_name, "C");
	memset(diskinodes, 0, sizeof(diskinodes));
	diskinodes[0].d_addr[0] = (sizeof(sb_type) + sizeof(diskinodes)) / BLOCKSIZE;
	diskinodes[0].d_size = sizeof(rootdirentry);
	diskinodes[0].d_mode = IFDIR;
	for (int i = 1; i <= 5; ++i){
		diskinodes[i].d_size = 0;
		diskinodes[i].d_mode = IFDIR;
	}
}

int main(){
	outf = fopen(DISKFILENAME, "wb");
	sb_type sb;
	superblock_init(sb);
	fseek(outf, 0, 0);
    root_dir_init();
	fwrite(&sb, sizeof(sb), 1, outf);//写 SuperBlock
	cout << "构建SuperBlock 共写入" << sizeof(sb) << "字节" << endl;
	fwrite(&diskinodes, sizeof(diskinodes), 1, outf);//写Inode
	cout << "构建inode 共写入 " << sizeof(diskinodes) << "字节" << endl;
	fseek(outf, sizeof(sb) + sizeof(diskinodes), 0);
	fwrite(&rootdirentry, sizeof(de_type), 5, outf);//写根目录
	cout << "构建root目录\n/home\n/usr\n/A\n/B\n/C\n共写入" << sizeof(de_type) * 5 << "字节" << endl;
	fclose(outf);
	cout << "按回车键结束 ..." << endl;
	cin.get();
    return 0;
}
