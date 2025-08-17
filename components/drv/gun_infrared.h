#ifndef GUN_INFRARED_H
#define GUN_INFRARED_H

#include <stdint.h>
#include <stdbool.h>
#include "msg_handle.h"

// 红外发射相关函数
void gun_ir_tx_init(void);
void gun_ir_tx_task(uint8_t channel);

// 红外接收相关函数
void gun_ir_rx_init(void);
void gun_ir_rx_task(void *arg);

// 红外数据实时上传函数
void gun_ir_upload_data_immediately(uint8_t user_id, uint8_t war_status);

// 设置红外数据上传模式（实时/批量）
void gun_ir_set_upload_mode(bool real_time_mode);

#endif
