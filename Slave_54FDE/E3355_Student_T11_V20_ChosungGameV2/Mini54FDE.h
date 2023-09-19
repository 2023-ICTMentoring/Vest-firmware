#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h> //srand, rand? ???? ?? ????

#include "Mini51Series.h"
#include "gpio.h"
#include "adc.h"

// Prototype
void setup(void);
void time0_delay(unsigned long T0); // 1 is 10mS
void time1_delay(unsigned long T1); // 1 is 1Sec
void TMR0_IRQHandler(void);
void TMR1_IRQHandler(void);

unsigned long isflag(unsigned long flag);
void setflag(unsigned long flag);
void clrflag(unsigned long flag);

// Project Function
void auto_off(unsigned char hardware, unsigned long time_autooff);

// Dot Function
void max7219_ch_write(uint8_t dot9, uint8_t addr, uint8_t value);
void shift8out(unsigned char data8);
void shift_delay(unsigned long nopdelay);
void max7219_ini(void);
void max7219_ini_bright(unsigned char bright);
void run_dotDisplay(unsigned char led9, unsigned char allONOFF);

// UART
unsigned char check_pop(void);
void ring_ini(void);
void putstring(unsigned char *pui8Buffer);
void rf_3355_student_read(void);
unsigned char rf_receive_checksum(unsigned char datalen);
void rf_student_processor(unsigned char com);
void run_send_mast(unsigned char cmd, unsigned char datalen);

// run function
unsigned char run_read_myaddress(void);
void run_game_mode(void);
void run_nop_delay(void);
void run_soundfunction(char option, unsigned long buzcount);
void run_chosung_font_Display(unsigned char chosung);
	

void Point_Display(void);
void NULL_Display(void);
void O_Display(void);
void X_Display(void);
void A_Display(void);
void B_Display(void);
void C_Display(void);
void D_Display(void);
void E_Display(void);
void F_Display(void);
void G_Display(void);
void H_Display(void);
void J_Display(void);
void K_Display(void);
void L_Display(void);
void N_Display(void);
void M_Display(void);
void OP_Display(void);

void Aa_Display(void);
void Cc_Display(void);
void Ff_Display(void);
void Gg_Display(void);
void Jj_Display(void);

	//void Random_Display(void);
void MAX7219_MatrixSetRow64(uint8_t index, uint64_t rows);
bool MAX7219_MatrixUpdate(void);
bool MAX7219_Digit(uint8_t index, uint8_t digit, int8_t value);
bool MAX7219_Write(uint8_t index, uint8_t addr, uint8_t data);

// System Config
#define QSIZE 120

// UART
#define MAST_PREAMBLE 	0x66
#define PC_PREAMBLE 	0x67
#define RF_STUDENT 		0x80
#define RF_ALLSTUDENT 	0xA1 // ALL
#define RF_ALLSTUDENT_PC	0xC2 //PC to student ALL
#define TAIL 			0x0D

// Dot Config
#define DECODE_MODE_ADDR (0x09)
#define DECODE_MODE_VAL (0x00) // No decode for digits 7-0

#define INTENSITY_ADDR (0x0A) // Bright Setting (0x00 ~ 0x0F)
//#define INTENSITY_VAL (0x07)
//#define INTENSITY_VAL (0x02)
#define INTENSITY_VAL (0x01)

#define SCAN_LIMIT_ADDR (0x0B) // How many use digit led (line count in matrix led device)
#define SCAN_LIMIT_VAL (0x07)  // digital = 0~7 , 8ea

#define POWER_DOWN_MODE_ADDR (0x0C) // Shutdown
#define POWER_DOWN_MODE_VAR (0x01)  // normal

#define TEST_DISPLAY_ADDR (0x0F) // Display Test
#define TEST_DISPLAY_VAL (0x00)  // normal, 1=test mode

#define NO_OP_ADDR (0x00)
#define NO_OP_VAL (0x00)

// RUN processor
#define STUDENT_NO	0xB0
#define STUDENT_O	0xFB
#define STUDENT_X	0x0F

// Game Mode
#define GM_DEFAULT		0x0
#define GM_XOGAME		0x01
#define GM_CHOSUNG		0x03
#define GM_CHOSUNG2     0x05
#define GM_R_P_S		0x07 //rock-paper-scissors
#define GM_SAMEFONT		0x09
#define GM_RUNNING		0x0B
#define GM_TAILGAME		0x0D
#define GM_MOSBUHO		0x0F
#define GM_HIDESEEK		0x11 // hide-and-(go-)seek, tag, (Brit) tig
#define GM_FRIEND		0x13 
#define GM_ZOMBEA		0x15

