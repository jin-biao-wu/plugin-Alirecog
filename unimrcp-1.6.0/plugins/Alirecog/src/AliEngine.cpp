#include"AliEngine.h"

CAliEngine::CAliEngine():CRecogEngine<Ali_recog_channel>(),
						m_engine(nullptr),
						m_ChFactory(nullptr),
						m_RecogPool(nullptr)
{

}

CAliEngine::~CAliEngine()
{

}

bool CAliEngine::EngineInit(mrcp_engine_t* engine)
{
	if (nullptr == engine)
		return FALSE;

	const char* RecordPath = mrcp_engine_param_get(engine, ALI_RECORD_PATH);
	if (nullptr == RecordPath) {
#if _WIN32
		RecordPath = "..\\var";
#else
		RecordPath = "../var";
#endif
	}

	const char* Record = mrcp_engine_param_get(engine, ALI_RECORD);
	if (nullptr == Record) {
		Record = "false";
	}

	const char* Alilog = mrcp_engine_param_get(engine, ALI_LOG);
	if (nullptr == Alilog) {
#if _WIN32
		Alilog = "..\\log\\alisdk.log";
#else
		Alilog = "../log/aliskd.log";
#endif
	}

	const char * appKey = nullptr;
	ALI_CFG_PARAM_GET(appKey, ALI_APPID, "", engine);

	const char * AccessKey = nullptr;
	ALI_CFG_PARAM_GET(AccessKey, ALI_KEYID, "", engine);

	const char * AccessSecret = nullptr;
	ALI_CFG_PARAM_GET(AccessSecret, ALI_SECRET, "", engine);

	const char * Format = nullptr;
	ALI_CFG_PARAM_GET(Format, ALI_FOEMAT, "pcm", engine);

	const char * SampleRate = nullptr;
	ALI_CFG_PARAM_GET(SampleRate, ALI_SAMPLERATE, "8000", engine);

	const char * SpeechRecogUrl = nullptr;
	ALI_CFG_PARAM_GET(SpeechRecogUrl, ALI_RECOG_URL, "", engine);

	const char * IntermediateResult = nullptr;
	ALI_CFG_PARAM_GET(IntermediateResult, ALI_INTERMEDIATE_RESULT, "false", engine);

	const char * PunctuationPrediction = nullptr;
	ALI_CFG_PARAM_GET(PunctuationPrediction, ALI_PUNCTUATION_PREDICTION, "false", engine);

	const char * InverseTextNormalization = nullptr;
	ALI_CFG_PARAM_GET(InverseTextNormalization, ALI_INVERSE_TEXT, "false", engine);

	const char * EnableVoiceDetection = nullptr;
	ALI_CFG_PARAM_GET(EnableVoiceDetection, ALI_VOICE_DETEION, "false", engine);

	const char * StartSilence = nullptr;
	ALI_CFG_PARAM_GET(StartSilence, ALI_START_SILENCE, "3", engine);

	const char * EndSilence = nullptr;
	ALI_CFG_PARAM_GET(EndSilence, ALI_END_SILENCE, "0", engine);

	const char * CustomizationId = nullptr;
	ALI_CFG_PARAM_GET(CustomizationId, ALI_CUSTOMIZATION_ID, "", engine);

	const char * VocabularyId = nullptr;
	ALI_CFG_PARAM_GET(VocabularyId, ALI_VOCABULARY_ID, "", engine);

	const char * OutputFormat = nullptr;
	ALI_CFG_PARAM_GET(OutputFormat, ALI_OUT_PUT_FORMAT, "UTF-8", engine);

	const char * ContextParam = nullptr;
	ALI_CFG_PARAM_GET(ContextParam, ALI_CONTEXT_PARAM, "", engine);


	m_engine = engine;
	AliNlsClientInit(Alilog);

	apt_bool_t record = (string(Record) == "true") ? true : false;
	m_ChFactory = new AliChannelFactory;
	if (!m_ChFactory->Init(
		engine->pool,
		engine->config->max_channel_count,
		appKey,
		AccessKey,
		AccessSecret,
		Format,
		SampleRate,
		IntermediateResult,
		InverseTextNormalization,
		EnableVoiceDetection,
		EndSilence,
		StartSilence,
		PunctuationPrediction,
		CustomizationId,
		VocabularyId,
		OutputFormat,
		ContextParam,
		RecordPath,
		record)) {
		LOG_ERROR("AliChannelFactory Init falied ...");
		return FALSE;
	}

	m_RecogPool = new CRecogPool;
	if (apr_status_t rv = m_RecogPool->Init(m_engine->pool, 1, engine->config->max_channel_count)) {
		LOG_ERROR("recog pool init failed Msg :%s", apr_error(rv).c_str());
		return FALSE;
	}

	LOG_INFO("<---- Ali Engine Init succee --->");

	return true;
}

void CAliEngine::EngineUinit()
{
	if (m_ChFactory) {
		m_ChFactory->Uinit();
		delete m_ChFactory;
		m_ChFactory = nullptr;
	}
	if (m_RecogPool) {
		m_RecogPool->Uinit();
		delete m_RecogPool;
		m_RecogPool = nullptr;
	}

	AliNlsClientUinit();
	LOG_INFO("<----------- Ali Engine Unint ------------->")
}

bool CAliEngine::EngineReocgStart(Ali_recog_channel * pCh)
{
	if (nullptr == pCh)
		return FALSE;

	//LOG_INFO("Ali EngineReocgStart ...");

	pCh->ch = m_ChFactory->NewChannel();
	if (nullptr == pCh->ch) {
		LOG_ERROR("Get Ali recog channel falied");
		return FALSE;
	}
	else {
		pCh->ch->SetRecogChannel(pCh);
		LOG_INFO("Strat recog...");
		if (!pCh->ch->Start()) {
			pCh->ch->Stop();
			LOG_ERROR("Start recog failed");
			return FALSE;
		}
		apr_status_t stu = m_RecogPool->TaskPush(pCh->ch, AliChannel::RecogMain);
		if (APR_SUCCESS != stu) {
			LOG_ERROR("recog task push failed errMsg:%s code:%d",apr_error(stu).c_str(),stu);
			return FALSE;
		}

		m_ChFactory->AddChannel(pCh->ch);
	}

	LOG_INFO("<---- Ali engine reocg start succee channel:%d ---> ", pCh->ch->GetChannelId());
	return TRUE;
}

bool CAliEngine::EngineReocgStop(Ali_recog_channel * pCh)
{
	if (nullptr == pCh)
		return TRUE;

	pCh->ch->Stop();
	m_ChFactory->DeleteChannel(pCh->ch);

	return true;
}

int	CAliEngine::EngineWriteFrame(Ali_recog_channel * pCh, const mpf_frame_t *frame)
{
	if (nullptr == pCh || nullptr == frame)
		return -1;
	if (pCh->ch->IsWork()) {
		pCh->ch->WriteFrame(frame);
	}
	return APR_SUCCESS;
}

CAliEngine* CAliEngine::GetAliEngine()
{
	static CAliEngine engine;
	return &engine;
}

void CAliEngine::AliNlsClientInit(const char* logPath)
{
	NlsClient::getInstance()->setLogConfig(logPath, LogDebug);
}

void CAliEngine::AliNlsClientUinit()
{
	NlsClient::releaseInstance();
}
