#include"Ali_RecogRecognizer.h"


CAliRecognizer::CAliRecognizer() noexcept
{

}

//CAliRecognizer::CAliRecognizer( string		appKey,
//								string		AccessKey,
//								string		Secret,
//								string		Format,
//								string		SampleRate,
//								string		intermediateResult,
//								string		inverseText,
//								string		voiceDetection,
//								string		maxEndSilence,
//								string		maxStartSilence,
//								string		PunctuationPrediction,
//								string		CustomizationId,
//								string		VocabularyId,
//								string		OutputFormat,
//								string		ContextParam) noexcept:
//								m_Callback(nullptr),
//								m_Request(nullptr),
//								m_Flag(FALSE),
//								m_result(""),
//								CAliResource(appKey,
//											 AccessKey,
//											 Secret,
//											 Format,
//											 SampleRate,
//											 intermediateResult,
//											 inverseText,
//											 voiceDetection,
//											 maxEndSilence,
//											 maxStartSilence,
//											 PunctuationPrediction,
//											 CustomizationId,
//											 VocabularyId,
//											 OutputFormat,
//											 ContextParam)
//
//{
//		//m_Callback = new SpeechRecognizerCallback;
//		//if (m_Callback) {
//		//	m_Callback->setOnRecognitionStarted(CAliResource::OnRecognitionStarted, this);
//		//	m_Callback->setOnRecognitionResultChanged(CAliResource::OnRecognitionResultChanged, this);
//		//	m_Callback->setOnRecognitionCompleted(CAliResource::OnRecognitionCompleted, this);
//		//	m_Callback->setOnChannelClosed(CAliResource::OnChannelClosed, this);
//		//	m_Callback->setOnTaskFailed(CAliResource::OnTaskFailed, this);
//		//}
//}

CAliRecognizer::~CAliRecognizer() noexcept
{
	if (m_Callback) {
		delete m_Callback;
		m_Callback = nullptr;
	}
}


//inline void	CAliRecognizer::OnRecognitionCompleted(NlsEvent* ev, void* AliRe)
//{
//	CAliRecognizer * re = static_cast<CAliRecognizer *>(AliRe);
//	if (re)
//		re->OnRecognitionCompleted(ev);
//	return;
//}
//
//inline void	CAliRecognizer::OnRecognitionStarted(NlsEvent* ev, void* AliRe)
//{
//	CAliRecognizer * re = static_cast<CAliRecognizer *>(AliRe);
//	if (re)
//		re->OnRecognitionStarted(ev);
//	return;
//}
//
//inline void	CAliRecognizer::OnRecognitionResultChanged(NlsEvent* ev, void* AliRe)
//{
//	CAliRecognizer * re = static_cast<CAliRecognizer *>(AliRe);
//	if (re)
//		re->OnRecognitionResultChanged(ev);
//	return;
//}
//
//inline void	CAliRecognizer::OnTaskFailed(NlsEvent* ev, void* AliRe)
//{
//	CAliRecognizer * re = static_cast<CAliRecognizer *>(AliRe);
//	if (re)
//		re->OnTaskFailed(ev);
//	return;
//}
//
//inline void	CAliRecognizer::OnChannelClosed(NlsEvent* ev, void* AliRe)
//{
//	CAliRecognizer * re = static_cast<CAliRecognizer *>(AliRe);
//	if (re)
//		re->OnChannelClosed(ev);
//	return;
//}

void CAliRecognizer::OnRecognitionCompleted(NlsEvent* ev)
{
	m_result = ev->getResult();
	if (nullptr != m_AliCh) {
		m_AliCh->RecogResultSet(m_result);
	}
}

void CAliRecognizer::OnRecognitionStarted(NlsEvent* ev)
{

}

void CAliRecognizer::OnRecognitionResultChanged(NlsEvent* ev)
{

}

void CAliRecognizer::OnTaskFailed(NlsEvent* ev)
{
	//m_Flag = TRUE;
	//LOG_ERROR("OnTaskFailed Error Msg :%s", ev->getErrorMessage());
}

void CAliRecognizer::OnChannelClosed(NlsEvent* ev)
{
	//m_Flag = TRUE;
	//LOG_ERROR("OnTaskFailed Error Msg :%s", ev->getErrorMessage());
}

