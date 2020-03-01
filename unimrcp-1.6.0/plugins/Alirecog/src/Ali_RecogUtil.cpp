#include"Ali_RecogUtil.h"


string CAliUtil::aprErrorToStr(apr_status_t stu)
{
	char errorTmp[1024] = { 0 };
	apr_strerror(stu, errorTmp, sizeof(errorTmp));
	return errorTmp;
}

string CAliUtil::aprTimeToStr(apr_time_t nowTime, const char* format)
{
	apr_time_exp_t tm;
	apr_time_exp_lt(&tm, nowTime);
	char tmp[255] = { 0 };
	apr_size_t size;
	apr_strftime(tmp, &size, sizeof(tmp), format, &tm);
	return tmp;
}

