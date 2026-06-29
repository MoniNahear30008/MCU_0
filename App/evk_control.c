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
#include "globals.h"
#include "q8_loader.h"
#include "fe_control.h"
#include "q8_image.h"

#define AWG_PACKET_SIZE 64
// Rx buffer can support up to 1024 bytes of data + 32 bytes of header
static uint8_t rxBuf[1024 + 32] __attribute__((section ("SRAM"))) = {0};
static uint8_t txBuf[1024] __attribute__((section ("SRAM"))) = {0};
static uint8_t rxIndex = 0;
static uint8_t wait_new_msg = 1;
static uint16_t rx_msg_len = 1;
uint16_t tempOffset = 0;
uint16_t awgLastPacketNum = 0;
uint16_t winLastPacketNum = 0;
uint16_t Q8CodeLastPacketNum = 0;
BCM_ErrorType g_retVal = BCM_ERR_INVAL_PARAMS;
uint16_t Q8CodeLen = 0;


#define GPIO_HWID (0UL)
#define TP_GPIO (GPIO_CHANNEL_43)
#define UART0_IRQ               (40U)

void UART0_IrqHandler();

static UART_ConfigType uartConfig = {
    .baud = UART_BAUD_1000000,
    .stopBits = UART_STOP_BITS1,
    .parity = UART_PARITY_NONE,
    .wordLength = UART_WORD_LENGTH_8B,
    .loopBackMode = 0UL,
    .rxFifoLvl = UART_FIFO_LVL_1DIV8,
    .txFifoLvl = UART_FIFO_LVL_1DIV8
};

BCM_ErrorType TP_Config()
{
	BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;

    GPIO_ConfigType gOutCfgDef ={
                                    .mode       = GPIO_CFG_MODE_OUTPUT,
                                    .oType      = GPIO_CFG_OUTPUT_OPEN_DRAIN,
                                    .pupd       = GPIO_CFG_PUPD_PULL_UP,
                                    .dout       = GPIO_LEVEL_LOW,
                                    .hys        = GPIO_CFG_HYSTERESIS_DISABLE,
                                    .strength   = GPIO_CFG_DRIVE_STRENGTH_8MA,
                                    .ind        = GPIO_CFG_INPUT_DISABLE,
                                    .slewRate   = GPIO_CFG_SLEW_RATE_SLEWED,
                                    .doutInvert = GPIO_CFG_DOUT_INVERT_DISABLE,
                                    .aCfgMask   = GPIO_CFG_MASK_MODE | GPIO_CFG_MASK_OTYPE | GPIO_CFG_MASK_PUPD | GPIO_CFG_MASK_DOUT | GPIO_CFG_MASK_HIST | GPIO_CFG_MASK_SEL | GPIO_CFG_MASK_IND | GPIO_CFG_MASK_SRC | GPIO_CFG_MASK_DOUT_INV
                                };

    retVal = GPIO_DrvInitChannel(GPIO_HW_ID_0, TP_GPIO, &gOutCfgDef);
	return retVal;
}


void TP_Control(uint8_t level)
{
    GPIO_LevelType gpio_level = (level) ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW;
    GPIO_DrvChannelWrite(GPIO_HWID, TP_GPIO, gpio_level);
}

BCM_ErrorType SendMsg(uint8_t *msg, uint32_t size)
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
    UART_EnableInterrupt(UART_HWID_0, UART_INTERRUPT_RX_TIMEOUT_INTR, 1); 

    NVIC_SetVector((IRQn_Type)UART0_IRQ, (uint32_t)UART0_IrqHandler);
    NVIC_EnableIRQ(UART0_IRQ);

    rxIndex = 0;
    return retVal;
}

static void readTempSensors()
{
    uint8_t tempBuf[10] = {0};
    BCM_ErrorType retVal = BRCM_i2c_read(0x26, tempBuf, 1); // read from TEC_CTRL_ADD which has both TEC and laser temps    
    if (retVal != BCM_ERR_OK)
    {
        // handle error
         memcpy(txBuf, (uint8_t[]){0x55, 0x55, 0x00, 0x07, 0x00, 0x01, 0x01}, 7); // indicate error in reading temp
         SendMsg(txBuf, 7);
    }
    else
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
}

