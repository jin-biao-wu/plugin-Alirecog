#ifndef __ALI_RECOG_CHANNEL_H__
#define __ALI_RECOG_CHANNEL_H__

#include<string>
#include<time.h>
#include<queue>
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
#include"Ali_RecogResource.h"
#if _WIN32
#include "nlsCommonSdk\Token.h"
#include"json\json.h"
#else
#include "nlsCommonSdk/Token.h"
#include "json/json.h"
#endif


using std::string;
using std::queue;
using AlibabaNls::NlsClient;
using AlibabaNls::NlsEvent;
using AlibabaNlsCommon::NlsToken;
using AlibabaNls::SpeechRecognizerRequest;
using AlibabaNls::SpeechRecognizerCallback;

using ResultQueue = queue<string>;

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
	bool			Init(unsigned int id,
						apr_pool_t* pool, 
						const char *recordPath,
						apt_bool_t  record);
	void			Uinit();
	bool			Start(EngineChannel* Ch, CAliResource* resource);
	void			Stop();
	apr_status_t	WriteFrame(const mpf_frame_t *frame);
	void			RecogResultSet(string& reslut);
	string			RecogResultGet();
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
private:
	void	RecogMain();
	void	RecordStart(string & VoiceFilePaht);
	void	RecordMain(const mpf_frame_t *frame);
	void	RecordStop();
private:
	inline void	lock() {
#if _WIN32
		//LOG_INFO("Get Channel Lock :%u",::GetCurrentThreadId());
#else

#endif		
		apr_thread_mutex_lock(m_mutex);
	}

	inline void	ulock() {
#if _WIN32
		//LOG_INFO("Free Chanel Lock :%u", ::GetCurrentThreadId());
#else

#endif
		apr_thread_mutex_unlock(m_mutex);
	}

private:
	unsigned int		m_id;
	apr_pool_t*			m_pool;
	apr_queue_t*		m_FrameQueue;
	apr_file_t*			m_file;
	apr_thread_mutex_t*	m_mutex;
	CAliResource*		m_Recoger;
	apt_bool_t			m_record;
	string				m_recordPath;
	apt_bool_t			m_starting;
	EngineChannel*		m_Channel;
	ResultQueue			m_resultQue;
};

#endif // !__ALI_RECOG_CHANNEL_H__

