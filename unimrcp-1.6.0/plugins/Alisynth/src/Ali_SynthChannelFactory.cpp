#include"Ali_SynthChannelFactory.h"


AliSynthChannelFactory::AliSynthChannelFactory()noexcept:
												m_ch_id(0),
												m_lock(nullptr),
												m_pool(nullptr)
{
	m_idle_pool.clear();
	m_busy_pool.clear();
}

AliSynthChannelFactory::~AliSynthChannelFactory()noexcept
{
	m_ch_id = 0;
	m_lock = nullptr;
	m_pool = nullptr;
	m_idle_pool.clear();
	m_busy_pool.clear();
}


apt_bool_t	AliSynthChannelFactory::Init(apr_size_t MaxChannel,
										apr_pool_t* pool,
										const char * appKey,
										const char * AccessKeyID,
										const char * AccessSecret,
										const char * VoiceName,
										const char * format,
										const char * sample,
										const char * volume,
										const char * speech,
										const char * pitch,
										const char * recordPath,
										const char * record)
{
	if (nullptr == pool)
		return FALSE;

	m_pool = pool;
	if (apr_thread_mutex_create(&m_lock, APR_THREAD_MUTEX_DEFAULT, m_pool) != APR_SUCCESS) {
		LOG_ERROR("factory lock init falied");
		return FALSE;
	}
	bool nrecord = ((string(record) == "true") ? TRUE : FALSE);
	apr_size_t i = 0;
	for (; i < MaxChannel; i++) {
		CAliSynthChannel* pCh = new CAliSynthChannel(i,
													appKey, 
													AccessKeyID, 
													AccessSecret, 
													VoiceName, 
													format, 
													sample, 
													volume, 
													speech,
													pitch,
													nrecord,
													recordPath);

			if (pCh && pCh->init(m_pool)) {
				LOG_INFO("<---- The Ali synth channel init succee id:%u ---->", i);
				AddChannel(pCh);
			}
			else {
				LOG_WARN("<---- The Ali synth channel init failed id:%u ---->", i);
				if (pCh) {
					pCh->uninit();
					delete pCh;
				}
				pCh = nullptr;
			}
	}

	m_ch_id = i;

	LOG_INFO("<---- Ali channel factory init succee ---->");
	return TRUE;
}

apt_bool_t AliSynthChannelFactory::UInit()
{
	lock();

	if (m_idle_pool.size() != 0) {
		auto it = m_idle_pool.begin();
		for (; it != m_idle_pool.end(); it++) {
			if (it->second) {
				it->second->stop();
				it->second->uninit();
				delete it->second;
				it->second = nullptr;
			}
		}
	}


	if (m_busy_pool.size() != 0) {
		auto it = m_busy_pool.begin();
		for (; it != m_busy_pool.end(); it++) {
			if (it->second) {
				it->second->stop();
				it->second->uninit();
				delete it->second;
				it->second = nullptr;
			}
		}
	}

	ulock();

	if(m_lock)
		apr_thread_mutex_destroy(m_lock);

	m_ch_id = 0;
	m_lock = nullptr;
	m_pool = nullptr;
	m_idle_pool.clear();
	m_busy_pool.clear();

	LOG_INFO("<---- Ali channel factory uninit succee ---->");
	return TRUE;
}



CAliSynthChannel *	AliSynthChannelFactory::GetIdleChannel()
{
	CAliSynthChannel * pCh = nullptr;
	auto it = m_idle_pool.begin();
	for (; it != m_idle_pool.end(); it++) {
		if(it->second) {
			pCh = it->second;
			m_idle_pool.erase(it->second->getChannelId());
			break;
		}
	}
	return pCh;
}

void AliSynthChannelFactory::AddChannel(CAliSynthChannel * pCh)
{
	if (nullptr == pCh)
		return;

	lock();

	if (!pCh->is_Synth()) {
		if(m_idle_pool.find(pCh->getChannelId()) == m_idle_pool.end())
			m_idle_pool[pCh->getChannelId()] = pCh;
	}
	else {
		if (m_busy_pool.find(pCh->getChannelId()) == m_busy_pool.end())
			m_busy_pool[pCh->getChannelId()] = pCh;
	}

	ulock();

	return;
}


CAliSynthChannel * 	AliSynthChannelFactory::NewChannel()
{
	CAliSynthChannel * pCh = nullptr;

	lock();

	pCh = GetIdleChannel();
	if (pCh) {
		if(m_busy_pool.find(pCh->getChannelId()) == m_busy_pool.end())
			m_busy_pool[pCh->getChannelId()] = pCh;
	}

	ulock();

	return pCh;
}

void AliSynthChannelFactory::DeleteChannel(CAliSynthChannel * pCh)
{
	if (nullptr == pCh)
		return;

	lock();

	if (m_busy_pool.find(pCh->getChannelId()) != m_busy_pool.end()){
		m_busy_pool.erase(pCh->getChannelId());
	}

	if (m_idle_pool.find(pCh->getChannelId()) == m_idle_pool.end()) {
		m_idle_pool[pCh->getChannelId()] = pCh;
	}

	ulock();

	return;
}

void AliSynthChannelFactory::lock()
{
	if(m_lock)
		apr_thread_mutex_lock(m_lock);
	return;
}

void AliSynthChannelFactory::ulock()
{
	if(m_lock)
		apr_thread_mutex_unlock(m_lock);
	return;
}