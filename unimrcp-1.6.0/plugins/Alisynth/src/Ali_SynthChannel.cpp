#include"Ali_SynthChannel.h"


CAliSynthChannel::CAliSynthChannel():
									m_id(0),
									m_appKey(""),
									m_AccessKey(""),
									m_AccessSecret(""),
									m_VoiceName(""),
									m_format(""),
									m_sample(""),
									m_volume(""),
									m_speech(""),
									m_pitch(""),
									m_Text(""),
									m_Token(""),
									m_Start(FALSE),
									m_Play(FALSE),
									m_Runing(FALSE),
									m_record(FALSE),
									m_recordPath(""),
									m_tokenTime(0),
									m_SynthCall(nullptr),
									m_SynthReq(nullptr),
									m_cursors(0),
									m_file(nullptr),
									m_lock(nullptr),
									m_VoiceQueue(nullptr),
									m_pool(nullptr)
{
	//m_VoiceData.clear();
	m_Voice.clear();
}

CAliSynthChannel::CAliSynthChannel(apr_size_t id,
									const char * appKey,
									const char * AccessKeyID,
									const char * AccessSecret,
									const char * VoiceName,
									const char * format,
									const char * sample,
									const char * volume,
									const char * speech,
									const char * pitch,
									const bool	record,
									const char * recordPaht)noexcept:
									m_id(0),
									m_appKey(appKey),
									m_AccessKey(AccessKeyID),
									m_AccessSecret(AccessSecret),
									m_VoiceName(VoiceName),
									m_format(format),
									m_sample(sample),
									m_volume(volume),
									m_speech(speech),
									m_pitch(pitch),
									m_Text(""),
									m_Token(""),
									m_Start(FALSE),
									m_Play(FALSE),
									m_Runing(FALSE),
									m_record(record),
									m_recordPath(recordPaht),
									m_tokenTime(0),
									m_SynthCall(nullptr),
									m_SynthReq(nullptr),
									m_cursors(0),
									m_file(nullptr),
									m_lock(nullptr),
									m_VoiceQueue(nullptr),
									m_pool(nullptr)
{
	//m_VoiceData.clear();
	m_VoiceFrame.cur = 0;
	m_VoiceFrame.frame.buffer = nullptr;
	m_VoiceFrame.frame.size = 0;
	m_Voice.clear();
}

CAliSynthChannel::~CAliSynthChannel() noexcept
{
	m_VoiceFrame.cur = 0;
	m_VoiceFrame.frame.buffer = nullptr;
	m_VoiceFrame.frame.size = 0;
}


apt_bool_t CAliSynthChannel::init(apr_pool_t * pool)
{
	if (nullptr == pool)
		return FALSE;

	apr_status_t stu = APR_SUCCESS;
	m_pool = pool;

	if ((stu = apr_thread_mutex_create(&m_lock, APR_THREAD_MUTEX_DEFAULT, m_pool)) != APR_SUCCESS) {
		LOG_ERROR("Ali Synth Channel init lock failed id:%d status:%d", m_id,stu);
		return FALSE;
	}

	if ((stu = apr_queue_create(&m_VoiceQueue, MAX_QUEUE_SIZE, m_pool)) != APR_SUCCESS) {
		LOG_ERROR("Ali Synth Channel init Voice Queue failed id:%d status:%d", m_id, stu);
		return FALSE;
	}

	m_SynthCall = new SpeechSynthesizerCallback();
	if (!m_SynthCall) {
		LOG_ERROR("Ali Synth Channel init SpeechSynthesizerCallback failed id:%d", m_id);
		return FALSE;
	}
	m_SynthCall->setOnSynthesisStarted(CAliSynthChannel::OnSynthesisStarted, this);
	m_SynthCall->setOnSynthesisCompleted(CAliSynthChannel::OnSynthesisCompleted, this);
	m_SynthCall->setOnBinaryDataReceived(CAliSynthChannel::OnBinaryDataReceived, this);
	m_SynthCall->setOnChannelClosed(CAliSynthChannel::OnChannelClosed, this);
	m_SynthCall->setOnTaskFailed(CAliSynthChannel::OnTaskFailed, this);

	LOG_INFO("Synth Channel id:%d AppKey:%s AccessKey:%s AccessSecret:%s VoiceName:%s format:%s sample:%s volume:%s speech:%s pitch:%s Init succee",
		m_id, m_appKey.c_str(), m_AccessKey.c_str(), m_AccessSecret.c_str(), m_VoiceName.c_str(), m_format.c_str(), m_sample.c_str(), m_volume.c_str(),
		m_speech.c_str(), m_pitch.c_str());


	return TRUE;
}

