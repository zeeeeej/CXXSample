#include "ota.h"
#include <curl/curl.h>  // 用于下载文件
//# include <openssl/md5.h> // 用于计算 MD5
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "../../cJSON.h"
#include <sys/reboot.h>

// 将 OtaInfo 写入配置文件
int writeOtaInfoToConfig(struct OtaInfo *info, const char *filename) {
    // 创建 JSON 对象
    cJSON *root = cJSON_CreateObject();
    if (!root) {
        fprintf(stderr, "Failed to create JSON object.\n");
        return -1;
    }

    // 添加字段到 JSON 对象
    cJSON_AddStringToObject(root, "url", info->url);
    cJSON_AddStringToObject(root, "md5", info->md5);
    cJSON_AddNumberToObject(root, "version", (double)info->version);
    cJSON_AddNumberToObject(root, "size", (double)info->size);

    // 将 JSON 对象转换为字符串
    char *json_str = cJSON_Print(root);
    if (!json_str) {
        fprintf(stderr, "Failed to convert JSON to string.\n");
        cJSON_Delete(root);
        return -1;
    }

    // 将 JSON 字符串写入文件
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open config file");
        free(json_str);
        cJSON_Delete(root);
        return -1;
    }

    fprintf(file, "%s\n", json_str);
    fclose(file);

    // 释放资源
    free(json_str);
    cJSON_Delete(root);

    printf("OtaInfo written to %s successfully.\n", filename);
    return 0;
}

// 从配置文件读取 OtaInfo
int readOtaInfoFromConfig(struct OtaInfo *info, const char *filename) {
    // 打开配置文件
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open config file");
        return -1;
    }

    // 读取文件内容
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *json_str = (char *)malloc(file_size + 1);
    if (!json_str) {
        perror("Failed to allocate memory");
        fclose(file);
        return -1;
    }

    fread(json_str, 1, file_size, file);
    json_str[file_size] = '\0';
    fclose(file);

    // 解析 JSON 字符串
    cJSON *root = cJSON_Parse(json_str);
    if (!root) {
        fprintf(stderr, "Failed to parse JSON.\n");
        free(json_str);
        return -1;
    }

    // 提取字段
    cJSON *url = cJSON_GetObjectItem(root, "url");
    cJSON *md5 = cJSON_GetObjectItem(root, "md5");
    cJSON *version = cJSON_GetObjectItem(root, "version");
    cJSON *size = cJSON_GetObjectItem(root, "size");

    if (!cJSON_IsString(url) || !cJSON_IsString(md5) || !cJSON_IsNumber(version) || !cJSON_IsNumber(size)) {
        fprintf(stderr, "Invalid JSON format.\n");
        cJSON_Delete(root);
        free(json_str);
        return -1;
    }

    info->url = strdup(url->valuestring);
    info->md5 = strdup(md5->valuestring);
    info->version = (size_t)version->valuedouble;
    info->size = (size_t)size->valuedouble;

    // 释放资源
    cJSON_Delete(root);
    free(json_str);

    printf("OtaInfo read from %s successfully. info is\nurl    :%s\nversion:%zu\nmd5    :%s\nsize   :%zu. \n", filename,info->url,info->version,info->md5,info->size);
    return 0;
}



/* 检查是否有更新 */ 
int checkUpdate(struct OtaInfo *info){
    // 模拟从服务器获取更新信息
    info->url = "https://iot2.qinyuan.cn/web/api/common/getFiles/6745340a93079705779bf605";
    info->md5 = "d41d8cd98f00b204e9800998ecf8427e";
    info->version = 2;
    info->size = 1024;

    // 假设总是有更新
    return 1;
}

/* 下载新程序 */
int download(char *temp_path, char *url) {
    CURL *curl;
    FILE *file;
    CURLcode res;

    file = fopen(temp_path, "wb");
    if (!file) {
        perror("Failed to open temporary file");
        return -1;
    }

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "Download failed: %s\n", curl_easy_strerror(res));
            fclose(file);
            return -1;
        }
    }

    fclose(file);
    return 0;
}

/* 备份老的程序 */
int backup(char *old_path, char *bak_path) {
    // 打开旧程序文件
    FILE *old_file = fopen(old_path, "rb");
    if (!old_file) {
        perror("Failed to open old file");
        return -1;
    }

    // 打开备份文件
    FILE *bak_file = fopen(bak_path, "wb");
    if (!bak_file) {
        perror("Failed to open backup file");
        fclose(old_file);
        return -1;
    }

    // 复制文件内容
    int ch;
    while ((ch = fgetc(old_file)) != EOF) {
        fputc(ch, bak_file);
    }

    fclose(old_file);
    fclose(bak_file);
    return 0;
}

/* 更新程序：将临时文件改名为目标文件名 */
int update(char *temp_path, char *file_name) {
    if (rename(temp_path, file_name) != 0) {
        perror("Failed to update firmware");
        return -1;
    }
    return 0;
}

/* 检查：验证文件的 MD5 */
int verify(char *temp_path, char *expectedMd5) {
    // FILE *file = fopen(temp_path, "rb");
    // if (!file) {
    //     perror("Failed to open file for verification");
    //     return -1;
    // }

    // MD5_CTX md5Context;
    // MD5_Init(&md5Context);

    // unsigned char buffer[1024];
    // size_t bytesRead;
    // while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) != 0) {
    //     MD5_Update(&md5Context, buffer, bytesRead);
    // }

    // unsigned char result[MD5_DIGEST_LENGTH];
    // MD5_Final(result, &md5Context);

    // char md5String[33];
    // for (int i = 0; i < 16; i++) {
    //     sprintf(&md5String[i*2], "%02x", (unsigned int)result[i]);
    // }

    // fclose(file);

    // if (strcmp(md5String, expectedMd5) == 0) {
    //     return 0;
    // } else {
    //     fprintf(stderr, "MD5 mismatch: expected %s, got %s\n", expectedMd5, md5String);
    //     return -1;
    // }
    return 0;
}

/* 清理：删除临时文件 */
int clear() {
    // if (remove(SERVER_TEMP_PATH) != 0) {
    //     perror("Failed to delete temporary file");
    //     return -1;
    // }
    return 0;
}


void reboot_system(){
    sync();
    reboot(RB_AUTOBOOT);
}

// /* 主函数 */
// int main() {
//     struct OtaInfo info;
//     if (checkUpdate(&info) <= 0) {
//         printf("No update available.\n");
//         return 0;
//     }

//     // 下载新程序
//     if (download(SERVER_TEMP_PATH, info.url) != 0) {
//         return -1;
//     }

//     // 验证下载的文件
//     if (verify(SERVER_TEMP_PATH, info.md5) != 0) {
//         return -1;
//     }

//     // 备份旧程序
//     if (backup(SERVER_PATH, SERVER_BAK_PATH) != 0) {
//         return -1;
//     }

//     // 更新程序
//     if (update(SERVER_TEMP_PATH, SERVER_PATH) != 0) {
//         return -1;
//     }

//     // 清理临时文件
//     if (clear() != 0) {
//         return -1;
//     }

//     printf("Update successful!\n");
//     return 0;
// }