// Command
#define CMD_PING			0x71
#define CMD_XO_REQ			0x72
#define CMD_GAMEMODE		0x73
#define CMD_HW_RESET		0x74
#define CMD_CHOSUNG_FONT	0x77
#define CMD_BUZZER			0x78
#define CMD_DOT_LED			0x79
#define CMD_DOT_LIGHT		0x7A
#define CMD_STUD_TEST		0x7B
#define CMD_TIME_OVER		0x7C
#define CMD_STUD_OUTE		0x7D

// timer
#define TM_1SEC 1
#define TM_2SEC 2
#define TM_3SEC 3
#define TM_4SEC 4
#define TM_5SEC 5
#define TM_10SEC 10
#define TM_20SEC 20
#define TM_30SEC 30
#define TM_40SEC 40

#define TM_10mS 1
#define TM_20mS 2
#define TM_50mS 5
#define TM_100mS 10
#define TM_200mS 20
#define TM_300mS 30
#define TM_400mS 40
#define TM_500mS 50
#define TM_800mS 80
#define TM_1000mS 	100
#define TM_2000mS 	200
#define TM_3000mS 	300

// runflag define
//#define F_BUTTON_ON 		0x80000000
#define F_TOUCH_ON 			0x40000000		// touch button ON
#define F_LEDB_AUTOOFF 		0x20000000
#define F_BUZZER_AUTOOFF 	0x10000000 // 32bit

#define F_RF_COM_OK 		0x08000000
#define F_RF_TIME 			0x04000000

#define F_xxxxxx_AUTOOFF 	0x02000000
#define F_RUN_RANDOM_FONT   0x01000001
#define F_RUN_FONT_ACTION 	0x01000000
#define F_RUN_BUZ_ACTION 	0x00800000
#define F_RUN_DOT_ACTION 	0x00400000
#define F_RUN_DOT_ON 		0x00200000
#define F_RUN_DOT_OFF 		0x00100000

#define F_OVERTIME_CHECK 	0x00080000
#define F_TIME_OVER	 		0x00040000
#define F_TIME_WARN         0x00020000
//#define F_SEND_INFOBUT		0x00020000
#define F_GAME_ING	 		0x00010000		// LED Toggle

#define F_GAME_TOUCH_START 	0x00008000
#define F_S______INPUT	 	0x00004000
#define F_S_____NFOBUT		0x00002000
#define F_G_____NG	 		0x00001000

//#define OX_GAME_START		0x00000001
//#define KOREAN_GAME_ON 		0x00000010
//#define KOREAN_RANDOM_ON 	0x00000020
// define Hardware

#define MAX7219_CLK (P35)
#define MAX7219_DIN (P34)

#define MAX7219_CSORG (P51)
// #define MAX7219_CS1  (PA8)
// #define MAX7219_CS2  (PA9)

#define LOW_GPIO 0
#define HIGH_GPIO 1
// =============================================
#define LEDBF(OOn) 	(P06 = (OOn) ? 0 : 1)
#define LEDBP 		(P06)

#define BUZZER(OOn) (P07 = (OOn) ? 0 : 1)

#define RFCONT(OOn) (P14 = (OOn) ? 0 : 1) // GND is Setting

//#define BUTTON (P04)
#define TOUCH_BUTTON (P25)

#define ADDRESS1 (P15)
#define ADDRESS2 (P32)
#define ADDRESS4 (P24)
#define ADDRESS8 (P05)

//const uint64_t a[] = { // 
//    /* 8     =>      1   row */
//    0xffffffffffffffff,  /* 0 */
//    0xffffffffffffffff,  /* 1 */
//    0xffffffffffffffff,  /* 2 */
//    0xffffffffffffffff,  /* 3 */
//    0x0000000000000000,  /* 4 */
//    0x0000000000000000,  /* 5 */
//    0x0000000000000000,  /* 6 */
//    0x0000000000000000,  /* 7 */
//    0xffffffffffffffff}; /* 8 */

const uint64_t a[] = { // 
    /* 8     =>      1   row */
    0xffffffffffffffff,  /* 1 */
    0x0000000000000000,  /* 2 */
    0x0000000000000000,  /* 3 */
    0x0000000000000000,  /* 4 */
    0x0000000000000000,  /* 5 */
    0xffffffffffffffff,  /* 6 */
    0xffffffffffffffff,  /* 7 */
    0xffffffffffffffff,  /* 8 */
    0xffffffffffffffff}; /* 9 */

const uint64_t aa[] = { // 
    0xE0E0E0E0E0E0E0E0,  /* 1 */
    0x0000000000000000,  /* 2 */
    0x0000000000000000,  /* 3 */
    0x0000000000FFFFFF,  /* 4 */
    0x0000000000FFFFFF,  /* 5 */
    0x0000000000070707,  /* 6 */
    0xE0E0E0E0E0E0E0E0,  /* 7 */
    0x0000000000FFFFFF,  /* 8 */
    0x0000000000FFFFFF}; /* 9 */	

