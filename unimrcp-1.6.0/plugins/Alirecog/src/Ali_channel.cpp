#include"Ali_channel.h"
//#include"Ali_define.h"

AliChannel::AliChannel():	
						 m_id(0)
						,m_appkey("")
						, m_AccessKey("")
						,m_Secret("")
						,m_Format("pcm")
						, m_token("")
						,m_SampleRate("8000")
						,m_intermediateResult("")
						, m_inverseText("")
						, m_voiceDetection("")
						, m_maxEndSilence("0")
						, m_maxStartSilence("3")
						, m_PunctuationPrediction("")
						, m_CustomizationId("")
						, m_VocabularyId("")
						, m_OutputFormat("")
						, m_ContextParam("")
						, m_expireTime(0)
						,m_Runing(FALSE)
						, m_Starting(FALSE)
						, m_reocgFlag(FALSE)
						,m_record(FALSE)
						, m_recordPath("../")
						,m_file(NULL)
						//,m_tid(NULL)
						,m_Mutex(NULL)
						,m_pool(NULL)
						, m_FrameQueue(NULL)
						, m_recogCallback(NULL)
						, m_recogRequest(NULL)
{

}

AliChannel::AliChannel(unsigned int id,
						const char* AppKid,
						const char * AccessKeyID,
						const char * AccessKeySecret,
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
						const char* recordPath,
						apt_bool_t record):
						m_id(id)
						, m_appkey(AppKid)
						, m_AccessKey(AccessKeyID)
						, m_Secret(AccessKeySecret)
						, m_Format(Format)
						, m_token("")
						, m_SampleRate(SampleRate)
						, m_intermediateResult(intermediateResult)
						, m_inverseText(inverseText)
						, m_voiceDetection(voiceDetection)
						, m_maxEndSilence(maxEndSilence)
						, m_maxStartSilence(maxStartSilence)
						, m_PunctuationPrediction(PunctuationPrediction)
						, m_CustomizationId(CustomizationId)
						, m_VocabularyId(VocabularyId)
						, m_OutputFormat(OutputFormat)
						, m_ContextParam(ContextParam)
						, m_expireTime(0)
						, m_Runing(FALSE)
						, m_Starting(FALSE)
						, m_reocgFlag(FALSE)
						//, m_tid(NULL)
						, m_record(record)
						, m_recordPath(recordPath)
						, m_file(NULL)
						, m_Mutex(NULL)
						, m_pool(NULL)
						, m_FrameQueue(NULL)
						, m_recogCallback(NULL)
						,m_recogRequest(NULL)
{

}



AliChannel::~AliChannel()
{

}

