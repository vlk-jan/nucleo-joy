/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    message.h
  * @author  Jan Vlk
  * @brief   Header file for defining the message type
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 Jan Vlk.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef MESSAGE_H
#define MESSAGE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	SNIFFER,
	CONTROLLER,
	ROBOT,
	UNDEFINED_MODE,
} Mode_t;

typedef struct {
	Mode_t mode;  // SNIFFER or CONTROLLER or ROBOT
	int8_t data0;  // left forward or battery state or estop
	int8_t data1;  // left sideways
	int8_t data2;  // right forward
	int8_t data3;  // right sideways
	uint8_t data4; // buttons
	uint8_t data5; // buttons
} Message_t;

// Flag for ADC conversion completion
volatile static int conv_completed = 0;

#ifdef __cplusplus
}
#endif

#endif
