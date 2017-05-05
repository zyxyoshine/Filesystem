#include "filesystem.h"

using namespace std;

const int MAXBUF = 2048 * 1000;
kernel_type kernel;
char extrabuffer[MAXBUF];

void testWrite(char *name, char *content){
	int fd = fopen(name, IWRITE);
	//cout << "got fd" << " " << fd <<endl;
	//fseek(fd,sizeof(de_type));
	if (fd >= 0) {
        fwrite(fd, content, strlen(content));
        fclose(fd);
        cout << "成功写入 " << strlen(content) << "字节" << endl;
	}else{
        cout << "ERROR：访问非法！" << endl;
	}
}

void testRead(char *name){
	char content[2048];
	memset(content, 0, sizeof(content));
	int fd = fopen(name, IREAD);
	if (fd >= 0) {
        fread(fd, content, 2048);
        fclose(fd);
        cout << content << endl;
	}else{
        cout << "ERROR：访问非法！" << endl;
	}
}

void init(){//初始化磁盘
	kernel.openFileCount = 0;
	kernel.freeBufCount = 0;
	for (int i = 0; i < BUFCOUNT; ++i)
		memset(&kernel.buffers[i].b_flags, 0, sizeof(unsigned int));
	for (int i = 0; i < FILECOUNT; ++i)
		memset(&kernel.openFiles[i].i_flag, 0, sizeof(unsigned int));
	loadSuperBlock();
}

void autotest(){//自动测试api
    char arg[2048];
	cout << endl;
	cout << "=======================自动测试开始=======================" << endl;
	cout << "测试 ls /" << endl;
	strcpy(arg,"/");
	ls(arg);
	cout << "---------------------------" <<endl;
	cout << "测试 mkdir /mkdirtest" << endl;
    strcpy(arg,"/mkdirtest");
    fcreat(arg, IFDIR);
    cout << "/ 目录信息为:\n";
    strcpy(arg,"/");
	ls(arg);
	cout << "---------------------------" <<endl;
	cout << "测试 mk /mkdirtest/mktest" << endl;
	strcpy(arg,"/mkdirtest/mktest");
	fcreat(arg, 0);
	cout << "/mkdirtest 目录信息为:\n";
	strcpy(arg,"/mkdirtest");
	ls(arg);
	cout << "测试 mkdir /mkdirtest/mktest2" << endl;
	strcpy(arg,"/mkdirtest/mktest2");
	fcreat(arg, IFDIR);
	cout << "/mkdirtest 目录信息为:\n";
	strcpy(arg,"/mkdirtest");
	ls(arg);
	cout << "---------------------------" <<endl;
	cout << "测试 write /mkdirtest/mktest Hello World!" << endl;
	strcpy(arg,"/mkdirtest/mktest");
	testWrite(arg, "Hello World!");
	cout << "---------------------------" <<endl;
	cout << "测试 read /mkdirtest/mktest" << endl;
	strcpy(arg,"/mkdirtest/mktest");
	testRead(arg);
	cout << "---------------------------" <<endl;
	cout << "测试 rm /mkdirtest/mktest"<<endl;
	strcpy(arg,"/mkdirtest/mktest");
	fdelete(arg);
	cout << "/mkdirtest 的目录信息为:\n";
	strcpy(arg,"/mkdirtest");
	ls(arg);
	cout << "---------------------------" <<endl;
	cout << "=======================自动测试结束=======================" << endl;
}

