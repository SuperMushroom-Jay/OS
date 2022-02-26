#include"stringtool.h"
#include<string.h>
#include<regex.h>   //正则表达式头文件

int string_match(const char *str,const char *pattern){
    int status;     //用来判断正则表达式的匹配正误
    int flag = REG_EXTENDED;
    regmatch_t p_match[1];
    const size_t n_match = 1;
    regex_t regex;
    regcomp(&regex, pattern, flag); //编译正则表达式
    status =regexec(&regex,str,n_match,p_match,0);  //执行正则表达式和str比较
    if(status == REG_NOMATCH){
        status = FALSE;
    }else if(status == 0){
        status = TRUE;
    }
    regfree(&regex);    //释放regex内存
    return status;
}