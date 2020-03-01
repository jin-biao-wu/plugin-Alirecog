#ifndef __ALI_RECOG_UTIL_H__
#define __ALI_RECOG_UTIL_H__

#include<string>
#include"apr.h"
#include"apt.h"
#include"apr_time.h"
#include"apr_errno.h"

using std::string;

class CAliUtil 
{
public:
	CAliUtil() noexcept{ }
	~CAliUtil() noexcept { }
public:
	static string aprErrorToStr(apr_status_t stu);
	static string aprTimeToStr(apr_time_t nowTime, const char* format);
};

#endif // !__ALI_RECOG_UTIL_H__
