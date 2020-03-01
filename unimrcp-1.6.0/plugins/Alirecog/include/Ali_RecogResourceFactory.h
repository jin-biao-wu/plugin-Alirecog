#ifndef __ALI_RECOG_RESOURCE_FACTORY_H__
#define __ALI_RECOG_RESOURCE_FACTORY_H__

#include<map>
#include<list>
#include<string>
#include"apr.h"
#include"apt.h"
#include"apr_thread_pool.h"
#include"apr_thread_mutex.h"
#include"Ali_RecogResource.h"
#include"Ali_RecogRecognizer.h"
#include"Ali_RecogTranscriber.h"

using std::list;
using std::map;
using std::string;
using MapRePool		= map<unsigned int, class CAliResource*>;
using ListRePool	= list<class CAliResource*>;


class CAliResourceFactory {
public:
	CAliResourceFactory() noexcept;
	~CAliResourceFactory()noexcept;
private:
	CAliResourceFactory(CAliResourceFactory&);
	CAliResourceFactory operator=(CAliResourceFactory&);

public:

	bool Init(	apr_pool_t*pool,
				apr_size_t MaxRe,
				const char* Appkey,
				const char * AccessKeyID,
				const char * AccessKeySecret,
				const char * Format = "pcm",
				const char * SampleRate = "8000",
				const char * intermediateResult = "",
				const char * inverseText = "",
				const char * voiceDetection = "",
				const char * maxEndSilence = "",
				const char * maxStartSilence = "",
				const char * PunctuationPrediction = "",
				const char * CustomizationId = "",
				const char * VocabularyId = "",
				const char * OutputFormat = "",
				const char * ContextParam = "",
				const bool	 recogniz = FALSE);

	void			Uinit();

public:
	CAliResource*	NewResource();
	void			DeleteResource(class CAliResource* resource);
	
	inline size_t	GetResourceSize()const {
		return m_ResourcePool.size();
	}
private:
	//CAliChannel*	GetIdleChannel();
	template<class ReType>
	ReType *		NewResourceer()noexcept;
	void			AddResource(class CAliResource* resource);
	
private:
	inline void	lock();
	inline void	ulock();
private:
	apr_pool_t *		m_pool;
	apr_thread_mutex_t* m_Mutex;
	//MapChPool			m_ChbusyPool;
	MapRePool			m_ResourcePool;
	ListRePool			m_RePool;
	
};

#endif // !__ALI_RECOG_RESOURCE_FACTORY_H__

