#ifndef __ALI_RECOG_ENGINE_H__
#define __ALI_RECOG_ENGINE_H__

#include"RecogEngine.h"
#include"apr.h"
#include"apr_pools.h"
#include"apt.h"
//#include"mrcp_engine_plugin.h"
#include"Ali_RecogDefine.h"
#include"Ali_RecogChannelFactory.h"
#include"Ali_RecogResourceFactory.h"
#include"Ali_RecogPool.h"
#include"nlsClient.h"

using AlibabaNls::LogLevel;
using AlibabaNls::NlsClient;

class CAliRecogEngine: public CRecogEngine<EngineChannel>
{
public:
	CAliRecogEngine() noexcept;
	~CAliRecogEngine() noexcept;
private:
	CAliRecogEngine(CAliRecogEngine&);
	CAliRecogEngine operator=(CAliRecogEngine&);

public:

	virtual	bool				EngineInit(mrcp_engine_t* engine);
	virtual	void				EngineUinit();
	virtual	bool				EngineReocgStart(EngineChannel * pCh);
	virtual	int					EngineWriteFrame(EngineChannel * pCh, const mpf_frame_t *frame);
	virtual	bool				EngineReocgStop(EngineChannel * pCh);
	virtual string				EngineRecogCompleted(EngineChannel * pCh);
	virtual recog_mod			EngineRecogMod();
public:
	inline static CAliRecogEngine* GetAliRecogEngine() {
		static CAliRecogEngine AliEngine;
		return &AliEngine;
	}

private:
	void AliNlsClientInit(const char* logPath);
	void AliNlsClientUinit();

private:
	mrcp_engine_t*			m_engine;
	CAliChannelFactory*		m_ChFactory;
	CAliResourceFactory*	m_ReFactory;
	CRecogPool*				m_RecogPool;
	recog_mod				m_RecogMod;
};

#endif // !__ALI_RECOG_ENGINE_H__





















