#include"Ali_SynthUtil.h"


string CUtil::GBKToUTF8(string& Text)
{
	string strOutUTF8 = "";
#if _WIN32

	WCHAR * str1;

	int n = MultiByteToWideChar(CP_ACP, 0, Text.c_str(), -1, NULL, 0);

	str1 = new WCHAR[n];

	MultiByteToWideChar(CP_ACP, 0, Text.c_str(), -1, str1, n);

	n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);

	char * str2 = new char[n];
	WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
	strOutUTF8 = str2;

	delete[] str1;
	str1 = NULL;
	delete[] str2;
	str2 = NULL;

	
#else
	strOutUTF8 = Text;
#endif
	return strOutUTF8;
}

string CUtil::TimeToStr(apr_time_t nowTime, const char* format)
{
	apr_time_exp_t tm;
	apr_time_exp_lt(&tm, nowTime);
	char tmp[255] = { 0 };
	apr_size_t size;
	apr_strftime(tmp, &size, sizeof(tmp), format, &tm);
	return tmp;
}


string CUtil::aprErrorToStr(apr_status_t stu)
{
	char errorTmp[1024] = { 0 };
	apr_strerror(stu, errorTmp, sizeof(errorTmp));
	return errorTmp;
}
