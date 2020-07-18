#include"Ali_RecogTranscriber.h"



CAliTranscriber::CAliTranscriber() noexcept
{

}


CAliTranscriber::~CAliTranscriber() noexcept
{
	if (m_CallBack) {
		delete m_CallBack;
		m_CallBack = nullptr;
	}

	if (m_lock != nullptr) {
		apr_thread_mutex_destroy(m_lock);
		m_lock = nullptr;
	}
}


void CAliTranscriber::OnTranscriptionStarted(NlsEvent* ev)
{

}

void CAliTranscriber::OnTranscriptionResultChanged(NlsEvent* ev)
{
	lock();
	string result = ev->getResult();
	if (result.length() != 0)
		m_resultQueue.push(result);
	//LOG_INFO("m_resultQueue size :%d", m_resultQueue.size());
	ulock();
}

void CAliTranscriber::OnTranscriptionCompleted(NlsEvent* ev)
{
	lock();
	string result = ev->getResult();
	if(result.length() != 0)
		m_resultQueue.push(result);
	//LOG_INFO("m_resultQueue size :%d", m_resultQueue.size());
	ulock();
}

void CAliTranscriber::OnSentenceBegin(NlsEvent* ev)
{

}

void CAliTranscriber::OnSentenceEnd(NlsEvent* ev)
{
	lock();
	string result = ev->getResult();
	if (result.length() != 0)
		m_resultQueue.push(result);
	//LOG_INFO("m_resultQueue size :%d", m_resultQueue.size());
	ulock();
}

void CAliTranscriber::OnChannelClosed(NlsEvent* ev)
{

}

void CAliTranscriber::OnTaskFailed(NlsEvent* ev)
{
	m_Flag = TRUE;
	LOG_ERROR("Ali Task Start Failed Msg :%s", ev->getErrorMessage());
}


bool CAliTranscriber::DoRecogRequestInit(	const string& appKey,
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
											apr_pool_t*	  pool)
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

	m_pool = pool;

	if (apr_thread_mutex_create(&m_lock, APR_THREAD_MUTEX_DEFAULT, m_pool) != APR_SUCCESS) {
		LOG_ERROR("init CAiTranscriber Failed ...");
		m_lock = nullptr;
		return FALSE;
	}

	m_CallBack = new  SpeechTranscriberCallback;
	if (m_CallBack) {
		m_CallBack->setOnChannelClosed(CAliResource::OnChannelClosed, this);
		m_CallBack->setOnTaskFailed(CAliResource::OnTaskFailed, this);
		m_CallBack->setOnTranscriptionStarted(CAliResource::OnTranscriptionStarted, this);// 设置识别启动回调函数
		m_CallBack->setOnTranscriptionResultChanged(CAliResource::OnTranscriptionResultChanged, this);// 设置识别结果变化回调函数
		m_CallBack->setOnTranscriptionCompleted(CAliResource::OnTranscriptionCompleted, this);// 设置语音转写结束回调函数
		m_CallBack->setOnSentenceBegin(CAliResource::OnSentenceBegin, this);// 设置一句话开始回调函
		m_CallBack->setOnSentenceEnd(CAliResource::OnSentenceEnd, this);// 设置一句话结束回调函数
	}

	while (m_resultQueue.size() != 0) {
		m_resultQueue.pop();
	}
	LOG_INFO("Init Recog Resource --> Transcriber");
	return TRUE;
}

bool CAliTranscriber::DoCreateRecogRequest()
{
	bool ret = TRUE;

	//清理上次残余的数据
	while (m_resultQueue.size() != 0) {
		m_resultQueue.pop();
	}

	if ((nullptr != m_CallBack) && (nullptr == m_Request)) {
		m_Request = NlsClient::getInstance()->createTranscriberRequest(m_CallBack);
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

			m_Request->setToken(m_token.c_str());
			m_Request->setAppKey(m_appkey.c_str());
			m_Request->setFormat(m_Format.c_str());
			m_Request->setSampleRate((m_SampleRate == "8000") ? 8000 : 16000);
			m_Request->setIntermediateResult((m_intermediateResult == "true") ? true : false);
			m_Request->setPunctuationPrediction((m_PunctuationPrediction == "true") ? true : false);
			m_Request->setInverseTextNormalization(true);		
			//m_Request->setIntermediateResult(((m_intermediateResult == "true") ? true : false)); //返回中间结果
			////m_Request->setEnableVoiceDetection(((m_voiceDetection == "true") ? true : false)); //开启语音检查
			//m_Request->setPunctuationPrediction(((m_PunctuationPrediction == "true") ? true : false));// 设置是否在后处理中添加标点, 可选参数. 默认false
			//m_Request->setInverseTextNormalization(((m_inverseText == "true") ? true : false));
			//m_Request->setSemanticSentenceDetection(false); // 设置是否语义断句, 可选参数. 默认false
			m_Request->setMaxSentenceSilence(800);

			//if (!m_CustomizationId.empty())
			//{
			//	m_Request->setCustomizationId(m_CustomizationId.c_str());
			//}

			//if (!m_VocabularyId.empty())
			//{
			//	m_Request->setVocabularyId(m_VocabularyId.c_str());
			//}
			if (m_Request->start() < 0) {
				LOG_ERROR("<--- Ali  Transcriber Start Failed --->");
				ret = FALSE;
			}
		}
	}
	return ret;
}

void CAliTranscriber::DoDestroyRecogRequest()
{
	if (m_Request) {
		if (m_Flag) {
			
			//直接断开，不等服务返回
			LOG_INFO("Wiat For Ali  Disconnect --> cancel")
				m_Request->cancel();
		}
		else {
			LOG_INFO("Wiat For Ali  Disconnect --> stop")
				m_Request->stop();
		}

		NlsClient::getInstance()->releaseTranscriberRequest(m_Request);
		DoCleanAliPram();
		m_Request = nullptr;
		m_Flag = FALSE;
	}


	LOG_INFO("<==== Wiat For Ali  Disconnect Succee ====>");
}

const string CAliTranscriber::DoRecogResultGet()
{
	string result;
	if (m_resultQueue.size() != 0) {
		lock();
		result = m_resultQueue.front();
		m_resultQueue.pop();
		ulock();
	}
	//LOG_INFO("CAliRecognizer::DoRecogResultGet :%s size:%s ", result.c_str(), m_resultQueue.size());
	return result;
}

int	CAliTranscriber::DoWriteFrame(const mpf_frame_t *frame)
{
	if (nullptr == m_Request || nullptr == frame)
		return 0;
	
	return m_Request->sendAudio(static_cast<const char *>(frame->codec_frame.buffer), frame->codec_frame.size);
}



