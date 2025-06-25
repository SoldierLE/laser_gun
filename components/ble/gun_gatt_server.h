#ifndef _GUN_GATT_SERVER_H_
#define _GUN_GATT_SERVER_H_

#include <string.h>
#include "esp_err.h"

void gun_ble_init(void);

/* Attributes State Machine */
enum
{
    IDX_SVC,

    IDX_CHAR_APP_TO_GUN,
    IDX_CHAR_VAL_APP_TO_GUN,

    IDX_CHAR_GUN_TO_APP_HEART,
    IDX_CHAR_VAL_GUN_TO_APP_HEART,
    IDX_CHAR_CFG_GUN_HEART,

    IDX_CHAR_GUN_TO_APP_KEY,
    IDX_CHAR_VAL_GUN_TO_APP_KEY,
    IDX_CHAR_CFG_GUN_KEY,

    IDX_CHAR_GUN_TO_APP_CONTROL,
    IDX_CHAR_VAL_GUN_TO_APP_CONTROL,
    IDX_CHAR_CFG_GUN_CONTROL,

    IDX_CHAR_GUN_TO_APP_INFRARED,
    IDX_CHAR_VAL_GUN_TO_APP_INFRARED,
    IDX_CHAR_CFG_GUN_INFRARED,

    IDX_CHAR_DEVICE_RENAME,
    IDX_CHAR_VAL_DEVICE_RENAME,

    IDX_CHAR_DEVICE_RENAME_FEEDBACK,
    IDX_CHAR_VAL_DEVICE_RENAME_FEEDBACK,
    IDX_CHAR_CFG_DEVICE_RENAME_FEEDBACK,

    HRS_IDX_NB,
};

uint8_t get_gun_ble_connect_status(void);

// 设备改名相关函数
esp_err_t gun_ble_set_device_name(const char* device_name);
esp_err_t gun_ble_get_device_name(char* device_name, size_t max_len);

#endif