bool CAliRecognizer::DoRecogRequestInit(const string& appKey,
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
										const string& ContextParam)
{

	m_appkey = appKey;
	m_AccessKey = AccessKey;
	m_Secret = Secret;
	m_Format = Format;
	m_SampleRate = SampleRate;
	m_intermediateResult = intermediateResult;
	m_inverseText = inverseText;
	m_voiceDetection = voiceDetection;
	m_maxEndSilence = maxEndSilence;
	m_maxStartSilence = maxStartSilence;
	m_PunctuationPrediction = PunctuationPrediction;
	m_CustomizationId = CustomizationId;
	m_VocabularyId = VocabularyId;
	m_OutputFormat = OutputFormat;
	m_ContextParam = ContextParam;

	m_Callback = new SpeechRecognizerCallback;
	if (m_Callback) {
		m_Callback->setOnRecognitionStarted(CAliResource::OnRecognitionStarted, this);
		m_Callback->setOnRecognitionResultChanged(CAliResource::OnRecognitionResultChanged, this);
		m_Callback->setOnRecognitionCompleted(CAliResource::OnRecognitionCompleted, this);
		m_Callback->setOnChannelClosed(CAliResource::OnChannelClosed, this);
		m_Callback->setOnTaskFailed(CAliResource::OnTaskFailed, this);
	}

	return TRUE;
}

bool CAliRecognizer::DoCreateRecogRequest()
{
	bool ret = TRUE;
	if ((nullptr != m_Callback) && (nullptr == m_Request)) {
		m_Request = NlsClient::getInstance()->createRecognizerRequest(m_Callback);
	}

	if (nullptr == m_Request) {
		m_Request = nullptr;
		ret = FALSE;
	}

	if (m_Request && ret) {
		if (!DoCheakRecogToKen()) {
			LOG_ERROR("<--- DoCheakRecogToKen falied --->");
			ret = FALSE;
		}
		else {

			LOG_INFO("recog init token:%s AppKey:%s Format:%s SampleRate:%s",
				m_token.c_str(), m_appkey.c_str(), m_Format.c_str(), m_SampleRate.c_str());

			m_Request->setToken(m_token.c_str());
			m_Request->setAppKey(m_appkey.c_str());
			m_Request->setFormat(m_Format.c_str());

			//m_Request->setSampleRate((m_SampleRate == "8000" ? 8000 : 16000));
			m_Request->setSampleRate(8000);

			m_Request->setIntermediateResult(((m_intermediateResult == "true") ? true : false)); //返回中间结果
			m_Request->setEnableVoiceDetection(((m_voiceDetection == "true") ? true : false)); //开启语音检查

			if (m_voiceDetection == "true") {
				int EndSilence = atoi(m_maxEndSilence.c_str());
				m_Request->setMaxEndSilence(EndSilence * 1000);//最大说话后的静音时长

				int StartSilence = atoi(m_maxStartSilence.c_str());
				m_Request->setMaxStartSilence(StartSilence * 1000); //最大开始静音的时长
			}

			m_Request->setPunctuationPrediction(((m_PunctuationPrediction == "true") ? true : false));
			m_Request->setInverseTextNormalization(((m_inverseText == "true") ? true : false));

			if (!m_CustomizationId.empty())
			{
				m_Request->setCustomizationId(m_CustomizationId.c_str());
			}

			if (!m_VocabularyId.empty())
			{
				m_Request->setVocabularyId(m_VocabularyId.c_str());
			}

			if (!m_OutputFormat.empty())
			{
				m_Request->setOutputFormat(m_OutputFormat.c_str());
			}

			if (!m_ContextParam.empty())
			{
				m_Request->setContextParam(m_ContextParam.c_str());
			}


			if (m_Request->start() < 0) {
				LOG_ERROR("<--- Ali server recog start failed --->");
				ret = FALSE;
			}
		}

	}
	return ret;
}

void CAliRecognizer::DoDestroyRecogRequest()
{
	if (m_Request) {
		if (m_Flag) {
			//直接断开，不等服务返回 遗留接口，在sdk2.0前，有过如果任务失败，阿里服务器不会返回，只能强制断开
			LOG_INFO("wiat for Ali server disconnect --> cancel")
				m_Request->cancel();
		}
		else {
			LOG_INFO("wiat for Ali server disconnect --> stop")
				m_Request->stop();
		}

		NlsClient::getInstance()->releaseRecognizerRequest(m_Request);
		DoCleanAliPram();
		m_Request = nullptr;
		m_Flag = FALSE;
	}


	LOG_INFO("<==== wiat for Ali server disconnect succee ====>");

	return;
}

const char * CAliRecognizer::DoRecogResultGet()const
{
	return m_result.c_str();
}

int	CAliRecognizer::DoWriteFrame(const mpf_frame_t *frame)
{
	if (nullptr == m_Request || nullptr == frame)
		return 0;

	return m_Request->sendAudio(static_cast<const char *>(frame->codec_frame.buffer), frame->codec_frame.size);
}

void CAliRecognizer::DoSetChannel(CAliChannel * AliCh)
{
	if (nullptr == AliCh)
		return;
	m_AliCh = AliCh;
	return;
}

