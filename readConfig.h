/***************************************************
 * filename:readConfig.h
 * author:mkirin
 * e-mail:lingfengtengfei@163.com
 * description: reading the thread config file's header file
 *           读取配置文件                   
 *                                                
 * 1、 通过 char *GetParamValue(char *cpConfFile, 
 *			        char *cpParam, char *cpValue) 
 *     函数获取指定配置文件中的指定参数的值，保存 
 *     在cpValue中                                
 * 2、 配置文件风格说明：                         
 *     (1) 自动忽略掉每行开头的空格               
 *     (2) 以 # 开头的行是注释行                  
 *     (3) 不能以 = 开头                          
 *     (4) 支持行末注释                           
***************************************************/
#ifndef _READCONFIG_HEAD_
#define _READCONFIG_HEAD_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
char *GetParamValue(char *cpConfFile, char *cpParam, char *cpValue);

#endif
