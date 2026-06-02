/**
    @file evk_control.C
    @brief EVK Control source file

    @version 
*/

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <bcm_err.h>
#include <bcm_time.h>
#include <uart.h>
#include <gpio.h>
#include "BCM8915x_CM7.h"
#include "BCM8915X_BareMetal_helper.h"
#include "DrvBrd.h"

#define AWG_PACKET_SIZE 64
static uint8_t rxBuf[1024] __attribute__((section ("SRAM"))) = {0};
static uint8_t txBuf[1024] __attribute__((section ("SRAM"))) = {0};
static uint8_t rxIndex = 0;
static uint8_t wait_new_msg = 1;
static uint16_t rx_msg_len = 1;
uint16_t tempOffset = 0;
uint16_t awgVecOffset = 0;
uint16_t awgLastPacketNum = 0;
BCM_ErrorType g_retVal = BCM_ERR_INVAL_PARAMS;


#define UART0_IRQ               (40U)

void UART0_IrqHandler();

static UART_ConfigType uartConfig = {
    .baud = UART_BAUD_115200,
    .stopBits = UART_STOP_BITS1,
    .parity = UART_PARITY_NONE,
    .wordLength = UART_WORD_LENGTH_8B,
    .loopBackMode = 0UL,
    .rxFifoLvl = UART_FIFO_LVL_1DIV8,
    .txFifoLvl = UART_FIFO_LVL_1DIV8
};

BCM_ErrorType __attribute__((unused)) SendMsg(uint8_t *msg, uint32_t size)
{
//    BCM_ErrorType retVal = BCM_ERR_OK;

    UART_TxStatusType txStatus = 0;
    UART_DrvGetTxStatus(UART_HWID_0, &txStatus);
    UART_DrvSend(UART_HWID_0, msg, size, &uartConfig);
    uint16_t timeout = 1000;
    while ((txStatus != 3) && (timeout > 0))
    {
        BCM_DelayUs(1000);
        UART_DrvGetTxStatus(UART_HWID_0, &txStatus);
        timeout--;
    }
    if (timeout == 0)
    {
        return BCM_ERR_TIME_OUT;
    }
    return BCM_ERR_OK;
}

BCM_ErrorType ConfigUart()
{
        BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;

        GPIO_ConfigType ch_cfg = {
        .mode = GPIO_CFG_MODE_ALT_FUNC,
        .altFunc = GPIO_CFG_ALT_FUNC_1, /* From Rigel_gpio_ctrl.xlsx, find in which Alternate function this peripheral is present */
        .oType = GPIO_CFG_OUTPUT_PUSH_PULL,
        .pupd = GPIO_CFG_PUPD_PULL_UP,
        .hys = GPIO_CFG_HYSTERESIS_DISABLE,
        .strength = GPIO_CFG_DRIVE_STRENGTH_8MA,
        .doutInvert = GPIO_CFG_DOUT_INVERT_DISABLE,
        .aCfgMask = GPIO_CFG_MASK_MODE | GPIO_CFG_MASK_OTYPE | GPIO_CFG_MASK_ALTF | GPIO_CFG_MASK_PUPD | GPIO_CFG_MASK_HIST | GPIO_CFG_MASK_SEL | GPIO_CFG_MASK_DOUT_INV
    };

    retVal = GPIO_DrvInitChannel(0UL, GPIO_CHANNEL_30, &ch_cfg);
    retVal = GPIO_DrvInitChannel(0UL, GPIO_CHANNEL_31, &ch_cfg);

    UART_DrvInit(UART_HWID_0, &uartConfig);

    UART_StateType state; 
    UART_DrvGetState(UART_HWID_0, &state);
    if (state != UART_STATE_INIT) 
    {
        return BCM_ERR_INVAL_STATE;
    }

    UART_EnableInterrupt(UART_HWID_0, UART_INTERRUPT_RX_INTR, 1);

    NVIC_SetVector((IRQn_Type)UART0_IRQ, (uint32_t)UART0_IrqHandler);
    NVIC_EnableIRQ(UART0_IRQ);

    rxIndex = 0;
    return retVal;
}

static void readTempSensors()
{
    uint16_t tempA = 5000 + tempOffset;  
    uint16_t tempB = 4500 + tempOffset;
    uint16_t tempC = 5900 + tempOffset;
    uint16_t tempD = 6200 + tempOffset;
    tempOffset += 10;
    memcpy(txBuf, (uint8_t[]){0x55, 0x55, 0x00, 0x0E, 0x00, 0x01, 0x00}, 7);
    txBuf[6] = (tempA >> 8) & 0xFF;
    txBuf[7] = tempA & 0xFF;
    txBuf[8] = (tempB >> 8) & 0xFF;
    txBuf[9] = tempB & 0xFF;
    txBuf[10] = (tempC >> 8) & 0xFF;    
    txBuf[11] = tempC & 0xFF;
    txBuf[12] = (tempD >> 8) & 0xFF;
    txBuf[13] = tempD & 0xFF;
    SendMsg(txBuf, 14);
}

