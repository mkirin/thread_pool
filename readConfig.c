

#include "readConfig.h"
/*获取指定文件中最长的行的长度*/
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

/*去掉字符串开头的空白字符，并返回指针*/
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

/*去掉字符串末尾的空白字符*/
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

/*去掉字符串开头和末尾的空白字符*/
static char *TrimStr(char *cpString)
{
	char *cpRet = NULL;
	
	TrimRight(cpString);
	cpRet = TrimLeft(cpString);
	
	return cpRet;
}

/*从给定行字符串中解析出指定参数名的参数值*/
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
