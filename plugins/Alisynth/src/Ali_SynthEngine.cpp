#include"Ali_SynthEngine.h"


CAliSynthEngine::CAliSynthEngine()noexcept
								:CSynthgEngine<Ali_synth_channel_t>(),
								m_engine(nullptr),
								m_SynthPool(nullptr),
								m_channelFactory(nullptr)					
{

}

CAliSynthEngine::~CAliSynthEngine()noexcept
{

}

bool CAliSynthEngine::EngineInit(mrcp_engine_t* engine)
{
	if (nullptr == engine)
		return FALSE;
	
	ALI_PARAM_GET(appkey, ALI_PARAM_appkey, "default")
	ALI_PARAM_GET(AccessKey, ALI_PARAM_AccessKey, "default")
	ALI_PARAM_GET(AccessSecret, ALI_PARAM_AccessSecret, "default")
	ALI_PARAM_GET(voice, ALI_PARAM_voice, "xiaoyun")
	ALI_PARAM_GET(format, ALI_PARAM_format, "pcm")
	ALI_PARAM_GET(sample, ALI_PARAM_sample, "8000")
	ALI_PARAM_GET(volume, ALI_PARAM_volume, "50")
	ALI_PARAM_GET(speech, ALI_PARAM_speech, "0")
	ALI_PARAM_GET(pitch, ALI_PARAM_pitch, "0")

	ALI_PARAM_GET(recogPath, ALI_RECORD_PATH, "../var")
	ALI_PARAM_GET(record, ALI_RECORD, "false")
	ALI_PARAM_GET(AliSdkLog, ALI_LOG, "../log")

	AliNlsClientInit(AliSdkLog);

	m_engine = engine;
	m_SynthPool = new CSynthPool();
	if (m_SynthPool->Init(engine->pool,1,engine->config->max_channel_count) != APR_SUCCESS) {
		LOG_ERROR(" synth pool Init failed ");
		return FALSE;
	}

	m_channelFactory = new AliSynthChannelFactory();
	if (!m_channelFactory->Init(engine->config->max_channel_count,
		m_engine->pool,
		appkey,
		AccessKey,
		AccessSecret,
		voice,
		format,
		sample,
		volume,
		speech,
		pitch,
		recogPath,
		record)) {
		LOG_ERROR(" synth channel factory  Init failed ");
		return FALSE;
	}
	LOG_INFO("<---- Ali Synth Engine init succee ---->");
	return TRUE;
}
void CAliSynthEngine::EngineUinit()
{
	if (m_SynthPool) {
		m_SynthPool->UInit();
		delete m_SynthPool;
	}

	if (m_channelFactory) {
		m_channelFactory->UInit();
		delete m_channelFactory;
	}

	m_SynthPool = nullptr;
	m_channelFactory = nullptr;
	m_engine = nullptr;

	AliNlsClientUinit();

	return;
}

bool CAliSynthEngine::EngineSynthStart(Ali_synth_channel_t * EngineCh,const char* speakTxt)
{
	if (nullptr == EngineCh || nullptr == speakTxt)
		return FALSE;

	if (!m_channelFactory || !m_SynthPool)
		return FALSE;

	CAliSynthChannel * pCh = m_channelFactory->NewChannel();
	if (pCh && pCh->start(speakTxt,EngineCh)) {
		EngineCh->AliSynthCh = pCh;
		m_SynthPool->TaskPush(pCh, CAliSynthChannel::synthMain);
	}
	else {
		LOG_ERROR("Channel Start falied  [ %u ]", pCh->getChannelId());
		m_channelFactory->DeleteChannel(pCh);
		EngineCh->AliSynthCh = nullptr;
		return FALSE;
	}

	return TRUE;
}

int  CAliSynthEngine::EngineReadFrame(Ali_synth_channel_t * EngineCh, mpf_frame_t **frame)
{

	memset((*frame)->codec_frame.buffer, 0, (*frame)->codec_frame.size);
	apr_status_t stu = APR_SUCCESS;
	do {

		if (!EngineCh || !EngineCh->AliSynthCh) {
			stu = -1; break;
		}

		if (!EngineCh->AliSynthCh->is_Play()) {
			stu = -1; break;
		}

		EngineCh->AliSynthCh->readAudioData((*frame)->codec_frame.buffer, (*frame)->codec_frame.size);
		(*frame)->type |= MEDIA_FRAME_TYPE_AUDIO;

	} while (FALSE);

	return stu;
}

bool CAliSynthEngine::EngineSynthStop(Ali_synth_channel_t * EngineCh)
{
	if (!EngineCh || !EngineCh->AliSynthCh)
		return TRUE;

	EngineCh->AliSynthCh->stop();
	m_channelFactory->DeleteChannel(EngineCh->AliSynthCh);
	EngineCh->AliSynthCh = nullptr;

	LOG_INFO("<========== Engine Delete Synth Channel ==========>")
	return TRUE;
}

void CAliSynthEngine::AliNlsClientInit(const char* logPath)
{
	NlsClient::getInstance()->setLogConfig(logPath, LogLevel::LogDebug);
}

void CAliSynthEngine::AliNlsClientUinit()
{
	NlsClient::releaseInstance();
}

CAliSynthEngine* CAliSynthEngine::GetAliSynthEngine()
{
	static CAliSynthEngine Synthengine;
	return &Synthengine;
}