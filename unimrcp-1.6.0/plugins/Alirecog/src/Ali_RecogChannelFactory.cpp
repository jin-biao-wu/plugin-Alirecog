#include"Ali_RecogChannelFactory.h"
//#include"Ali_define.h"

//mrcp_engine_t* AliChannelFactory::m_engine = NULL;

CAliChannelFactory::CAliChannelFactory()noexcept:
												m_Chid(0), 
												m_pool(nullptr),
												m_Mutex(nullptr)
{
	m_ChidlePool.clear();
	m_ChbusyPool.clear();
}

CAliChannelFactory::~CAliChannelFactory()noexcept
{

}


apt_bool_t CAliChannelFactory::Init(
	apr_pool_t * pool,
	apr_size_t	 MaxCh,
	const char * Appkey,
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
	const char * recordPath,
	apt_bool_t	 record)
{
	/*
	if (nullptr == pool)
		return FALSE;

	m_pool = pool;
	m_appKey = Appkey;
	m_AccessKey = AccessKeyID;
	m_Secret =	AccessKeySecret;
	m_Format =	Format;
	m_SampleRate = SampleRate;

	if (apr_thread_mutex_create(&m_Mutex, APR_THREAD_MUTEX_DEFAULT, m_pool) != APR_SUCCESS) {
		//LOG_ERROR("init channelFac failed...");
		return FALSE;
	}
	*/
/*
	unsigned int i = 0;
	for (i = 0; i < MaxCh; i++)
	{
		AliChannel * AliCh = new AliChannel(
		i,
		Appkey, 
		AccessKeyID, 
		AccessKeySecret,
		Format, 
		SampleRate, 
		intermediateResult, 
		inverseText, 
		voiceDetection, 
		maxEndSilence, 
		maxStartSilence,
		PunctuationPrediction,
		CustomizationId,
		VocabularyId,
		OutputFormat,
		ContextParam,
		recordPath, 
		record);

		if (AliCh->Init(m_pool)) {
			AddChannel(AliCh);
		}
		else {
			AliCh->Uninit();
			delete AliCh;
			AliCh = nullptr;
			LOG_WARN("AliChannel init failed Ch id :%d", i);
		}
	}
	m_Chid = i;

	LOG_INFO("Init Chanel count:%d appKey:%s AccessKey:%s AccessSecret:%s Format:%s SampleRate:%s Record:%s RecordPath:%s",
		MaxCh, Appkey, AccessKeyID, AccessKeySecret, Format, SampleRate, (record?"true":"false"), recordPath);
*/
	if (nullptr == pool)
		return FALSE;

	m_pool = pool;

	if (apr_thread_mutex_create(&m_Mutex, APR_THREAD_MUTEX_DEFAULT, m_pool) != APR_SUCCESS) {
		LOG_ERROR("init ChannelFactory Failed ...");
		return FALSE;
	}

	unsigned int id = 0;

	for (; id < MaxCh; id++) {

		CAliChannel * AliCh = new CAliChannel;
		if (AliCh && AliCh->Init(	id,
									m_pool, 
									Appkey, 
									AccessKeyID, 
									AccessKeySecret, 
									Format,
									SampleRate, 
									intermediateResult, 
									inverseText, 
									voiceDetection, 
									maxEndSilence,
									maxStartSilence, 
									PunctuationPrediction,
									CustomizationId, 
									VocabularyId, 
									OutputFormat, 
									ContextParam, 
									recordPath, 
									record)) 
		{
			AddChannel(AliCh);
		}
		else {
			LOG_WARN("");
			if (AliCh) {
				AliCh->Uinit();
				delete AliCh;
			}
		}

	}

	return TRUE;
}

void CAliChannelFactory::AddChannel(class CAliChannel* AliCh)
{
	if (!AliCh)
		return;

	lock();

	if (m_ChidlePool.find(AliCh->GetChannelId()) == m_ChidlePool.end())
		m_ChidlePool[AliCh->GetChannelId()] = AliCh;

	ulock();

	return;
}

void CAliChannelFactory::Uinit()
{

	auto begine = m_ChbusyPool.begin();
	if (m_ChidlePool.size() != 0) {
		for (; begine != m_ChbusyPool.end(); begine++)
		{
			begine->second->Stop();
			begine->second->Uinit();
			delete begine->second;

		}
	}

	begine = m_ChidlePool.begin();
	if (m_ChbusyPool.size() != 0) {
		for (; begine != m_ChidlePool.end(); begine++)
		{
			begine->second->Stop();
			begine->second->Uinit();
			delete begine->second;

		}
	}

	if (m_Mutex) {
		apr_thread_mutex_destroy(m_Mutex);
		m_Mutex = nullptr;
	}

	LOG_INFO("Ali Engine Recog ChannelFactory ---> Uninit success <---");

	return;
}


CAliChannel * CAliChannelFactory::GetIdleChannel()
{

	CAliChannel * AliCh = NULL;

	if (m_ChidlePool.size() != 0) {
		auto begine = m_ChidlePool.begin();
		for (; begine != m_ChidlePool.end(); begine++) {
			if (begine->second) {
				AliCh = begine->second;
				m_ChidlePool.erase(AliCh->GetChannelId());
				break;
			}
		}
	}

	return AliCh;

}

CAliChannel *CAliChannelFactory::NewChannel()
{
	CAliChannel * AliCh = nullptr;

	lock();

	AliCh = GetIdleChannel();
	if (!AliCh) {
		//添加到使用池
		if (m_ChbusyPool.find(AliCh->GetChannelId()) == m_ChbusyPool.end())
			m_ChbusyPool[AliCh->GetChannelId()] = AliCh;
	}

	ulock();

	return AliCh;
}

void CAliChannelFactory::DeleteChannel(class CAliChannel* AliCh)
{
	if (nullptr == AliCh)
		return;

	lock();

	//添加到空闲池
	if (m_ChidlePool.find(AliCh->GetChannelId()) == m_ChidlePool.end())
		m_ChidlePool[AliCh->GetChannelId()] = AliCh;

	//移除使用池
	if (m_ChbusyPool.find(AliCh->GetChannelId()) != m_ChbusyPool.end())
		m_ChbusyPool.erase(AliCh->GetChannelId());

	ulock();

	return;
}


void CAliChannelFactory::lock()
{
	apr_thread_mutex_lock(m_Mutex);
}

void CAliChannelFactory::ulock()
{
	apr_thread_mutex_unlock(m_Mutex);
}