apt_bool_t CAliSynthChannel::start(const string& speakText,Ali_synth_channel_t * engineCh)
{
	if (nullptr == engineCh || 0 == speakText.length())
		return FALSE;

	lock();

	m_engineCh = engineCh;
	//string text = "哎呀，你要问我喜欢吃什么，我能答上一堆。问我不喜欢的啊，那我还真答不上来。";
	m_Text = speakText;
	//text = CUtil::GBKToUTF8(text);

	//LOG_INFO("start GBKToUTF8 : %s", text.c_str());
	LOG_INFO("start Text :%s", m_Text.c_str())

	m_Start = TRUE;
	m_Play = TRUE;
	m_Runing = TRUE;

	//m_VoiceData.clear();
	m_Voice.clear();

    if (m_VoiceFrame.frame.buffer) {
        delete[] m_VoiceFrame.frame.buffer;
    }
	m_VoiceFrame.cur = 0;
    m_VoiceFrame.frame.buffer = nullptr;
	m_VoiceFrame.frame.size = 0;
	m_VoiceFrame.item = 0;

    while (0 != apr_queue_size(m_VoiceQueue)) {        //如果队列还保存着上次通道的数据
        Frame * frame = nullptr;
        apr_queue_pop(m_VoiceQueue, (void**)&frame);
        if (nullptr != frame) {
            delete[] frame->buffer;
            delete frame;
            frame = nullptr;
        }
    }

	if (m_record) {

		char szFileName[255] = { 0 };
		string FileName = "";
		apr_snprintf(szFileName, sizeof(szFileName), "[synth]-[Channel-%s]-[%s-Hz]-[Date-%s].pcm",
			m_engineCh->channel->id.buf, m_sample.c_str(), CUtil::TimeToStr(apr_time_now(), "%Y-%m-%d %H-%M-%S").c_str());

		if (m_recordPath[m_recordPath.length() - 1] == PATHDIA) {
			FileName = m_recordPath.append(szFileName);
		}
		else {
			FileName = m_recordPath + PATHDIA + szFileName;
		}

		recordStart(FileName);
		LOG_INFO("Record Start  File:%s", FileName.c_str());
	}

	unlock();

	LOG_INFO("<---- Ali Synth  Channel Start Succee id:[ %d ]---->", m_id);
	return TRUE;
}

apt_bool_t CAliSynthChannel::stop()
{
	lock();
	m_Start = FALSE;
	m_Play = FALSE;
	m_Runing = FALSE;
	unlock();
	return TRUE;
}

apt_bool_t CAliSynthChannel::uninit()
{
	m_engineCh = nullptr;
	if (nullptr != m_file)
		apr_file_close(m_file);

	if (nullptr != m_SynthCall)
		delete m_SynthCall;

	if (nullptr != m_VoiceQueue)
		apr_queue_term(m_VoiceQueue);

	if (nullptr != m_lock)
		apr_thread_mutex_destroy(m_lock);


	return TRUE;
}

apt_bool_t CAliSynthChannel::is_Synth()const
{
	return m_Start;
}

apt_bool_t	CAliSynthChannel::is_Play()const {
	return m_Play;
}

void *	CAliSynthChannel::synthMain(apr_thread_t* tid, void * arg)
{
	CAliSynthChannel * pCh = (CAliSynthChannel *)arg;
	if (nullptr != pCh)
		pCh->synthMain();

	return nullptr;
}

