#ifndef __ALI_SYNTH_ENGINE_H__
#define __ALI_SYNTH_ENGINE_H__

#include<iostream>
#include<string>
#include"SynthEngine.h"
#include"Ali_SynthDefine.h"
#include"Ali_SynthPool.h"
#include"Ali_SynthChannelFactory.h"
#include"nlsClient.h"

using namespace std;
using AlibabaNls::NlsClient;
using AlibabaNls::LogLevel;

class CAliSynthEngine:public CSynthgEngine<Ali_synth_channel_t> {
public:
	CAliSynthEngine() noexcept;
	~CAliSynthEngine() noexcept;
public:
	bool virtual	EngineInit(mrcp_engine_t* engine);
	void virtual	EngineUinit();
	bool virtual	EngineSynthStart(Ali_synth_channel_t * pCh,const char * speakTxt);
	int  virtual	EngineReadFrame(Ali_synth_channel_t * pCh, mpf_frame_t **frame);
	bool virtual	EngineSynthStop(Ali_synth_channel_t * pCh);
public:
	static CAliSynthEngine* GetAliSynthEngine();
private:
	void AliNlsClientInit(const char* logPath);
	void AliNlsClientUinit();
private:
	mrcp_engine_t*			m_engine;
	CSynthPool*				m_SynthPool;
	AliSynthChannelFactory*	m_channelFactory;
};

#endif // !__ALI_SYNTH_ENGINE_H__


