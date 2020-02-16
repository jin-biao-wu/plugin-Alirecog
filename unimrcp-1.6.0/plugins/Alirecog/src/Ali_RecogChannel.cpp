#include"Ali_RecogChannel.h"

CAliChannel::CAliChannel()noexcept:	m_id(0), 
									m_pool(nullptr), 
									m_FrameQueue(nullptr), 
									m_file(nullptr),
									m_mutex(nullptr),
									m_result(""),
									m_appkey(""),
									m_AccessKey(""),
									m_Secret(""),
									m_Format(""),
									m_token(""),
									m_SampleRate(""),
									m_intermediateResult(""),
									m_inverseText(""),
									m_voiceDetection(""),
									m_maxEndSilence(""),
									m_maxStartSilence(""),
									m_PunctuationPrediction(""),
									m_CustomizationId(""),
									m_VocabularyId(""),
									m_OutputFormat(""),
									m_ContextParam(""),
									m_record(FALSE),
									m_recordPath(""),
									m_expireTime(0),
									m_Flag(FALSE),
									m_starting(FALSE),
									m_Channel(nullptr),
									m_recogCallback(nullptr),
									m_recogRequest(nullptr)
{

}

CAliChannel::~CAliChannel() noexcept
{

}

bool CAliChannel::Init(	unsigned int id,
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
						apt_bool_t   record)
{

	if (nullptr == pool) {
		return FALSE;
	}

	m_id = id;
	m_pool = pool;
	m_appkey = appkey;
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
	m_recordPath = recordPath;
	m_record = record;

	if (apr_thread_mutex_create(&m_mutex, APR_THREAD_MUTEX_DEFAULT, m_pool) != APR_SUCCESS) {
		LOG_ERROR("Ali recog Channel Init mutex failed id:%d ", m_id);
		return FALSE;
	}

	if (apr_queue_create(&m_FrameQueue, MAX_QUEUE_SIZE, m_pool) != APR_SUCCESS) {
		LOG_ERROR("Ali recog Channel Init frame queue failed id:%d ", m_id);
		return FALSE;
	}

	m_recogCallback = new SpeechRecognizerCallback;
	if (m_recogCallback) {
		m_recogCallback->setOnRecognitionStarted(OnRecognitionStarted, this);
		m_recogCallback->setOnRecognitionResultChanged(OnRecognitionResultChanged, this);
		m_recogCallback->setOnRecognitionCompleted(OnRecognitionCompleted, this);
		m_recogCallback->setOnChannelClosed(OnChannelClosed, this);
		m_recogCallback->setOnTaskFailed(OnTaskFailed, this);
	}
	// ...

	LOG_INFO("Channel Init ChId:%d appKey:%s AccessKeyID:%s AccessKeySecret:%s Format:%s SampleRate:%s\
			IntermediateResult:%s PunctuationPrediction:%s InverseTextNormalization:%s EnableVoiceDetection:%s \
			StartSilence:%s EndSilence:%s CustomizationId:%s VocabularyId:%s OutputFormat:%s ContextParam:%s",
			m_id, m_appkey.c_str(), m_AccessKey.c_str(), m_Secret.c_str(), m_Format.c_str(), m_SampleRate.c_str(),
			m_intermediateResult.c_str(), m_PunctuationPrediction.c_str(), m_inverseText.c_str(), m_voiceDetection.c_str(),
			m_maxStartSilence.c_str(), m_maxEndSilence.c_str(), m_CustomizationId.c_str(), m_VocabularyId.c_str(), m_OutputFormat.c_str(),
			m_ContextParam.c_str());

	return TRUE;
}

void CAliChannel::Uinit()
{
	if (nullptr != m_file) {
		apr_file_close(m_file);
	}
	if (nullptr != m_mutex) {
		apr_thread_mutex_destroy(m_mutex);
	}
	if (nullptr != m_recogCallback) {
		delete m_recogCallback;
	}
	if (nullptr != m_FrameQueue) {
		apr_queue_term(m_FrameQueue);
	}
}

bool CAliChannel::Start(EngineChannel* Ch)
{
	if (nullptr == Ch)
		return FALSE;

	lock();

	m_Channel = Ch;
	m_Flag = FALSE;
	m_starting = TRUE;
	m_result.clear();
	//清理上次残余的原始数据
	while (apr_queue_size(m_FrameQueue) != 0) {
		mpf_frame_t *frame = nullptr;
		apr_queue_pop(m_FrameQueue, (void **)&frame);
	}

	if (m_record) {
		char szFileName[255] = { 0 };
		string FileName = "";
		const mpf_codec_descriptor_t *descriptor = mrcp_engine_sink_stream_codec_get(m_Channel->channel);
		apr_snprintf(szFileName, sizeof(szFileName), "[recog]-[Channel-%s]-[%d-Hz]-[Date-%s].pcm",
			m_Channel->channel->id.buf, descriptor->sampling_rate, CAliUtil::aprTimeToStr(apr_time_now(), "%Y-%m-%d %H-%M-%S").c_str());

		if (m_recordPath[m_recordPath.length() - 1] == PATHDIA) {
			FileName = m_recordPath.append(szFileName);
		}
		else {
			FileName = m_recordPath + PATHDIA + szFileName;
		}

		RecordStart(FileName);
		LOG_INFO("Record Start  File:%s", FileName.c_str());
	}

	ulock();

	return TRUE;
}

