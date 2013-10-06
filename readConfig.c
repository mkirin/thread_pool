/***************************************************
 * filename:readConfig.c
 * author:mkirin
 * e-mail:lingfengtengfei@163.com
 * description: reading the thread config file
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
#include "readConfig.h"


/* function name: GetMaxLineLen
 * function parameter :
 * 	cpFileName: the thread pool config file's full path (with filename)
 * function description： to get the thread pool config file's size of the max line(获取指定文件中最长的行的长度)
 * returned value： the size of max line  
 */
static unsigned long GetMaxLineLen(char *cpFileName)
{
	FILE *fpFile = NULL;
	unsigned long ulMaxLineLen = 0;
	unsigned long ulCntLineLen = 0;
	char cCh = EOF;
	
	/*打开文件*/
	fpFile = fopen(cpFileName, "r");
	if(NULL == fpFile)
	{
		return 0;
	}
	
	/*逐行读取文件，遇到'/n'结束一行*/
	while(1)
	{
		cCh = fgetc(fpFile);
		if(EOF == cCh)
		{
			if(ulCntLineLen > ulMaxLineLen)
			{
				ulMaxLineLen = ulCntLineLen;
				ulCntLineLen = 0;
			}
			break;
		}
		ulCntLineLen++;
		if('/n' == cCh)
		{
			if(ulCntLineLen > ulMaxLineLen)
			{
				ulMaxLineLen = ulCntLineLen;
				ulCntLineLen = 0;
			}
			ulCntLineLen = 0;
		}
	}
	
	fclose(fpFile);
	
	return ulMaxLineLen;
}


/* function name: TrimLeft
 * function parameter :
 * 	cpString: any strings
 * function description： remove the string at the beginning of the blank character(去掉字符串开头的空白字符)
 * returned value： the pointer to the string  
 */
static char *TrimLeft(char *cpString)
{
	char *cpRet = NULL;
	
	/*过滤空字符串*/
	if('\0' == *cpString)
	{
		return cpString;
	}
	
	/*忽略头部的 空格 '/r' '/n' '/t'*/
	cpRet = cpString;
	while(' ' == *cpRet || '\t' == *cpRet || '\r' == *cpRet || '\n' == *cpRet)
	{
		cpRet++;
	}
	
	return cpRet;
}


/* function name: TrimRight
 * function parameter :
 * 	cpString: any strings
 * function description： remove the string at the end of the blank character(去掉字符串末尾的空白字符)
 * returned value： the pointer to the string  
 */
static void TrimRight(char *cpString)
{
	unsigned long ulStrLen = 0;
	
	/*过滤空字符串*/
	if('\0' == *cpString)
	{
		return;
	}
	
	/*去掉尾部的 空格 '/r' '/n' '/t'*/
	ulStrLen = strlen(cpString);	
	while(' ' == cpString[ulStrLen - 1] || '\t' == cpString[ulStrLen - 1] 
		  || '\r' == cpString[ulStrLen - 1] || '\n' == cpString[ulStrLen - 1])
	{
		ulStrLen--;
	}
	cpString[ulStrLen] = '\0';
}


/* function name: TrimStr
 * function parameter :
 * 	cpString: any strings
 * function description： remove the string at the beginning of the blank character 
 *      and remove the string at the end of the blank character(去掉字符串开头和末尾的空白字符)
 * returned value： the pointer to the string  
 */
static char *TrimStr(char *cpString)
{
	char *cpRet = NULL;
	
	TrimRight(cpString);
	cpRet = TrimLeft(cpString);
	
	return cpRet;
}


/* function name: ParseLine
 * function parameter :
 * 	cpLine: any strings of any lines
 *	cpParam: The specified parameter name
 *	cpValue：The specified parameter value
 * function description： From the value of parameter specifies the parameter name string in a given string line
 	（从给定行字符串中解析出指定参数名的参数值)
 * returned value： the pointer to the specified parameter value  
 */
