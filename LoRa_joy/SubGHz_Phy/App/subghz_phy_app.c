/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    subghz_phy_app.c
  * @author  MCD Application Team & Jan Vlk & Ales Kucera
  * @brief   Application of the SubGHz_Phy Middleware
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "platform.h"
#include "sys_app.h"
#include "subghz_phy_app.h"
#include "radio.h"

/* USER CODE BEGIN Includes */
#include "stm32_timer.h"
#include "stm32_seq.h"
#include "subghz_phy_version.h"
#include "adc.h"
#include "usart.h"
/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum {
  RX,
  RX_TIMEOUT,
  RX_ERROR,
  TX,
  TX_TIMEOUT,
} States_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// Timeouts
#define RX_TIMEOUT_VALUE 500
#define TX_TIMEOUT_VALUE 500

// Sync messages
#define SYNC_REQ "sync_req"
#define SYNC_ACK "sync_ack"

//Payload size
#define MAX_APP_BUFFER_SIZE 2*PAYLOAD_LEN
#if (PAYLOAD_LEN > MAX_APP_BUFFER_SIZE)
  #error PAYLOAD_LEN must be less or equal than MAX_APP_BUFFER_SIZE
#endif // (PAYLOAD_LEN > MAX_APP_BUFFER_SIZE)

#define RX_TIME_MARGIN 200
#define LED_PERIOD_MS 200
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* Radio events function pointer */
static RadioEvents_t RadioEvents;

/* USER CODE BEGIN PV */
static States_t State = RX;
static Mode_t Mode = ROBOT;
static Message_t message;
static Message_t rec_message;
static int8_t msg_uart1[6];

static uint8_t BufferRx[MAX_APP_BUFFER_SIZE];
static uint8_t BufferTx[MAX_APP_BUFFER_SIZE];

uint16_t RxBufferSize = 0;
uint8_t RssiValue = 0;
uint8_t SnrValue = 0;

static uint32_t random_delay;
bool synced = false;

static UTIL_TIMER_Object_t timerLed;
static UTIL_TIMER_Object_t timerSend;
static UTIL_TIMER_Object_t timerEStop;

uint8_t EStopSendCount = 0;
uint8_t EStopData;

uint16_t msgTimers[] = {0, 350, 350};

uint16_t raw_values[4];

// Unsigned 12-bit integers range from 0 to 4096
uint16_t lh_joy_raw = 0, lv_joy_raw = 0,
        rh_joy_raw = 0, rv_joy_raw = 0;

// Signed 12-bit integers range from -2048 to 2047
int16_t lh_joy_signed = 0, lv_joy_signed = 0,
        rh_joy_signed = 0, rv_joy_signed = 0;

static int8_t lh_joy = 0, lv_joy = 0,
              rh_joy = 0, rv_joy = 0;

// Buttons are '1' at rest and '0' if pushed
static uint8_t l1_butt = 1, l2_butt = 1, l3_butt = 1;
static uint8_t r1_butt = 1, r2_butt = 1, r3_butt = 1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/*!
* @brief Function to be executed on Radio Tx Done event
*/
static void OnTxDone(void);

/**
  * @brief Function to be executed on Radio Rx Done event
  * @param  payload ptr of buffer received
  * @param  size buffer size
  * @param  rssi
  * @param  LoraSnr_FskCfo
  */
static void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t LoraSnr_FskCfo);

/**
  * @brief Function executed on Radio Tx Timeout event
  */
static void OnTxTimeout(void);

/**
  * @brief Function executed on Radio Rx Timeout event
  */
static void OnRxTimeout(void);

/**
  * @brief Function executed on Radio Rx Error event
  */
static void OnRxError(void);

/* USER CODE BEGIN PFP */
/**
 * @brief Function executed on when led timer elapses
 * @param context ptr of LED context
 */
static void OnLedEvent(void *context);

/**
 * @brief Function executed on when there is data to send
 */
void OnSendEvent(void *context);

void OnEStopEvent(void *context);

/**
 * @brief Main app function
 */
