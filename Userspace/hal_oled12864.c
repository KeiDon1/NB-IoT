#include "hal_oled12864.h"
#include "font_v_8x16.h"
#include "hal_lcd_spi.h"

#include "hal_system.h"

#include "stm32f0xx_gpio.h"

typedef unsigned char uint8;
typedef unsigned short uint16;

static void halOLED12864Reset(void);
static void halOLED12864ChipInit(void);
static void halOLED12864SetPosition(uint8 page, uint8 x);

static void halOLEDShowChar8x16(uint16 x, uint16 page, uint8 ch);

void halOLED12864Init(void)
{    
    /* Init SPI-GPIO */
    halLcdSpiInit();

    /* Init Chip */
    halOLED12864ChipInit();

    /* Setting */
    halOLED12864ClearScreen();
    halOLED12864SetPosition(0,0);
}

void halOLED12864ClearScreen(void)
{
    uint8 page, x;
    
    for (page = 0; page < HAL_OLED12864_PAGE; page++) {
        halLcdSpiTxCmd(0xb0 + page);
        halLcdSpiTxCmd(0x01);
        halLcdSpiTxCmd(0x10);
        
        for (x = 0; x < HAL_OLED12864_X; x++) halLcdSpiTxData(0);
    }
}

void halOLED12864ShowX16(uint8 line, uint8 column, const uint8 *str)
{
    if (!str || line > 3) return;
    
    uint8 page = line * 2;    // 2 page per line
    const uint8 *ptext = str; // text
    
    /* Show text */
    while(*ptext != 0) {
        /* ASCII Code: 0~127 */
        if((*ptext) < 128) {
            /* End of line */
            if((column + 8) > HAL_OLED12864_X) return;
          
            /* Show 8x16 ASCII Char. */
            halOLEDShowChar8x16(column, page, *ptext);
            column += 8;
            
            ptext++;
        }
    } /* while(*ptext != 0) */
}

static void halOLED12864Reset(void)
{
#ifdef STM32F030x8
    /* RST: PA12 */
    GPIO_InitTypeDef  lcdGPIO;

    lcdGPIO.GPIO_Mode = GPIO_Mode_OUT;
    lcdGPIO.GPIO_Speed = GPIO_Speed_2MHz;
    lcdGPIO.GPIO_Pin = GPIO_Pin_12;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

    GPIO_Init(GPIOA, &lcdGPIO);

    /* Reset OLED12864 */
    GPIO_ResetBits(GPIOA, GPIO_Pin_12);

    halSystemDelayUs(60000);

    GPIO_SetBits(GPIOA, GPIO_Pin_12);
#else
    /* RST: PA3 */
    GPIO_InitTypeDef  lcdGPIO;

    lcdGPIO.GPIO_Mode = GPIO_Mode_OUT;
    lcdGPIO.GPIO_Speed = GPIO_Speed_2MHz;
    lcdGPIO.GPIO_Pin = GPIO_Pin_3;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

    GPIO_Init(GPIOA, &lcdGPIO);
    
    /* Reset OLED12864 */
    GPIO_ResetBits(GPIOA, GPIO_Pin_3);

    halSystemDelayUs(60000);

    GPIO_SetBits(GPIOA, GPIO_Pin_3);
#endif
}

static void halOLED12864ChipInit(void)
{
    halOLED12864Reset();
  
    halLcdSpiTxCmd(0xae);  // --turn off oled panel
    halLcdSpiTxCmd(0x00);  // ---set low column address
    halLcdSpiTxCmd(0x10);  // ---set high column address
    halLcdSpiTxCmd(0x40);  // --set start line address  Set Mapping  
                                    //   RAM Display Start Line (0x00~0x3F)
    halLcdSpiTxCmd(0x81);  // --set contrast control register
    halLcdSpiTxCmd(0xcf);  // --Set SEG Output Current Brightness
    halLcdSpiTxCmd(0xa1);  // --Set SEG/Column Mapping     
    halLcdSpiTxCmd(0xc8);  // --Set COM/Row Scan Direction  
    halLcdSpiTxCmd(0xa6);  // --set normal display
    halLcdSpiTxCmd(0xa8);  // --set multiplex ratio(1 to 64)
    halLcdSpiTxCmd(0x3f);  // --1/64 duty
    halLcdSpiTxCmd(0xd3);  // --set display offset Shift Mapping RAM 
                                    //   Counter(0x00~0x3F)
    halLcdSpiTxCmd(0x00);  // --not offset
    halLcdSpiTxCmd(0xd5);  // --set display clock divide
                                    //   ratio/oscillator oscillator frequency
    halLcdSpiTxCmd(0x80);  // --set divide ratio, Set Clock as 100 
                                    //   Frames/Sec
    halLcdSpiTxCmd(0xd9);  // --set pre-charge period
    halLcdSpiTxCmd(0xf1);  // --Set Pre-Charge as 15 Clocks & Discharge 
                                    //   as 1 Clock
    halLcdSpiTxCmd(0xda);  // --set com pins hardware configuration
    halLcdSpiTxCmd(0x12);
    halLcdSpiTxCmd(0xdb);  // --set vcomh
    halLcdSpiTxCmd(0x40);  // --Set VCOM Deselect Level
    halLcdSpiTxCmd(0x20);  // --Set Page Addressing Mode (0x00/0x01/0x02)
    halLcdSpiTxCmd(0x02);  //
    halLcdSpiTxCmd(0x8d);  // --set Charge Pump enable/disable
    halLcdSpiTxCmd(0x14);  // --set(0x10) disable
    halLcdSpiTxCmd(0xa4);  // --Disable Entire Display On (0xa4/0xa5)
    halLcdSpiTxCmd(0xa6);  // --Disable Inverse Display On (0xa6/a7) 
    halLcdSpiTxCmd(0xaf);  // --turn on oled panel
}

static void halOLED12864SetPosition(uint8 page, uint8 x)
{
    halLcdSpiTxCmd( 0xb0 + page );
    halLcdSpiTxCmd( ((x&0xf0)>>4)|0x10 );
    halLcdSpiTxCmd( (x&0x0f)|0x01 ); 
}

static void halOLEDShowChar8x16(uint16 x, uint16 page, uint8 ch)
{
    uint16 charIndex;
    
    /* index of font table, height: 16 */
    if(ch > 32) charIndex = (ch - 32) * 16;
    else charIndex = 0;
    
    /* Set first page */
    halOLED12864SetPosition(page, x);
    for (uint8 j = 0; j < 8; j++)  halLcdSpiTxData( FONT_TABLE_8x16[charIndex + j] );
    
    /* Set second page */
    halOLED12864SetPosition(page + 1, x);
    for (uint8 j = 0; j < 8; j++) halLcdSpiTxData( FONT_TABLE_8x16[charIndex + j + 8] );
}
