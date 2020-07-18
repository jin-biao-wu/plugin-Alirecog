#ifndef __ALI_SYNTH_CHANNEL_H__
#define __ALI_SYNTH_CHANNEL_H__

#include<iostream>
#include<string>
#include<vector>
#include<queue>
#include<ctime>
#include<algorithm>
#include"apr.h"
#include"apr_thread_proc.h"
#include"apt.h"
#include"apr_queue.h"
#include"apt_pool.h"
#include "nlsClient.h"
#include "nlsEvent.h"
#ifdef _WIN32
#include"nlsCommonSdk\Token.h"
#else
#include"nlsCommonSdk/Token.h"
#endif
#include"iNlsRequest.h"
#include"speechSynthesizerRequest.h"
#include"Ali_SynthDefine.h"
#include"Ali_SynthUtil.h"

#define MAX_QUEUE_SIZE	1024

using namespace std;
//using std::string;
//using std::vector;
using AlibabaNls::INlsRequest;
using AlibabaNls::SpeechSynthesizerCallback;
using AlibabaNls::SpeechSynthesizerRequest;
using AlibabaNls::SpeechSynthesizerParam;
using AlibabaNlsCommon::NlsToken;
using AlibabaNls::NlsClient;
using AlibabaNls::NlsEvent;

using AudioList = queue<vector<unsigned char>>;

class CAliSynthChannel {

private:

	struct Frame {

		unsigned char * buffer;
		apr_size_t size;
	};

	struct AudioFrame {
		Frame frame; //音频的数据
		apr_size_t cur; //已经读了多少数据
		apr_size_t item; //这是第几块数据
	};

public:
	CAliSynthChannel();
	CAliSynthChannel(apr_size_t id,
		const char * appKey,
		const char * AccessKeyID,
		const char * AccessSecret,
		const char * VoiceName = "xiaoyun",
		const char * format = "pcm",
		const char * sample = "16000",
		const char * volume = "50",
		const char * speech = "0",
		const char * pitch = "0",
		const bool	 record = FALSE,
		const char * recordPaht="")noexcept;
	~CAliSynthChannel()noexcept;
private:
	CAliSynthChannel(CAliSynthChannel&);
	CAliSynthChannel& operator=(CAliSynthChannel&);

public:
	apt_bool_t	init(apr_pool_t * pool);
	apt_bool_t	start(const string& speakText,Ali_synth_channel_t * engineCh);
	apt_bool_t	stop();
	apt_bool_t	uninit();
	apt_bool_t	is_Synth()const;
	apt_bool_t	is_Play()const;
public:
	void		setSpeekText(const string& strText) { m_Text = strText; }
	void		setSpeekVoiceName(const string& voice) { m_VoiceName = voice; }
	void		setSpeekVoiceAge(const string& age) { return; } //扩展接口
	apr_size_t	getChannelId()const { return m_id; }
	void		readAudioData(void *out_frame,const apr_size_t size);
public:
	static void *	APR_THREAD_FUNC synthMain(apr_thread_t* tid, void * arg);
private:
	void			synthMain();
	void			recordMain(unsigned char * vioce_data,const apr_size_t& size);
	void			recordStart(const string& FileName);
	void			recordClose();

public:
	static void OnSynthesisStarted(NlsEvent* ev, void * arg);
	static void OnSynthesisCompleted(NlsEvent* ev, void * arg);
	static void OnTaskFailed(NlsEvent* ev, void * arg);
	static void OnChannelClosed(NlsEvent* ev, void * arg);
	static void OnBinaryDataReceived(NlsEvent* ev, void * arg);
private:
	void	OnSynthesisStarted(NlsEvent* ev);
	void	OnSynthesisCompleted(NlsEvent* ev);
	void	OnTaskFailed(NlsEvent* ev);
	void	OnChannelClosed(NlsEvent* ev);
	void	OnBinaryDataReceived(NlsEvent* ev);
private:
	int		DoGetAliToken();
	bool	DoCheakSynthToKen();
	bool	DoSynthRequestInit();
	bool	DoCreateSynthRequest();
	void	DoDestroySynthRequest();

private:
	void	lock();
	void	unlock();

private:
	apr_size_t	m_id;
	string		m_appKey; // 阿里控制台项目id
	string		m_AccessKey; // 阿里控制台 用户Key
	string		m_AccessSecret; //阿里控制台 用户Secret
	string		m_VoiceName; // 发音人，参考阿里sdk说明
	string		m_format; //音频编码格式，默认是pcm
	string		m_sample; // 音频采样率，默认是16000
	string		m_volume; // 音量，范围是0~100，默认50
	string		m_speech; //语速，范围是-500~500，默认是0
	string		m_pitch; //语调，范围是-500~500，默认是0
	string		m_Text; //文本，限制300字符内
	string		m_Token; // 合成要使用额token
	apt_bool_t	m_Start; // 还在等待播放
	apt_bool_t	m_Play; //是否在播放
	apt_bool_t	m_Runing; //播放中
	apt_bool_t	m_record; //是否录音
	string		m_recordPath; //录音路径
	unsigned long m_tokenTime;
	SpeechSynthesizerCallback	*m_SynthCall;
	SpeechSynthesizerRequest	*m_SynthReq;

	//apr_size_t	m_cursors; //移动游标
	AudioFrame				m_AudioFrame; //总的音频数据
	AudioList				m_AudioList;
	Ali_synth_channel_t     *m_engineCh;
	apr_file_t				*m_file;
	apr_thread_mutex_t		*m_lock;
	apr_queue_t				*m_AudioQueue;
	apr_pool_t				*m_pool;

};



#endif // !__ALI_SYNTH_CHANNEL_H__