static void App_Process(void);


static void FillMessage(void);

static void ClearMessage(Message_t msg);

static void SendAgain(void);

static void GetControls(void);

static int16_t apply_dead_zone(int16_t value);

static int8_t shift_and_scale(int16_t raw_value, int16_t offset);
/* USER CODE END PFP */

/* Exported functions ---------------------------------------------------------*/
void SubghzApp_Init(void)
{
  /* USER CODE BEGIN SubghzApp_Init_1 */
  APP_LOG(TS_OFF, VLEVEL_L, "\n\rAPP init\n\r");

  /* Get MW SubGhz_Phy info */
  APP_LOG(TS_OFF, VLEVEL_M, "MW_RADIO_VERSION:    V%X.%X.%X\r\n",
          (uint8_t)(SUBGHZ_PHY_VERSION_MAIN),
          (uint8_t)(SUBGHZ_PHY_VERSION_SUB1),
          (uint8_t)(SUBGHZ_PHY_VERSION_SUB2));

  /* Led Timers - blink red & green until connection */
  UTIL_TIMER_Create(&timerLed, LED_PERIOD_MS, UTIL_TIMER_ONESHOT, OnLedEvent, NULL);
  UTIL_TIMER_Start(&timerLed);

  /* Timers for CONTROLLER and ROBOT modes */
  if (Mode > 0 && Mode < 3) {
    UTIL_TIMER_Create(&timerSend, msgTimers[Mode], UTIL_TIMER_PERIODIC, OnSendEvent, NULL);
  }
  UTIL_TIMER_Create(&timerEStop, msgTimers[1], UTIL_TIMER_ONESHOT, OnEStopEvent, NULL);
  /* USER CODE END SubghzApp_Init_1 */

  /* Radio initialization */
  RadioEvents.TxDone = OnTxDone;
  RadioEvents.RxDone = OnRxDone;
  RadioEvents.TxTimeout = OnTxTimeout;
  RadioEvents.RxTimeout = OnRxTimeout;
  RadioEvents.RxError = OnRxError;

  Radio.Init(&RadioEvents);

  /* USER CODE BEGIN SubghzApp_Init_2 */
  /* Calculate random delay for synchronization */
  random_delay = (Radio.Random()) >> 22; /*10bits random e.g. from 0 to 1023 ms*/

  /* Radio Set frequency */
  Radio.SetChannel(RF_FREQUENCY);

  APP_LOG(TS_OFF, VLEVEL_M, "---------------\n\r");
  APP_LOG(TS_OFF, VLEVEL_M, "LORA_MODULATION\n\r");
  APP_LOG(TS_OFF, VLEVEL_M, "LORA_BW=%d kHz\n\r", (1 << LORA_BANDWIDTH) * 125);
  APP_LOG(TS_OFF, VLEVEL_M, "LORA_SF=%d\n\r", LORA_SPREADING_FACTOR);
  APP_LOG(TS_OFF, VLEVEL_M, "Mode=%d\n\r", Mode);
  APP_LOG(TS_OFF, VLEVEL_H, "Message len=%d\n\r", sizeof(Message_t));

  Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                    LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                    LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                    true, 0, 0, LORA_IQ_INVERSION_ON, TX_TIMEOUT_VALUE);

  Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                    LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                    LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                    0, true, 0, 0, LORA_IQ_INVERSION_ON, true);

  Radio.SetMaxPayloadLength(MODEM_LORA, MAX_APP_BUFFER_SIZE);

  /* Fills tx buffer */
  memset(BufferTx, 0x0, MAX_APP_BUFFER_SIZE);

  APP_LOG(TS_ON, VLEVEL_H, "rand=%d\n\r", random_delay);
  /* Starts reception */
  Radio.Rx(RX_TIMEOUT_VALUE + random_delay);

  /* Register task to to be run in while(1) after Radio IT */
  UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), UTIL_SEQ_RFU, App_Process);

  if (Mode == SNIFFER) {
    synced = true;
    UTIL_TIMER_Stop(&timerLed);
  }
  /* USER CODE END SubghzApp_Init_2 */
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private functions ---------------------------------------------------------*/
static void OnTxDone(void)
{
  /* USER CODE BEGIN OnTxDone */
  APP_LOG(TS_ON, VLEVEL_H, "OnTxDone\n\r");
  /* Update the State of the FSM */
  State = TX;
  /* Run app process in background */
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
  /* USER CODE END OnTxDone */
}

