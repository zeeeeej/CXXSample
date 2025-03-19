
#include <curl/curl.h>
#include "CurlDemo.h"
#include "../http/ResponseData.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



size_t write_callback(void * ptr,size_t size,size_t nmemb,void * userdata){
    struct ResponseData * response = (struct ResponseData * ) userdata;
    size_t total_size = size *nmemb;
    response->data = realloc(response->data, response->size + total_size + 1);
    if (response->data == NULL) {
        fprintf(stderr, "Failed to allocate memory for response data.\n");
        return 0;
    }

    // 将新数据追加到响应数据中
    memcpy(&(response->data[response->size]), ptr, total_size);
    response->size += total_size;
    response->data[response->size] = '\0';  // 添加字符串结束符

    return total_size;
}


void testPost(){
    CURL *curl;
    CURLcode res;
    const char *  json_data = "";
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if(curl){
        curl_easy_setopt(curl,CURLOPT_URL,"https://qinyuan-test.yunext.com/web/api/app/user/login");
        curl_easy_setopt(curl,CURLOPT_POST,1L);
        // curl_easy_setopt(curl,CURLOPT_POSTFIELDS,json_data);
        
      
        /*
         // 创建一个 multipart/form-data 表单
         curl_mime *form = curl_mime_init(curl);

         // 添加文本字段
         curl_mimepart *field = curl_mime_addpart(form);
         curl_mime_name(field, "phone");
         curl_mime_data(field, "testuser", CURL_ZERO_TERMINATED);
 
         // 添加文件字段
         field = curl_mime_addpart(form);
         curl_mime_name(field, "file");
         curl_mime_filedata(field, "example.txt");  // 文件路径
 
         // 将表单设置为 POST 数据
         curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);

         */


         /*
           //  // 设置 HTTP 头（指定内容类型为 JSON）
        //  struct curl_slist *headers = NULL;
        //  headers = curl_slist_append(headers, "Content-Type: application/json");
        //  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        */


          // 设置表单数据
          const char *data = "phone=15267858695&password=3A241F8AFA68ED6C5FD65A0240AE9BEC&appType=1&appVersion=3701&accessKey=1SNc3DS4W65fYLBI03SokWOSwUHcfjV4";
          curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
            // 设置 HTTP 头（可选）
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");

    // 初始化响应数据结构
    struct ResponseData response;
    response.data = malloc(1);  // 初始分配 1 字节
    response.size = 0;

      // 设置回调函数，用于处理响应数据
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

          // 执行请求
        res = curl_easy_perform(curl);

        // 检查请求是否成功
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }else{
            // 打印响应数据
            printf("ResponseX: %s\n", response.data);
        }


        free(response.data);
        // 清理 HTTP 头
        curl_slist_free_all(headers);

        // 清理 cURL
        curl_easy_cleanup(curl);
    }
}

void t1(){
    CURL *curl;
    CURLcode res;
    char* host = "https://qinyuan-test.yunext.com/";


    // 初始化 cURL
    curl = curl_easy_init();
    if(curl) {
        // 设置要请求的 URL
        curl_easy_setopt(curl, CURLOPT_URL, "https:/www.baidu.com");

        // 执行请求
        res = curl_easy_perform(curl);

        // 检查请求是否成功
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // 清理 cURL
        curl_easy_cleanup(curl);
    }
}