const uint64_t b[] = {
    0xffffffffffffffff,  /* 1 */
    0xffffffffffffffff,  /* 2 */
    0xffffffffffffffff,  /* 3 */
    0xffffffffffffffff,  /* 4 */
    0x0000000000000000,  /* 5 */
    0x0000000000000000,  /* 6 */
    0x0000000000000000,  /* 7 */
    0x0000000000000000,  /* 8 */
    0xffffffffffffffff}; /* 9 */	

const uint64_t c[] = {	//
    0xffffffffffffffff,  /* 1 */
    0xffffffffffffffff,  /* 2 */
    0xffffffffffffffff,  /* 3 */
    0xffffffffffffffff,  /* 4 */
    0x0000000000000000,  /* 5 */
    0xffffffffffffffff,  /* 6 */
    0xffffffffffffffff,  /* 7 */
    0x0000000000000000,  /* 8 */
    0xffffffffffffffff}; /* 9 */	

const uint64_t cc[] = {	
    0xC0C0C0C0C0C0C0C0,  /* 1 */
    0xFFFF000000000000,  /* 2 */
    0x0303030303030303,  /* 3 */
    0xC0C0000000000000,  /* 4 */
    0xFFFF000000000000,  /* 5 */
    0x0303000000000000,  /* 6 */
    0xC0C0C0C0C0C0C0C0,  /* 7 */
    0x0000000000000000,  /* 8 */
    0x0303030303030303}; /* 9 */		

const uint64_t d[] = {//
    /* 8     =>      1   row */
    0x0f0f0f0f0f0f0f0f,  /* 0 */
    0xfcfcfcfc3c3c3c3c,  /* 1 */
    0xfffffffff0f0f0f0,  /* 2 */
    0x0f0f0f0f0f0f0f0f,  /* 3 */
    0x3c3c3c3c3c3c3c3c,  /* 4 */
    0xf0f0f0f0f0f0f0f0,  /* 5 */
    0x0f0f0f0fffffffff,  /* 6 */
    0x3c3c3c3c3f3f3f3f,  /* 7 */
    0xf0f0f0f0f0f0f0f0}; /* 8 */

const uint64_t e[] = { //
    /* 8     =>      1   row */
    0xffffffffffffffff,  /* 0 */
    0xffffffffffffffff,  /* 1 */
    0xffffffffffffffff,  /* 2 */
    0xffffffffffffffff,  /* 3 */
    0x0000000000000000,  /* 4 */
    0xffffffffffffffff,  /* 5 */
    0xffffffffffffffff,  /* 6 */
    0xffffffffffffffff,  /* 7 */
    0xffffffffffffffff}; /* 8 */

const uint64_t f[] = { // 6  
    0xffffffff00000000,  /* 1 */
    0xffffffff3C3C3C3C,  /* 2 */
    0xffffffffF0F0F0F0,  /* 3 */
    0x0f0f0f0f0f0f0f0f,  /* 4 */
    0x3C3C3C3C3C3C3C3C,  /* 5 */
    0x0000000000000000,  /* 6 */
    0x00000000ffffffff,  /* 7 */
    0x3C3C3C3Cffffffff,  /* 8 */
    0xF0F0F0F0ffffffff}; /* 9 */	

const uint64_t ff[] = {	//
    0x0000000000000000,  /* 1 */
    0xFFFFCCCCCCCCFFFF,  /* 2 */
    0x0000000000000000,  /* 3 */
    0x0000000000000000,  /* 4 */
    0x0000000000000000,  /* 5 */
    0x0000000000000000,  /* 6 */
    0x0000000000000000,  /* 7 */
    0xFFFFCCCCCCCCFFFF,  /* 8 */
    0x0000000000000000}; /* 9 */	

const uint64_t g[] = { // 
	
    0x0000000000000000,  /* 1 */
    0x00000080E07C0F03,  /* 2 */
    0xC0D03C0F01000000,  /* 3 */
    0x0000000000000000,  /* 4 */
    0x0000000000000000,  /* 5 */
    0x030F3CE0E03C0F03,  /* 6 */
    0x0000000000000000,  /* 7 */
    0x030F7CE080000000,  /* 8 */
    0x000000010F3CD0C0}; /* 9 */	

