#include"Ali_RecogResource.h"

CAliResource::CAliResource()noexcept
{

}

//CAliResource::CAliResource(	string& appKey,
//							string& AccessKey,
//							string& Secret,
//							string& Format,
//							string& SampleRate,
//							string& intermediateResult,
//							string& inverseText,
//							string& voiceDetection,
//							string& maxEndSilence,
//							string& maxStartSilence,
//							string& PunctuationPrediction,
//							string& CustomizationId,
//							string& VocabularyId,
//							string& OutputFormat,
//							string& ContextParam)noexcept:
//							m_expireTime(0),
//							m_appkey(appKey),
//							m_AccessKey(AccessKey),
//							m_Secret(Secret),
//							m_Format(Format),
//							m_SampleRate(intermediateResult),
//							m_intermediateResult(intermediateResult),
//							m_inverseText(inverseText),
//							m_voiceDetection(voiceDetection),
//							m_maxEndSilence(maxEndSilence),
//							m_maxStartSilence(maxStartSilence),
//							m_PunctuationPrediction(PunctuationPrediction),
//							m_CustomizationId(CustomizationId),
//							m_VocabularyId(VocabularyId),
//							m_OutputFormat(OutputFormat),
//							m_ContextParam(ContextParam),
//							m_token("")
//{
//
//}

CAliResource::~CAliResource()noexcept
{

}


void CAliResource::OnRecognitionCompleted(NlsEvent* ev, void* AliRe)
{
	//LOG_INFO("CAliResource::OnRecognitionCompleted :%x", AliRe);
	CAliResource * re = (CAliResource *)(AliRe);
	if (re)
		re->OnRecognitionCompleted(ev);
	return;
}

void CAliResource::OnRecognitionStarted(NlsEvent* ev, void* AliRe)
{
	//LOG_INFO("CAliResource::OnRecognitionStarted :%x", AliRe);
	CAliResource * re = (CAliResource *)(AliRe);
	if (re)
		re->OnRecognitionStarted(ev);
	return;
}

void CAliResource::OnRecognitionResultChanged(NlsEvent* ev, void* AliRe)
{
	//LOG_INFO("CAliResource::OnRecognitionResultChanged :%x", AliRe);
	CAliResource * re = (CAliResource *)(AliRe);
	if (re)
		re->OnRecognitionResultChanged(ev);
	return;
}

void  CAliResource::OnTranscriptionStarted(NlsEvent* ev, void* AliRe)
{
	CAliResource * re = (CAliResource *)(AliRe);
	if (re)
		re->OnTranscriptionStarted(ev);
	return;
}

void  CAliResource::OnTranscriptionResultChanged(NlsEvent* ev, void* AliRe)
{
	CAliResource * re = (CAliResource *)(AliRe);
	if (re)
		re->OnTranscriptionResultChanged(ev);
	return;
}

void  CAliResource::OnTranscriptionCompleted(NlsEvent* ev, void* AliRe)
{
	CAliResource * re = (CAliResource *)(AliRe);
	if (re)
		re->OnTranscriptionCompleted(ev);
	return;
}

void  CAliResource::OnSentenceBegin(NlsEvent* ev, void* AliRe)
{
	CAliResource * re = (CAliResource *)(AliRe);
	if (re)
		re->OnSentenceBegin(ev);
	return;
}

void  CAliResource::OnSentenceEnd(NlsEvent* ev, void* AliRe)
{
	CAliResource * re = (CAliResource *)(AliRe);
	if (re)
		re->OnSentenceEnd(ev);
	return;
}

void CAliResource::OnTaskFailed(NlsEvent* ev, void* AliRe)
{
	//LOG_INFO("CAliResource::OnTaskFailed :%x error :%s", AliRe, ev->getErrorMessage());
	CAliResource * re = (CAliResource *)(AliRe);
	if (re)
		re->OnTaskFailed(ev);
	return;
}

void CAliResource::OnChannelClosed(NlsEvent* ev, void* AliRe)
{
	//LOG_INFO("CAliResource::OnChannelClosed :%x error :%s", AliRe, ev->getErrorMessage());
	CAliResource * re = (CAliResource *)(AliRe);
	if (re)
		re->OnChannelClosed(ev);
	return;
}

//void	CAliResource::OnRecognitionCompleted(NlsEvent* ev)
//{
//
//}
//
//void	CAliResource::OnRecognitionStarted(NlsEvent* ev)
//{
//
//}
//
//void	CAliResource::OnRecognitionResultChanged(NlsEvent* ev)
//{
//
//}
//
//void	CAliResource::OnTaskFailed(NlsEvent* ev)
//{
//
//}
//
//void	CAliResource::OnChannelClosed(NlsEvent* ev)
//{
//
//}
//
//void	CAliResource::OnTranscriptionStarted(NlsEvent* ev)
//{
//
//}
//
//void	CAliResource::OnTranscriptionResultChanged(NlsEvent* ev)
//{
//
//}
//
//void	CAliResource::OnTranscriptionCompleted(NlsEvent* ev)
//{
//
//}
//
//void	CAliResource::OnSentenceBegin(NlsEvent* ev)
//{
//
//}
//
//void	CAliResource::OnSentenceEnd(NlsEvent* ev)
//{
//
//}

int	CAliResource::DoAliToKenGet(string& AccessKeyID, string& AccessKeySecret)
{
	NlsToken nlsTokenRequest;
	nlsTokenRequest.setAccessKeyId(AccessKeyID);
	nlsTokenRequest.setKeySecret(AccessKeySecret);

	if (nlsTokenRequest.applyNlsToken() == -1) {
		LOG_ERROR("Get Ali nlsToken Failed AccessKeyID:%s AccessKeySecret:%s ErrMsg:%s",
			AccessKeyID.c_str(),
			AccessKeySecret.c_str(),
			nlsTokenRequest.getErrorMsg());
		return -1;
	}

	m_token = nlsTokenRequest.getToken();
	m_expireTime = nlsTokenRequest.getExpireTime();
	return 0;
}

void CAliResource::DoCleanAliPram()
{
	m_expireTime = 0;
	m_token.clear();
}

bool CAliResource::DoCheakRecogToKen()
{
	time_t curTime = time(0);
	bool ret = TRUE;
	if ((m_expireTime - curTime < 10) || m_token.empty()) {
		if (-1 == DoAliToKenGet(m_AccessKey, m_Secret)) {
			ret = FALSE;
		}
		else {
			//
			LOG_INFO("<--------- Get Ali Recog ToKen :%s ---------->", m_token.c_str());
		}
	}
	return ret;
}