int main(){
	init();
	string cmd = "help", arg1, arg2, now = "/";
	string nnext;
	char src[2048], arg1c[2048], arg2c[2048];
	char ckt[2048];
    printf("=====================================二级文件系统=====================================\n");
    printf("                                                                 作者:周宇星(1352652)\n");
	while (cmd != "exit"){
		if (cmd == "help"){
			printf("指令及参数：注意，一定要使用exit退出！若使用绝对路径，请以'/'开头\n");
			printf("help                               |指令简介\n");
			printf("autotest                           |自动测试\n");
			printf("write     [文件名] [内容字符串]    |向文件写内容  |示例: write /A/B Hello World\n");
			printf("                                                  |示例: write B Hello World\n");
            printf("read      [文件名]                 |读文件的内容  |示例：read /A/B\n");
            printf("                                                  |示例: read B\n");
            printf("ls        [目录名]                 |显示目录的内容|示例：ls /A\n");
            printf("                                                  |示例: ls\n");
            printf("mk        [文件名]                 |创建文件      |示例：mk /A/B\n");
            printf("                                                  |示例: mk B\n");
            printf("mkdir     [目录名]                 |创建目录      |示例：mkdir /A\n");
            printf("                                                  |示例: mkdir A\n");
            printf("rm        [文件或目录名]           |删除文件或目录|示例：rm /A/B\n");
            printf("                                                  |示例: rm B\n");
            printf("fin       [外部文件名] [内部文件名]|从外部系统输入|示例：fin input.txt /A/B\n");
            printf("                                                  |示例: fin input.txt B\n");
            printf("fout      [内部文件名] [外部文件名]|输出到外部系统|示例：fout /A/B output.txt\n");
            printf("                                                  |示例: fout B output.txt\n");
            printf("cd        [目录名或绝对路径或'..'] |更改工作路径  |示例：cd /A\n");
            printf("                                                  |示例: cd A\n");
            printf("                                                  |示例: cd ..\n");
		}
		else if (cmd == "autotest"){
			autotest();
		}
		else if (cmd == "write"){
            if (arg1c[0] == '/')
                testWrite(arg1c, arg2c);
            else{
                nnext = now + arg1c;
                strcpy(ckt,nnext.c_str());
                testWrite(ckt, arg2c);
            }
		}
		else if (cmd == "read"){
            if (arg1c[0] == '/')
                testRead(arg1c);
            else{
                nnext = now + arg1c;
                strcpy(ckt,nnext.c_str());
                testRead(ckt);
            }
		}
		else if (cmd == "ls"){
            if (arg1c[0] == '/')
                ls(arg1c);
            else{
                if(now != "/" )
                    nnext = now.substr(0,now.length() - 1);
                else
                    nnext = now;
                strcpy(ckt,nnext.c_str());
                ls(ckt);
            }
		}
		else if (cmd == "mk"){
            if (arg1c[0] == '/')
                fcreat(arg1c, 0);
            else{
                nnext = now + arg1c;
                strcpy(ckt,nnext.c_str());
                fcreat(ckt, 0);
            }
		}
		else if (cmd == "mkdir"){
			if (arg1c[0] == '/')
                fcreat(arg1c, IFDIR);
            else{
                nnext = now + arg1c;
                strcpy(ckt,nnext.c_str());
                fcreat(ckt, IFDIR);
            }
		}
		else if (cmd == "rm"){
            if (arg1c[0] == '/')
                fdelete(arg1c);
            else{
                nnext = now + arg1c;
                strcpy(ckt,nnext.c_str());
                fdelete(ckt);
            }
		}
		else if (cmd == "fin"){
            if (arg2c[0] == '/') {
                FILE *externalFile = fopen(arg1c, "rb");
                fseek(externalFile, 0, SEEK_END);
                int size = ftell(externalFile);
                fseek(externalFile, 0, 0);
                fread(extrabuffer, size, 1, externalFile);
                int wd = fopen(arg2c, IWRITE);
                if (wd != -1) {
                    fwrite(wd, extrabuffer, size);
                    fclose(wd);
                }
                fclose(externalFile);
            }else{
                nnext = now + arg2c;
                strcpy(ckt,nnext.c_str());
                FILE *externalFile = fopen(arg1c, "rb");
                fseek(externalFile, 0, SEEK_END);
                int size = ftell(externalFile);
                fseek(externalFile, 0, 0);
                fread(extrabuffer, size, 1, externalFile);
                int wd = fopen(ckt, IWRITE);
                if (wd != -1) {
                    fwrite(wd, extrabuffer, size);
                    fclose(wd);
                }
                fclose(externalFile);
            }
		}
		else if (cmd == "fout"){
            if (arg1c[0] == '/') {
                int rd = fopen(arg1c, IREAD);
                if (rd != -1) {
                    int flen = kernel.openFiles[rd].i_size;
                    fread(rd, extrabuffer, flen);
                    FILE *extrafile = fopen(arg2c, "wb");
                    fwrite(extrabuffer, flen, 1, extrafile);
                    fclose(extrafile);
                }
                fclose(rd);
            }else{
                nnext = now + arg1c;
                strcpy(ckt,nnext.c_str());
                int rd = fopen(ckt, IREAD);
                if (rd != -1) {
                    int flen = kernel.openFiles[rd].i_size;
                    fread(rd, extrabuffer, flen);
                    FILE *extrafile = fopen(arg2c, "wb");
                    fwrite(extrabuffer, flen, 1, extrafile);
                    fclose(extrafile);
                }
                fclose(rd);
            }
		}
		else if (cmd == "cd") {
            if (arg1c[0] == '/') {
                if (strlen(arg1c) > 1) {
                    if (ck(arg1c))
                        now = string(arg1c) + "/";
                }
                else
                    now = "/";
            }else if(arg1c[0] == '.' && arg1c[1] == '.') {
                if (now != "/") {
                    int k;
                    for (k = now.length() - 2;k > 0;k--) {
                        if(now[k] == '/')
                            break;
                    }
                    now = now.substr(0,k + 1);
                }else
                    cout << "已经是根目录了！" << endl;
            }else{
                nnext = now + arg1c;
                strcpy(ckt,nnext.c_str());
                if (ck(ckt))
                    now = nnext + "/";
            }
		}
		else{
            cout << "ERROR：指令不存在，输入help查看简介" << endl;
		}
		cout << "zhouyuxing(1352652)-FileSystem "<< now << ">";
		cin.getline(src, 2048);
		char *p = src;
		cmd = "";
		arg1 = "";
		arg2 = "";
        while ((*p != ' ') && (*p != 0)){
			cmd += *p;
			++p;
		}
		if (*p!= 0) ++p;
		while ((*p != ' ') && (*p != 0)){
			arg1 += *p;
			++p;
		}
		if (*p!= 0) ++p;
		while ((*p != ' ') && (*p != 0)){
			arg2 += *p;
			++p;
		}
		strcpy(arg1c, arg1.c_str());
		strcpy(arg2c, arg2.c_str());
	}
	releaseBuffers();//释放全部缓存，写入磁盘
}
