#ifndef AIENGINE_H
#define AIENGINE_H
#ifdef __cplusplus
extern "C" {
#endif

#if (!(defined AIENGINE_CALL) || !(defined AIENGINE_IMPORT_OR_EXPORT))
#    if defined __WIN32__ || defined _WIN32 || defined _WIN64
#       define AIENGINE_CALL __stdcall
#       ifdef  AIENGINE_IMPLEMENTION
#           define AIENGINE_IMPORT_OR_EXPORT __declspec(dllexport)
#       else
#           define AIENGINE_IMPORT_OR_EXPORT __declspec(dllimport)
#       endif
#    elif defined __ANDROID__
#       define AIENGINE_CALL
#       define AIENGINE_IMPORT_OR_EXPORT
#    else
#       define AIENGINE_CALL
#       define AIENGINE_IMPORT_OR_EXPORT __attribute ((visibility("default")))
#    endif
#endif

#define AIENGINE_VERSION "2.9.4"

enum {
    AIENGINE_MESSAGE_TYPE_JSON = 1,
    AIENGINE_MESSAGE_TYPE_BIN
};
struct aiengine;
typedef struct aiauth {
    char    *provision;
    char    *serialnumber;
    int     provision_size;
    int     serialnumber_size;
} aiauth;
typedef int (AIENGINE_CALL *aiengine_callback)(void *userdata, const char *id, int type, const void *message, int size);
typedef int (AIENGINE_CALL *http_request_cb_t)(void *userdata, const char *url, char **response);
typedef int (AIENGINE_CALL *get_device_id_cb_t)(char deviceId[64], const char *type);

/*
{
    "appKey":"fdjalfjdlg",
    "secretKey":"fdjksafji8",
    "provision":"aiengien.provison",
    "serialNumber":"aiengine.serialnumber",
    "version":"aiengine-2.9.4",
    "native":{
        "cn.wakeup":{
            "resBinPath":""
        }
    }
}
*/
AIENGINE_IMPORT_OR_EXPORT struct aiengine* AIENGINE_CALL aiengine_new(const char *cfg, get_device_id_cb_t getid, struct aiauth *auth);
AIENGINE_IMPORT_OR_EXPORT int AIENGINE_CALL aiengine_start(struct aiengine *engine, const char *param, char id[64], aiengine_callback callback, void *userdata);
AIENGINE_IMPORT_OR_EXPORT int AIENGINE_CALL aiengine_feed(struct aiengine *engine, const void *data, int size);
AIENGINE_IMPORT_OR_EXPORT int AIENGINE_CALL aiengine_cancel(struct aiengine *engine);
AIENGINE_IMPORT_OR_EXPORT int AIENGINE_CALL aiengine_stop(struct aiengine *engine);
AIENGINE_IMPORT_OR_EXPORT int AIENGINE_CALL aiengine_delete(struct aiengine *engine);
/* cfg string format
{
    "appKey":"fdjalfjdlg",
    "secretKey":"fdjksafji8",
    "provision":"aiengien.provison",
    "serialNumber":"aiengine.serialnumber",
    "verision":"aiengine-2.9.4"
}
*/
AIENGINE_IMPORT_OR_EXPORT int AIENGINE_CALL aiengine_check_auth(const char *cfg, char **err, get_device_id_cb_t getid, struct aiauth *auth);

/* cfg string format
{
    "appKey":"fdjalfjdlg",
    "secretKey":"fdjksafji8",
    "provision":"aiengien.provison",
    "serialNumber":"aiengine.serialnumber",
    "verision":"aiengine-2.9.4"
}
*/
AIENGINE_IMPORT_OR_EXPORT int AIENGINE_CALL aiengine_do_auth(const char *cfg, char **err, http_request_cb_t cb, get_device_id_cb_t getid, void *user_data, struct aiauth *auth, char **serialnumber, int *serialnumber_size);

#ifdef __cplusplus
}
#endif
#endif