void CAliChannel::Stop()
{
	lock();

	m_starting = FALSE;
	apr_queue_interrupt_all(m_FrameQueue);

	ulock();
	return;
}


void CAliChannel::RecogMain()
{
	if (!DoRecogRequestInit()) {
		LOG_ERROR("<--- RecogMain init recog request failed --->");
		goto END;
	}

	while (m_starting)
	{
		if (!DoCheakRecogToKen()) {
			LOG_ERROR("<--- RecogMain Cheak Recog ToKen failed --->");
			break;
		}

		mpf_frame_t *frame = nullptr;
		apr_status_t stu = apr_queue_pop(m_FrameQueue, (void **)&frame);
		if (APR_EOF == stu) {
			LOG_WARN("APR_EOF Frame Queue exit Msg:%s",CAliUtil::aprErrorToStr(stu).c_str());
			break;
		}
		else if (APR_EINTR == stu) {
			break;
		}

		if (nullptr != frame && nullptr != m_recogRequest) {
			//实时数据无需计算休眠
			if (m_recogRequest->sendAudio((const char *)frame->codec_frame.buffer, frame->codec_frame.size) != -1 ) {
				RecordMain(frame);
			}
			else {
				break;
			}
		}
	}

END:

	lock();

	DoDestroyRecogRequest();
	RecordStop();
	m_token.clear();
	m_starting = FALSE;
	m_expireTime = 0;
	m_Channel = nullptr;

	ulock();

	LOG_INFO("<---- [ Channel:%d recog exit ] ---->", m_id);
	return;
}
void CAliChannel::RecordStart(string & VoiceFilePaht)
{
	if (m_record) {
		apr_status_t stu = apr_file_open(&m_file, VoiceFilePaht.c_str(),
			APR_FOPEN_CREATE | APR_FOPEN_WRITE | APR_FOPEN_BINARY, APR_FPROT_OS_DEFAULT,
			m_pool);

		if (APR_SUCCESS != stu) {
			LOG_WARN("record error Msg :%s status:%d", CAliUtil::aprErrorToStr(stu).c_str(), stu);
			m_file = nullptr;
			m_record = FALSE;
		}
	}
	return;
}

void CAliChannel::RecordMain(const mpf_frame_t *frame)
{
	if (m_record && m_file) {
		apr_size_t size = frame->codec_frame.size;
		if (apr_file_write(m_file, frame->codec_frame.buffer, &size) != APR_SUCCESS) {
			LOG_WARN("record file write data failed write size :%d...",size);
		}
	}
	return;
}

void CAliChannel::RecordStop()
{
	if (nullptr != m_file) {
		apr_file_close(m_file);
	}
	return;
}

apr_status_t CAliChannel::WriteFrame(const mpf_frame_t *frame)
{
	apr_status_t stu = APR_SUCCESS;
	if (m_FrameQueue && m_starting) {
		stu = apr_queue_push(m_FrameQueue, (void*)frame);
	}
	return stu;
}

inline  void CAliChannel::OnRecognitionCompleted(NlsEvent* ev, void* AliCh)
{
	if (AliCh) {
		CAliChannel* Ch = static_cast<CAliChannel*>(AliCh);
		Ch->OnRecognitionCompleted(ev);
	}
	return;
}

inline  void CAliChannel::OnRecognitionStarted(NlsEvent* ev, void* AliCh)
{
	if (AliCh) {
		CAliChannel* Ch = static_cast<CAliChannel*>(AliCh);
		Ch->OnRecognitionStarted(ev);
	}
	return;
}

inline  void CAliChannel::OnRecognitionResultChanged(NlsEvent* ev, void* AliCh)
{
	if (AliCh) {
		CAliChannel* Ch = static_cast<CAliChannel*>(AliCh);
		Ch->OnRecognitionResultChanged(ev);
	}
	return;
}

inline  void CAliChannel::OnTaskFailed(NlsEvent* ev, void* AliCh)
{
	if (AliCh) {
		CAliChannel* Ch = static_cast<CAliChannel*>(AliCh);
		Ch->OnTaskFailed(ev);
	}
	return;
}

inline  void CAliChannel::OnChannelClosed(NlsEvent* ev, void* AliCh)
{
	if (AliCh) {
		CAliChannel* Ch = static_cast<CAliChannel*>(AliCh);
		Ch->OnChannelClosed(ev);
	}
	return;
}

