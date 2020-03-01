#include"Ali_RecogChannel.h"

CAliChannel::CAliChannel()noexcept:	m_id(0), 
									m_pool(nullptr), 
									m_FrameQueue(nullptr), 
									m_file(nullptr),
									m_mutex(nullptr),
									m_Recoger(nullptr),
									m_record(FALSE),
									m_recordPath(""),
									m_starting(FALSE),
									m_Channel(nullptr)									
{

}

CAliChannel::~CAliChannel() noexcept
{

}

bool CAliChannel::Init(unsigned int id,
	apr_pool_t* pool,
	const char *recordPath,
	apt_bool_t  record
						)
{

	if (nullptr == pool) {
		return FALSE;
	}

	m_id = id;
	m_pool = pool;
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
	/*
	//LOG_INFO("Channel Init ChId:%d appKey:%s AccessKeyID:%s AccessKeySecret:%s Format:%s SampleRate:%s\
	//		IntermediateResult:%s PunctuationPrediction:%s InverseTextNormalization:%s EnableVoiceDetection:%s \
	//		StartSilence:%s EndSilence:%s CustomizationId:%s VocabularyId:%s OutputFormat:%s ContextParam:%s",
	//		m_id, m_appkey.c_str(), m_AccessKey.c_str(), m_Secret.c_str(), m_Format.c_str(), m_SampleRate.c_str(),
	//		m_intermediateResult.c_str(), m_PunctuationPrediction.c_str(), m_inverseText.c_str(), m_voiceDetection.c_str(),
	//		m_maxStartSilence.c_str(), m_maxEndSilence.c_str(), m_CustomizationId.c_str(), m_VocabularyId.c_str(), m_OutputFormat.c_str(),
	//		m_ContextParam.c_str());
	*/
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

	if (nullptr != m_FrameQueue) {
		apr_queue_term(m_FrameQueue);
	}
}

bool CAliChannel::Start(EngineChannel* Ch, CAliResource* resource)
{
	if (nullptr == Ch || nullptr == resource)
		return FALSE;

	lock();

	m_Channel = Ch;
	m_starting = TRUE;
	m_Recoger = resource;
	m_Recoger->DoSetChannel(this);
	//清理上次残余的原始数据
	//LOG_INFO(" Frame Queue size :%d", apr_queue_size(m_FrameQueue));
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
	if (!m_Recoger->DoCreateRecogRequest()){
		LOG_ERROR("<--- RecogMain Create recog request failed --->");
		goto END;
	}

	//if (!m_Recoger->DoRecogRequestInit()) {
	//	LOG_ERROR("<--- RecogMain Init recog request failed --->");
	//	goto END;
	//}

	while (m_starting)
	{
		if (!m_Recoger->DoCheakRecogToKen()) {
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
	
		if (nullptr != frame) {
			//LOG_INFO("<------ Send Audio Data -------->");
			if (m_Recoger->DoWriteFrame(frame) == -1) {
				break;
			}
			RecordMain(frame);
		}
	}

END:

	lock();

	m_Recoger->DoDestroyRecogRequest();
	RecordStop();
	m_Channel = nullptr;
	m_Recoger = nullptr;
	m_starting = FALSE;

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

void CAliChannel::RecogResultSet(string& reslut)
{
	if (reslut.length() == 0)
		return;

	lock();
	m_resultQue.push(reslut);
	ulock();
	return;
}

string CAliChannel::RecogResultGet()
{
	lock();

	string result = "";
	if (m_resultQue.size() != 0) {
		result = m_resultQue.front();
		m_resultQue.pop();
	}

	ulock();

	return result;
}













