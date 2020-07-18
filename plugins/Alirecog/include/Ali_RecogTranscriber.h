#ifndef __ALI_RECOG_TRANSCRIBER_H__
#define __ALI_RECOG_TRANSCRIBER_H__

#include"apr_thread_mutex.h"
#include"Ali_RecogResource.h"
//#include"Ali_RecogChannel.h"
#include"speechTranscriberRequest.h"

using AlibabaNls::SpeechTranscriberCallback;
using AlibabaNls::SpeechTranscriberRequest;

//实时识别
class CAliTranscriber : public CAliResource
{
public:
	CAliTranscriber() noexcept;
	~CAliTranscriber() noexcept;
private:
	CAliTranscriber(CAliTranscriber&);
	CAliTranscriber& operator=(CAliTranscriber&);
protected:
	 void	OnTranscriptionStarted(NlsEvent* ev);
	 void	OnTranscriptionResultChanged(NlsEvent* ev);
	 void	OnTranscriptionCompleted(NlsEvent* ev);
	 void	OnSentenceBegin(NlsEvent* ev);
	 void	OnSentenceEnd(NlsEvent* ev);
	 void	OnTaskFailed(NlsEvent* ev);
	 void	OnChannelClosed(NlsEvent* ev);
public:
	virtual bool			DoRecogRequestInit( const string& appKey,
												const string& AccessKey,
												const string& Secret,
												const string& Format,
												const string& SampleRate,
												const string& intermediateResult,
												const string& inverseText,
												const string& voiceDetection,
												const string& maxEndSilence,
												const string& maxStartSilence,
												const string& PunctuationPrediction,
												const string& CustomizationId,
												const string& VocabularyId,
												const string& OutputFormat,
												const string& ContextParam,
												apr_pool_t *  pool);
	virtual bool			DoCreateRecogRequest();
	virtual void			DoDestroyRecogRequest();
	virtual const string	DoRecogResultGet();
	virtual int				DoWriteFrame(const mpf_frame_t *frame);
	//virtual void			DoSetChannel(CAliChannel * AliCh);
private:
	inline void	lock()
	{ apr_thread_mutex_lock(m_lock);}

	inline void	ulock()
	{ apr_thread_mutex_unlock(m_lock);}
private:
	SpeechTranscriberCallback*	m_CallBack;
	SpeechTranscriberRequest*	m_Request;
	//string						m_result;
	bool						m_Flag;
};

#endif