static void updateAWG()
{
    uint16_t PacketNum = (rxBuf[6] << 8) | rxBuf[7];
    uint16_t payloadSize = rx_msg_len-8;
    if (PacketNum == 0)
    {
        awgLen = 0;
    }
    uint8_t *dataPtr = (uint8_t *)awg_vector + awgLen;
    memcpy(dataPtr, &rxBuf[8], payloadSize);
    awgLen += payloadSize;
    memcpy(txBuf, (uint8_t[]){0x55, 0x55, 0x00, 0x09, 0x00, 0x02, 0x00, 0x00, 0x00}, 9);
    txBuf[7] = rxBuf[6] & 0x7f; // return packet number
    txBuf[8] = rxBuf[7];

    if (PacketNum == 0)
    {
        awgLastPacketNum = 0;
    }
    else if ((PacketNum & 0x8000) == 0) // if MSB is 1, it's the last packet
    {
        if (PacketNum != awgLastPacketNum)
        {
            txBuf[6] = 0x03; // indicate packet loss
        }
    }
    else
    {
        awgLen /= 2; // convert from bytes to number of samples
    }

    awgLastPacketNum++;
    
    SendMsg(txBuf, 9);
}

static void updateWin()
{
    uint16_t PacketNum = (rxBuf[6] << 8) | rxBuf[7];
    uint16_t payloadSize = rx_msg_len-8;
    if (PacketNum == 0)
    {
        winLen = 0;
    }
    uint8_t *dataPtr = (uint8_t *)win_vector + winLen;
    memcpy(dataPtr, &rxBuf[8], payloadSize);
    winLen += payloadSize;
    memcpy(txBuf, (uint8_t[]){0x55, 0x55, 0x00, 0x09, 0x00, 0x07, 0x00, 0x00, 0x00}, 9);
    txBuf[7] = rxBuf[6] & 0x7f; // return packet number
    txBuf[8] = rxBuf[7];

    if (PacketNum == 0)
    {
        winLastPacketNum = 0;
    }
    else if ((PacketNum & 0x8000) == 0) // if MSB is 1, it's the last packet
    {
        if (PacketNum != winLastPacketNum)
        {
            txBuf[6] = 0x03; // indicate packet loss
        }
    }
    else
    {
        winLen /= 2; // convert from bytes to number of samples
    }

    winLastPacketNum++;
    
    SendMsg(txBuf, 9);
}

static uint32_t RegReadWrite(uint8_t Rd, uint32_t add, uint32_t val, uint32_t mask)
{
    uint32_t rval;
    // If write
    if (Rd == 0)
    {
        if (mask == 0xffffffff)
        {
            reg_wr(add, val);
        }
        else
        {
            reg_rd(add, &rval);
            rval &= ~mask;
            rval |= val;
            reg_wr(add, rval);
        }

        reg_rd(add, &rval);
    }
    else 
        reg_rd(add, &rval);

    return rval;

}

static void memoryDump(uint32_t sa, uint32_t np)
{
    uint32_t readAdd = sa;
    uint32_t i,j;
    for (i = 0; i < np; i++)
    {
        BCM_DelayUs(10000);
        memcpy(txBuf, (uint8_t[]){0x55, 0x55, 0, 0x89, 0x00, 14, 0}, 7);
        txBuf[7] = (uint8_t)((i >> 8) & 0xff);
        txBuf[8] = (uint8_t)(i & 0xff);
        SendMsg(txBuf, 9);
        for (j = 0; j < 4; j++)
        {
            BCM_DelayUs(1000);
            memcpy(txBuf, (uint8_t *)readAdd, 32);
            SendMsg(txBuf, 32);
            readAdd += 32;
        }
    }
}