const uint64_t gg[] = {	//
    0x0000000000000000,  /* 1 */
    0xC060381C1C3860C0,  /* 2 */
    0x0000000000000000,  /* 3 */
    0x0000000000000000,  /* 4 */
    0x0000000000000000,  /* 5 */
    0x0000000000000000,  /* 6 */
    0x0000000000000000,  /* 7 */
    0xC060381C1C3860C0,  /* 8 */
    0x0000000000000000}; /* 9 */		

	const uint64_t h[] = { // 8 o
    /* 8     =>      1   row */
    0x0000000080c06030,  /* 1 */
    0x0000007eff000000,  /* 2 */
    0x000000000103060c,  /* 3 */
    0x1018181818181810,  /* 4 */
    0x0000000000000000,  /* 5 */
    0x0818181818181808,  /* 6 */
    0x3060c08000000000,  /* 7 */
    0x000000ff7e000000,  /* 8 */
    0x0c06030100000000}; /* 9 */		
	

//const uint64_t h[] = { // 8
//    /* 8     =>      1   row */
//    0x0000000e3efef8e0,  /* 1 */
//    0x0000000000000303,  /* 2 */
//    0x0000383838383838,  /* 3 */
//    0x1c1cfcfcfcfc1c1c,  /* 4 */
//    0xf07c7f1f1f7f7cf0,  /* 5 */
//    0x0100000000000001,  /* 6 */
//    0xe0f8fe3e0e000000,  /* 7 */
//    0x0303000000000000,  /* 8 */
//    0x3838383838380000}; /* 9 */

const uint64_t j[] = { // 9
    0xE0E0E0E0E0E0E0E0,  /* 1 */
    0x0000000000000000,  /* 2 */
    0xC0E070381C0E0703,  /* 3 */
    0x0000000000000000,  /* 4 */
    0xC3E77E3C1C0E0703,  /* 5 */
    0x0707070707070707,  /* 6 */
    0xE0E0E0E0E0E0E0E0,  /* 7 */
    0x0000000000000000,  /* 8 */
    0x03070E1C3870E0C0}; /* 9 */	

const uint64_t jj[] = {	//
    0x0000000000000000,  /* 1 */
    0xC3633B1F1F3B63C3,  /* 2 */
    0x0000000000000000,  /* 3 */
    0x0000000000000000,  /* 4 */
    0x0000000000000000,  /* 5 */
    0x0000000000000000,  /* 6 */
    0x0000000000000000,  /* 7 */
    0xC3633B1F1F3B63C3,  /* 8 */
    0x0000000000000000}; /* 9 */		

    /* 8     =>      1   row */
//    0x0000000e3efef8e0,  /* 1 */
//    0x0000000000000303,  /* 2 */
//    0x00000e0e0e0e0e0e,  /* 3 */
//    0x7070fefefefe7070,  /* 4 */
//    0xf07c7f1f1f7f7cf0,  /* 5 */
//    0x0100000000000001,  /* 6 */
//    0xe0f8fe3e0e000000,  /* 7 */
//    0x0303000000000000,  /* 8 */
//    0x0e0e0e0e0e0e0000}; /* 9 */

const uint64_t k[] = {
    0xE0E0E0E0E0E0E0E0,  /* 1 */
    0x0000000000000000,  /* 2 */
    0xC0E070381C1E0703,  /* 3 */
    0x0000000000000000,  /* 4 */
    0xC3E77E3C1C0E0703,  /* 5 */
    0x0707F7F7F7F70707,  /* 6 */
    0xE0E0E0E0E0E0E0E0,  /* 7 */
    0x0000000000000000,  /* 8 */
    0x03070E1C3870E0C0}; /* 9 */	

	
//    0xE0E0E0E0E0E0E0E0,  /* 1 */
//    0x0000000000000000,  /* 2 */
//    0xC0E070381C3EE7C3,  /* 3 */
//    0x8000000000000080,  /* 4 */
//    0xC3E77E3C1C0E0703,  /* 5 */
//    0x030303F3F3030303,  /* 6 */
//    0xE0E0E0E0E0E0E0E0,  /* 7 */
//    0x0000000000000000,  /* 8 */
//    0x03070E1C3870E0C0}; /* 9 */	

const uint64_t l[] = {
	0x0707070707070707,  /* 1 */
    0x0000000000000000,  /* 2 */
    0x0000000000000000,  /* 3 */
    0x0000000000000000,  /* 4 */
    0x1C1C1C1C1C1C1C1C,  /* 5 */
    0xE0E0E0E0E0E0E0E0,  /* 6 */
    0x0707070707FFFFFF,  /* 7 */
    0x3838383838FFFFFF,  /* 8 */
    0x0000000000FFFFFF}; /* 9 */	

