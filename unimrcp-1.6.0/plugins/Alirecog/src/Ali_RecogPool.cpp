#include"Ali_RecogPool.h"


apr_status_t CRecogPool::Init(apr_pool_t* pool, apr_size_t m_init_threads, apr_size_t m_max_threads)
{
	if (nullptr == pool) {
		return -1;
	}
	m_pool = pool;
	return apr_thread_pool_create(&m_thread_pool, m_init_threads, m_max_threads, pool);
}

apr_status_t CRecogPool::Uinit()
{
	return apr_thread_pool_destroy(m_thread_pool);
}

apr_status_t CRecogPool::TaskPush(void * Channel, apr_thread_start_t proce_func)
{
	if (m_thread_pool) {
		return apr_thread_pool_push(m_thread_pool, proce_func, Channel, 0, this);
	}
	else {
		return -1;
	}
}

