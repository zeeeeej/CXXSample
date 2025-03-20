#include <stdio.h>
#include <curl/curl.h>
#include "curl/CurlDemo.h"
#include "curl/ota/ota.h"
#include <pthread.h>
 #include <unistd.h>
  #include <string.h>

// #define SERVER_CFG              "/etc/server.cfg"
#define SERVER_FILE_NAME        "rpc_server"


// #define SERVER_PATH             "/root/rpc_server"
// #define SERVER_TEMP_PATH        "/root/rpc_server.temp"
// #define SERVER_BAK_PATH         "/root/rpc_server.bak"


#define SERVER_PATH             "./rpc_server"
#define SERVER_TEMP_PATH        "./rpc_server.temp"
#define SERVER_BAK_PATH         "./rpc_server.bak"
#define SERVER_CFG              "./server.cfg"

void * watchDog(void * arg){
    int running = 1;
    while (1)
    {
        printf("check running ......\n");
        if (running==1)
        {
            /* code */
            sleep(10);
        } else{
           
            break;
        }   
    }
    printf("watchDog end !\n");
    /* 重启系统 */
    return NULL;
}

void * checkNewVersion(void * arg){
    struct OtaInfo info;
    struct OtaInfo last_info;
    int ret;
    int hasNew;
    while (1)
    {
        printf("checkNewVersion running ......\n");
            sleep(3);
            /* code */
            printf("checkNewVersion->checkUpdate ......\n");
            if (checkUpdate(&info) <= 0) {
                printf("No update available.\n");
                continue;;
            }

            // 
            ret = readOtaInfoFromConfig(&last_info,SERVER_CFG);

            if (ret !=0)
            {
                printf("read cfg  fail .\n");
                hasNew=1;
            }else{
                if(info.version>last_info.version){
                    hasNew = 1;
                }else{
                    hasNew = 0;
                }
            }
            if (!hasNew)
            {
                continue;
            }
            
            printf("checkNewVersion->download ......\n");
            // 下载新程序
            if (download(SERVER_TEMP_PATH, info.url) != 0) {
                continue;
            }
            printf("checkNewVersion->verify ......\n");
            // 验证下载的文件
            if (verify(SERVER_TEMP_PATH, info.md5) != 0) {
                continue;
            }
            printf("checkNewVersion->backup ......\n");
            // 备份旧程序
            if (backup(SERVER_PATH, SERVER_BAK_PATH) != 0) {
                //continue;
            }
            printf("checkNewVersion->update ......\n");
            sleep(3);
            // 更新程序
            if (update(SERVER_TEMP_PATH, SERVER_PATH) != 0) {
                continue;
            }
            // 写入新的配置文件
            printf("checkNewVersion->writeOtaInfoToConfig ......\n");
            writeOtaInfoToConfig(&info,SERVER_CFG);
            printf("checkNewVersion->clear ......\n");
            // 清理临时文件
            if (clear() != 0) {
                continue;
            }
            // 重启
        
        
        
    }
    printf("rebocheckNewVersion end !\n");
    return NULL;
}

int main(void) {
    //int a = 10;
    //int b = 20;
    //int sum = a + b;
    //printf("Sum: %d\n", sum);
    // t1();
    //testPost();
    int ret;
    pthread_t watchDog_t;
    pthread_t checkNewVersion_t;

    ret = pthread_create(&watchDog_t,NULL,watchDog,NULL);
    if (ret != 0) {
        fprintf(stderr, "Failed to create watchDog_t thread: %s\n", strerror(ret));
        return -1;
    }
    sleep(1);
    
    pthread_create(&checkNewVersion_t,NULL,checkNewVersion,NULL);
    if (ret != 0) {
        fprintf(stderr, "Failed to create checkNewVersion_t thread: %s\n", strerror(ret));
        return -1;
    }


    pthread_join(checkNewVersion_t,NULL);
    pthread_join(watchDog_t,NULL);
  


    
    return 0;
}