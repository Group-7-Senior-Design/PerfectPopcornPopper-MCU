//Bluetooth setup vars
//this is the size of the read buffer for incoming data
#define BUFSIZE             128 
//this setting enables debug output
#define VERBOSE_MODE        true 
//clear all GATT services/chars if set to 1
#define GATT_CLEAR_ALL    0 
//setting this flag to 1 will halt initialization until an end device connects
#define WAIT_FOR_CONN   0 

//SPI Pin settings tailored to ATMega328p/pb
//Arbitrarily selected pins- can be any GPIO 
//5 //WORKS //0 //PD0 - 30
#define BLUEFRUIT_SPI_CS  8
//6 //WORKS //1 //PD1 - 31
#define BLUEFRUIT_SPI_IRQ 7
//7 //WORKS //7 //optional, set as -1 if unused -> PD7 - 11
#define BLUEFRUIT_SPI_RST 4

//hardware specific to the 328p SMD - MUST BE THESE 
//0//13 //PB5 - 17
#define BLUEFRUIT_SPI_SCK    13
//1//12 //PD5 - 16
#define BLUEFRUIT_SPI_MISO   12
//2//11 //PD6 - 15
#define BLUEFRUIT_SPI_MOSI   11

#define FACTORYRESET_ENABLE 0

#define FACTORYRESET_ENABLE 1
//NEED TO REDEFINE FOR THE SAMD21 PINOUT
/*
 * define BLUEFRUIT_SPI_SCK 17
 * define BLUEFRUIT_SPI_MISO 16
 * define BLUEFRUIT_SPI_MOSI 15
 * 
 */