static void ProcQ8CodePacket()
{
    uint16_t q8Num = rxBuf[6];
    uint16_t PacketNum = (rxBuf[7] << 8) | rxBuf[8];
    uint16_t lastPacket = PacketNum & 0x8000; // check if MSB is set, indicating last packet
    PacketNum = PacketNum & 0x7FFF; // actual packet number without MSB
    uint16_t payloadSize = rx_msg_len-9;
    if (PacketNum == 0)
    {
        Q8CodeLen = 0;
    }
    memcpy((uint8_t *)gp_buffer + Q8CodeLen, &rxBuf[9], payloadSize);
    Q8CodeLen += payloadSize;
    memcpy(txBuf, (uint8_t[]){0x55, 0x55, 0x00, 0x09, 0x00, 0x0A, 0x00, 0x00, 0x00}, 9);
    txBuf[7] = rxBuf[7] & 0x7f; // return packet number
    txBuf[8] = rxBuf[8]; // return packet number

    if (PacketNum == 0)
    {
        Q8CodeLastPacketNum = 0;
    }

    if (PacketNum != Q8CodeLastPacketNum)
    {
        txBuf[6] = 0x03; // indicate packet loss
    }
    else if (lastPacket != 0)
    {
        for (uint32_t i = 0; i < Q8CodeLen / 4; i++) 
        {
            if (gp_buffer[i] != q8_0_image_single[i]) 
            {
                txBuf[6] = 0x01; // indicate error in processing Q8 code
                break;
            }
        }

        if (txBuf[6] == 0)
        {
            BCM_ErrorType retVal = ProgQ8Code(q8Num, gp_buffer, Q8CodeLen / 4);
            if (retVal != BCM_ERR_OK)
            {
                txBuf[6] = 0x01; // indicate error in processing Q8 code
            }
            else
            {
                retVal = RunQ8(q8Num + 1);
                if (retVal != BCM_ERR_OK)
                {
                    txBuf[6] = 0x01; // indicate error in processing Q8 code
                }
            }
        }
    }

    Q8CodeLastPacketNum++;
    
    SendMsg(txBuf, 9);
}

