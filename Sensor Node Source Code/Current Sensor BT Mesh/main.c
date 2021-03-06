/***********************************************************************************************//**
 * \file   main.c
 * \brief  Silicon Labs Empty Example Project
 *
 * This example demonstrates the bare minimum needed for a Blue Gecko C application
 * that allows Over-the-Air Device Firmware Upgrading (OTA DFU). The application
 * starts advertising after boot and restarts advertising after a connection is closed.
 ***************************************************************************************************
 * <b> (C) Copyright 2016 Silicon Labs, http://www.silabs.com</b>
 ***************************************************************************************************
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 **************************************************************************************************/

/* Board headers */
#include "init_mcu.h"
#include "init_board.h"
#include "init_app.h"
#include "ble-configuration.h"
#include "board_features.h"

/* Bluetooth stack headers */
#include "bg_types.h"
#include "native_gecko.h"
#include "gatt_db.h"

/* Libraries containing default Gecko configuration values */
#include "em_emu.h"
#include "em_cmu.h"
#include "em_adc.h"

/* Device initialization header */
#include "hal-config.h"
#include "retargetserial.h"

#include "cmu.h"
#include "adc0.h"
#include "letimer.h"
#include "app_include.h"
#include "infrastructure.h"
#include "gpio.h"
#if defined(HAL_CONFIG)
#include "bsphalconfig.h"
#else
#include "bspconfig.h"
#endif

/***********************************************************************************************//**
 * @addtogroup Application
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup app
 * @{
 **************************************************************************************************/

#ifndef MAX_CONNECTIONS
#define MAX_CONNECTIONS 4
#endif
uint8_t bluetooth_stack_heap[DEFAULT_BLUETOOTH_HEAP(MAX_CONNECTIONS)];

// Gecko configuration parameters (see gecko_configuration.h)
static const gecko_configuration_t config = {
  .config_flags = 0,
  .sleep.flags = SLEEP_FLAGS_DEEP_SLEEP_ENABLE,
  .bluetooth.max_connections = MAX_CONNECTIONS,
  .bluetooth.heap = bluetooth_stack_heap,
  .bluetooth.heap_size = sizeof(bluetooth_stack_heap),
  .bluetooth.sleep_clock_accuracy = 100, // ppm
  .gattdb = &bg_gattdb_data,
  .ota.flags = 0,
  .ota.device_name_len = 3,
  .ota.device_name_ptr = "OTA",
#if (HAL_PA_ENABLE) && defined(FEATURE_PA_HIGH_POWER)
  .pa.config_enable = 1, // Enable high power PA
  .pa.input = GECKO_RADIO_PA_INPUT_VBAT, // Configure PA input to VBAT
#endif // (HAL_PA_ENABLE) && defined(FEATURE_PA_HIGH_POWER)
};

// Flag for indicating DFU Reset must be performed
uint8_t boot_to_dfu = 0;

/**
 * @brief  Main function
 */
