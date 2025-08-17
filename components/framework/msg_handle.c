#include "msg_handle.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include <string.h>
#include "driver/timer.h"
#include "esp_timer.h"

static const char *TAG = "msg_handle";

#define APP_TO_GUN_DATA_SIZE    sizeof(app_to_gun_data_t)
#define GUN_TO_APP_DATA_SIZE    sizeof(ble_notify_msg_t)
#define QUEUE_LENGTH            10 

QueueHandle_t msg_Queue_to_gun, msg_Queue_to_app;
static app_to_gun_data_t app_to_gun_data = {0x00};
// 移除定时器相关代码
// static esp_timer_handle_t msg_timer;
static ble_event_callback cb[BLE_NOTIFY_MSG_MAX_NUM];

app_to_gun_data_t *get_app_to_gun_data(void)
{
    return &app_to_gun_data; 
}

// 新增：实时更新数据的函数
void msg_handle_update_data(void *p_msg)
{
    if(p_msg == NULL) {
        ESP_LOGE(TAG, "p_msg is NULL");
        return;
    }
    
    // 立即更新数据，不等待定时器
    memcpy(&app_to_gun_data, p_msg, sizeof(app_to_gun_data_t));
    ESP_LOGI(TAG, "Data updated immediately, user_code: 0x%04X", app_to_gun_data.user_code);
}

// 新增：清空数据的函数
void msg_handle_clear_data(void)
{
    memset(&app_to_gun_data, 0x00, sizeof(app_to_gun_data_t));
    ESP_LOGI(TAG, "Data cleared");
}

void msg_handle_send(void *p_msg)
{
    if(p_msg == NULL) {
        ESP_LOGE(TAG, "p_msg is NULL");
        return;
    }
    
    // 立即更新数据
    msg_handle_update_data(p_msg);
    
    // 同时发送到队列（保持向后兼容）
    xQueueSend(msg_Queue_to_gun, p_msg, 0);
}

void msg_handle_register(ble_type_t type, ble_event_callback event_cb)
{
    if(event_cb == NULL)
        return;
    
    if(type >= BLE_NOTIFY_MSG_MAX_NUM)
        return;

    cb[type] = event_cb;
}

void msg_handle_notify(ble_type_t type, void *data, uint8_t len)
{
    if(data == NULL)
        return;

    ble_notify_msg_t notify_msg_t = {0};
    notify_msg_t.handle_type = type;
    notify_msg_t.len = len;
    memcpy(notify_msg_t.data, data, len);
    
    xQueueSend(msg_Queue_to_app, &notify_msg_t, portMAX_DELAY);
}

void msg_handle_task(void)
{
    ble_notify_msg_t notify_msg_t;

    if(xQueueReceive(msg_Queue_to_app, &notify_msg_t, portMAX_DELAY) == pdTRUE) {
        cb[notify_msg_t.handle_type](&notify_msg_t);
    }
}

void msg_handle_init(void)
{
    ESP_LOGI(TAG, "data_msg_handle init");

    msg_Queue_to_gun = xQueueCreate(QUEUE_LENGTH, APP_TO_GUN_DATA_SIZE);
    msg_Queue_to_app = xQueueCreate(QUEUE_LENGTH, GUN_TO_APP_DATA_SIZE);
    if ((msg_Queue_to_gun == NULL) && (msg_Queue_to_app == NULL)) {
        ESP_LOGE(TAG, "data_msg_handle queue create failed");
    }

    ESP_LOGI(TAG, "data_msg_handle end - Event driven mode enabled");

    // 移除定时器创建和启动代码
    // 现在使用事件驱动，数据会立即处理
}