static void updateAWG()
{
    // uint16_t PacketNum = (rxBuf[6] << 8) | rxBuf[7];
    // uint16_t awgVecOffset = PacketNum * AWG_PACKET_SIZE; // each data point is 2 bytes
    // memcpy(&awg_vector[awgVecOffset], &rxBuf[8], 2 * AWG_PACKET_SIZE);
    // memcpy(txBuf, (uint8_t[]){0x55, 0x55, 0x00, 0x09, 0x00, 0x02, 0x00, 0x00, 0x00}, 9);
    // txBuf[7] = rxBuf[6]; // return packet number
    // txBuf[8] = rxBuf[7];

    // if (PacketNum == 0)
    // {
    //     awgLastPacketNum = 0;
    // }
    // else if (PacketNum != awgLastPacketNum)
    // {
    //     txBuf[6] = 0x03; // indicate packet loss
    // }
    // awgLastPacketNum++;
    
    SendMsg(txBuf, 9);
}

static void ProcMsg()
{
    uint8_t paramID;
    uint32_t paramVal;

    BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
    if (wait_new_msg)
    {
        if (rxIndex < 4)
        {
            return; // not enough data to determine message length
        }
        // check flag
        if ((rxBuf[0] != 0x55) || (rxBuf[1] != 0x55))
        {
            rxIndex = 0;
            return;
        } 
        rx_msg_len = (256 * rxBuf[2]) | rxBuf[3];
    }
    wait_new_msg = 0;

    // Check if all bytes have been received
    if (rx_msg_len > rxIndex)
    {
        return;
    }

    // entire message has been received, process it
    rxIndex = 0;
    wait_new_msg = 1;

    /* code */
     uint16_t mCode = rxBuf[5] + (rxBuf[4] << 8);

    switch (mCode)
    {
         // Ping message
         case 0:
            memcpy(txBuf, (uint8_t[]){0x55, 0x55, 0x00, 0x07, 0x00, 0x00, 0x00}, 7);
            SendMsg(txBuf, 7);
            break;

        // Read termeratures    
        case 1:
            BRCM_i2c_read();
            BCM_DelayUs(10);
            readTempSensors();
            break;

        // AWG Vector Transfer    
        case 2:
            updateAWG();
            break;

        // Set paraemter value    
        case 3:
            paramID = (uint8_t)rxBuf[6];
            paramVal = (rxBuf[7] << 24) | (rxBuf[8] << 16) | (rxBuf[9] << 8) | rxBuf[10];
            retVal = SetParamValue(paramID, paramVal);
            memcpy(txBuf, (uint8_t[]){0x55, 0x55, 0x00, 0x07, 0x00, 0x03, 0x00}, 7);
            if (retVal != BCM_ERR_OK)
            {
                txBuf[6] = 0x01; // success
            }
            SendMsg(txBuf, 7);
            break;

        // Run/Stop AWG    
        case 4:
//            retVal = ConfigAWG();
            memcpy(txBuf, (uint8_t[]){0x55, 0x55, 0x00, 0x07, 0x00, 0x04, 0x00}, 7);
            if (retVal != BCM_ERR_OK)
            {
                txBuf[6] = 0x01; // success
            }
            SendMsg(txBuf, 7);
            break;

        case 5:
            /* code to trigger AWG output */
//            AwgControl(rxBuf[6]); // assuming rxBuf[6] contains 1 to start and 0 to stop
             memcpy(txBuf, (uint8_t[]){0x55, 0x55, 0x00, 0x07, 0x00, 0x05, 0x00}, 7);
             SendMsg(txBuf, 7);
             break;    
            
        default:
             break;
    }
}   

void UART0_IrqHandler()
{
    BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
    retVal = UART_IRQHandler(UART_HWID_0, &uartConfig);
    ASSERT(retVal == BCM_ERR_OK);

    uint32_t rxSize  = 512;
    retVal = UART_DrvReceive(UART_HWID_0, &rxBuf[rxIndex], &rxSize, &uartConfig);
    ASSERT(retVal == BCM_ERR_OK);
    if (rxSize > 0)
    {
        rxIndex += rxSize;
        ProcMsg();
    }
}