void CAliSynthChannel::synthMain()
{
	do {
		if (!DoCreateSynthRequest())
			break;
		if (!DoSynthRequestInit())
			break;

		while (m_Start){
			if (m_Runing) {
				lock();
				if (0 != m_Voice.size()) {
					unsigned int i = m_cursors;
					for (; i < m_Voice.size(); i++) {
						Frame *frame = new Frame;
						frame->buffer = new unsigned char[m_Voice[i].size()];
						memset(frame->buffer, 0, m_Voice[i].size());
						memcpy(frame->buffer,&m_Voice[i][0], m_Voice[i].size());
						frame->size = m_Voice[i].size();
						//LOG_INFO("synthMain send data :%d frame :%x cur :%d size :%d", m_Voice[i].size(), frame, i, frame->size);
						apr_queue_push(m_VoiceQueue, (void *)frame);
						recordMain((unsigned char*)&m_Voice[i][0], m_Voice[i].size());
					}
					m_cursors = i; //记录已经发送了多少数据
				}
				unlock();
			}
		}

	} while (FALSE);

	m_Voice.clear();
	m_cursors = 0;
	m_Token.clear();
	m_tokenTime = 0;
	//m_VoiceFrame.cur = 0;
	DoDestroySynthRequest();
	recordClose();
	LOG_INFO("<---- Synth Channel Exit ---->");
	return;
}

void CAliSynthChannel::readVoiceData(unsigned char *in_ptr,const apr_size_t size)
{
    // 每次只读160字节的原始数据
    if (m_Play) {
        if (0 != m_VoiceFrame.frame.size) {
            if (m_VoiceFrame.frame.size > size && m_VoiceFrame.frame.buffer) {
                memcpy(in_ptr, m_VoiceFrame.frame.buffer + m_VoiceFrame.cur, size);
                if (0 == m_VoiceFrame.cur) {
                    m_VoiceFrame.cur = size;
                }
                else {
                    m_VoiceFrame.cur += size;
                }
                m_VoiceFrame.frame.size -= size;
            }
            else {
                memcpy(in_ptr, m_VoiceFrame.frame.buffer + m_VoiceFrame.cur, m_VoiceFrame.frame.size);
                memset(in_ptr + m_VoiceFrame.frame.size, 0, size - m_VoiceFrame.frame.size);
                m_VoiceFrame.cur = 0;
                m_VoiceFrame.frame.size = 0;
                delete[] m_VoiceFrame.frame.buffer;
                m_VoiceFrame.frame.buffer = nullptr;
                if (1 == m_VoiceFrame.item && 0 == apr_queue_size(m_VoiceQueue)) {
                    // 最后一块数据已经读完
                    lock();
                    m_Play = FALSE; //停止播放
                    unlock();
                }
            }
        }
        else {
            Frame * frame = nullptr;
            if (0 != apr_queue_size(m_VoiceQueue)) {
                m_VoiceFrame.item = apr_queue_size(m_VoiceQueue);
                apr_queue_pop(m_VoiceQueue, (void**)&frame);
                if (nullptr != frame) {
                    m_VoiceFrame.frame.buffer = frame->buffer;
                    m_VoiceFrame.frame.size = frame->size;
                    m_VoiceFrame.cur = 0;
                    delete frame;
					//LOG_INFO("<------------ Get PCM Data Size :%d ------------->", m_VoiceFrame.frame.size);
                }
            }
            else {
                memset(in_ptr, 0, size);
            }
        }
    }
    else{
        memset(in_ptr, 0, size);
    }
	return;
}

void CAliSynthChannel::recordMain(unsigned char * vioce_data,const apr_size_t& size)
{
	if (nullptr == vioce_data || 0 == size)
		return;
	apr_size_t nsize = size;
	if (m_record && m_file) {
		if (apr_file_write(m_file, vioce_data, &nsize) != APR_SUCCESS) {
			LOG_WARN("record write data failed Channel id :%d", m_id);
		}
	}

	return;
}

