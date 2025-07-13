#include "gun_presskey.h"
#include "driver/gpio.h"
#include "gun_multi_button.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gun_gatt_server.h"

#include "gun_infrared.h"
#include "msg_handle.h"

static const char *TAG = "gun_presskey";

static Button botton_shoot;
static Button botton_map;
static Button botton_panel;
static Button botton_skill;
static Button botton_score;
static Button botton_reload;
static Button botton_ultre;
static Button botton_scope;
static Button botton_voice;

#define PIN_LOW                 0x00
#define PIN_HIGH                0x01 

#define GUN_MAP_PIN                   GPIO_NUM_2
#define GUN_READ_MAP_PIN              gpio_get_level(GUN_MAP_PIN)
#define GUN_PANEL_PIN                 GPIO_NUM_4
#define GUN_READ_PANEL_PIN            gpio_get_level(GUN_PANEL_PIN)
#define GUN_SCORE_PIN                 GPIO_NUM_14
#define GUN_READ_SCORE_PIN            gpio_get_level(GUN_SCORE_PIN)
#define GUN_SHOOT_PIN                 GPIO_NUM_18
#define GUN_READ_SHOOT_PIN            gpio_get_level(GUN_SHOOT_PIN)
#define GUN_VOICE_PIN                 GPIO_NUM_21
#define GUN_READ_VOICE_PIN            gpio_get_level(GUN_VOICE_PIN)
#define GUN_RELOAD_PIN                GPIO_NUM_37
#define GUN_READ_RELOAD_PIN           gpio_get_level(GUN_RELOAD_PIN)
#define GUN_ULTRE_PIN                 GPIO_NUM_40
#define GUN_READ_ULTRE_PIN            gpio_get_level(GUN_ULTRE_PIN)
#define GUN_SCOPE_PIN                 GPIO_NUM_41
#define GUN_READ_SCOPE_PIN            gpio_get_level(GUN_SCOPE_PIN)
#define GUN_SKILL_PIN                 GPIO_NUM_42
#define GUN_READ_SKILL_PIN            gpio_get_level(GUN_SKILL_PIN)


static uint8_t read_button_shoot(void)
{
    return GUN_READ_SHOOT_PIN;
}

static uint8_t read_button_map(void)
{
    return GUN_READ_MAP_PIN;
}

static uint8_t read_button_panel(void)
{
    return GUN_PANEL_PIN;
}

static uint8_t read_botton_skill(void)
{
    return GUN_READ_SKILL_PIN;
}

static uint8_t read_botton_score(void)
{
    return GUN_READ_SCORE_PIN;
}

static uint8_t read_botton_reload(void)
{
    return GUN_READ_RELOAD_PIN;
}

static uint8_t read_botton_ultre(void)
{
    return GUN_READ_ULTRE_PIN;
}

static uint8_t read_botton_scope(void)
{
    return GUN_READ_SCOPE_PIN;
}

static uint8_t read_botton_voice(void)
{
    return GUN_READ_VOICE_PIN;
}

static void button_cbk(void *button)
{
    uint8_t event_val = 0, button_data[4] = {0x00}, data_len = 0;
	
	event_val = get_button_event((struct Button *)button);
    uint8_t ble_cnn_status = get_gun_ble_connect_status();

    if (event_val == SINGLE_CLICK && ble_cnn_status == 1)
    {
        button_data[0] = 0x68;
        button_data[1] = 0x00;
        button_data[2] = 0x00;
        button_data[3] = 0x16;

        if ((struct Button *)button == &botton_map)
        {
            ESP_LOGI(TAG, "----MAP SINGLE_CLICK----");
            button_data[1] = 0x00;  // D1: 地图按键
            button_data[2] = 0x02;  // 第二个字节
        }
        else if ((struct Button *)button == &botton_score)
        {
            ESP_LOGI(TAG, "----SCORE SINGLE_CLICK----");
            button_data[1] = 0x00;  // D2: 积分按键
            button_data[2] = 0x04;  // 第二个字节
        }
        else if ((struct Button *)button == &botton_scope)
        {
            ESP_LOGI(TAG, "----SCOPE SINGLE_CLICK----");
            button_data[1] = 0x00;  // D3: 倍镜按键
            button_data[2] = 0x08;  // 第二个字节
        }
        else if ((struct Button *)button == &botton_ultre)
        {
            ESP_LOGI(TAG, "----ULTRE SINGLE_CLICK----");
            button_data[1] = 0x00;  // D4: 大招按键
            button_data[2] = 0x10;  // 第二个字节
        }
        else if ((struct Button *)button == &botton_skill)
        {
            ESP_LOGI(TAG, "----SKILL SINGLE_CLICK----");
            button_data[1] = 0x00;  // D7: 小招按键
            button_data[2] = 0x80;  // 第二个字节
        }
        else if ((struct Button *)button == &botton_panel)
        {
            ESP_LOGI(TAG, "----PANEL SINGLE_CLICK----");
            button_data[1] = 0x00;  // D0: 放倒按键 (原来D8改为D0)
            button_data[2] = 0x01;  // 第二个字节
        }

        data_len = sizeof(button_data);
        msg_handle_notify(BLE_KEY_EVENT, button_data, data_len);
    }
    else if (event_val == PRESS_DOWN && ble_cnn_status == 1)
    {
        // SHOOT、RELOAD和VOICE按键按下立即触发
        button_data[0] = 0x68;
        button_data[1] = 0x00;
        button_data[2] = 0x00;
        button_data[3] = 0x16;

        if ((struct Button *)button == &botton_shoot)
        {
            ESP_LOGI(TAG, "----SHOOT PRESS_DOWN----");
            button_data[1] = 0x00;  // D6: 射击按键
            button_data[2] = 0x40;  // 第二个字节
        }
        else if ((struct Button *)button == &botton_reload)
        {
            ESP_LOGI(TAG, "----RELOAD PRESS_DOWN----");
            button_data[1] = 0x00;  // D5: 上弹按键
            button_data[2] = 0x20;  // 第二个字节
        }
        else if ((struct Button *)button == &botton_voice)
        {
            ESP_LOGI(TAG, "----VOICE PRESS_DOWN (语音开)----");
            button_data[1] = 0x01;  // 第一个字节
            button_data[2] = 0x00;  // D8: 队内语音开 (第二个字节bit 0)
        }
        else
        {
            return;  // 其他按键不处理PRESS_DOWN事件
        }

        data_len = sizeof(button_data);
        msg_handle_notify(BLE_KEY_EVENT, button_data, data_len);
    }
    else if (event_val == PRESS_UP && ble_cnn_status == 1)
    {
        // VOICE按键释放时触发
        if ((struct Button *)button == &botton_voice)
        {
            ESP_LOGI(TAG, "----VOICE PRESS_UP (语音关)----");
            button_data[0] = 0x68;
            button_data[1] = 0x02;  // 第一个字节
            button_data[2] = 0x00;  // D9: 队内语音关 (第二个字节bit 1)
            button_data[3] = 0x16;

            data_len = sizeof(button_data);
            msg_handle_notify(BLE_KEY_EVENT, button_data, data_len);
        }
    }
}

