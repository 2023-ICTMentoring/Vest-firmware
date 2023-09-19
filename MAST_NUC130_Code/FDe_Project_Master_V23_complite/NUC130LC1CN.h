/*	Purpose : AER(ARM Embedded RTOS) Project
	Target Name : Cortex-M0 NUC130 RTOS-T37
	Compiler : MDK-ARM Compiler V5
	CMSIS : NUC130_CMSIS302_LIBRARY
	Document : ARM_Embedded_RTOS_�о�α�.txt
	Information : https://cafe.naver.com/fws
	---------------------------------------------------------------
	Copyright by FirmwareBank Inc,. All Rights Reserved. */

#ifndef __AER_H__
#define __AER_H__

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "NUC100Series.h"

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

// Proto Type
// timer
void time0_delay(unsigned long T0); // 10m
void time1_delay(unsigned long T1); // 1S

// system
void setup(void);

// Utility
unsigned long isflag(unsigned long flag);
void setflag(unsigned long flag);
void clrflag(unsigned long flag);
void auto_off(unsigned char hardware, unsigned long time_autooff);

// UART
// void ring_ini_lcd(void);
void ring_ini_rf(void);
void ring_ini_usb(void);
unsigned char check_poprf(void);
unsigned char check_popusb(void);
unsigned char check_pop(void);
void rf_3355_mast_read(void);
// project
void usb2rf(void);
void rf2usb(void);

// Module
void Module_LCD_string(unsigned char x, unsigned char y, char *lcdstring16);
void Module_LCD_onebyte(unsigned char x, unsigned char y, char lcdbyte);

// ADC
unsigned short adc_read(unsigned char adcchannel, unsigned long avr);

//void run_student_read_pop(void);
unsigned char run_student_receive_checksum(unsigned char datalen);
//void run_student_processor(unsigned char com);
void rf_3355_mast_read(void);
void run_keyboard_define(unsigned short adc_key);

// RUN // make game
//char run_keyboard_processor(unsigned short adc_key);
char run_keyboard_adc_return(unsigned short adc_key);
void run_student_send(unsigned char add, unsigned char cmd, unsigned char datalen);
void run_game_mode(void);
void run_function(unsigned char func);

char run_processor_keyin_ret_ascii_chosung(void);
unsigned char run_processor_chosung_to_number(char chosung_ascii);

//unsigned char run_keyboard_chosung_input(void);
//void run_processor_ascii_chosung(unsigned char keyvalue);

// Project Define
#define QSIZE 			120
#define ADC_KEY_CH 		5

// LCD DEFINE
#define LCD_SCREEN_CLR		0xC0
//UART_WRITE(UART_LCD, LCD_SCREEN_CLR); // all Screen Clear


// game define
#define STU_NO_BUTTON	0xB0
#define STU_O_BUTTON	0xFB
#define STU_X_BUTTON	0x0F

// Game Mode
#define GM_DEFAULT		0x0
#define GM_XOGAME		0x01
#define GM_CHOSUNG		0x03
#define GM_R_P_S		0x05 //rock-paper-scissors
#define GM_SAMEFONT		0x07
#define GM_RUNNING		0x09
#define GM_TAILGAME		0x0B
#define GM_MOSBUHO		0x0D
#define GM_HIDESEEK		0x0F // hide-and-(go-)seek, tag, (Brit) tig
#define GM_FRIEND		0x11 
#define GM_ZOMBEA		0x13

// void run_function(unsigned char func);
// function 
#define MAX_ATTEND		20// 최대 학생수, 참가자 출석표 작성

#define FUC_XOGAME			0x01
#define FUC_CHOSUNG			0x03
#define FUC_CHOSUNG_FONT 	0x04 // reandom send infor
#define FUC_TIMEOUTTIME		0x41
#define FUC_READXO			0x42
#define FUC_ATTEND			0x84
#define FUC_KEY_CAL			0x85
#define FUC_REBOOT			0x86



// UART
#define RF_OR0x80			0x80
#define RF_MAST_PREAMBLE 	0x66
#define RF_PC_PREAMBLE 		0x67
#define RF_STUDENT_PREAMBLE 0x80
#define RF_ALLSTUDENT 		0xA1 // ALL
#define TAIL 				0x0D

#define CMD_PING			0x71
#define CMD_XO_REQ			0x72
#define CMD_GAMEMODE		0x73
#define CMD_STU_RESET		0x74
#define CMD_CHOSUNG_FONT	0x77

#define CMD_BUZZER			0x78
#define CMD_DOT_LED			0x79
#define CMD_DOT_LIGHT		0x7A
#define CMD_STUD_TEST		0x7B
#define CMD_TIME_OVER		0x7C
#define CMD_STUD_OUTE		0x7D

// runflag define
#define F_IN15_BUTTON 		0x80000000
#define F_LEDB_AUTOOFF 		0x40000000
#define F_LEDR_AUTOOFF 		0x20000000
#define F_BUZZER_AUTOOFF 	0x10000000

#define F_RF_COM_OK 		0x08000000
#define F_RF_TIME 			0x04000000
#define F_STU_READ_TIMECHK	0x02000000
#define F_TIMEOUT_CHK_ING 	0x01000000
#define F_USB_TIME			0x00800000
#define F_K55555RD_IN 		0x00400000
#define F_A5555UTTON 		0x00200000
#define F_T5555555OVER	 	0x00100000

#define F_KEYBD_READ_OK		0x00080000
#define F_KEYBOARD_IN 		0x00040000
#define F_ADC_BUTTON 		0x00020000
#define F_TIMEOUT_OVER	 	0x00010000

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
#define TM_1000mS 100
#define TM_2000mS 200
#define TM_3000mS 300

// Hardware, OUT
#define LEDRF(bon) (PA6 = (bon) ? 0 : 1)
#define LEDBF(bon) (PA7 = (bon) ? 0 : 1)
#define BUZZERF(OOn) (PB12 = (OOn) ? 0 : 1)
#define RFSETPORT(OOn) (PA8 = (OOn) ? 0 : 1)

#define LEDRP (PA6)
#define LEDBP (PA7)
#define BUZZERP (PB12)

// Hardware, IN
#define IN15 (PB15) 

#endif
