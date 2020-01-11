#ifndef __SYNTH_ENGINE_H__
#define __SYNTH_ENGINE_H__


//#include"Ali_define.h"
#include "mrcp_synth_engine.h"
//#include"apr.h"
//#include"apt.h"


template<class Channel>
class CSynthgEngine {

public:
	CSynthgEngine() noexcept {};
	virtual ~CSynthgEngine() noexcept {};
private:
	CSynthgEngine(CSynthgEngine&);
	CSynthgEngine& operator=(CSynthgEngine&);
public:
	bool virtual	EngineInit(mrcp_engine_t* engine) = 0;
	void virtual	EngineUinit() = 0;
	bool virtual	EngineSynthStart(Channel * pCh, const char* speakTxt) = 0;
	int  virtual	EngineReadFrame(Channel * pCh,mpf_frame_t **frame) = 0;
	bool virtual	EngineSynthStop(Channel * pCh) = 0;
public:
	static CSynthgEngine<Channel>* synthgEngine;
};
template<class Channel>
CSynthgEngine<Channel>* CSynthgEngine<Channel>::synthgEngine = nullptr;


#endif // !__SYNTH_ENGINE_H__