static void presskey_thread_entry(void *arg)
{
    for (;;)
    {
        button_ticks();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void gun_presskey_config(void)
{
    xTaskCreate(presskey_thread_entry, "presskey_thread_entry", 3584, NULL, 6, NULL);

    button_init(&botton_shoot, read_button_shoot, PIN_LOW);
    button_attach(&botton_shoot, PRESS_DOWN, button_cbk);  // 注册按下事件，立即触发
    button_start(&botton_shoot);

    button_init(&botton_map, read_button_map, PIN_LOW);
    button_attach(&botton_map, SINGLE_CLICK, button_cbk); // 注册单击事件
    button_start(&botton_map);

    button_init(&botton_panel, read_button_panel, PIN_LOW);
    button_attach(&botton_panel, SINGLE_CLICK, button_cbk); // 注册单击事件
    button_start(&botton_panel);

    button_init(&botton_skill, read_botton_skill, PIN_LOW);
    button_attach(&botton_skill, SINGLE_CLICK, button_cbk); // 注册skill单击事件
    button_start(&botton_skill);

    button_init(&botton_score, read_botton_score, PIN_LOW);
    button_attach(&botton_score, SINGLE_CLICK, button_cbk); // 注册score单击事件
    button_start(&botton_score);

    button_init(&botton_reload, read_botton_reload, PIN_LOW);
    button_attach(&botton_reload, PRESS_DOWN, button_cbk);  // 注册按下事件，立即触发
    button_start(&botton_reload);

    button_init(&botton_ultre, read_botton_ultre, PIN_LOW);
    button_attach(&botton_ultre, SINGLE_CLICK, button_cbk);
    button_start(&botton_ultre);

    button_init(&botton_scope, read_botton_scope, PIN_LOW);
    button_attach(&botton_scope, SINGLE_CLICK, button_cbk);
    button_start(&botton_scope);

    button_init(&botton_voice, read_botton_voice, PIN_LOW);
    button_attach(&botton_voice, PRESS_DOWN, button_cbk);  // 注册按下事件
    button_attach(&botton_voice, PRESS_UP, button_cbk);    // 注册释放事件
    button_start(&botton_voice);
}

void gun_presskey_init(void)
{
    gpio_config_t chg_config = {
        .mode = GPIO_MODE_INPUT,
        .intr_type = GPIO_INTR_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    chg_config.pin_bit_mask = 1ULL << GUN_MAP_PIN;
    gpio_config(&chg_config);
    chg_config.pin_bit_mask = 1ULL << GUN_PANEL_PIN;
    gpio_config(&chg_config);
    chg_config.pin_bit_mask = 1ULL << GUN_SKILL_PIN;
    gpio_config(&chg_config);
    chg_config.pin_bit_mask = 1ULL << GUN_SCORE_PIN;
    gpio_config(&chg_config);
    chg_config.pin_bit_mask = 1ULL << GUN_ULTRE_PIN;
    gpio_config(&chg_config);
    chg_config.pin_bit_mask = 1ULL << GUN_SCOPE_PIN;
    gpio_config(&chg_config);
    chg_config.pin_bit_mask = 1ULL << GUN_VOICE_PIN;
    gpio_config(&chg_config);

    chg_config.pull_up_en = GPIO_PULLUP_DISABLE;

    chg_config.pin_bit_mask = 1ULL << GUN_SHOOT_PIN;
    gpio_config(&chg_config);
    chg_config.pin_bit_mask = 1ULL << GUN_RELOAD_PIN;
    gpio_config(&chg_config);
}