static void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t LoraSnr_FskCfo)
{
  /* USER CODE BEGIN OnRxDone */
  APP_LOG(TS_ON, VLEVEL_H, "OnRxDone\n\r");
  /* Update the State of the FSM */
  State = RX;

  APP_LOG(TS_ON, VLEVEL_L, "RssiValue=%d dBm, SnrValue=%ddB\n\r", rssi, LoraSnr_FskCfo);
  RssiValue = rssi;
  SnrValue = LoraSnr_FskCfo;

  /* Clear BufferRx */
  memset(BufferRx, 0, MAX_APP_BUFFER_SIZE);
  /* Record payload size and fill buffer */
  RxBufferSize = size;
  if (RxBufferSize <= MAX_APP_BUFFER_SIZE) {
    memcpy(BufferRx, payload, RxBufferSize);
  }

  if (synced) {
    ClearMessage(rec_message);
    memcpy(&rec_message, BufferRx, sizeof(Message_t));
    APP_LOG(TS_ON, VLEVEL_M, "Received message content:\n\r")
    APP_LOG(TS_ON, VLEVEL_M, "Mode: %d\n\r", rec_message.mode);
    APP_LOG(TS_ON, VLEVEL_M, "Data0: %d\n\r", rec_message.data0);
    APP_LOG(TS_ON, VLEVEL_M, "Data1: %d\n\r", rec_message.data1);
    APP_LOG(TS_ON, VLEVEL_M, "Data2: %d\n\r", rec_message.data2);
    APP_LOG(TS_ON, VLEVEL_M, "Data3: %d\n\r", rec_message.data3);
    APP_LOG(TS_ON, VLEVEL_M, "Data4: %d\n\r", rec_message.data4);
    APP_LOG(TS_ON, VLEVEL_M, "Data5: %d\n\r", rec_message.data5);
  }

  /* Record payload content */
  APP_LOG(TS_ON, VLEVEL_H, "Payload, size=%d \n\r", size);
  for (int32_t i = 0; i < PAYLOAD_LEN; ++i)
  {
    APP_LOG(TS_OFF, VLEVEL_H, "%02X", BufferRx[i]);
    if (i % 16 == 15)
    {
      APP_LOG(TS_OFF, VLEVEL_H, "\n\r");
    }
  }
  APP_LOG(TS_OFF, VLEVEL_H, "\n\r");
  /* Run app process in background */
  if (synced) {
    HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
  }
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
  /* USER CODE END OnRxDone */
}

static void OnTxTimeout(void)
{
  /* USER CODE BEGIN OnTxTimeout */
  APP_LOG(TS_ON, VLEVEL_H, "OnTxTimeout\n\r");
  /* Update the State of the FSM */
  State = TX_TIMEOUT;
  /* Run app process in background */
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
  /* USER CODE END OnTxTimeout */
}

static void OnRxTimeout(void)
{
  /* USER CODE BEGIN OnRxTimeout */
  APP_LOG(TS_ON, VLEVEL_H, "OnRxTIMEOUT\n\r");
  /* Update the State of the FSM */
  State = RX_TIMEOUT;
  /* Run app process in background */
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
  /* USER CODE END OnRxTimeout */
}

static void OnRxError(void)
{
  /* USER CODE BEGIN OnRxError */
  APP_LOG(TS_ON, VLEVEL_H, "OnRxError\n\r");
  /* Update the State of the FSM */
  State = RX_ERROR;
  /* Run app process in background */
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
  /* USER CODE END OnRxError */
}