//    /* 8     =>      1   row */
//    0x0000fcfcfcfc0000,  /* 0 */
//    0x0000ffffffff0000,  /* 1 */
//    0x00003f3f3f3f3c3c,  /* 2 */
//    0x3c3c3c3c3c3c3c3c,  /* 3 */
//    0x3c3c3c3c3c3c3c3c,  /* 4 */
//    0x0000000000000000,  /* 5 */
//    0x0000000000000000,  /* 6 */
//    0x3c3c3c3c3c3c0000,  /* 7 */
//    0x3c3c3c3c3c3c0000}; /* 8 */
	
    /* 8     =>      1   row */
//    0x00003c3c3c3c3c3c,  /* 0 */
//    0x00003c3c3c3c3c3c,  /* 1 */
//    0x00003c3c3c3c3c3c,  /* 2 */
//    0x3c3c3c3c3c3c3c3c,  /* 3 */
//    0x3c3c3c3c3c3c3c3c,  /* 4 */
//    0x3c3c3c3c3c3c3c3c,  /* 5 */
//    0x0000000000000000,  /* 6 */
//    0x3c3cffffffff0000,  /* 7 */
//    0x3c3cfcfcfcfc0000}; /* 8 */

const uint64_t m[] = {
	
    0xFFFFFF0707070707,  /* 1 */
    0xFFFFFF3838383838,  /* 2 */
    0xFFFFFFE0E0E0E0E0,  /* 3 */
    0x0707070707070707,  /* 4 */
    0x1C1C1C1C1C1C1C1C,  /* 5 */
    0xE0E0E0E0E0E0E0E0,  /* 6 */
    0x0707070707070707,  /* 7 */
    0x0000000000000000,  /* 8 */
    0xE0E0E0E0E0E0E0E0}; /* 9 */
	
const uint64_t n[] = {
    0x3c3c3c3c3c3cfcfc,  /* 1 */
    0x000000000000ffff,  /* 2 */
    0x00003c3c3c3c3f3f,  /* 3 */
    0x3C3C3C3C3C3C3C3C,  /* 4 */
    0x0000000000000000,  /* 5 */
    0x3C3C3C3C3C3C3C3C,  /* 6 */
    0xfcfc3c3c3c3c3C3C,  /* 7 */
    0xffff000000000000,  /* 8 */
    0x3f3f3c3c3c3c0000}; /* 9 */
	
const uint64_t op[] = { // 9
	
    0x0606060686C66636,  /* 1 */
    0x0000007eff000000,  /* 2 */
    0x000000000103060c,  /* 3 */
    0x1018181818181810,  /* 4 */
    0x0000000000000000,  /* 5 */
    0x6878F8F8F8F87868,  /* 6 */
    0x3666c68606060606,  /* 7 */
    0x000000ff7e000000,  /* 8 */
    0x0c06030100000000}; /* 9 */		

const uint64_t OX[] = {
    0x0000000000000000,
    0xffffffffffffffff};

void Point_Display(void){
    MAX7219_MatrixSetRow64(0, OX[0]);
    MAX7219_MatrixSetRow64(1, OX[0]);
    MAX7219_MatrixSetRow64(2, OX[0]);
    MAX7219_MatrixSetRow64(3, OX[0]);
    MAX7219_MatrixSetRow64(4, OX[1]); 
    MAX7219_MatrixSetRow64(5, OX[0]);
    MAX7219_MatrixSetRow64(6, OX[0]);
    MAX7219_MatrixSetRow64(7, OX[0]);
    MAX7219_MatrixSetRow64(8, OX[0]);
    MAX7219_MatrixUpdate();
}

void NULL_Display(void){
    MAX7219_MatrixSetRow64(0, OX[0]);
    MAX7219_MatrixSetRow64(1, OX[0]);
    MAX7219_MatrixSetRow64(2, OX[0]);
    MAX7219_MatrixSetRow64(3, OX[0]);
    MAX7219_MatrixSetRow64(4, OX[0]); 
    MAX7219_MatrixSetRow64(5, OX[0]);
    MAX7219_MatrixSetRow64(6, OX[0]);
    MAX7219_MatrixSetRow64(7, OX[0]);
    MAX7219_MatrixSetRow64(8, OX[0]);
    MAX7219_MatrixUpdate();
}