bool	AliChannel::Init(apr_pool_t* pool)
{
	if (nullptr == pool) {
		return FALSE;
	}
	m_pool = pool;

	if (apr_thread_mutex_create(&m_Mutex, APR_THREAD_MUTEX_DEFAULT, m_pool) != APR_SUCCESS) {
		LOG_ERROR("Ali recog Channel Init mutex failed id:%d ",m_id);
		return FALSE;
	}

	if (apr_queue_create(&m_FrameQueue, 100, m_pool) != APR_SUCCESS) {
		LOG_ERROR("Ali recog Channel Init frame queue failed id:%d ", m_id);
		return FALSE;
	}

	m_recogCallback = new SpeechRecognizerCallback();
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

void AliChannel::Uninit()
{
	if (nullptr != m_file) {
		apr_file_close(m_file);
	}
	if (nullptr != m_Mutex) {
		apr_thread_mutex_destroy(m_Mutex);
	}
	if (nullptr != m_recogCallback) {
		delete m_recogCallback;
	}
	if (nullptr != m_FrameQueue) {
		apr_queue_term(m_FrameQueue);
	}
}

apt_bool_t	AliChannel::Start()
{
	//...
	Thread_Mutex_Lock();

	m_Runing = TRUE;
	m_Starting = TRUE;
	if (m_record) {
		char szFileName[255] = { 0 };
		string FileName = "";
		const mpf_codec_descriptor_t *descriptor = mrcp_engine_sink_stream_codec_get(m_Ch->channel);
		apr_snprintf(szFileName, sizeof(szFileName), "[recog]-[Channel-%s]-[%d-Hz]-[Date-%s].pcm",
			m_Ch->channel->id.buf, descriptor->sampling_rate, TimeToStr(apr_time_now(), "%Y-%m-%d %H-%M-%S").c_str());

		if (m_recordPath[m_recordPath.length() - 1] == PATHDIA) {
			FileName = m_recordPath.append(szFileName);
		}
		else {
			FileName = m_recordPath + PATHDIA + szFileName;
		}

		RecordStart(FileName);
		LOG_INFO("Record Start  File:%s", FileName.c_str());	
	}



	Thread_Mutex_ULock();

	return TRUE;
}

apt_bool_t	AliChannel::Stop()
{
	Thread_Mutex_Lock();

	m_expireTime = 0;
	m_Ch = NULL;
	m_reocgFlag = FALSE;
	m_token.clear();
	apr_queue_interrupt_all(m_FrameQueue);
	//apr_queue_term(m_FrameQueue);

	Thread_Mutex_ULock();
	return true;
}


apr_size_t AliChannel::GetChannelId()const
{
	return m_id;
}

apt_bool_t AliChannel::IsWork()const
{
	return m_Starting;
}

apr_status_t AliChannel::WriteFrame(const mpf_frame_t *frame)
{
	apr_status_t stu = APR_SUCCESS;
	if (m_FrameQueue && m_Runing) {
			stu = apr_queue_push(m_FrameQueue,(void*)frame);
	}
	return stu;
}

int	AliChannel::GetAliToKen(string&AccessKeyID, string&AccessKeySecret)
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


void AliChannel::OnRecognitionCompleted(NlsEvent* ev, void* arg)
{
	AliChannel *pCh =(AliChannel *) arg;
	if (pCh) {
		pCh->OnRecognitionCompleted(ev);
	}
}

void AliChannel::OnRecognitionStarted(NlsEvent* ev, void* arg)
{
	AliChannel *pCh = (AliChannel *)arg;
	if (pCh) {
		pCh->OnRecognitionStarted(ev);
	}
}

void AliChannel::OnRecognitionResultChanged(NlsEvent* ev, void* arg)
{
	AliChannel *pCh = (AliChannel *)arg;
	if (pCh) {
		pCh->OnRecognitionResultChanged(ev);
	}
}

void AliChannel::OnTaskFailed(NlsEvent* ev, void* arg)
{
	AliChannel *pCh = (AliChannel *)arg;
	if (pCh) {
		pCh->OnTaskFailed(ev);
	}
}

void AliChannel::OnChannelClosed(NlsEvent* ev, void* arg)
{
	AliChannel *pCh = (AliChannel *)arg;
	if (pCh) {
		pCh->OnChannelClosed(ev);
	}
}

apt_bool_t	 AliChannel::OnCreateRecogRequest()
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

apt_bool_t AliChannel::OnRecogRequestInit()
{
	//...
	apt_bool_t ret = TRUE;

	if (nullptr == m_recogRequest) {
		if (!OnCreateRecogRequest()) {
			LOG_ERROR("<--- OnCreateRecogRequest falied --->");
			ret = FALSE;
		}
	}

	if (m_recogRequest && ret) {
		if (!OnCheakRecogToKen()) {
			LOG_ERROR("<--- OnCheakRecogToKen --->");
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
			{m_recogRequest->setCustomizationId(m_CustomizationId.c_str());}

			if(!m_VocabularyId.empty())
			{m_recogRequest->setVocabularyId(m_VocabularyId.c_str());}

			if (!m_OutputFormat.empty()) 
			{m_recogRequest->setOutputFormat(m_OutputFormat.c_str());}

			if(!m_ContextParam.empty())
			{m_recogRequest->setContextParam(m_ContextParam.c_str());}


			if (m_recogRequest->start() < 0) {
				LOG_ERROR("<--- Ali server recog start failed --->");
				ret = FALSE;
			}
		}

	}

	return ret;
}

void AliChannel::OnRecognitionCompleted(NlsEvent* ev)
{
	LOG_DEBUG("<------------- Ali Ch:%d reocg Completed result :%s ------------>", m_id,ev->getResult());

	Json::Value root;
	Json::FastWriter write;

	root["result"] = string(ev->getResult()).empty() ? "NO_RESULT" : ev->getResult();
	string Result = write.write(root);
	OnMrcpRecogCompleted(Result);

	Thread_Mutex_Lock();
	//m_Runing = FALSE;
	m_reocgFlag = TRUE;
	Thread_Mutex_ULock();
}

void AliChannel::OnRecognitionStarted(NlsEvent* ev)
{
	LOG_DEBUG("<------------- Ali Ch:%d reocg Start ------------>", m_id);
}

void AliChannel::OnRecognitionResultChanged(NlsEvent* ev)
{
	LOG_DEBUG("<------------- Ali Ch:%d reocg ResultChanged result :%s ------------>", m_id,ev->getResult());
}

void AliChannel::OnTaskFailed(NlsEvent* ev)
{
	LOG_ERROR("<------------- Ali Ch:%d reocg Failed ------------>", m_id);
	LOG_ERROR("<---[ Ali TaskFailed Error Msg :%s statusCode:%d ] --->", ev->getErrorMessage(), ev->getStausCode())

	Thread_Mutex_Lock();
	//m_Runing = FALSE;
	m_reocgFlag = TRUE;
	Thread_Mutex_ULock();

}

void AliChannel::OnChannelClosed(NlsEvent* ev)
{
	LOG_DEBUG("<------------- Ali Ch:%d reocg Closed ------------>", m_id);
	Thread_Mutex_Lock();
	//m_Runing = FALSE;
	m_reocgFlag = TRUE;
	Thread_Mutex_ULock();
}

void* APR_THREAD_FUNC AliChannel::RecogMain(apr_thread_t*tid, void* This)
{
	AliChannel * pCh = (AliChannel*)This;
	if (nullptr != pCh) {
		pCh->RecogMain();
	}

	return nullptr;
}
void AliChannel::RecogMain()
{

	if (!OnRecogRequestInit()) {
		LOG_ERROR("<--- RecogMain init recog request failed --->");
		goto END;
	}

	while (m_Starting) {

		if (!OnCheakRecogToKen()) {
			LOG_ERROR("<--- RecogMain Cheak Recog ToKen failed --->");
			break;
		}

		mpf_frame_t *frame = nullptr;
		apr_status_t stu = apr_queue_pop(m_FrameQueue,(void **) &frame);
		if (APR_EOF == stu) {
			RecogExit();
			LOG_WARN("APR_EOF Frame Queue exit Msg:%s",apr_error(stu).c_str());
			break;
		}
		else if (APR_EINTR == stu) {			
			RecogExit();
			LOG_INFO("APR_EINTR Frame Queue exit Msg:%s", apr_error(stu).c_str());
			break;
		}
		else  {
			if (nullptr != frame) {
				//实时数据无需计算休眠
				if (nullptr != m_recogRequest) {
					if (m_recogRequest->sendAudio((const char*)frame->codec_frame.buffer,
						frame->codec_frame.size) == -1) {
						RecogExit();
						LOG_ERROR("<---- send Audio failed --->");
						break;
					}
				}

				RecordMain(frame);
			}

		}	
	}

END:

	OnDestroyRecogRequest();

	Thread_Mutex_Lock();
	RecordClose();
	m_Starting = FALSE;
	m_Runing = FALSE;
	m_expireTime = 0;
	m_Ch = NULL;
	m_reocgFlag = FALSE;
	m_token.clear();
	Thread_Mutex_ULock();
	LOG_INFO("<---- [ Channel:%d recog exit ] ---->", m_id);
	return;
}

void AliChannel::RecordStart(string & VoiceFilePaht)
{
	if (m_record) {
		apr_status_t stu = apr_file_open(&m_file, VoiceFilePaht.c_str(),
			APR_FOPEN_CREATE | APR_FOPEN_WRITE | APR_FOPEN_BINARY, APR_FPROT_OS_DEFAULT,
			m_pool);

		if (APR_SUCCESS != stu) {
			LOG_WARN("error Msg :%s status:%d", apr_error(stu).c_str(),stu);
			m_file = NULL;
			m_record = FALSE;
		}
	}
}

void AliChannel::RecordMain(const mpf_frame_t *frame)
{
	if (m_record && m_file) {
		if (apr_file_write(m_file, frame->codec_frame.buffer, const_cast<apr_size_t *>(&frame->codec_frame.size)) != APR_SUCCESS) {
			LOG_WARN("record file write data failed ...");
		}
	}
}

void AliChannel::RecordClose()
{
	if (nullptr != m_file) {
		apr_file_close(m_file);
	}
}

void AliChannel::RecogExit()
{
	Thread_Mutex_Lock();
	m_Runing = FALSE;
	Thread_Mutex_ULock();
	return;
}

void AliChannel::Thread_Mutex_Lock()
{
	apr_thread_mutex_lock(m_Mutex);
}

void AliChannel::Thread_Mutex_ULock()
{
	apr_thread_mutex_unlock(m_Mutex);
}

apt_bool_t	AliChannel::OnCheakRecogToKen()
{
	time_t curTime = time(0);
	bool ret = TRUE;
	if ((m_expireTime - curTime < 10) || m_token.empty()) {
		if (-1 == GetAliToKen(m_AccessKey, m_Secret)) {
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

void AliChannel::OnDestroyRecogRequest()
{
	if (m_recogRequest) {
		LOG_INFO("wiat for ali server disconnect");
		if (m_reocgFlag) {
			LOG_INFO("<==== wiat for ali server disconnect stop ====> ");
			m_recogRequest->stop();
		}
		else {
			//直接断开，不等服务返回
			LOG_INFO("<==== wiat for ali server disconnect cancel ====>");
			m_recogRequest->cancel();
		}
		NlsClient::getInstance()->releaseRecognizerRequest(m_recogRequest);
		m_recogRequest = nullptr;
		LOG_INFO("wiat for ali server disconnect succee");
	}
}


void AliChannel::OnMrcpRecogCompleted(string&  result)
{
	//创建发送 mrcp的识别完成的事件消息
	if (nullptr == m_Ch) {
		return;
	}

	mrcp_recog_header_t *recog_header;
	mrcp_message_t *message = mrcp_event_create(
		m_Ch->recog_request,
		RECOGNIZER_RECOGNITION_COMPLETE,
		m_Ch->recog_request->pool);
	if (!message) {
		LOG_ERROR("");
		return;
	}

	//创建资源头
	recog_header = (mrcp_recog_header_t *)mrcp_resource_header_prepare(message);
	if (recog_header) {
		//先事件头添加完成码
		recog_header->completion_cause = RECOGNIZER_COMPLETION_CAUSE_SUCCESS;
		mrcp_resource_header_property_add(message, RECOGNIZER_HEADER_COMPLETION_CAUSE);
	}
	//添加识别状态
	message->start_line.request_state = MRCP_REQUEST_STATE_COMPLETE;

	//添加结果
	apt_string_assign_n(&message->body, result.c_str(), result.length(), message->pool);

	//添加结果的格式
	mrcp_generic_header_t *generic_header = mrcp_generic_header_prepare(message);
	if (generic_header) {
		apt_string_assign(&generic_header->content_type, "application/json", message->pool);
		mrcp_generic_header_property_add(message, GENERIC_HEADER_CONTENT_TYPE);
	}

	//m_Ch->recog_request = NULL;
	//发送异步事件
	mrcp_engine_channel_message_send(m_Ch->channel, message);

	return;
}



