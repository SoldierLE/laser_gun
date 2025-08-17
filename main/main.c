#include <stdio.h>
#include "sdkconfig.h"
#include "nvs_flash.h"

#include "gun_adc.h"
#include "gun_charge.h"
#include "gun_presskey.h"
#include "gun_ws2812.h"
#include "gun_infrared.h"
#include "gun_gatt_server.h"
#include "gun_ble_app.h"

// 测试蓝牙改名功能
static void test_ble_rename(void)
{
    // 等待蓝牙初始化完成
    //vTaskDelay(pdMS_TO_TICKS(2000));
    
    // 测试设置设备名称
    esp_err_t ret = gun_ble_set_device_name("LG123456789");
    if (ret == ESP_OK) {
        printf("设备名称设置成功: LG123456789\n");
    } else {
        printf("设备名称设置失败: %s\n", esp_err_to_name(ret));
    }
    
    // 测试获取设备名称
    char device_name[20];
    ret = gun_ble_get_device_name(device_name, sizeof(device_name));
    if (ret == ESP_OK) {
        printf("当前设备名称: %s\n", device_name);
    } else {
        printf("获取设备名称失败: %s\n", esp_err_to_name(ret));
    }
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    //初始化蓝牙
    gun_ble_init();
    gun_ble_app_init();
    
    //初始化按键
    gun_presskey_init();
    gun_presskey_config();
    //使能adc
    gun_adc_init();
    //初始化充电task
    gun_charge_init();
    //初始化红外发送
    gun_ir_tx_init();
    //初始化红外接收
    gun_ir_rx_init();
    //初始化ws2812
    gun_ws2812_init();
    
    // 测试蓝牙改名功能
    //test_ble_rename();
}
