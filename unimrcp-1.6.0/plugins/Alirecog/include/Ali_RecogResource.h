#ifndef __ALI_RECOG_RESOURCE_H__
#define __ALI_RECOG_RESOURCE_H__

#include<string>
#include"nlsClient.h"
#include"nlsEvent.h"
#include"Ali_RecogDefine.h"


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

class CAliChannel;

class CAliResource 
{
public:
	CAliResource()noexcept;
	//CAliResource(	string& appKey,
	//				string& AccessKey,
	//				string& Secret,
	//				string& Format,
	//				string& SampleRate,
	//				string& intermediateResult,
	//				string& inverseText,
	//				string& voiceDetection,
	//				string& maxEndSilence,
	//				string& maxStartSilence,
	//				string& PunctuationPrediction,
	//				string& CustomizationId,
	//				string& VocabularyId,
	//				string& OutputFormat,
	//				string& ContextParam)noexcept;

	virtual ~CAliResource()noexcept;
private:
	CAliResource(CAliResource&);
	CAliResource& operator=(CAliResource&);

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
												const string& ContextParam) = 0;
	virtual bool			DoCreateRecogRequest() = 0;
	virtual void			DoDestroyRecogRequest() = 0;
	virtual const char *	DoRecogResultGet()const = 0;
	virtual int				DoWriteFrame(const mpf_frame_t *frame) = 0;
	virtual void			DoSetChannel(CAliChannel * AliCh) = 0;
public:
	//实时识别回调
	 static void	OnTranscriptionStarted(NlsEvent* ev, void* AliRe);
	 static void	OnTranscriptionResultChanged(NlsEvent* ev, void* AliRe);
	 static void	OnTranscriptionCompleted(NlsEvent* ev, void* AliRe);
	 static void	OnSentenceBegin(NlsEvent* ev, void* AliRe);
	 static void	OnSentenceEnd(NlsEvent* ev, void* AliRe);

	//一句话回调
	 static void	OnRecognitionCompleted(NlsEvent* ev, void* AliRe);
	 static void	OnRecognitionStarted(NlsEvent* ev, void* AliRe);
	 static void	OnRecognitionResultChanged(NlsEvent* ev, void* AliRe);

	 static void	OnTaskFailed(NlsEvent* ev, void* AliRe);
	 static void	OnChannelClosed(NlsEvent* ev, void* AliRe);

protected:
	virtual void	OnRecognitionCompleted(NlsEvent* ev) { };
	virtual void	OnRecognitionStarted(NlsEvent* ev) { };
	virtual void	OnRecognitionResultChanged(NlsEvent* ev) { };
	virtual void	OnTranscriptionStarted(NlsEvent* ev) { };
	virtual void	OnTranscriptionResultChanged(NlsEvent* ev) { };
	virtual void	OnTranscriptionCompleted(NlsEvent* ev) { };
	virtual void	OnSentenceBegin(NlsEvent* ev) { };
	virtual void	OnSentenceEnd(NlsEvent* ev) { };

	virtual void	OnTaskFailed(NlsEvent* ev) = 0;
	virtual void	OnChannelClosed(NlsEvent* ev) = 0;

protected:
	int				DoAliToKenGet(string& AccessKeyID, string& AccessKeySecret);
	void			DoCleanAliPram();
public:
	bool			DoCheakRecogToKen();

protected:
	unsigned long		m_expireTime;
	string				m_appkey;
	string				m_AccessKey;
	string				m_Secret;
	string				m_Format;
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
	string				m_token;
	CAliChannel*		m_AliCh;
};

#endif // !__ALI_RECOG_RESOURCE_H__

