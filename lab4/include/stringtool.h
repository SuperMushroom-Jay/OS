#ifndef __STRINFTOOL_H__
#define __STRINGTOOL_H__
#include<stdio.h>
#include<string.h>

/*宏定义*************************************************/
#define TRUE 1
#define FALSE 0

/*函数定义*************************************************/
/**
 * @brief 字符串匹配
 * 
 * @param str 源字符串
 * @param pattern 正则表达式
 * @return int 匹配返回TRUE,否则返回FALSE
 */
int stringMatch(const char *str,const char *pattern);


#endif