
#ifndef __ALI_DEFINE_H__
#define __ALI_DEFINE_H__

#include "mrcp_recog_engine.h"
#include "mpf_activity_detector.h"
#include "apt_consumer_task.h"
#include "apt_log.h"
//#include "Ali_channel_factory.h"

//#include "Ali_channel_factory.h"

/*********************** Ali 识别参数定义 ***********************/
#define ALI_APPID					"appKey"
#define ALI_KEYID					"AccessKeyID"
#define ALI_SECRET					"AccessKeySecret"
#define ALI_FOEMAT					"Format"
#define ALI_SAMPLERATE				"SampleRate"
#define ALI_RECORD_PATH				"recordPath"
#define ALI_RECORD					"record"
#define ALI_LOG						"logDir"

#define ALI_RECOG_URL				"SpeechRecogUrl"
#define ALI_INTERMEDIATE_RESULT		"IntermediateResult"
#define ALI_PUNCTUATION_PREDICTION	"PunctuationPrediction"
#define ALI_INVERSE_TEXT			"InverseTextNormalization"
#define ALI_VOICE_DETEION			"EnableVoiceDetection"
#define ALI_START_SILENCE			"StartSilence"
#define ALI_END_SILENCE				"EndSilence"
#define ALI_CUSTOMIZATION_ID		"CustomizationId"
#define ALI_VOCABULARY_ID			"VocabularyId"
#define ALI_OUT_PUT_FORMAT			"OutputFormat"
#define ALI_CONTEXT_PARAM			"ContextParam"

#define ALI_CFG_PARAM_GET(param,name,defaultvalue,engine)\
													param = mrcp_engine_param_get(engine, name);\
													if(nullptr == param)\
													{param = defaultvalue; }
													
												


#if _WIN32
#define PATHDIA	 '\\'
#else
#define PATHDIA  '/'
#endif

class AliChannel;

/** Declaration of Ali recognizer engine */
struct Ali_recog_engine_t {
	apt_consumer_task_t    *task;
};

/** Declaration of Ali recognizer channel */
typedef struct Ali_recog_channel_t {
	/** Back pointer to engine */
	Ali_recog_engine_t     *Ali_engine;
	/** Engine channel base */
	mrcp_engine_channel_t   *channel;

	/** Active (in-progress) recognition request */
	mrcp_message_t          *recog_request;
	/** Pending stop response */
	mrcp_message_t          *stop_response;
	/** Indicates whether input timers are started */
	apt_bool_t               timers_started;
	/** Voice activity detector */
	mpf_activity_detector_t *detector;
	/** File to write utterance to */
	FILE                    *audio_out;
	class AliChannel*ch;
}Ali_recog_channel;

typedef enum {
	Ali_RECOG_MSG_OPEN_CHANNEL,
	Ali_RECOG_MSG_CLOSE_CHANNEL,
	Ali_RECOG_MSG_REQUEST_PROCESS
} Ali_recog_msg_type_e;

/** Declaration of Ali recognizer task message */
struct Ali_recog_msg_t {
	Ali_recog_msg_type_e  type;
	mrcp_engine_channel_t *channel;
	mrcp_message_t        *request;
};

//apt_log(RECOG_LOG_MARK, APT_PRIO_WARNING, "Failed to Get Codec Descriptor " APT_SIDRES_FMT, MRCP_MESSAGE_SIDRES(request));
extern apt_log_source_t * ALI_LOG_RECOG_PLUGIN;
/** Use custom log source mark */
#define ALI_RECOG_LOG_MARK   APT_LOG_MARK_DECLARE(ALI_LOG_RECOG_PLUGIN)
#define LOG_BUF	4096

extern void WriteLog(apt_log_priority_e leve, const char * format, ...);
//extern int  RecogStart(void * channel, apr_thread_start_t proce_func);
extern std::string TimeToStr(apr_time_t nowTime, const char* format);
extern std::string apr_error(apr_status_t stu);

#define LOG_INFO(fmt,...)\
	apt_log(ALI_RECOG_LOG_MARK,APT_PRIO_INFO,fmt, ## __VA_ARGS__);

#define LOG_DEBUG(fmt,...)\
	apt_log(ALI_RECOG_LOG_MARK,APT_PRIO_DEBUG,fmt, ## __VA_ARGS__);

#define LOG_WARN(fmt,...)\
	apt_log(ALI_RECOG_LOG_MARK,APT_PRIO_WARNING,fmt, ## __VA_ARGS__);

#define LOG_ERROR(fmt,...)\
	apt_log(ALI_RECOG_LOG_MARK,APT_PRIO_ERROR,fmt, ## __VA_ARGS__);

#if 0
#define LOG_INFO(fmt,...) WriteLog(APT_PRIO_INFO,fmt, ## __VA_ARGS__);
	

#define LOG_DEBUG(fmt,...) WriteLog(APT_PRIO_DEBUG,fmt, ## __VA_ARGS__);
	

#define LOG_WARN(fmt,...) WriteLog(APT_PRIO_WARNING,fmt, ## __VA_ARGS__);
	

#define LOG_ERROR(fmt,...) WriteLog(APT_PRIO_ERROR,fmt, ## __VA_ARGS__);
#endif

#if 0
/*
//apt_log(ALI_RECOG_LOG_MARK, APT_PRIO_INFO, "Detected End of Event " APT_SIDRES_FMT " id:%d duration:%d ts",
//	MRCP_MESSAGE_SIDRES(recog_channel->recog_request),

//#define LOG_TEST(fmt,...)	apt_log(ALI_RECOG_LOG_MARK,APT_PRIO_INFO,fmt,## __VA_ARGS__);
//apt_log(FS_ASR_LOG_MARK, APT_PRIO_INFO, "<%s> " fmt, __FUNCTION__, ## __VA_ARGS__)

//#define LOG_TEST(fmt,...)	\
//	apt_log(ALI_RECOG_LOG_MARK,APT_PRIO_INFO, "<%s> " fmt, __FUNCTION__, ## __VA_ARGS__);
*/
#endif

#endif // !__ALI_DEFINE_H__


















