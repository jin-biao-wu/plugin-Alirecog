#include"Ali_RecogResourceFactory.h"


CAliResourceFactory::CAliResourceFactory() noexcept:
													m_pool(nullptr),
													m_Mutex(nullptr)
{
	m_ResourcePool.clear();
	m_RePool.clear();
}

CAliResourceFactory::~CAliResourceFactory()noexcept
{
	
}

CAliResource* CAliResourceFactory::NewResource()
{
	CAliResource* resource = nullptr;

	lock();

	if (m_RePool.size() != 0) {
		resource = m_RePool.back();
		m_RePool.pop_back();
	}

	ulock();

	return resource;
}

template<class ReType>
ReType * CAliResourceFactory::NewResourceer()noexcept
{
	return new ReType;
}

void CAliResourceFactory::DeleteResource(class CAliResource* resource)
{
	if (nullptr == resource)
		return;

	lock();

	m_RePool.push_back(resource);

	ulock();

	return;
}

void CAliResourceFactory::AddResource(class CAliResource* resource)
{
	if (nullptr == resource)
		return;

	lock();

	m_RePool.push_back(resource);

	ulock();
}

bool CAliResourceFactory::Init( apr_pool_t * pool,
								apr_size_t	 MaxRe,
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
								const bool	 recogniz)
{

	if (nullptr == pool)
		return FALSE;

	if (apr_thread_mutex_create(&m_Mutex, APR_THREAD_MUTEX_DEFAULT, pool) != APR_SUCCESS) {
		LOG_ERROR("init Resource Factory Failed ...");
		return FALSE;
	}

	for (unsigned int i = 0; i < MaxRe; i++) {
		class CAliResource* resource = nullptr;
		/*
		if (recogniz) {
			//创建一句话识别
			resource = new CAliRecognizer(  Appkey,
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
											ContextParam);
		}
		else {
			//创建实时识别
			resource = new CAliTranscriber( Appkey,
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
											ContextParam);
		}


		if (resource) {
			AddResource(resource);
		}
	}
	*/
		if (recogniz) {
			resource = NewResourceer<CAliRecognizer>();
		}
		else {
			resource = NewResourceer<CAliTranscriber>();
		}

		resource->DoRecogRequestInit(Appkey,
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
			ContextParam);

		if (resource) {
			AddResource(resource);
		}
	}

	return TRUE;
}


void CAliResourceFactory::Uinit()
{
	if (m_RePool.size() != 0) {
		auto begine = m_RePool.begin();
		for (; begine != m_RePool.end(); begine++) {
			delete *begine;
		}
		m_RePool.clear();
	}

	if(m_Mutex)
		apr_thread_mutex_destroy(m_Mutex);

	m_Mutex = nullptr;

	return;
}


inline void	CAliResourceFactory::lock()
{
	apr_thread_mutex_lock(m_Mutex);
}

inline void	CAliResourceFactory::ulock()
{
	apr_thread_mutex_unlock(m_Mutex);
}


