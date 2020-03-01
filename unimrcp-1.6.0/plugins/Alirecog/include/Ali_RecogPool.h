#ifndef __ALI_RECOG_POOL_H__
#define __ALI_RECOG_POOL_H__

#include<iostream>
#include<string>
#include"apr.h"
#include"apt.h"
#include"apr_thread_proc.h"
#include"apr_thread_mutex.h"
#include"apr_thread_pool.h"
//#include"Ali_channel.h"

using namespace std;

class CRecogPool {

public:
	CRecogPool()noexcept:
						m_thread_pool(NULL),
						m_init_threads(0),
						m_max_threads(0),
						m_pool(0) 
						{ };

	~CRecogPool() noexcept { };
public:
	// typedef void *(APR_THREAD_FUNC *apr_thread_start_t)(apr_thread_t*, void*);
	apr_status_t Init(apr_pool_t* pool, apr_size_t m_init_threads, apr_size_t m_max_threads);
	apr_status_t Uinit();
	apr_status_t TaskPush(void * Channel, apr_thread_start_t processTask);
private:
	CRecogPool(CRecogPool&);
	CRecogPool operator=(CRecogPool&);
private:
	apr_thread_pool*	m_thread_pool;
	apr_size_t			m_init_threads;
	apr_size_t			m_max_threads;
	apr_pool_t*			m_pool;
};

#endif // !__ALI_RECOG_POOL_H__