/* USER CODE BEGIN PrFD */
static void App_Process(void)
{
  //Radio.Sleep();
  Radio.Standby();

  switch(State) {
    case RX:
      if (!synced) {
        if (RxBufferSize > 0) {
          if (strncmp((const char*)BufferRx, SYNC_REQ, sizeof(SYNC_REQ) - 1) == 0) {
            APP_LOG(TS_ON, VLEVEL_M, "Received SYNC_REQ, sending SYNC_ACK\n\r");
            //HAL_Delay(Radio.GetWakeupTime() + RX_TIME_MARGIN);
            memcpy(BufferTx, SYNC_ACK, sizeof(SYNC_ACK) - 1);
            Radio.Send(BufferTx, PAYLOAD_LEN);
          } else if (strncmp((const char*)BufferRx, SYNC_ACK, sizeof(SYNC_ACK) - 1) == 0) {
            APP_LOG(TS_ON, VLEVEL_M, "Received SYNC_ACK, sending SYNC_ACK\n\r");
            synced = true;
            UTIL_TIMER_Stop(&timerLed);
            HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
            if (Mode > 0 && Mode < 3) {
              UTIL_TIMER_Start(&timerSend);
            }
            //HAL_Delay(Radio.GetWakeupTime() + RX_TIME_MARGIN);
            if (Mode == ROBOT){
              HAL_Delay(100);
            }
            memcpy(BufferTx, SYNC_ACK, sizeof(SYNC_ACK) - 1);
            Radio.Send(BufferTx, PAYLOAD_LEN);
          } else {
            APP_LOG(TS_ON, VLEVEL_L, "Received message, but not yet synced\n\r");
            Radio.Rx(RX_TIMEOUT);
          }
        }
      } else {
        if (Mode == rec_message.mode) {
          APP_LOG(TS_ON, VLEVEL_M, "Received message from same mode: %d\n\r", Mode);
        } else if (rec_message.mode < 0 || rec_message.mode > 2) {
          if (strncmp((const char*)BufferRx, SYNC_REQ, sizeof(SYNC_REQ) - 1) == 0) {
            APP_LOG(TS_ON, VLEVEL_M, "Synced but received SYNC_REQ, sending SYNC_ACK\n\r");
            //HAL_Delay(Radio.GetWakeupTime() + RX_TIME_MARGIN);
            memcpy(BufferTx, SYNC_ACK, sizeof(SYNC_ACK) - 1);
            Radio.Send(BufferTx, PAYLOAD_LEN);
          }
          APP_LOG(TS_ON, VLEVEL_M, "Received message from undefined mode\n\r");
        }
        switch(Mode) {
          case SNIFFER:
            break;
          case CONTROLLER:
          //break;
          case ROBOT:
            if (rec_message.mode == SNIFFER) {
              if (rec_message.data0) {
                HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET);
              } else if (!rec_message.data0) {
                HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
              } else {
                APP_LOG(TS_ON, VLEVEL_M, "Wrong message from SNIFFER\n\r")
              }
            } else if (rec_message.mode == CONTROLLER) {
              // Header
              msg_uart1[0] = 0xff;

              // Data
              msg_uart1[1] = rec_message.data0;
              msg_uart1[2] = rec_message.data1;
              msg_uart1[3] = rec_message.data2;
              msg_uart1[4] = rec_message.data3;
              msg_uart1[5] = rec_message.data4;

              // Fill the last data byte with zeros
              msg_uart1[5] &= 0b00111111;

              HAL_UART_Transmit_DMA(&huart1, (uint8_t*)msg_uart1, 6);
            }
            break;
            case UNDEFINED_MODE:
            default:
              APP_LOG(TS_ON, VLEVEL_M, "Undefined mode: %d, received a message\n\r", Mode);
            break;
        }
        APP_LOG(TS_ON, VLEVEL_H, "Rx start\n\r");
        Radio.Rx(RX_TIMEOUT);
      }
      break;
    case TX:
      APP_LOG(TS_ON, VLEVEL_H, "Rx start\n\r");
      Radio.Rx(RX_TIMEOUT_VALUE);
      break;
    case RX_TIMEOUT:
    case RX_ERROR:
      if (!synced) {
        APP_LOG(TS_ON, VLEVEL_H, "Sending SYNC_REQ\n\r");
        HAL_Delay(Radio.GetWakeupTime() + RX_TIME_MARGIN + random_delay);
        memcpy(BufferTx, SYNC_REQ, sizeof(SYNC_REQ) - 1);
        Radio.Send(BufferTx, PAYLOAD_LEN);
      } else {
        APP_LOG(TS_ON, VLEVEL_H, "Rx start\n\r");
      Radio.Rx(RX_TIMEOUT_VALUE);
      }
      break;
    case TX_TIMEOUT:
      HAL_Delay(TX_TIMEOUT * 2);
      SendAgain();
      break;
    default:
      break;
  }
}

