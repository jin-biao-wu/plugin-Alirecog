//#pragma once

#ifndef __ALI_CHANNEL_H__
#define __ALI_CHANNEL_H__

#include<iostream>
#include<string>
#include<time.h>
#include "nlsClient.h"
#include "nlsEvent.h"
#include "speechRecognizerRequest.h"
#if _WIN32
#include "nlsCommonSdk\Token.h"
#include"json\json.h"
#else
#include "nlsCommonSdk/Token.h"
#include "json/json.h"
#endif
#include "iNlsRequest.h"
#include "apr.h"
#include "apt.h"
#include "apr_errno.h"
#include "apr_queue.h"
#include "apr_thread_proc.h"
#include "apr_thread_mutex.h"
#include "mpf_activity_detector.h"
#include"Ali_define.h"

// typedef void (*NlsCallbackMethod)(NlsEvent*, void*);
//extern class AliChannel;
using std::string;
using namespace AlibabaNls;
using namespace AlibabaNlsCommon;

class AliChannel {

public:
	AliChannel();
	AliChannel(unsigned int id ,
		const char* Appkey, 
		const char * AccessKeyID, 
		const char * AccessKeySecret,
		const char * Format="pcm",
		const char * SampleRate="8000",
		const char * intermediateResult="false",
		const char * inverseText="false",
		const char * voiceDetection="false",
		const char * maxEndSilence="0",
		const char * maxStartSilence="1",
		const char * PunctuationPrediction = "false",
		const char * CustomizationId = "",
		const char * VocabularyId = "",
		const char * OutputFormat = "",
		const char * ContextParam = "",
		const char * recordPath="../",
		apt_bool_t	 record = FALSE);

	~AliChannel();

private:
	AliChannel(AliChannel&ch);
	AliChannel& operator=(AliChannel&ch);
public:
	bool	Init(apr_pool_t* pool);
	void	Uninit();
public:
	apt_bool_t	Start();
	apt_bool_t	Stop();
public:
	apr_status_t WriteFrame(const mpf_frame_t *frame);
public:
	apt_bool_t IsWork()const;
	apr_size_t GetChannelId()const;
public:
	//void *(APR_THREAD_FUNC *apr_thread_start_t)(apr_thread_t*, void*);
	static void* APR_THREAD_FUNC RecogMain(apr_thread_t*tid, void* This);
private:
	 void RecogMain();
	 void RecogExit();
	 void RecordStart(string & VoiceFilePaht);
	 void RecordMain(const mpf_frame_t *frame);
	 void RecordClose();

private:
	void Thread_Mutex_Lock();
	void Thread_Mutex_ULock();
private:
	int	 GetAliToKen(string&AccessKeyID, string&AccessKeySecret);
	void OnRecognitionCompleted(NlsEvent* ev);
	void OnRecognitionStarted(NlsEvent* ev);
	void OnRecognitionResultChanged(NlsEvent* ev);
	void OnTaskFailed(NlsEvent* ev);
	void OnChannelClosed(NlsEvent* ev);
	void OnMrcpRecogCompleted(string&  result);
private:
	apt_bool_t	 OnRecogRequestInit();
	apt_bool_t	 OnCreateRecogRequest();
	void		 OnDestroyRecogRequest();
	apt_bool_t	 OnCheakRecogToKen();
public:
	inline void SetRecogChannel(Ali_recog_channel_t * Channel) {
		m_Ch = Channel;
	}

	inline Ali_recog_channel_t * GetRecogChannel() {
		return m_Ch;
	}

public:
	static void OnRecognitionCompleted(NlsEvent* ev, void* arg);
	static void OnRecognitionStarted(NlsEvent* ev, void* arg);
	static void OnRecognitionResultChanged(NlsEvent* ev, void* arg);
	static void OnTaskFailed(NlsEvent* ev, void* arg);
	static void OnChannelClosed(NlsEvent* ev, void* arg);

private:
	unsigned int		m_id;
	string				m_appkey;
	string				m_AccessKey;
	string				m_Secret;
	string				m_Format;
	string				m_token;
	string				m_SampleRate;
	string				m_intermediateResult;//中间结果
	string				m_inverseText;//
	string				m_voiceDetection;
	string				m_maxEndSilence;
	string				m_maxStartSilence;

	string				m_PunctuationPrediction;
	string				m_CustomizationId;
	string				m_VocabularyId;
	string				m_OutputFormat;
	string				m_ContextParam;

	long				m_expireTime; // token的创建时间
	apt_bool_t			m_Runing;
	apt_bool_t			m_Starting;
	apt_bool_t			m_reocgFlag;
	apt_bool_t			m_record;
	string				m_recordPath;
	apr_file_t*			m_file;
	Ali_recog_channel_t * m_Ch;
	//apr_thread_t*		m_tid;
	apr_thread_mutex_t* m_Mutex;
	apr_pool_t*			m_pool;
	apr_queue_t*		m_FrameQueue;
	SpeechRecognizerCallback* m_recogCallback;
	SpeechRecognizerRequest*  m_recogRequest;

};

#endif // !__ALI_CHANNEL_H__




