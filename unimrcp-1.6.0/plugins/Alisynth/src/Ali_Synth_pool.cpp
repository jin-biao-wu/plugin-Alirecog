#include"Ali_Synth_pool.h"


CSynthPool::CSynthPool()noexcept:
	m_thread_pool(nullptr),
	m_init_threads(0),
	m_max_threads(0),
	m_pool(nullptr)
{

}

CSynthPool::~CSynthPool()noexcept
{
	m_init_threads = 0;
	m_max_threads = 0;
	m_pool = nullptr;
	m_thread_pool = nullptr;
}


apr_status_t CSynthPool::Init(apr_pool_t* pool,const apr_size_t init_threads,const apr_size_t max_threads)
{
	if (nullptr == pool) {
		return -1;
	}
	m_pool = pool;
	m_init_threads = init_threads;
	m_max_threads = max_threads;
	return apr_thread_pool_create(&m_thread_pool, m_init_threads, m_max_threads, m_pool);
}

apr_status_t CSynthPool::UInit()
{
	if(m_thread_pool)
		apr_thread_pool_destroy(m_thread_pool);

	m_init_threads = 0;
	m_max_threads = 0;
	m_pool = nullptr;
	m_thread_pool = nullptr;

	return APR_SUCCESS;
}

apr_status_t CSynthPool::TaskPush(void * Channel, apr_thread_start_t proce_func)
{
	if (m_thread_pool) {
		return apr_thread_pool_push(m_thread_pool, proce_func, Channel, 0, this);
	}
	else {
		return -1;
	}
}