void O_Display(void){
    MAX7219_MatrixSetRow64(0, OX[1]);
    MAX7219_MatrixSetRow64(1, OX[1]);
    MAX7219_MatrixSetRow64(2, OX[1]);
    MAX7219_MatrixSetRow64(3, OX[1]);
    MAX7219_MatrixSetRow64(4, OX[0]); // 느리지만 led에 O 또는 X만 표현됨
    MAX7219_MatrixSetRow64(5, OX[1]);
    MAX7219_MatrixSetRow64(6, OX[1]);
    MAX7219_MatrixSetRow64(7, OX[1]);
    MAX7219_MatrixSetRow64(8, OX[1]);
    MAX7219_MatrixUpdate();
}
void X_Display(void){
    MAX7219_MatrixSetRow64(0, OX[1]);
    MAX7219_MatrixSetRow64(1, OX[0]);
    MAX7219_MatrixSetRow64(2, OX[1]);
    MAX7219_MatrixSetRow64(3, OX[0]);
    MAX7219_MatrixSetRow64(4, OX[1]);
    MAX7219_MatrixSetRow64(5, OX[0]); // 느리지만 led에 O 또는 X만 표현됨
    MAX7219_MatrixSetRow64(6, OX[1]);
    MAX7219_MatrixSetRow64(7, OX[0]);
    MAX7219_MatrixSetRow64(8, OX[1]);
    MAX7219_MatrixUpdate();
}
void A_Display() // ㄱ
{
    MAX7219_MatrixSetRow64(0, a[0]);
    MAX7219_MatrixSetRow64(1, a[1]);
    MAX7219_MatrixSetRow64(2, a[2]);
    MAX7219_MatrixSetRow64(3, a[3]);
    MAX7219_MatrixSetRow64(4, a[4]);
    MAX7219_MatrixSetRow64(5, a[5]);
    MAX7219_MatrixSetRow64(6, a[6]);
    MAX7219_MatrixSetRow64(7, a[7]);
    MAX7219_MatrixSetRow64(8, a[8]);
    MAX7219_MatrixUpdate();
}

void B_Display() // ㄴ
{
    MAX7219_MatrixSetRow64(0, b[0]);
    MAX7219_MatrixSetRow64(1, b[1]);
    MAX7219_MatrixSetRow64(2, b[2]);
    MAX7219_MatrixSetRow64(3, b[3]);
    MAX7219_MatrixSetRow64(4, b[4]);
    MAX7219_MatrixSetRow64(5, b[5]);
    MAX7219_MatrixSetRow64(6, b[6]);
    MAX7219_MatrixSetRow64(7, b[7]);
    MAX7219_MatrixSetRow64(8, b[8]);
    MAX7219_MatrixUpdate();
}
void C_Display() // ㄷ
{
    MAX7219_MatrixSetRow64(0, c[0]);
    MAX7219_MatrixSetRow64(1, c[1]);
    MAX7219_MatrixSetRow64(2, c[2]);
    MAX7219_MatrixSetRow64(3, c[3]);
    MAX7219_MatrixSetRow64(4, c[4]);
    MAX7219_MatrixSetRow64(5, c[5]);
    MAX7219_MatrixSetRow64(6, c[6]);
    MAX7219_MatrixSetRow64(7, c[7]);
    MAX7219_MatrixSetRow64(8, c[8]);
    MAX7219_MatrixUpdate();
}
void D_Display() // ㄹ
{
    MAX7219_MatrixSetRow64(0, d[0]);
    MAX7219_MatrixSetRow64(1, d[1]);
    MAX7219_MatrixSetRow64(2, d[2]);
    MAX7219_MatrixSetRow64(3, d[3]);
    MAX7219_MatrixSetRow64(4, d[4]);
    MAX7219_MatrixSetRow64(5, d[5]);
    MAX7219_MatrixSetRow64(6, d[6]);
    MAX7219_MatrixSetRow64(7, d[7]);
    MAX7219_MatrixSetRow64(8, d[8]);
    MAX7219_MatrixUpdate();
}
void E_Display() // ㅁ
{
    MAX7219_MatrixSetRow64(0, e[0]);
    MAX7219_MatrixSetRow64(1, e[1]);
    MAX7219_MatrixSetRow64(2, e[2]);
    MAX7219_MatrixSetRow64(3, e[3]);
    MAX7219_MatrixSetRow64(4, e[4]);
    MAX7219_MatrixSetRow64(5, e[5]);
    MAX7219_MatrixSetRow64(6, e[6]);
    MAX7219_MatrixSetRow64(7, e[7]);
    MAX7219_MatrixSetRow64(8, e[8]);
    MAX7219_MatrixUpdate();
}
void F_Display() // ㅂ
{
    MAX7219_MatrixSetRow64(0, f[0]);
    MAX7219_MatrixSetRow64(1, f[1]);
    MAX7219_MatrixSetRow64(2, f[2]);
    MAX7219_MatrixSetRow64(3, f[3]);
    MAX7219_MatrixSetRow64(4, f[4]);
    MAX7219_MatrixSetRow64(5, f[5]);
    MAX7219_MatrixSetRow64(6, f[6]);
    MAX7219_MatrixSetRow64(7, f[7]);
    MAX7219_MatrixSetRow64(8, f[8]);
    MAX7219_MatrixUpdate();
}
void G_Display() // ㅅ
{
    MAX7219_MatrixSetRow64(0, g[0]);
    MAX7219_MatrixSetRow64(1, g[1]);
    MAX7219_MatrixSetRow64(2, g[2]);
    MAX7219_MatrixSetRow64(3, g[3]);
    MAX7219_MatrixSetRow64(4, g[4]);
    MAX7219_MatrixSetRow64(5, g[5]);
    MAX7219_MatrixSetRow64(6, g[6]);
    MAX7219_MatrixSetRow64(7, g[7]);
    MAX7219_MatrixSetRow64(8, g[8]);
    MAX7219_MatrixUpdate();
}