static char *ParseLine(char *cpLine, char *cpParam, char *cpValue)
{
	char *cpTmp = NULL;
	char *cpPtr = NULL;
	unsigned long ulSepLoc = 0;
	
	/*去掉行首的空格*/
	cpTmp = TrimLeft(cpLine);
	
	/*过滤注释行，即以 # 开头的行, 第一个字符不可以是 = */
	if('#' == *cpTmp || '=' == *cpTmp)
	{
		return NULL;
	}
	
	/*查找 # 号，移除行末注释*/
	ulSepLoc = 0;
	while(ulSepLoc < strlen(cpTmp))
	{
		if('#' == cpTmp[ulSepLoc])
		{
			break;
		}
		ulSepLoc++;
	}
	cpTmp[ulSepLoc] = '\0';
	
	/*查找 = */
	ulSepLoc = 0;
	while(ulSepLoc < strlen(cpTmp))
	{
		if('=' == cpTmp[ulSepLoc])
		{
			break;
		}
		ulSepLoc++;
	}
	if(ulSepLoc == strlen(cpTmp))
	{
		return NULL;
	}
	
	/*获取参数*/
	cpTmp[ulSepLoc] = '\0'; 
	cpPtr = TrimStr(cpTmp);
	if(strcmp(cpPtr, cpParam))
	{
		return NULL;
	}
	
	/*获取参数的值*/
	cpPtr = TrimStr(cpTmp + ulSepLoc + 1);
	strcpy(cpValue, cpPtr);
	
	return cpValue;
}


/*从指定配置文件中获取指定参数名的参数值*/
/* function name: GetParamValue
 * function parameter :
 * 	cpConfFile: the config file's full path (with filename)
 *	cpParam: The specified parameter name
 *	cpValue：The specified parameter value
 * function description： From the value of parameter specifies the parameter name string in a given config file
 	（从指定配置文件中获取指定参数名的参数值)
 * returned value： the pointer to the specified parameter value  
 */
char *GetParamValue(char *cpConfFile, char *cpParam, char *cpValue)
{
	FILE *fpConf = NULL;
	char *cpLine = NULL;
	char *cpRet  = NULL;
	char cCh     = EOF;
	unsigned long ulMaxLineLen = 0;
	unsigned long ulLineLen    = 0;
	
	/*获取最大行长度*/
	ulMaxLineLen = GetMaxLineLen(cpConfFile);
	if(0 == ulMaxLineLen)
	{
		return NULL;
	}
	
	/*打开文件*/
	fpConf = fopen(cpConfFile, "r");
	if(NULL == fpConf)
	{
		return NULL;
	}
	
	/*分配内存*/
	cpLine = (char *)malloc(ulMaxLineLen + 1);
	if(NULL == cpLine)
	{
		fclose(fpConf);
		return NULL;
	}
	
	/*逐行读取并分析*/
	ulLineLen = 0;
	while(1)
	{
		cCh = fgetc(fpConf);
		switch(cCh)
		{
			case '\n':
			cpLine[ulLineLen++] = cCh;
			cpLine[ulLineLen++] = '\0';
			if(NULL != (cpRet = ParseLine(cpLine, cpParam, cpValue)))
			{
				break;
			}
			ulLineLen = 0;
			break;
			
			case EOF:
			cpLine[ulLineLen++] = '\0';
			if(NULL != (cpRet = ParseLine(cpLine, cpParam, cpValue)))
			{
				break;
			}
			ulLineLen = 0;
			break;
			
			default:
			cpLine[ulLineLen++] = cCh;
			break;
		}
		
		if(NULL != cpRet || EOF == cCh)
		{
			break;
		}
	}
	
	/*释放内存、关闭文件*/
	free(cpLine);
	fclose(fpConf);
	
	return cpRet;
}
/*
following just a test……

*/
/*
int				g_def_thread_num = 0;

int				g_manage_adjust_interval = 0;
int 			g_max_thread_num = 0;
int 			g_min_thread_num = 0;
int 			g_thread_worker_high_ratio = 0;
int 			g_thread_worker_low_ratio = 0;
 int get_config_value(char *item)
{
	char value[50] = {0};
printf("+++++%s\n",item);
	if(GetParamValue("thread_pool_config.conf",item,value) == NULL)
	{
		return -1;
	}
	printf("%s = %s\n",item,value);
	return atoi(value);
}

void conf_init()
{
	g_max_thread_num = get_config_value("MAX_THREAD_NUM");
	g_min_thread_num = get_config_value("MIN_THREAD_NUM");
	g_def_thread_num = get_config_value("DEF_THREAD_NUM");
	g_manage_adjust_interval = get_config_value("MANAGE_ADJUST_INTERVAL");
	g_thread_worker_high_ratio = get_config_value("THREAD_WORKER_HIGH_RATIO");
	g_thread_worker_low_ratio = get_config_value("THREAD_WORKER_LOW_RATIO");

}

int main()
{
	conf_init(); 	
	return 0;
}
*/