void CAliSynthChannel::recordStart(const string& FileName)
{
	if (m_record) {
		apr_status_t stu = apr_file_open(&m_file, FileName.c_str(),
			APR_FOPEN_CREATE | APR_FOPEN_WRITE | APR_FOPEN_BINARY, APR_FPROT_OS_DEFAULT,
			m_pool);

		if (APR_SUCCESS != stu) {
			LOG_WARN("<---- Start record filed error Msg :%s status:%d ---->", CUtil::aprErrorToStr(stu).c_str(), stu);
			recordClose();
		}
	}
}

void CAliSynthChannel::recordClose()
{
	if (m_file) {
		apr_file_close(m_file);
		m_file = NULL;
	}
	return;
}

int CAliSynthChannel::DoGetAliToken()
{

	NlsToken nlsTokenRequest;
	nlsTokenRequest.setAccessKeyId(m_AccessKey);
	nlsTokenRequest.setKeySecret(m_AccessSecret);
	if (-1 == nlsTokenRequest.applyNlsToken()) {
		LOG_ERROR("<---- Get Ali Token Failed:%s ---->", nlsTokenRequest.getErrorMsg()); /*获取失败原因*/
		return -1;
	}
	m_Token = nlsTokenRequest.getToken();
	m_tokenTime = nlsTokenRequest.getExpireTime();	
	return 0;
}

bool CAliSynthChannel::DoCheakSynthToKen()
{
	///**
	//* 获取当前系统时间戳，判断token是否过期
	//*/
	std::time_t curTime = 0;
	curTime = std::time(0);
	if ((m_tokenTime - curTime < 10) || m_Token.empty()) {
		if (-1 == DoGetAliToken()) {
			return FALSE;
		}
		else {
			LOG_INFO("<----------- Get ALi Token :%s --------------->", m_Token.c_str());
			if (m_SynthReq) {
				m_SynthReq->setToken(m_Token.c_str());
			}
		}
	}

	return TRUE;
}

bool CAliSynthChannel::DoSynthRequestInit()
{
	if (m_SynthReq && (0 != m_Text.length())){

		if (-1 == DoCheakSynthToKen()) {
			return FALSE;
		}

		// 设置AppKey, 必填参数, 请参照官网申请
		m_SynthReq->setAppKey(m_appKey.c_str()); 

		//string Text = CUtil::GBKToUTF8(m_Text);
		// 设置待合成文本, 必填参数. 文本内容必须为UTF-8编码
		m_SynthReq->setText(m_Text.c_str());

		// 发音人, 包含"xiaoyun", "ruoxi", "xiaogang"等. 可选参数, 默认是xiaoyun
		if (0 != m_VoiceName.length()) {
			//LOG_INFO("m_VoiceName :%s", m_VoiceName.c_str())
			m_SynthReq->setVoice(m_VoiceName.c_str());
		}

		// 音量, 范围是0~100, 可选参数, 默认50
		if (0 != m_volume.length()) {
			//LOG_INFO("m_volume :%d", atoi(m_volume.c_str()));
			m_SynthReq->setVolume(atoi(m_volume.c_str()));
		}

		//音频编码格式, 可选参数, 默认是wav. 支持的格式pcm, wav, mp3 .偷懒，只支持pcm，不支持其他格式  
		m_SynthReq->setFormat("pcm"); 

		// 音频采样率, 包含8000, 16000. 可选参数, 默认是16000
		if (0 != m_sample.length()) {
			//LOG_INFO("m_sample :%d", atoi(m_sample.c_str()));
			m_SynthReq->setSampleRate(atoi(m_sample.c_str())); 
		}
		
		// 语速, 范围是-500~500, 可选参数, 默认是0
		if (0 != m_speech.length()) {
			//LOG_INFO("m_speech :%d", atoi(m_speech.c_str()));
			m_SynthReq->setSpeechRate(atoi(m_speech.c_str())); 
		}

		// 语调, 范围是-500~500, 可选参数, 默认是0
		if (0 != m_pitch.length()) {
			//LOG_INFO("m_speech :%d", atoi(m_pitch.c_str()));
			m_SynthReq->setPitchRate(atoi(m_pitch.c_str())); 
		}	

		if (-1 == m_SynthReq->start()) {
			LOG_ERROR("Synth Request Start Failed id :%d", m_id);
			DoDestroySynthRequest();
			return FALSE;
		}
	}
	else {
		LOG_ERROR("Synth Request Init Failed id :%d", m_id);
		return FALSE;
	}

	return TRUE;
}

