#ifndef RM_CAN_LOG_H_
#define RM_CAN_LOG_H_

#define SAFE_DELETE(p)          do { if(p) { delete (p); (p) = 0; } } while(0)
#define SAFE_DELETE_ARRAY(p)    do { if(p) { delete[] (p); (p) = 0; } } while(0)
#define IS_NULL(ptr)            (NULL == (ptr))

#include <unistd.h>
#include <android/log.h>

#define RM_CAN_LOGD
#define RM_CAN_LOGI
//#define RM_CAN_LOGV
#define RM_CAN_LOGW
#define RM_CAN_LOGE
#define RM_CAN_LOGF

#ifdef RM_CAN_LOGD
#define LOGD(TAG, ...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#else
#define LOGD(TAG, ...)
#endif

#ifdef RM_CAN_LOGI
#define LOGI(TAG, ...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#else
#define LOGI(TAG, ...)
#endif

#ifdef RM_CAN_LOGV
#define LOGV(TAG, ...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#else
#define LOGV(TAG, ...)
#endif

#ifdef RM_CAN_LOGW
#define LOGW(TAG, ...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)
#else
#define LOGW(TAG, ...)
#endif

#ifdef RM_CAN_LOGE
#define LOGE(TAG, ...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#else
#define LOGE(TAG, ...)
#endif

#ifdef RM_CAN_LOGF
#define LOGF(TAG, ...) __android_log_print(ANDROID_LOG_FATAL, TAG, __VA_ARGS__)
#else
#define LOGF(TAG, ...)
#endif

#endif /* RM_CAN_LOG_H_ */
