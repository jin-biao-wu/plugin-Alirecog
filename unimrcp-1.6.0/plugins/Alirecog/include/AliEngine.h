#ifndef __ALIENGINE_H__
#define __ALIENGINE_H__

#include"RecogEngine.h"
#include"apr.h"
#include"apr_pools.h"
//#include"apt.h"
//#include"mrcp_engine_plugin.h"
#include"Ali_channel_factory.h"
#include"Ali_recog_pool.h"
#include"Ali_channel.h"


class CAliEngine: public CRecogEngine<Ali_recog_channel>{

public:
	CAliEngine();
	~CAliEngine();
public:
	bool	virtual	EngineInit(mrcp_engine_t* engine);
	void	virtual	EngineUinit();
	bool	virtual	EngineReocgStart(Ali_recog_channel * pCh);
	int		virtual	EngineWriteFrame(Ali_recog_channel * pCh, const mpf_frame_t *frame);
	bool	virtual	EngineReocgStop(Ali_recog_channel * pCh);
private:
	void AliNlsClientInit(const char* logPath);
	void AliNlsClientUinit();
public:
	static CAliEngine* GetAliEngine();
private:
	mrcp_engine_t*		m_engine;
	AliChannelFactory*	m_ChFactory;
	CRecogPool*			m_RecogPool;
};



#endif // !__ALIENGINE_H__





















