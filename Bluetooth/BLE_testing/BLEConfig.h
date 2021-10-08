//Bluetooth setup vars
#define BUFSIZE             128 //this is the size of the read buffer for incoming data
#define VERBOSE_MODE        true //this setting enables debug output

//SPI Pin settings tailored to ATMega328p during BREADBOARDING
//Arbitrarily selected pins- can be any GPIO 
#define BLUEFRUIT_SPI_CS    8
#define BLUEFRUIT_SPI_IRQ   7
#define BLUEFRUIT_SPI_RST   4 //optional, set as -1 if unused

//hardware specific to the 328p SMD - MUST BE THESE 
#define BLUEFRUIT_SPI_SCK    3
#define BLUEFRUIT_SPI_MISO   5
#define BLUEFRUIT_SPI_MOSI   6

#define FACTORYRESET_ENABLE 1
//NEED TO REDEFINE FOR THE SAMD21 PINOUT
/*
 * define BLUEFRUIT_SPI_SCK 17
 * define BLUEFRUIT_SPI_MISO 16
 * define BLUEFRUIT_SPI_MOSI 15
 * 
 */