void ProcHostMsg()
{
    uint8_t paramID, Rd;
    uint32_t paramVal, add, val, rval, mask;
    BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;

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
            readTempSensors();
            break;

        // AWG Vector Transfer    
        case 2:
            updateAWG();
            break;

        // Config AWG    
        case 3:
            memcpy(txBuf, (uint8_t[]){0x55, 0x55, 0x00, 0x07, 0x00, 0x03, 0x01}, 7); // indicate AWG size mismatch error
            if ((uint16_t)((rxBuf[6] << 8) | rxBuf[7]) == awgLen)
            {
                retVal = ConfigAWG();
                if (retVal == BCM_ERR_OK)
                {
                    txBuf[6] = 0x00;
                }
            }
            SendMsg(txBuf, 7);
            break;

        // Set paraemter value    
        case 4:
            paramID = (uint8_t)rxBuf[6];
            paramVal = (rxBuf[7] << 24) | (rxBuf[8] << 16) | (rxBuf[9] << 8) | rxBuf[10];
            retVal = SetParamValue(paramID, paramVal);
            memcpy(txBuf, (uint8_t[]){0x55, 0x55, 0x00, 0x07, 0x00, 0x04, 0x00}, 7);
            if (retVal != BCM_ERR_OK)
            {
                txBuf[6] = 0x01; // error in setting parameter
            }
            SendMsg(txBuf, 7);
            break;

        // Run/Stop AWG    
        case 5:
            /* code to trigger AWG output */
//            AwgControl(rxBuf[6]); // assuming rxBuf[6] contains 1 to start and 0 to stop
             memcpy(txBuf, (uint8_t[]){0x55, 0x55, 0x00, 0x07, 0x00, 0x05, 0x00}, 7);
             SendMsg(txBuf, 7);
             break;    
            
        // Get and configure the FIR    
        case 6:
            firLen = rx_msg_len-6;
            memcpy(fir_vector, &rxBuf[6], firLen);
            firLen /= 2; // convert from bytes to number of coefficients
             memcpy(txBuf, (uint8_t[]){0x55, 0x55, 0x00, 0x07, 0x00, 0x06, 0x00}, 7);
             SendMsg(txBuf, 7);
             break;
            memcpy(txBuf, (uint8_t[]){0x55, 0x55, 0x00, 0x07, 0x00, 0x06, 0x00}, 7);
            SendMsg(txBuf, 7);
            break;    
        
        // Window Vector Transfer    
        case 7:
            updateWin();
            break;

        // Config Window    
        case 8:
            memcpy(txBuf, (uint8_t[]){0x55, 0x55, 0x00, 0x07, 0x00, 0x08, 0x01}, 7);
            if ((uint16_t)((rxBuf[6] << 8) | rxBuf[7]) == winLen)
            {
                ConfigWin();
                txBuf[6] = 0x00;
            }
            SendMsg(txBuf, 7);
            break;

        // Config FIR    
        case 9:
            memcpy(txBuf, (uint8_t[]){0x55, 0x55, 0x00, 0x07, 0x00, 0x09, 0x01}, 7);
            if ((uint16_t)((rxBuf[6] << 8) | rxBuf[7]) == firLen)
            {
                ConfigFIR();
                txBuf[6] = 0x00;
            }
            SendMsg(txBuf, 7);
            break;

        // Window Vector Transfer    
        case 10:
            ProcQ8CodePacket();
             break;

        // ??
        // case 11:
        //     break;

        // Start ACQ
        case 12:
            memcpy(txBuf, (uint8_t[]){0x55, 0x55, 0x00, 0x07, 0x00, 12, 0x00}, 7);
            acq_sft_trg();
            BCM_DelayUs(1000);
            SendMsg(txBuf, 7);
            break;

        // Register Read/Write
        case 13:
            Rd = (uint8_t)rxBuf[6];
            add = ((uint32_t)rxBuf[7] << 24) | ((uint32_t)rxBuf[8] << 16) | ((uint32_t)rxBuf[9] <<  8) | ((uint32_t)rxBuf[10]);
            val = ((uint32_t)rxBuf[11] << 24) | ((uint32_t)rxBuf[12] << 16) | ((uint32_t)rxBuf[13] <<  8) | ((uint32_t)rxBuf[14]);
            mask = ((uint32_t)rxBuf[15] << 24) | ((uint32_t)rxBuf[16] << 16) | ((uint32_t)rxBuf[17] <<  8) | ((uint32_t)rxBuf[18]);
            rval = RegReadWrite(Rd, add, val, mask);
            memcpy(txBuf, (uint8_t[]){0x55, 0x55, 0, 11, 0, 13, 0, 0, 0, 0, 0}, 11);
            txBuf[7] = (uint8_t)((rval >> 24) & 0xff);
            txBuf[8] = (uint8_t)((rval >> 16) & 0xff);
            txBuf[9] = (uint8_t)((rval >> 8) & 0xff);
            txBuf[10] = (uint8_t)(rval & 0xff);
            SendMsg(txBuf, 11);
            break;

        // Memory dump
        case 14:
            add = ((uint32_t)rxBuf[6] << 24) | ((uint32_t)rxBuf[7] << 16) | ((uint32_t)rxBuf[8] <<  8) | ((uint32_t)rxBuf[9]);
            val = ((uint32_t)rxBuf[10] << 8) | ((uint32_t)rxBuf[11]);
            memoryDump(add, val);
            break;

        default:
            memcpy(txBuf, rxBuf, 7);
            txBuf[6] = 2;
            SendMsg(txBuf, 7);
             break;
    }
    newMsg = 0; // reset flag after processing
    NVIC_EnableIRQ(UART0_IRQ);
}   

void ReadMsgBytes()
{
    uint32_t rxSize = 512;
    BCM_ErrorType retVal = UART_DrvReceive(UART_HWID_0, &rxBuf[rxIndex], &rxSize, &uartConfig);
    ASSERT(retVal == BCM_ERR_OK);
    if (rxSize > 0) 
    {
        rxIndex += rxSize;
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
        
        NVIC_DisableIRQ(UART0_IRQ);
        newMsg = 1; // set flag to indicate new message is ready for processing
    }
}

void UART0_IrqHandler()
{
    if (newMsg == 0)
    {
        ReadMsgBytes();
    }
}
