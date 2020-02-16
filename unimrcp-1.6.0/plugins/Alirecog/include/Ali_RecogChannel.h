#ifndef __ALI_RECOG_CHANNEL_H__
#define __ALI_RECOG_CHANNEL_H__

#include<string>
#include<time.h>
#include"apr_thread_pool.h"
#include"apr_thread_mutex.h"
#include"apr_queue.h"
#include"nlsEvent.h"
#include"nlsClient.h"
#include"nlsGlobal.h"
#include"speechRecognizerRequest.h"
#include"apr_errno.h"
#include"apr_queue.h"
#include"apr.h"
#include"apt.h"
//#include"mpf_activity_detector.h"
#include"Ali_RecogDefine.h"
#include"Ali_RecogUtil.h"
#if _WIN32
#include "nlsCommonSdk\Token.h"
#include"json\json.h"
#else
#include "nlsCommonSdk/Token.h"
#include "json/json.h"
#endif


using std::string;
using AlibabaNls::NlsClient;
using AlibabaNls::NlsEvent;
using AlibabaNlsCommon::NlsToken;
using AlibabaNls::SpeechRecognizerRequest;
using AlibabaNls::SpeechRecognizerCallback;

class CAliChannel 
{
public:
	CAliChannel()noexcept;
	~CAliChannel()noexcept;
private:
	//不允许出现临时对象及对象拷贝
	CAliChannel(CAliChannel&);
	CAliChannel& operator=(CAliChannel&);

public:
	bool			Init(	unsigned int id, 
							apr_pool_t* pool,
							const char * appkey,
							const char * AccessKey,
							const char * Secret,
							const char * Format,
							const char * SampleRate,
							const char * intermediateResult,
							const char * inverseText,
							const char * voiceDetection,
							const char * maxEndSilence,
							const char * maxStartSilence,
							const char * PunctuationPrediction,
							const char * CustomizationId,
							const char * VocabularyId,
							const char * OutputFormat,
							const char * ContextParam,
							const char * recordPath,
							apt_bool_t   record);
	void			Uinit();
	bool			Start(EngineChannel* Ch);
	void			Stop();
	apr_status_t	WriteFrame(const mpf_frame_t *frame);

	inline const char * ResultGet() const {		
		if (m_starting) {
			return m_result.c_str();
		}
		else {
			return "";
		}
	}

	inline unsigned int GetChannelId()const {
		return m_id;
	}
public:
	//  typedef void *(APR_THREAD_FUNC *apr_thread_start_t)(apr_thread_t*, void*);
	inline static void * APR_THREAD_FUNC RecogMain(apr_thread_t* tid, void * AliCh) {
		if (AliCh) {
			CAliChannel* pCh = static_cast<CAliChannel*>(AliCh);
			pCh->RecogMain();
		}
		return nullptr;
	}

public:
	inline static void	OnRecognitionCompleted(NlsEvent* ev, void* AliCh);
	inline static void	OnRecognitionStarted(NlsEvent* ev, void* AliCh);
	inline static void	OnRecognitionResultChanged(NlsEvent* ev, void* AliCh);
	inline static void	OnTaskFailed(NlsEvent* ev, void* AliCh);
	inline static void	OnChannelClosed(NlsEvent* ev, void* AliCh);
private:
	void				OnRecognitionCompleted(NlsEvent* ev);
	void				OnRecognitionStarted(NlsEvent* ev);
	void				OnRecognitionResultChanged(NlsEvent* ev);
	void				OnTaskFailed(NlsEvent* ev);
	void				OnChannelClosed(NlsEvent* ev);
private:
	int					DoAliToKenGet(string& AccessKeyID, string& AccessKeySecret);
	apt_bool_t			DoRecogRequestInit();
	apt_bool_t			DoCreateRecogRequest();
	void				DoDestroyRecogRequest();
	apt_bool_t			DoCheakRecogToKen();

private:
	void	RecogMain();
	void	RecordStart(string & VoiceFilePaht);
	void	RecordMain(const mpf_frame_t *frame);
	void	RecordStop();
private:
	inline void	lock() {
		apr_thread_mutex_lock(m_mutex);
	}

	inline void	ulock() {
		apr_thread_mutex_unlock(m_mutex);
	}

private:
	unsigned int		m_id;
	apr_pool_t*			m_pool;
	apr_queue_t*		m_FrameQueue;
	apr_file_t*			m_file;
	apr_thread_mutex_t*	m_mutex;
	string				m_result;
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
	apt_bool_t			m_record;
	string				m_recordPath;
	unsigned long		m_expireTime;
	apt_bool_t			m_Flag;
	apt_bool_t			m_starting;
	EngineChannel*		m_Channel;
	SpeechRecognizerCallback* m_recogCallback;
	SpeechRecognizerRequest*  m_recogRequest;
};


#endif // !__ALI_RECOG_CHANNEL_H__

