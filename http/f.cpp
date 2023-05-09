#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//测试一下 sscanf 解析http

int main(){
    char method[72];
    char url[45];
    char pr[46];
    char text[512];
    bzero(method, 72);
    bzero(url, 45);
    bzero(pr, 46);
    bzero(text, 512);

    char str[] = "GET /index/kk.txt HTTP/1.1\r\nUser-Agent: UbuntuPc\r\nHost: www.baidu.com\r\n\r\nskadgkasgas";
    //sscanf(str, "%[^ ] %[^ ] %[^ \r\n]\r\n%[0-9a-zA-Z:'-;\r\n\\/ ]", method, url, pr, text);
    sscanf(str, "%[^ ] %[^ ] %[^\r]\r\n%[ *]\r\n\r\n", method, url, pr, text);
    printf("%s\n",str);
    printf("======================\n[%s]\n[%s]\n[%s]\n[%s]\n", method, url, pr, text);

    return 0;
}
