#ifndef __ALI_RECOG_DEFINE_H__
#define __ALI_RECOG_DEFINE_H__

#include "mrcp_recog_engine.h"
#include "mpf_activity_detector.h"
#include "apt_consumer_task.h"
#include "apt_log.h"

#if _WIN32
#include<windows.h>

#else
#include<pthread.h>
#endif

#define ALI_PARAM_GET(name,value,default_value)\
											const char * name = nullptr;\
											name = mrcp_engine_param_get(engine, value);\
											if(nullptr == name)\
											{ name = default_value;}

#define MAX_QUEUE_SIZE			10 * 100
#define SEEP_RECOGNIZER			"recognizer"
#define SEEP_TRANSCRIBER		"Transcriber"

#if _WIN32
#define PATHDIA	 '\\'
#else
#define PATHDIA  '/'
#endif

class CAliChannel;
class CAliResource;

/** Declaration of Ali recognizer engine */
struct Ali_recog_engine_t {
	apt_consumer_task_t    *task;
};

/** Declaration of Ali recognizer channel */
struct Ali_recog_channel_t {
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
	CAliChannel				*AliCh;
	CAliResource			*AliRe;
};

using EngineChannel = Ali_recog_channel_t;

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

extern apt_log_source_t * ALI_LOG_RECOG_PLUGIN;
/** Use custom log source mark */
#define ALI_RECOG_LOG_MARK   APT_LOG_MARK_DECLARE(ALI_LOG_RECOG_PLUGIN)

#define LOG_INFO(fmt,...)\
	apt_log(ALI_RECOG_LOG_MARK,APT_PRIO_INFO,fmt, ## __VA_ARGS__);

#define LOG_DEBUG(fmt,...)\
	apt_log(ALI_RECOG_LOG_MARK,APT_PRIO_DEBUG,fmt, ## __VA_ARGS__);

#define LOG_WARN(fmt,...)\
	apt_log(ALI_RECOG_LOG_MARK,APT_PRIO_WARNING,fmt, ## __VA_ARGS__);

#define LOG_ERROR(fmt,...)\
	apt_log(ALI_RECOG_LOG_MARK,APT_PRIO_ERROR,fmt, ## __VA_ARGS__);

#endif // !__ALI_RECOG_DEFINE_H__
