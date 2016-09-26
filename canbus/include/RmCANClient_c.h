/*
 */

#ifndef RM_CLIENT_C_H_
#define RM_CLIENT_C_H_

#ifdef __cplusplus
extern "C"{
#endif

struct can_client_callback
{
    void (*RmRecvCANData)(int priority, int src_id, const void *pdata, int len);
    void (*RmCANServiceDied)(void);
};

/*
 * success: 0
 * failed: -1
 */
int RmInitCANClient(int module_id, struct can_client_callback *callback);
void RmDeinitCANClient(void);

/*
 * success: 0
 * failed: -1
 */
int RmSendCANData(int dest_id, const void *pdata, int len, int priority);

#ifdef __cplusplus
}
#endif

#endif /* RM_CLIENT_C_H_ */
