#include"Ali_RecogEngine.h"

CAliRecogEngine::CAliRecogEngine()	noexcept:
									m_engine(NULL),
									m_ChFactory(NULL),
									m_RecogPool(NULL)
{ 

}

CAliRecogEngine::~CAliRecogEngine() noexcept 
{

}

bool	CAliRecogEngine::EngineInit(mrcp_engine_t* engine)
{
	if (nullptr == engine)
		return FALSE;

	ALI_PARAM_GET(appKey, "appKey", "default");
	ALI_PARAM_GET(AccessKeyID, "AccessKeyID", "default");
	ALI_PARAM_GET(AccessKeySecret, "AccessKeySecret", "default");
	ALI_PARAM_GET(Format, "Format", "pcm");
	ALI_PARAM_GET(SampleRate, "SampleRate", "8000");
	ALI_PARAM_GET(LogDir, "LogDir", (PATHDIA+string("..log").c_str()));
	ALI_PARAM_GET(SpeechRecogUrl, "SpeechRecogUrl", "");
	ALI_PARAM_GET(IntermediateResult, "IntermediateResult", "false");
	ALI_PARAM_GET(PunctuationPrediction, "PunctuationPrediction", "false");
	ALI_PARAM_GET(InverseTextNormalization, "InverseTextNormalization", "false");
	ALI_PARAM_GET(EnableVoiceDetection, "EnableVoiceDetection", "false");
	ALI_PARAM_GET(StartSilence, "StartSilence", "5");
	ALI_PARAM_GET(EndSilence, "EndSilence", "0");
	ALI_PARAM_GET(CustomizationId, "CustomizationId", "");
	ALI_PARAM_GET(VocabularyId, "VocabularyId", "");
	ALI_PARAM_GET(OutputFormat, "OutputFormat", "UTF-8");
	ALI_PARAM_GET(ContextParam, "ContextParam", "");
	ALI_PARAM_GET(recordPath, "recordPath", (PATHDIA + string("..var").c_str()));
	ALI_PARAM_GET(record, "record", "false");
	ALI_PARAM_GET(recoger, "SeepRecognizer", "recognizer");
	
	bool brecord = (string(record) == "true");
	bool brecoger = (string(recoger) == "recognizer");

	LOG_INFO("<---------- Ali Recog Engine Init For [ %s ] ----------->", recoger);

	if (brecoger) {
		m_RecogMod = RECOGNIZER;
	}
	else {
		m_RecogMod = TRANSCRIBER;
	}

	AliNlsClientInit(LogDir);

	m_engine = engine;
	m_RecogPool = new CRecogPool;
	if (m_RecogPool->Init(m_engine->pool, 1, m_engine->config->max_channel_count) != APR_SUCCESS) {
		LOG_ERROR("Ali Engine Init Recog Pool failed ! ! !");
		return FALSE;
	}

	m_ChFactory = new CAliChannelFactory;
	if (m_ChFactory->Init(
		m_engine->pool,
		m_engine->config->max_channel_count,
		recordPath,
		brecord
		) != TRUE) 
	{
		LOG_ERROR("Ali Engine Init Channel Factory failed ! ! ! ");
		return FALSE;
	}

	m_ReFactory = new CAliResourceFactory;
	if (m_ReFactory->Init(m_engine->pool,
		m_engine->config->max_channel_count, 
		appKey,
		AccessKeyID, 
		AccessKeySecret, 
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
		brecoger) != TRUE) 
	{
		LOG_ERROR("Ali Engine Init Resource Factory failed ! ! ! ");
		return FALSE;
	}

	LOG_INFO("Ali Engine Init Success ... ");
	return TRUE;
}

void	CAliRecogEngine::EngineUinit()
{
	if (m_RecogPool) {
		m_RecogPool->Uinit();
		delete m_RecogPool;
		m_RecogPool = nullptr;
	}

	if (m_ReFactory) {
		m_ReFactory->Uinit();
		delete m_ReFactory;
		m_ReFactory = nullptr;
	}

	if (m_ChFactory) {
		m_ChFactory->Uinit();
		delete m_ChFactory;
		m_ChFactory = nullptr;
	}

	m_engine = nullptr;

	AliNlsClientUinit();

	return;
}

bool	CAliRecogEngine::EngineReocgStart(EngineChannel * pCh)
{
	if (nullptr == pCh)
		return FALSE;

	CAliChannel *	AliCh = nullptr;
	CAliResource *	AliRes = nullptr;
	bool start = FALSE;

	do {
		AliCh = m_ChFactory->NewChannel();
		if (!AliCh) {
			LOG_ERROR("Ali Engine Recog Start failed , Get AliChannel Failed ! ! !");
			break;
		}
		AliRes = m_ReFactory->NewResource();
		if (!AliRes) {
			LOG_ERROR("Ali Engine Recog Start failed , Get AliResource Failed ! ! !");
			break;
		}

		if (!AliCh->Start(pCh, AliRes)) {
			LOG_ERROR("Ali Engine Channel Recog Start Failed ! ! !");
			break;
		}

		if (m_RecogPool->TaskPush(AliCh, CAliChannel::RecogMain) != APR_SUCCESS) {
			LOG_ERROR("Ali Engine Recog Start failed , Recog Task Push Failed ! ! !");
			break;
		}

		pCh->AliCh = AliCh;
		pCh->AliRe = AliRes;

		start = TRUE;

	} while (FALSE);

	if (!start) {
		if (AliCh) {
			AliCh->Stop();
			m_ChFactory->DeleteChannel(AliCh);
			m_ReFactory->DeleteResource(AliRes);
		}
		pCh->AliCh = nullptr;
		pCh->AliRe = nullptr;
		return FALSE;
	}
	
	return TRUE;
}

int		CAliRecogEngine::EngineWriteFrame(EngineChannel * pCh, const mpf_frame_t *frame)
{
	if (nullptr == pCh || nullptr == pCh->AliCh)
		return -1;

	if (pCh->AliCh->WriteFrame(frame) != APR_SUCCESS) {
		LOG_WARN("Engine Write Frame Failed Channel id [%d]", pCh->AliCh->GetChannelId());
		return -1;
	}

	return 0;
}

bool	CAliRecogEngine::EngineReocgStop(EngineChannel * pCh)
{
	if (nullptr == pCh || nullptr == pCh->AliCh || nullptr == pCh->AliRe)
		return TRUE;

	pCh->AliCh->Stop();
	m_ChFactory->DeleteChannel(pCh->AliCh); //回收通道资源
	m_ReFactory->DeleteResource(pCh->AliRe); //回收识别资源
	pCh->AliCh = nullptr;
	pCh->AliRe = nullptr;

	return TRUE;
}

string	CAliRecogEngine::EngineRecogCompleted(EngineChannel * pCh)
{
	if (nullptr == pCh || nullptr == pCh->AliCh)
		return "";

	return pCh->AliCh->RecogResultGet();
}


recog_mod	CAliRecogEngine::EngineRecogMod()
{
	return m_RecogMod;
}

void CAliRecogEngine::AliNlsClientInit(const char* logPath)
{
	NlsClient::getInstance()->setLogConfig(logPath, LogLevel::LogDebug);
}

void CAliRecogEngine::AliNlsClientUinit()
{
	NlsClient::releaseInstance();
}

