#ifndef HC12CONFIG_H_
#define HC12CONFIG_H_
#include "Arduino.h"

//#define HC12_DEBUG
#define HC12_SUCCESS        0
#define HC12_ERROR          ( -1 )
#define HC12_MS_DELAY       250       /* ms to wait after entering/leaving the programming mode - this could be faster for faster baud-rates */
#define HC12_MS_TIMEOUT     2000      /* Timeout in ms to wait for a byte to arrive */
#define HC12_ALL_BAUDS      {9600UL, 115200UL, 57600UL, 38400UL, 19200UL, 4800UL, 2400UL, 1200UL}   /* all available baudrates of the HC12 */
#define HC12_DEFAULT_BAUD   9600    /* default baudrate of the HC12 */


/**
 * @brief   Helper-Class for configuring the HC12 module
 */
class hc12Config {
  private:
    HardwareSerial *SerialHC12;
    uint8_t setPin;

    int start();
    int testBaud();
    int findBaud();
    int sendCommandAndTestResult(const char* command, bool silent = false);

  public:
/**
 * @brief   Helper-Class for configuring the HC12 module
 *
 * @param   SerialHC12 the serial-decive the HC12 is connected to. e.g. Serial2 (Pins 16/17)
 * @param   setPin the GPIO pin connected to the SET pin of the HC12
 */  
    hc12Config(HardwareSerial *SerialHC12, uint8_t setPin);

/**
 * @brief   Checks if we can communicate to the HC12
 *          will also configure the HC12 to use the baudrate of the supplied serial-device
 *          the other settings present at the HC12 will not be changed
 * 
 * @return
 *     - HC12_SUCCESS Success @
 *     - HC12_ERROR Could not confirm a connection
 */
    int begin();

/**
 * @brief Checks if we can communicate to the HC12 and configure it
 *
 * @param baud target baudrate - one of HC12_ALL_BAUDS
 * @param channel target channel - values from 1 to 100 are possible
 *                Channels are 400kHz apart, check your local legislation which channels you can use (EU is most-likely channels 1-4)
 * @param mode function mode - values from 1-4 are possible
 * @param power power level - values from 1-8 are possible, check your local legislation which power output is allowed (EU is most-likely < 5)   
 * 
 * @return
 *     - HC12_SUCCESS Success
 *     - HC12_ERROR Something went wrong
 */
    int begin(unsigned long baud, int8_t channel = 1, int mode = 3, int power = 4);

/**
 * @brief restore the default-settings of the HC12 (FU3, Baud 9600)
 */      
    int set_defaults();

/**
 * @brief activate the sleep-mode
 */      
    int sleep();

/**
 * @brief wake up from sleep
 */      
    int wakeup();

/**
 * @brief enter/leave programming mode
 * 
 * @param enter true = enter programming mode / false = leave programming mode
 */      
    void programming_mode(bool enter);
};

#endif /* HC12CONFIG_H_ */