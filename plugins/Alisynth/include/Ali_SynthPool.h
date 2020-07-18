#ifndef __ALI_RECOG_POOL_H__
#define __ALI_RECOG_POOL_H__

//#include<iostream>
//#include<string>
#include"apr.h"
#include"apt.h"
#include"apr_thread_proc.h"
#include"apr_thread_mutex.h"
#include"apr_thread_pool.h"
//#include"Ali_channel.h"

//using namespace std;

class CSynthPool {

public:
	CSynthPool()noexcept;
	~CSynthPool()noexcept;
public:
	// typedef void *(APR_THREAD_FUNC *apr_thread_start_t)(apr_thread_t*, void*);
	apr_status_t Init(apr_pool_t* pool,const apr_size_t m_init_threads,const apr_size_t m_max_threads);
	apr_status_t UInit();
	apr_status_t TaskPush(void * Channel, apr_thread_start_t processTask);

private:
	CSynthPool(CSynthPool&);
	CSynthPool& operator=(CSynthPool&);
private:
	apr_thread_pool*	m_thread_pool;
	apr_size_t			m_init_threads;
	apr_size_t			m_max_threads;
	apr_pool_t*			m_pool;
};



#endif // !__ALI_RECOG_POOL_H__