void OP_Display() // ㅇ
{
    MAX7219_MatrixSetRow64(0, op[0]);
    MAX7219_MatrixSetRow64(1, op[1]);
    MAX7219_MatrixSetRow64(2, op[2]);
    MAX7219_MatrixSetRow64(3, op[3]);
    MAX7219_MatrixSetRow64(4, op[4]);
    MAX7219_MatrixSetRow64(5, op[5]);
    MAX7219_MatrixSetRow64(6, op[6]);
    MAX7219_MatrixSetRow64(7, op[7]);
    MAX7219_MatrixSetRow64(8, op[8]);
    MAX7219_MatrixUpdate();
}
void H_Display() // O
{
    MAX7219_MatrixSetRow64(0, h[0]);
    MAX7219_MatrixSetRow64(1, h[1]);
    MAX7219_MatrixSetRow64(2, h[2]);
    MAX7219_MatrixSetRow64(3, h[3]);
    MAX7219_MatrixSetRow64(4, h[4]);
    MAX7219_MatrixSetRow64(5, h[5]);
    MAX7219_MatrixSetRow64(6, h[6]);
    MAX7219_MatrixSetRow64(7, h[7]);
    MAX7219_MatrixSetRow64(8, h[8]);
    MAX7219_MatrixUpdate();
}
void J_Display() // ㅈ 
{
    MAX7219_MatrixSetRow64(0, j[0]);
    MAX7219_MatrixSetRow64(1, j[1]);
    MAX7219_MatrixSetRow64(2, j[2]);
    MAX7219_MatrixSetRow64(3, j[3]);
    MAX7219_MatrixSetRow64(4, j[4]);
    MAX7219_MatrixSetRow64(5, j[5]);
    MAX7219_MatrixSetRow64(6, j[6]);
    MAX7219_MatrixSetRow64(7, j[7]);
    MAX7219_MatrixSetRow64(8, j[8]);
    MAX7219_MatrixUpdate();
}
void K_Display() // ㅊ 
{
    MAX7219_MatrixSetRow64(0, k[0]);
    MAX7219_MatrixSetRow64(1, k[1]);
    MAX7219_MatrixSetRow64(2, k[2]);
    MAX7219_MatrixSetRow64(3, k[3]);
    MAX7219_MatrixSetRow64(4, k[4]);
    MAX7219_MatrixSetRow64(5, k[5]);
    MAX7219_MatrixSetRow64(6, k[6]);
    MAX7219_MatrixSetRow64(7, k[7]);
    MAX7219_MatrixSetRow64(8, k[8]);
    MAX7219_MatrixUpdate();
}
void L_Display() // ㅌ
{
    MAX7219_MatrixSetRow64(0, l[0]);
    MAX7219_MatrixSetRow64(1, l[1]);
    MAX7219_MatrixSetRow64(2, l[2]);
    MAX7219_MatrixSetRow64(3, l[3]);
    MAX7219_MatrixSetRow64(4, l[4]);
    MAX7219_MatrixSetRow64(5, l[5]);
    MAX7219_MatrixSetRow64(6, l[6]);
    MAX7219_MatrixSetRow64(7, l[7]);
    MAX7219_MatrixSetRow64(8, l[8]);
    MAX7219_MatrixUpdate();
}

void M_Display() // ㅍ
{
    MAX7219_MatrixSetRow64(0, m[0]);
    MAX7219_MatrixSetRow64(1, m[1]);
    MAX7219_MatrixSetRow64(2, m[2]);
    MAX7219_MatrixSetRow64(3, m[3]);
    MAX7219_MatrixSetRow64(4, m[4]);
    MAX7219_MatrixSetRow64(5, m[5]);
    MAX7219_MatrixSetRow64(6, m[6]);
    MAX7219_MatrixSetRow64(7, m[7]);
    MAX7219_MatrixSetRow64(8, m[8]);
    MAX7219_MatrixUpdate();
}

