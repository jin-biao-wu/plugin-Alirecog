//#pragma once

#ifndef __ALI_CHANNEL_FACTORY_H__
#define __ALI_CHANNEL_FACTORY_H__

#include<iostream>
#include<string>
#include<map>
#include<algorithm>
#include"apr.h"
#include"apt.h"
#include"apr_thread_pool.h"
#include"Ali_channel.h"
#include"Ali_define.h"
#include "mrcp_recog_engine.h"


using namespace std;

typedef map<unsigned int, class AliChannel*> MapChPool;

class AliChannelFactory {
public:
	AliChannelFactory();
	~AliChannelFactory();

public:
	apt_bool_t Init(
		apr_pool_t*pool,
		apr_size_t MaxCh,
		const char* Appkey,
		const char * AccessKeyID,
		const char * AccessKeySecret,
		const char * Format = "pcm",
		const char * SampleRate = "8000",
		const char * intermediateResult = "",
		const char * inverseText = "",
		const char * voiceDetection = "",
		const char * maxEndSilence = "",
		const char * maxStartSilence = "",
		const char * PunctuationPrediction="",
		const char * CustomizationId = "",
		const char * VocabularyId="",
		const char * OutputFormat="",
		const char * ContextParam="",
		const char * recordPath = "../",
		apt_bool_t	 record = FALSE);

	void		Uinit();

public:
	AliChannel* NewChannel();
	void		DeleteChannel(class AliChannel*pCh);
	void		AddChannel(class AliChannel*pCh);

private:
	AliChannel* GetIdleChannel();

private:
	void Thread_mutex_lock();
	void Thread_mutex_ulock();
private:
	static mrcp_engine_t* m_engine;
	MapChPool		m_ChbusyPool;
	MapChPool		m_ChidlePool;
	apr_thread_pool*m_Thread_pool;
	apr_pool_t *	m_pool;
	unsigned int	m_Chid;
	string			m_appKey;
	string			m_AccessKey;
	string			m_Secret;
	string			m_Format;
	string			m_SampleRate;
	apr_thread_mutex_t* m_Mutex;

};


#endif // !__ALI_CHANNEL_FACTORY_H__




