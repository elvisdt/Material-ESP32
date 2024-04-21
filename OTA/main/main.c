// includes do sistema --------------------------------------------------------
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_event.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// #include <math.h>
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_netif.h"
#include "esp_rom_gpio.h"

// Carrega os protótipos e variáveis globais ----------------------------------
#include "main.h"

void print_heap(int i) {
   printf("Free heap [%d]: %lu\r\n", i, esp_get_free_heap_size());
}

#ifdef DEBUG_TASK_MONITOR
static void print_task_monitor(void *pvParameters) {
   char msg[500]={0};
   while (1) {
      print_heap(0);
      vTaskList(msg);
      printf("Task----------State----Prio----Stack---Num---Pinned");
      printf("\r\n%s", msg);
      vTaskDelay(2000 / portTICK_PERIOD_MS);
   }
}
#endif

void app_controller(void *pvParameters) {
   ESP_LOGI(LOG_MAIN, "Init app_controller");
   bool blink = 0;
   while (1) {
      gpio_set_level(PIN_LED, blink);
      vTaskDelay(TEMPO_BLINK / portTICK_PERIOD_MS);
      blink^=1;
   }
   vTaskDelete(NULL);
}

// Hardware -------------------------------------------------------------------
void inicia_hardware() {
   // Configura o pino do LED como sída e desliga na inicialização.
   esp_rom_gpio_pad_select_gpio(PIN_LED);
   gpio_set_direction(PIN_LED, GPIO_MODE_OUTPUT);
   gpio_set_level(PIN_LED, 0);
}

void inicia_variaveis_aplicacao() {
   // Cria os semáforos para controle de operações
   app_event_group = xEventGroupCreate();
}

void app_main() {
   ESP_LOGI("APP_MAIN","Compiled at: "__TIME__ ", " __DATE__);

   ESP_LOGI("APP_MAIN","Init NVS keys of data");
	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
	}
    ESP_ERROR_CHECK(err);

   inicia_hardware();
   inicia_variaveis_aplicacao();
   xTaskCreatePinnedToCore(app_controller, "app_controller", 4000, NULL, 15, &taskhandle_app_controller, 1);
   wifi_app_init();

// Monitoramento de heap
#ifdef DEBUG_TASK_MONITOR
   xTaskCreatePinnedToCore(&print_task_monitor, "print_task_monitor", 3500, NULL, 1, NULL, 0);
#endif
}