#ifndef __ALI_SYNTH_UTIL_H__
#define __ALI_SYNTH_UTIL_H__

#if _WIN32
#include<Windows.h>
#else
#include <unistd.h>
#endif

#include<string>
#include"apr.h"
#include"apr_time.h"

using std::string;

class CUtil {
public:
	CUtil()noexcept {};
	~CUtil()noexcept {};
public:
	/**
	* @brief gdk 转 utf-8
	* @param Text	传入的文本
	* @return 转换后的文本
	*/
	static string GBKToUTF8(string& Text);

	/**
	* @brief 时间转文本
	* @param nowTime		时间
	* @param format			转换的格式
	* @return 转换后的文本
	*/
	static string TimeToStr(apr_time_t nowTime, const char* format);

	/**
	* @brief apr错误转字符串
	* @param stu	apr传入的状态码	
	* @return 转换后的文本
	*/
	static string aprErrorToStr(apr_status_t stu);
};

#endif // !__ALI_SYNTH_UTIL_H__