void N_Display()  // ㅎ
{
    MAX7219_MatrixSetRow64(0, n[0]);
    MAX7219_MatrixSetRow64(1, n[1]);
    MAX7219_MatrixSetRow64(2, n[2]);
    MAX7219_MatrixSetRow64(3, n[3]);
    MAX7219_MatrixSetRow64(4, n[4]);
    MAX7219_MatrixSetRow64(5, n[5]);
    MAX7219_MatrixSetRow64(6, n[6]);
    MAX7219_MatrixSetRow64(7, n[7]);
    MAX7219_MatrixSetRow64(8, n[8]);
    MAX7219_MatrixUpdate();
}

void Aa_Display(){
    MAX7219_MatrixSetRow64(0, aa[0]);
    MAX7219_MatrixSetRow64(1, aa[1]);
    MAX7219_MatrixSetRow64(2, aa[2]);
    MAX7219_MatrixSetRow64(3, aa[3]);
    MAX7219_MatrixSetRow64(4, aa[4]);
    MAX7219_MatrixSetRow64(5, aa[5]);
    MAX7219_MatrixSetRow64(6, aa[6]);
    MAX7219_MatrixSetRow64(7, aa[7]);
    MAX7219_MatrixSetRow64(8, aa[8]);
    MAX7219_MatrixUpdate();
}

void Cc_Display(){
    MAX7219_MatrixSetRow64(0, cc[0]);
    MAX7219_MatrixSetRow64(1, cc[1]);
    MAX7219_MatrixSetRow64(2, cc[2]);
    MAX7219_MatrixSetRow64(3, cc[3]);
    MAX7219_MatrixSetRow64(4, cc[4]);
    MAX7219_MatrixSetRow64(5, cc[5]);
    MAX7219_MatrixSetRow64(6, cc[6]);
    MAX7219_MatrixSetRow64(7, cc[7]);
    MAX7219_MatrixSetRow64(8, cc[8]);
    MAX7219_MatrixUpdate();
}

void Ff_Display(){
    MAX7219_MatrixSetRow64(0, ff[0]);
    MAX7219_MatrixSetRow64(1, ff[1]);
    MAX7219_MatrixSetRow64(2, ff[2]);
    MAX7219_MatrixSetRow64(3, ff[3]);
    MAX7219_MatrixSetRow64(4, ff[4]);
    MAX7219_MatrixSetRow64(5, ff[5]);
    MAX7219_MatrixSetRow64(6, ff[6]);
    MAX7219_MatrixSetRow64(7, ff[7]);
    MAX7219_MatrixSetRow64(8, ff[8]);
    MAX7219_MatrixUpdate();
}

void Gg_Display(){
    MAX7219_MatrixSetRow64(0, gg[0]);
    MAX7219_MatrixSetRow64(1, gg[1]);
    MAX7219_MatrixSetRow64(2, gg[2]);
    MAX7219_MatrixSetRow64(3, gg[3]);
    MAX7219_MatrixSetRow64(4, gg[4]);
    MAX7219_MatrixSetRow64(5, gg[5]);
    MAX7219_MatrixSetRow64(6, gg[6]);
    MAX7219_MatrixSetRow64(7, gg[7]);
    MAX7219_MatrixSetRow64(8, gg[8]);
    MAX7219_MatrixUpdate();
}

void Jj_Display(){
    MAX7219_MatrixSetRow64(0, jj[0]);
    MAX7219_MatrixSetRow64(1, jj[1]);
    MAX7219_MatrixSetRow64(2, jj[2]);
    MAX7219_MatrixSetRow64(3, jj[3]);
    MAX7219_MatrixSetRow64(4, jj[4]);
    MAX7219_MatrixSetRow64(5, jj[5]);
    MAX7219_MatrixSetRow64(6, jj[6]);
    MAX7219_MatrixSetRow64(7, jj[7]);
    MAX7219_MatrixSetRow64(8, jj[8]);
    MAX7219_MatrixUpdate();
}
void Random_Display()
{
    int randomValue = rand() % 18; // Generate a random number between 0 and 18

    switch (randomValue)
    {
    case 0:
        A_Display();
        break;
    case 1:
        B_Display();
        break;
    case 2:
        C_Display();
        break;
    case 3:
        D_Display();
        break;
    case 4:
        E_Display();
        break;
    case 5:
        F_Display();
        break;
    case 6:
        G_Display();
        break;
    case 7:
        OP_Display();
        break;
    case 8:
        H_Display();
        break;
    case 9:
        J_Display();
        break;
    case 10:
        K_Display();
        break;
    case 11:
        L_Display();
        break;
    case 12:
        M_Display();
    case 13:
        N_Display();
        break;
    case 14:
        Aa_Display();
        break;
    case 15:
        Cc_Display();
        break;
    case 16:
        Ff_Display();
        break;
    case 17:
        Gg_Display();
        break;
    case 18:
        Jj_Display();
        break;   
    default:
        // You can handle the default case if needed.
        break;
    }
}
#endif
