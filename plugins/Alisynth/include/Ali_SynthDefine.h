#ifndef __ALI_DEFINE_H__
#define __ALI_DEFINE_H__

#include "mrcp_synth_engine.h"
#include "mpf_activity_detector.h"
#include "apt_consumer_task.h"
#include "apt_log.h"


#define ALI_PARAM_appkey		"appkey"
#define ALI_PARAM_AccessKey		"AccessKey"
#define ALI_PARAM_AccessSecret	"AccessSecret"
#define ALI_PARAM_voice			"voice"
#define ALI_PARAM_format		"format"
#define ALI_PARAM_sample		"sample"
#define ALI_PARAM_volume		"volume"
#define ALI_PARAM_speech		"speech"
#define ALI_PARAM_pitch			"pitch"
#define ALI_RECORD_PATH			"recordPath"
#define ALI_RECORD				"record"
#define ALI_LOG					"logDir"

#define ALI_PARAM_GET(name,value,default_value)\
											const char * name = nullptr;\
											name = mrcp_engine_param_get(engine, value);\
											if(nullptr == name)\
											{ name = default_value;}

#if _WIN32
#define PATHDIA	 '\\'
#else
#define PATHDIA  '/'
#endif

class CAliSynthChannel;
/** Declaration of Ali synthesizer engine */
struct Ali_synth_engine_t {
	apt_consumer_task_t    *task;
};

/** Declaration of Ali synthesizer channel */
struct Ali_synth_channel_t {
	/** Back pointer to engine */
	Ali_synth_engine_t   *Ali_engine;
	/** Engine channel base */
	mrcp_engine_channel_t *channel;

	/** Active (in-progress) speak request */
	mrcp_message_t        *speak_request;
	/** Pending stop response */
	mrcp_message_t        *stop_response;
	/** Estimated time to complete */
	apr_size_t             time_to_complete;
	/** Is paused */
	apt_bool_t             paused;
	/** Speech source (used instead of actual synthesis) */
	FILE                  *audio_file;
	CAliSynthChannel	  *AliSynthCh;
};

typedef enum {
	Ali_SYNTH_MSG_OPEN_CHANNEL,
	Ali_SYNTH_MSG_CLOSE_CHANNEL,
	Ali_SYNTH_MSG_REQUEST_PROCESS
} Ali_synth_msg_type_e;

/** Declaration of Ali synthesizer task message */
struct Ali_synth_msg_t {
	Ali_synth_msg_type_e  type;
	mrcp_engine_channel_t *channel;
	mrcp_message_t        *request;
};

extern apt_log_source_t * ALI_LOG_SYNTH_PLUGIN;
/** Use custom log source mark */
#define ALI_SYNTH_LOG_MARK   APT_LOG_MARK_DECLARE(ALI_LOG_SYNTH_PLUGIN)

#define LOG_INFO(fmt,...)\
	apt_log(ALI_SYNTH_LOG_MARK,APT_PRIO_INFO,fmt, ## __VA_ARGS__);

#define LOG_DEBUG(fmt,...)\
	apt_log(ALI_SYNTH_LOG_MARK,APT_PRIO_DEBUG,fmt, ## __VA_ARGS__);

#define LOG_WARN(fmt,...)\
	apt_log(ALI_SYNTH_LOG_MARK,APT_PRIO_WARNING,fmt, ## __VA_ARGS__);

#define LOG_ERROR(fmt,...)\
	apt_log(ALI_SYNTH_LOG_MARK,APT_PRIO_ERROR,fmt, ## __VA_ARGS__);

#endif // !__ALI_DEFINE_H__

