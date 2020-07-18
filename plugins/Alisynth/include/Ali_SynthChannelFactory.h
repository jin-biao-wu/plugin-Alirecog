#ifndef __ALI_SYNTH_CHANNEL_FACTORY_H__
#define __ALI_SYNTH_CHANNEL_FACTORY_H__

#include<string>
#include<map>
#include"apr.h"
#include"apt.h"
#include"Ali_SynthChannel.h"
#include"Ali_SynthDefine.h"

using std::map;
using std::string;
using ChannelPool = map<apr_size_t, CAliSynthChannel *>;

class AliSynthChannelFactory {

public:
	AliSynthChannelFactory()noexcept;
	~AliSynthChannelFactory()noexcept;
private:
	AliSynthChannelFactory(AliSynthChannelFactory&);
	AliSynthChannelFactory& operator=(AliSynthChannelFactory&);

public:
	apt_bool_t	Init(apr_size_t MaxChannel,
					apr_pool_t* pool,
					const char * appKey,
					const char * AccessKeyID,
					const char * AccessSecret,
					const char * VoiceName = "xiaoyun",
					const char * format = "pcm",
					const char * sample = "16000",
					const char * volume = "50",
					const char * speech = "0",
					const char * pitch = "0",
					const char * recordPath="../var",
					const char * record="false");
	apt_bool_t UInit();
public:
	CAliSynthChannel * 	NewChannel();
	void				DeleteChannel(CAliSynthChannel * pCh);
	apr_size_t			GetIdleSize()const { return m_idle_pool.size(); }
	apr_size_t			GetBusySize()const { return m_busy_pool.size(); }
private:
	CAliSynthChannel *	GetIdleChannel();
	void				AddChannel(CAliSynthChannel * pCh);
private:
	void				lock();
	void				ulock();
private:
	apr_size_t				m_ch_id;
	ChannelPool				m_idle_pool;
	ChannelPool				m_busy_pool;
	apr_thread_mutex_t*		m_lock;
	apr_pool_t*				m_pool;
};


#endif // !__ALI_SYNTH_CHANNEL_FACTORY_H__