void CAliChannel::OnRecognitionCompleted(NlsEvent* ev)
{
	m_result = ev->getResult();
	return;
}

void CAliChannel::OnRecognitionStarted(NlsEvent* ev)
{
	return;
}

void CAliChannel::OnRecognitionResultChanged(NlsEvent* ev)
{
	return;
}

void CAliChannel::OnTaskFailed(NlsEvent* ev)
{
	Stop();
	m_Flag = TRUE;
	return;
}

void CAliChannel::OnChannelClosed(NlsEvent* ev)
{
	return;
}

int	CAliChannel::DoAliToKenGet(string& AccessKeyID, string& AccessKeySecret)
{
	NlsToken nlsTokenRequest;
	nlsTokenRequest.setAccessKeyId(AccessKeyID);
	nlsTokenRequest.setKeySecret(AccessKeySecret);

	if (nlsTokenRequest.applyNlsToken() == -1) {
		LOG_ERROR("");
		return -1;
	}

	m_token = nlsTokenRequest.getToken();
	m_expireTime = nlsTokenRequest.getExpireTime();
	return 0;
}

apt_bool_t	CAliChannel::DoRecogRequestInit()
{
	apt_bool_t ret = TRUE;

	if (nullptr == m_recogRequest) {
		if (!DoCreateRecogRequest()) {
			LOG_ERROR("<--- DoCreateRecogRequest falied --->");
			ret = FALSE;
		}
	}

	if (m_recogRequest && ret) {
		if (!DoCheakRecogToKen()) {
			LOG_ERROR("<--- DoCheakRecogToKen falied --->");
			ret = FALSE;
		}
		else {
			m_recogRequest->setAppKey(m_appkey.c_str());
			m_recogRequest->setFormat(m_Format.c_str());
			m_recogRequest->setSampleRate((m_SampleRate == "8000" ? 8000 : 16000));
			m_recogRequest->setIntermediateResult(((m_intermediateResult == "true") ? true : false)); //返回中间结果
			m_recogRequest->setEnableVoiceDetection(((m_voiceDetection == "true") ? true : false)); //开启语音检查

			if (m_voiceDetection == "true") {
				int EndSilence = atoi(m_maxEndSilence.c_str());
				m_recogRequest->setMaxEndSilence(EndSilence * 1000);//最大说话后的静音时长

				int StartSilence = atoi(m_maxStartSilence.c_str());
				m_recogRequest->setMaxStartSilence(StartSilence * 1000); //最大开始静音的时长
			}

			m_recogRequest->setPunctuationPrediction(((m_PunctuationPrediction == "true") ? true : false));
			m_recogRequest->setInverseTextNormalization(((m_inverseText == "true") ? true : false));

			if (!m_CustomizationId.empty())
			{
				m_recogRequest->setCustomizationId(m_CustomizationId.c_str());
			}

			if (!m_VocabularyId.empty())
			{
				m_recogRequest->setVocabularyId(m_VocabularyId.c_str());
			}

			if (!m_OutputFormat.empty())
			{
				m_recogRequest->setOutputFormat(m_OutputFormat.c_str());
			}

			if (!m_ContextParam.empty())
			{
				m_recogRequest->setContextParam(m_ContextParam.c_str());
			}


			if (m_recogRequest->start() < 0) {
				LOG_ERROR("<--- Ali server recog start failed --->");
				ret = FALSE;
			}
		}

	}

	return ret;
}

apt_bool_t	CAliChannel::DoCreateRecogRequest()
{
	if ((nullptr != m_recogCallback) && (nullptr == m_recogRequest)) {
		m_recogRequest = NlsClient::getInstance()->createRecognizerRequest(m_recogCallback);
	}

	if (nullptr == m_recogRequest) {
		m_recogRequest = nullptr;
		return FALSE;
	}
	else {
		return TRUE;
	}
}

void CAliChannel::DoDestroyRecogRequest()
{
	if (m_recogRequest) {
		if (m_Flag) {
			//直接断开，不等服务返回
			LOG_INFO("wiat for Ali server disconnect --> cancel")
			m_recogRequest->cancel();
		}
		else {
			LOG_INFO("wiat for Ali server disconnect --> stop")
			m_recogRequest->stop();
		}

		NlsClient::getInstance()->releaseRecognizerRequest(m_recogRequest);
		m_recogRequest = nullptr;
	}


	LOG_INFO("<==== wiat for Ali server disconnect succee ====>");

	return;
}

apt_bool_t	CAliChannel::DoCheakRecogToKen()
{
	time_t curTime = time(0);
	bool ret = TRUE;
	if ((m_expireTime - curTime < 10) || m_token.empty()) {
		if (-1 == DoAliToKenGet(m_AccessKey, m_Secret)) {
			ret = FALSE;
		}
		else {
			if (m_recogRequest) {
				m_recogRequest->setToken(m_token.c_str());
			}
		}
	}
	return ret;
}













