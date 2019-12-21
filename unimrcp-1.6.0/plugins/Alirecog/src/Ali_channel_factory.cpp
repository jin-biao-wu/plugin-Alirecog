#include"Ali_channel_factory.h"
//#include"Ali_define.h"

mrcp_engine_t* AliChannelFactory::m_engine = NULL;

AliChannelFactory::AliChannelFactory()
{

}

AliChannelFactory::~AliChannelFactory()
{

}


apt_bool_t AliChannelFactory::Init(
	apr_pool_t*pool,
	apr_size_t MaxCh,
	const char* Appkey,
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
	if (nullptr == pool)
		return FALSE;

	m_pool = pool;
	m_appKey = Appkey;
	m_AccessKey = AccessKeyID;
	m_Secret =	AccessKeySecret;
	m_Format =	Format;
	m_SampleRate = SampleRate;

	if (apr_thread_mutex_create(&m_Mutex, APR_THREAD_MUTEX_DEFAULT, m_pool) != APR_SUCCESS) {
		LOG_ERROR("init channelFac failed...");
		return FALSE;
	}

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

	return TRUE;
}

void AliChannelFactory::AddChannel(class AliChannel*pCh)
{
	Thread_mutex_lock();

	if (nullptr != pCh) {
		if (!pCh->IsWork()) {
			//添加到空闲池
			auto it = m_ChidlePool.find(pCh->GetChannelId());
			if (it == m_ChidlePool.end()) {
				LOG_INFO("Add channel to lide --->[%d]", pCh->GetChannelId());
				m_ChidlePool[pCh->GetChannelId()] = pCh;
			}
			
		}
		//添加到使用池
		else {
			auto it = m_ChbusyPool.find(pCh->GetChannelId());
			if (it == m_ChbusyPool.end()) {
				LOG_INFO("Add channel to busy --->[%d]",pCh->GetChannelId());
				m_ChbusyPool[pCh->GetChannelId()] = pCh;
			}
		}

	}

	Thread_mutex_ulock();
}

void AliChannelFactory::Uinit()
{


	auto begine = m_ChbusyPool.begin();
	for (; begine != m_ChbusyPool.end(); begine++)
	{
		begine->second->Stop();
		begine->second->Uninit();
		delete begine->second;

	}

	begine = m_ChidlePool.begin();
	for (; begine != m_ChidlePool.end(); begine++)
	{
		begine->second->Stop();
		begine->second->Uninit();
		delete begine->second;

	}

	apr_thread_mutex_destroy(m_Mutex);

	LOG_INFO("Ali recog ChannelFactory ---> Uninit success <---");
	return;
}


AliChannel * AliChannelFactory::GetIdleChannel()
{
	AliChannel *pCh = NULL;
	auto begine = m_ChidlePool.begin();
	for (; begine != m_ChidlePool.end(); begine++) {
		if (!begine->second->IsWork()) {
			pCh = begine->second;
			m_ChidlePool.erase(pCh->GetChannelId());
			LOG_INFO("New Chaannel Get Idle recog channel :%d", pCh->GetChannelId());
			break;
		}
	}

	begine = m_ChbusyPool.begin();
	for (; begine != m_ChbusyPool.end(); ) {
		if (!begine->second->IsWork()) {
			LOG_INFO("New Chaannel delete Ch to busy and Add Ch to idle :%d", begine->second->GetChannelId());
			m_ChidlePool[begine->second->GetChannelId()] = begine->second;
			m_ChbusyPool.erase(begine++);
		}
		else {
			begine++;
		}
	}


	return pCh;
}

AliChannel *AliChannelFactory::NewChannel()
{

	Thread_mutex_lock();

	AliChannel *pCh = NULL;
	pCh = GetIdleChannel();

	Thread_mutex_ulock();

	return pCh;
}

void AliChannelFactory::DeleteChannel(class AliChannel*pCh)
{
	Thread_mutex_lock();

	do {
		if (nullptr == pCh)
			break;

		if (!pCh->IsWork()) {
			if (m_ChidlePool.find(pCh->GetChannelId()) == m_ChidlePool.end()) {
				LOG_INFO("Add channel to lide ---> [%d]", pCh->GetChannelId());
				m_ChidlePool[pCh->GetChannelId()] = pCh;

				if (m_ChbusyPool.find(pCh->GetChannelId()) != m_ChbusyPool.end()) {
					LOG_INFO("Remove channel to busy ---->[%d]", pCh->GetChannelId());
					m_ChbusyPool.erase(pCh->GetChannelId());
				}

			}
		}

		//pCh->Clean();

	} while (false);

	Thread_mutex_ulock();

	return;
}


void AliChannelFactory::Thread_mutex_lock()
{
	apr_thread_mutex_lock(m_Mutex);
}

void AliChannelFactory::Thread_mutex_ulock()
{
	apr_thread_mutex_unlock(m_Mutex);
}













