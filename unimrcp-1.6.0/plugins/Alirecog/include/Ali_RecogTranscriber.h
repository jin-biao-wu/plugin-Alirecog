#ifndef __ALI_RECOG_TRANSCRIBER_H__
#define __ALI_RECOG_TRANSCRIBER_H__

#include"Ali_RecogResource.h"
#include"Ali_RecogChannel.h"
#include"speechTranscriberRequest.h"

using AlibabaNls::SpeechTranscriberCallback;
using AlibabaNls::SpeechTranscriberRequest;

//实时识别
class CAliTranscriber : public CAliResource
{
public:
	CAliTranscriber() noexcept;
	//CAliTranscriber(string appKey,
	//				string AccessKey,
	//				string Secret,
	//				string Format,
	//				string SampleRate, 
	//				string intermediateResult, 
	//				string inverseText, 
	//				string voiceDetection, 
	//				string maxEndSilence,
	//				string maxStartSilence, 
	//				string PunctuationPrediction, 
	//				string CustomizationId, 
	//				string VocabularyId, 
	//				string OutputFormat,
	//				string ContextParam) noexcept;

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
												const string& ContextParam);
	virtual bool			DoCreateRecogRequest();
	virtual void			DoDestroyRecogRequest();
	virtual const char *	DoRecogResultGet()const;
	virtual int				DoWriteFrame(const mpf_frame_t *frame);
	virtual void			DoSetChannel(CAliChannel * AliCh);
private:
	SpeechTranscriberCallback*	m_CallBack;
	SpeechTranscriberRequest*	m_Request;
	string						m_result;
	bool						m_Flag;
};

#endif