int main(void)
{
  // Initialize device
  initMcu();
  // Initialize board
  initBoard();
  // Initialize application
  initApp();
  // Initialize stack
  gecko_init(&config);
  RETARGET_SerialInit();
  RETARGET_SerialCrLf(true);
  cmu_init();
  cmu_init_LETIMER0(1);
  gpio_init();
  // Initialize LETIMER
  letimer_init();
  // Set prescalar
  letimer_set_prescalar();
  // COMP0 and COMP1 values
  letimer_set_compvalue();
  // Setup ADC
  ADC0_setup();
  // Start ADC conversion
  ADC0_start();

  printf("Energy Auditor\n");

  while (1) {

    /* Event pointer for handling events */
    struct gecko_cmd_packet* evt;

    /* Check for stack event. */
    evt = gecko_wait_event();

    /* Handle events */
    switch (BGLIB_MSG_ID(evt->header)) {
    	  /* This event is generated when a connected client has either
    	   * 1) changed a Characteristic Client Configuration, meaning that they have enabled
    	   * or disabled Notifications or Indications, or
    	   * 2) sent a confirmation upon a successful reception of the indication. */
      case gecko_evt_gatt_server_characteristic_status_id:
    	  /* Check that the characteristic in question is temperature - its ID is defined
    	   * in gatt.xml as "temperature_measurement". Also check that status_flags = 1, meaning that
    	   * the characteristic client configuration was changed (notifications or indications
    	   * enabled or disabled). */
    	  if ((evt->data.evt_gatt_server_characteristic_status.characteristic == gattdb_current_value)
    			  && (evt->data.evt_gatt_server_characteristic_status.status_flags == 0x01)) {
    		  if (evt->data.evt_gatt_server_characteristic_status.client_config_flags == 0x02) {
    			  /* Indications have been turned ON - start the repeating timer. The 1st parameter '32768'
    			   * tells the timer to run for 1 second (32.768 kHz oscillator), the 2nd parameter is
    			   * the timer handle and the 3rd parameter '0' tells the timer to repeat continuously until
    			   * stopped manually.*/
    			  letimer_enable();
    			  //gecko_cmd_hardware_set_soft_timer(328, 0, 0);
    		  } else if (evt->data.evt_gatt_server_characteristic_status.client_config_flags == 0x00) {
    			  /* Indications have been turned OFF - stop the timer. */
    			  gecko_cmd_hardware_set_soft_timer(0, 0, 0);
    			  letimer_disable();
    		  }
    	  }
    	  break;

	  case gecko_evt_system_boot_id:
		  //gecko_bgapi_class_le_connection_init();
		  /* Set advertising parameters. 100ms advertisement interval. All channels used.
		   * The first two parameters are minimum and maximum advertising interval, both in
		   * units of (milliseconds * 1.6). The third parameter '7' sets advertising on all channels. */
		  gecko_cmd_le_gap_set_adv_parameters(BLE_ADVERTISING_MIN_COUNT,BLE_ADVERTISING_MAX_COUNT,7);
		  //gecko_cmd_le_gap_set_conn_parameters(BLE_CONNECTION_MIN_COUNT,BLE_CONNECTION_MAX_COUNT,BLE_SLAVE_LATENCY,BLE_CONNECTION_TIMEOUT_COUNT);
		  /* Start general advertising and enable connections. */
		  gecko_cmd_le_gap_set_mode(le_gap_general_discoverable, le_gap_undirected_connectable);
		  /*Set connetion parameters*/
		  gecko_cmd_le_gap_set_conn_parameters(BLE_CONNECTION_MIN_COUNT,BLE_CONNECTION_MAX_COUNT,BLE_SLAVE_LATENCY,BLE_CONNECTION_TIMEOUT_COUNT);
		break;

  	  case gecko_evt_le_connection_opened_id:
//		  letimer_enable();
  		break;

      case gecko_evt_gatt_server_user_read_request_id:
		  if(evt->data.evt_gatt_server_user_read_request.characteristic == gattdb_current_value)
		  {
			  uint8_t Buffer[2] = {0};
			  uint8_t *p = Buffer;

			  float voltage = (((float)avg_adcValue)*3.3)/4095.0;

			  uint16_t current = (voltage/330.0)*2000.0*707;

			  UINT16_TO_BITSTREAM(p, current);
			  gecko_cmd_gatt_server_send_user_read_response(
					evt->data.evt_gatt_server_user_read_request.connection, gattdb_current_value, 0, 2, Buffer);

			  avg_adcValue = 0;
		  }
    	break;

      case gecko_evt_system_external_signal_id:
      {
       	  uint8_t Buffer[2] = {0};
       	  uint8_t *p = Buffer;

//   		  printf("ADC Raw Value: %lu\n",avg_adcValue);

   		  float voltage = ((float)avg_adcValue*3.3)/4095;

//   		  printf("ADC Voltage: %f\n",voltage);

   		  uint16_t current = (voltage/330.0)*2000*707;

   		  printf("ADC Current: %d\n",current);

       	  UINT16_TO_BITSTREAM(p, current);
       	  gecko_cmd_gatt_server_send_characteristic_notification(
       	        0xFF, gattdb_current_value, 2, Buffer);

   		  avg_adcValue = 0;
      }
    	break;

//      case gecko_evt_hardware_soft_timer_id:
//      {
//    	  uint8_t Buffer[5] = {0};
//    	  uint8_t *p = Buffer;
//    	  //adcSampleValue = ADC_DataSingleGet(ADC0);
//		  printf("ADC Raw Value: %lu\n",avg_adcValue);
//
//		  float voltage = ((float)avg_adcValue*3.3)/4095;
//
//		  printf("ADC Voltage: %f\n",voltage);
//
//		  float current = (voltage/330.0)*2000;
//
//		  printf("ADC Current: %f\n",current);
//
//    	  UINT8_TO_BITSTREAM(p, 155);
//    	  UINT32_TO_BITSTREAM(p, (uint32_t)current);
//    	  gecko_cmd_gatt_server_send_characteristic_notification(
//    	        0xFF, gattdb_current_value, 5, Buffer);
//
//    	  avg_adcValue = 0;
//      }
//    	break;

      case gecko_evt_le_connection_closed_id:
    	  /*TX Power set to 0db*/
    	  //gecko_cmd_system_set_tx_power(0);
    	  /* Check if need to boot to dfu mode */
    	  if (boot_to_dfu){
    		  /* Enter to DFU OTA mode */
    		  gecko_cmd_system_reset(2);
    	  }
    	  else{
			  /* Restart advertising after client has disconnected */
			  gecko_cmd_le_gap_set_mode(le_gap_general_discoverable, le_gap_undirected_connectable);
    	  }
    	break;

      /* Events related to OTA upgrading
         ----------------------------------------------------------------------------- */

      /* Check if the user-type OTA Control Characteristic was written.
       * If ota_control was written, boot the device into Device Firmware Upgrade (DFU) mode. */
      case gecko_evt_gatt_server_user_write_request_id:
    	  if (evt->data.evt_gatt_server_user_write_request.characteristic == gattdb_ota_control){
			  /* Set flag to enter to OTA mode */
			  boot_to_dfu = 1;
			  /* Send response to Write Request */
			  gecko_cmd_gatt_server_send_user_write_response(
				evt->data.evt_gatt_server_user_write_request.connection,
				gattdb_ota_control,
				bg_err_success);

			  /* Close connection to enter to DFU OTA mode */
			  gecko_cmd_le_connection_close(evt->data.evt_gatt_server_user_write_request.connection);
    	  }
    	  else if(evt->data.evt_gatt_server_user_write_request.characteristic == gattdb_sampling_rate){
			  uint32_t samplingRateSec = *(uint32_t*)evt->data.evt_gatt_server_user_write_request.value.data;
			  gecko_cmd_hardware_set_soft_timer(32768*samplingRateSec, 0, 0);
    	  }
    	break;

      default:
        break;
    }
  }
  return 0;
}

/** @} (end addtogroup app) */
/** @} (end addtogroup Application) */
