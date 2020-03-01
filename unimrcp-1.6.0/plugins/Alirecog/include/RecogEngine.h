#ifndef __RECOGENGINE_H__
#define __RECOGENGINE_H__

//#include"Ali_define.h"
#include<string>
#include"mrcp_recog_engine.h"
using std::string;
//#include"apr.h"
//#include"apt.h"

typedef enum {
	//引擎识别模式
	RECOGNIZER = 0,
	TRANSCRIBER
}recog_mod;

template<class Channel>
class CRecogEngine 
{
public:
	CRecogEngine()noexcept { }
	virtual ~CRecogEngine()noexcept { }
private:
	CRecogEngine(CRecogEngine&);
	CRecogEngine& operator=(CRecogEngine&);
public:
	virtual	bool		EngineInit(mrcp_engine_t* engine)= 0;
	virtual	void		EngineUinit() = 0;
	virtual	bool		EngineReocgStart(Channel * pCh) = 0;
	virtual	int			EngineWriteFrame(Channel * pCh,const mpf_frame_t *frame) = 0;
	virtual	bool		EngineReocgStop(Channel * pCh) = 0;
	virtual	string		EngineRecogCompleted(Channel * pCh) = 0;
	virtual recog_mod	EngineRecogMod() = 0;
public:
	static CRecogEngine<Channel>* recogEngine;
};

template<class Channel>
CRecogEngine<Channel>* CRecogEngine<Channel>::recogEngine = nullptr;

//template<class Channel>
//CRecogEngine<Channel>*
//AliRecogEngine(Channel) {
//	static CRecogEngine<Channel>* recogEngine;
//	return recogEngine;
//}

#endif // !__RECOGENGINE_H__
