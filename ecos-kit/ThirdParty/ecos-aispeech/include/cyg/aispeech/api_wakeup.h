/**
 * Project  : AIWakeup.v1.0.7
 * FileName : api/api_wakeup.h
 *
 * COPYRIGHT (C) 2015-2016, AISpeech Ltd.. All rights reserved.
 */
#ifndef __AISPEECH__API_WAKEUP_H__
#define __AISPEECH__API_WAKEUP_H__

typedef enum wakeup_status {
    WAKEUP_STATUS_ERROR = -1,     // 唤醒出现异常
    WAKEUP_STATUS_WAIT,           // 等待唤醒
    WAKEUP_STATUS_WOKEN,          // 已唤醒
    WAKEUP_STATUS_WOKEN_BOUNDARY,
    WAKEUP_STATUS_RESTART
} wakeup_status_t;

typedef struct wakeup wakeup_t;

/**
 * 回调函数定义
 *
 * @param user_data: 用户传入的数据结构指针
 * @param status: 唤醒状态
 * @param json: 返回唤醒模块的信息
 * @param bytes: JSON串的字节数
 */
typedef int (*wakeup_handler_t)(void *user_data, wakeup_status_t status, char *json, int bytes);
typedef int (*vad_handler_t)(void *user_data,int frame_status,int frame_index);

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 创建唤醒模块的实例
 *
 * @param mem_base: reserved memory for wakeup。
 * @param json: 个性化定制时，此文件路径为加密后的本地通用唤醒的配置信息，内容详见[个性化定制配置格式]
 */
wakeup_t *wakeup_new(unsigned char *mem_base, const char *json);

/**
 * 清理唤醒实例并释放内存
 *
 * @param w: 唤醒模块的instance
 */
void wakeup_delete(wakeup_t *w);

/**
 * 重置唤醒模块
 *
 * @param w: 唤醒模块的intance
 */
void wakeup_reset(wakeup_t *w);

/**
 * 启动唤醒检查
 *
 * @param w: 唤醒模块的instance
 * @param env: 自定义参数
 * @param bytes: 自定义参数的长度
 */
int wakeup_start(wakeup_t *w, char *env, int bytes);

/**
 * 提供音频数据给唤醒模块，判断唤醒状态
 *
 * @param w: 唤醒模块的instance
 * @param data: 音频数据
 * @param bytes: 数据字节数
 *
 * @return 唤醒状态
 */
wakeup_status_t wakeup_feed(wakeup_t *w, char *data, int bytes);

/**
 * 清理唤醒模块的cache及状态
 *
 * @param w: 唤醒模块的instance
 */
wakeup_status_t wakeup_end(wakeup_t *w);

/**
 * 注册状态的回调函数
 *
 * @param w: 唤醒模块的instance
 * @param user_data: 用户的数据结构
 * @param func: 注册的函数
 */
void wakeup_register_handler(wakeup_t *w, void *user_data, wakeup_handler_t func);
void vad_register_handler(wakeup_t *w,void *user_data,vad_handler_t func);

#ifdef __cplusplus
}
#endif

#endif //__AISPEECH__API_WAKEUP_H__
