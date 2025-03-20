#if !defined(__OTA)
#define __OTA
#include <stdio.h>

// #define SERVER_FILE_NAME        "rpc_server"
// #define SERVER_PATH             "/root/rpc_server"
// #define SERVER_TEMP_PATH        "/root/rpc_server.temp"
// #define SERVER_BAK_PATH         "/root/rpc_server.bak"

struct OtaInfo
{
    /* 文件下载地址 */
    char *url;
    /* 文件md5 */
    char * md5;
    /* 版本 */
    size_t version;
    /* 文件大小 */
    size_t size;
};

int writeOtaInfoToConfig(struct OtaInfo *info, const char *filename);

int readOtaInfoFromConfig(struct OtaInfo *info, const char *filename);

/* 检查是否有更新 */ 
int checkUpdate(struct OtaInfo *info);

/* 
 *  下载新程序 
 *  temp_path : 下载的临时文件存放位置
 *  url       : 下载地址
 */ 
int download(char * temp_path,char *url );

/* 
 *  备份老的程序 
 *  path        : 旧程序的位置
 *  bak_path    : 备份老程序的位置
 */
int backup(char * old_path,char * bak_path);

/* 
 *  更新程序 ：把临时文件改名file_name
 *  temp_path   : 临时文件名称
 *  file_name   : 程序名称 
 */
int update(char * temp_path,char*file_name);

/* 
 *  检查 ：把临时文件改名file_name
 *  temp_path       : 临时文件名称
 *  expectedMd5     : md5 
 */
int verify(char *temp_path, char *expectedMd5);

/* 
 * 清理
 */
int clear();


#endif // __OTA