static void OnLedEvent(void *context)
{
  HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin); /* LED_GREEN */
  HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin); /* LED_RED */
  UTIL_TIMER_Start(&timerLed);
}

void OnSendEvent(void *context)
{
  //HAL_Delay(Radio.GetWakeupTime() + RX_TIME_MARGIN);

  FillMessage();
  memcpy(BufferTx, &message, sizeof(Message_t));
  APP_LOG(TS_ON, VLEVEL_M, "OnSendEvent\n\r");

  Radio.Send(BufferTx, PAYLOAD_LEN);
}

void OnEStopEvent(void *content)
{
  //HAL_Delay(Radio.GetWakeupTime() + RX_TIME_MARGIN);

  ClearMessage(message);
  message.mode = Mode;
  message.data0 = EStopData;

  memcpy(BufferTx, &message, sizeof(Message_t));

  ++EStopSendCount;
  if (EStopSendCount < 5) {
    UTIL_TIMER_Start(&timerEStop);
  }
  Radio.Send(BufferTx, PAYLOAD_LEN);
}

void SendAgain(void)
{
  //HAL_Delay(Radio.GetWakeupTime() + RX_TIME_MARGIN);
  Radio.Send(BufferTx, PAYLOAD_LEN);
}

void SendEStop(uint8_t stop)
{
  if (!synced) {
  APP_LOG(TS_ON, VLEVEL_M, "Sending EStop but not synced\n\r");
    return;
  }
  if (Mode != SNIFFER) {
    APP_LOG(TS_ON, VLEVEL_M, "Sending EStop in wrong mode: %d\n\r", Mode);
    return;
  }

  EStopSendCount = 0;
  EStopData = stop;
  UTIL_TIMER_Start(&timerEStop);
}

void FillMessage(void)
{
  ClearMessage(message);
  message.mode = Mode;

  switch(Mode) {
    case SNIFFER:
      APP_LOG(TS_ON, VLEVEL_M, "Filling message in SNIFFER mode is not supported\n\r");
      break;
    case CONTROLLER:
      GetControls();
      message.data0 = lv_joy;
      message.data1 = lh_joy;
      message.data2 = rv_joy;
      message.data3 = rh_joy;
      message.data4 = l1_butt << 5 | l2_butt << 4 | l3_butt << 3 | r1_butt << 2 | r2_butt << 1 | r3_butt;
      break;
    case ROBOT:
      message.data0 = battery_state;
      message.data1 = temperature;
      APP_LOG(TS_ON, VLEVEL_M, "Current battery state: %d\n\r", battery_state);
      APP_LOG(TS_ON, VLEVEL_M, "Current CPU temperature: %d\n\r", temperature);
      break;
    case UNDEFINED_MODE:
    default:
      APP_LOG(TS_ON, VLEVEL_M, "Filling message with an undefined mode: %d\n\r", Mode);
      break;
  }
}

void ClearMessage(Message_t msg)
{
  msg.mode = UNDEFINED_MODE;
  msg.data0 = 0;
  msg.data1 = 0;
  msg.data2 = 0;
  msg.data3 = 0;
  msg.data4 = 0;
  msg.data5 = 0;
}

Mode_t GetMode(void)
{
  return Mode;
}

