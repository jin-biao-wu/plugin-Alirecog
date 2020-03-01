//#pragma once

#ifndef __ALI_CHANNEL_FACTORY_H__
#define __ALI_CHANNEL_FACTORY_H__

#include<iostream>
#include<string>
#include<map>
#include<algorithm>
#include"apr.h"
#include"apt.h"
#include "apr_thread_mutex.h"
#include"apr_thread_pool.h"
#include"Ali_RecogChannel.h"
#include"Ali_RecogDefine.h"
#include "mrcp_recog_engine.h"


//using namespace std;
using std::map;
using std::string;
using MapChPool = map<unsigned int, class CAliChannel*>;

class CAliChannelFactory {
public:
	CAliChannelFactory() noexcept;
	~CAliChannelFactory()noexcept;
private:
	CAliChannelFactory(CAliChannelFactory&);
	CAliChannelFactory operator=(CAliChannelFactory&);

public:
		apt_bool_t Init(apr_pool_t*		pool,
						apr_size_t		MaxCh,
						const char*		recordPath = "../",
						apt_bool_t		record = FALSE);

	void			Uinit();

public:
	CAliChannel*	NewChannel();
	void			DeleteChannel(class CAliChannel*pCh);

	inline size_t	GetIdleSize()const {
		return m_ChidlePool.size();
	}

	inline size_t	GetBusySize()const {
		return m_ChbusyPool.size();
	}

private:
	CAliChannel*	GetIdleChannel();
	void			AddChannel(class CAliChannel*pCh);
private:
	void			lock();
	void			ulock();
private:

	unsigned int		m_Chid;
	apr_pool_t *		m_pool;
	apr_thread_mutex_t* m_Mutex;
	MapChPool			m_ChbusyPool;
	MapChPool			m_ChidlePool;

};

#endif // !__ALI_CHANNEL_FACTORY_H__




