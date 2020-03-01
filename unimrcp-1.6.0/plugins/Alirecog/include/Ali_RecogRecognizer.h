#ifndef __ALI_RECOG_RECOGNIZER_H__
#define __ALI_RECOG_RECOGNIZER_H__

#include"Ali_RecogResource.h"
#include"Ali_RecogChannel.h"
#include"speechRecognizerRequest.h"

using AlibabaNls::SpeechRecognizerCallback;
using AlibabaNls::SpeechRecognizerRequest;

//一句话识别异步
class CAliRecognizer : public CAliResource {
public:
	CAliRecognizer() noexcept;
	//CAliRecognizer( string appKey,
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

	~CAliRecognizer() noexcept;
private:
	CAliRecognizer(CAliRecognizer&);
	CAliRecognizer& operator=(CAliRecognizer&);

protected:
	virtual void	OnRecognitionCompleted(NlsEvent* ev);
	virtual void	OnRecognitionStarted(NlsEvent* ev);
	virtual void	OnRecognitionResultChanged(NlsEvent* ev);
	virtual void	OnTaskFailed(NlsEvent* ev);
	virtual void	OnChannelClosed(NlsEvent* ev);
public:
	virtual bool			DoRecogRequestInit( const  string& appKey,
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
	SpeechRecognizerCallback*	m_Callback;
	SpeechRecognizerRequest*	m_Request;
	bool						m_Flag;
	string						m_result;
};

#endif // !__ALI_RECOG_RECOGNIZER_H__