bool CAliSynthChannel::DoCreateSynthRequest()
{
	if (nullptr == m_SynthReq && nullptr != m_SynthCall) {
		m_SynthReq = NlsClient::getInstance()->createSynthesizerRequest(m_SynthCall);
		if (nullptr == m_SynthReq) {
			LOG_ERROR("<---- Create Synth Request Falied Ch id :%d--->",m_id);
			return FALSE;
		}
	}

	return TRUE;
}

void CAliSynthChannel::DoDestroySynthRequest()
{
	if (m_SynthReq) {
		m_SynthReq->stop();
		NlsClient::getInstance()->releaseSynthesizerRequest(m_SynthReq);
	}
	m_SynthReq = nullptr;
	return;
}

void CAliSynthChannel::OnSynthesisStarted(NlsEvent* ev)
{

}

void CAliSynthChannel::OnSynthesisCompleted(NlsEvent* ev)
{
	if (nullptr == m_engineCh)
		return;
	LOG_INFO("<---------- Ali  OnSynthesisCompleted ------------->")
	lock();
	m_Runing = FALSE;
	unlock();

	return;
}

void CAliSynthChannel::OnTaskFailed(NlsEvent* ev)
{
	lock();
	m_Start = FALSE;
	m_Runing = FALSE;
	m_Play = FALSE;
	unlock();
	return;
}

void CAliSynthChannel::OnChannelClosed(NlsEvent* ev)
{

}

void CAliSynthChannel::OnBinaryDataReceived(NlsEvent* ev)
{
	lock();
	//LOG_INFO("<----------Ali OnBinaryDataReceived size:%d ---------->", ev->getBinaryData().size())
	vector<unsigned char> data = ev->getBinaryData();
	if (0 != data.size()) { // 一块数据的最大的大小目前是8000
		m_Voice.push_back(data);
		//recordMain(&data[0], data.size());
	}

	unlock();
}

void CAliSynthChannel::lock()
{
	if (m_lock)
		apr_thread_mutex_lock(m_lock);
	return;
}

void CAliSynthChannel::unlock()
{
	if (m_lock)
		apr_thread_mutex_unlock(m_lock);
	return;
}

void CAliSynthChannel::OnSynthesisStarted(NlsEvent* ev, void * arg)
{
	CAliSynthChannel * This = static_cast<CAliSynthChannel*>(arg);
	if (This)
		This->OnSynthesisStarted(ev);
	return;
}

void CAliSynthChannel::OnSynthesisCompleted(NlsEvent* ev, void * arg)
{
	CAliSynthChannel * This = static_cast<CAliSynthChannel*>(arg);
	if (This)
		This->OnSynthesisCompleted(ev);
	return;
}

void CAliSynthChannel::OnTaskFailed(NlsEvent* ev, void * arg)
{
	CAliSynthChannel * This = static_cast<CAliSynthChannel*>(arg);
	if (This)
		This->OnTaskFailed(ev);
	return;
}

void CAliSynthChannel::OnChannelClosed(NlsEvent* ev, void * arg)
{
	CAliSynthChannel * This = static_cast<CAliSynthChannel*>(arg);
	if (This)
		This->OnChannelClosed(ev);
	return;
}

void CAliSynthChannel::OnBinaryDataReceived(NlsEvent* ev, void * arg)
{
	CAliSynthChannel * This = static_cast<CAliSynthChannel*>(arg);
	if (This)
		This->OnBinaryDataReceived(ev);
	return;
}