void GetControls(void)
{
  HAL_ADC_Start_DMA(&hadc, (uint32_t *)raw_values, 4);

  while(conv_completed);
  conv_completed = 0;

  for (uint8_t i = 0; i < hadc.Init.NbrOfConversion; i++){
    lh_joy_raw = (uint16_t)raw_values[0];
    lv_joy_raw = (uint16_t)raw_values[1];
    rh_joy_raw = (uint16_t)raw_values[2];
    rv_joy_raw = (uint16_t)raw_values[3];
  }

  lh_joy_signed = (int16_t)(lh_joy_raw - 2048);
  lv_joy_signed = (int16_t)(lv_joy_raw - 2048);
  rh_joy_signed = (int16_t)(rh_joy_raw - 2048);
  rv_joy_signed = (int16_t)(rv_joy_raw - 2048);

  lh_joy = shift_and_scale(lh_joy_signed, LH_JOY_OFFSET);
  lv_joy = shift_and_scale(lv_joy_signed, LV_JOY_OFFSET);
  rh_joy = shift_and_scale(rh_joy_signed, RH_JOY_OFFSET);
  rv_joy = shift_and_scale(rv_joy_signed, RV_JOY_OFFSET);

  l1_butt = HAL_GPIO_ReadPin(L_BUTT_Port, L1_BUTT_Pin);
  l2_butt = HAL_GPIO_ReadPin(L_BUTT_Port, L2_BUTT_Pin);
  l3_butt = HAL_GPIO_ReadPin(L_BUTT_Port, L3_BUTT_Pin);

  r1_butt = HAL_GPIO_ReadPin(R_BUTT_Port, R1_BUTT_Pin);
  r2_butt = HAL_GPIO_ReadPin(R_BUTT_Port, R2_BUTT_Pin);
  r3_butt = HAL_GPIO_ReadPin(R_BUTT_Port, R3_BUTT_Pin);
  APP_LOG(TS_ON, VLEVEL_M, "ADC conversion done\n\r");
}

int16_t apply_dead_zone(int16_t value)
{
  if (value < DEAD_ZONE && value > -DEAD_ZONE) {
    return 0;
  } else if (value >= DEAD_ZONE) {
    return value - DEAD_ZONE;
  } else { // value <= -DEAD_ZONE
    return value + DEAD_ZONE;
  }
}

int8_t shift_and_scale(int16_t raw_value, int16_t offset)
{
  int val; // shifted raw value
  int lb, ub; // shifted upper and lower bound
  float pos; // the position (between 0 and 1) either on positive or negative side
  int8_t ret;

  // Crop the value to lower or upper bound
  if (raw_value < LOWER_JOY_BOUND) {
    raw_value = LOWER_JOY_BOUND;
  } else if (raw_value > UPPER_JOY_BOUND){
    raw_value = UPPER_JOY_BOUND;
  }

  // -1948 - 72 + 100 = -1920
  // -29 - 72 + 100 = -1

  // 172 - 72 - 100  = 0
  // 2047 - 72 - 100 = 1875

  val = apply_dead_zone(raw_value - offset);
  lb = LOWER_JOY_BOUND - offset + DEAD_ZONE;  // -1948 - 72 - 100 = -1920
  ub = UPPER_JOY_BOUND - offset - DEAD_ZONE;  // 2047 - 72 - 100 = 1875

  if (val < 0) {
    pos = 1 - (float)(lb - val)/lb; // 1 - (-1920 + 1920)/(-1920) = 1 - 0 = 1 || 1 - (-1920 + 1)/(-1920) = 5*10-4
    ret = (int8_t)(pos * INT8_MIN); // -128 || -0.064 = 0
    if (ret == -1) {
      ret = -2;
    }
  } else {
    pos = 1 - (float)(ub - val)/ub; // 1 - (1875 - 0)/1875 = 0 || 1 - (1875 - 1875)/1875 = 1 - 0 = 1
    ret = (int8_t)(pos * INT8_MAX); // 0 || 127
  }

  return ret;
}
/* USER CODE END PrFD */
