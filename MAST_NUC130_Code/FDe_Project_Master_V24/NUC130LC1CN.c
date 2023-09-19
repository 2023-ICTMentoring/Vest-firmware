/*	Purpose : AER(ARM Embedded RTOS) Project
	Target Name : E3355-T11
	Compiler : MDK-ARM Compiler V5
	CMSIS : NUC130_CMSIS302_LIBRARY
	Document : xxxx_.txt
	Information :
	- dot led 8*8 Workig
	- each 9 ea
	- Max7219
	---------------------------------------------------------------
	Copyright by FirmwareBank Inc,. All Rights Reserved. */

// Cortex-M0 NUC130 Main Board Embedded Project Code

#include "NUC130LC1CN.h"

struct ring
{
	volatile unsigned long wp;
	volatile unsigned long rp;
	volatile unsigned char buffer[QSIZE];
	volatile unsigned char get_char;
};
// struct ring lcd;					// for ring buffer for UART0
struct ring rf;	 // for ring buffer for UART1
struct ring usb; // for ring buffer for UART2

struct time_value
{
	volatile unsigned long ledred_off, ledblue_off, buzzer_off;
	volatile unsigned long com_check_usb, com_check_rf;
};
struct time_value time;

struct project{
	volatile unsigned short keyboard_adc;
	char key_now, key_temp;

	volatile unsigned char gamemode, gamemode_sw;
	unsigned char senddata[10];
	unsigned char rfadd, rfcommand, rfreaddata[20];
	unsigned short KEY[13]; // key board 1~12
	unsigned char student_attend[MAX_ATTEND];// MAX_ATTEND, #define MAX_ATTEND		20// 최대 학생수, 참가자 출석표 작성
	unsigned char student_att_exit;
	unsigned char howmanystudent; // attend 학생수
	unsigned long timeout_sec;// timeout
	unsigned char student_OX_check;
	unsigned char student_chosung_font[16];	
	unsigned char key_lcd_position;
	unsigned char while_event;
	
	unsigned char ucpass; // use general
};
struct project run;

struct com_uart
{
	volatile unsigned long comtimeout;
	volatile unsigned char get_char;
	volatile unsigned char sequence;
	volatile unsigned char count;
	volatile unsigned char len;
	volatile unsigned char sum;
	unsigned char readdata[10];

	volatile unsigned char command;
	volatile unsigned char add;
	char textlcd[16];
};
struct com_uart mast;

// General Value
unsigned long ulpass;
unsigned short uspass;
unsigned char ucpass;
char cpass;
unsigned long runflag;

// timer
volatile unsigned long timer0tick, timer1tick; //, timer2tick; // Timer Value.

#define UART_LCD 	UART0
#define UART_RF 	UART1
#define UART_USB 	UART2

#define CODE_VERSION	23

//int random[MAX_ATTEND] = {0, };
// Project, Running
int main(void){
	
	setup();

	UART_Open(UART_LCD, 9600); //
	UART_Open(UART_RF, 9600);  // mast <-> student
	UART_Open(UART_USB, 9600); //

	ring_ini_rf();
	ring_ini_usb();

	// for LCD ini wating 5 Sec
	LEDRF(1);
	LEDBF(1);
	time1_delay(TM_2SEC);
	LEDRF(0);
	LEDBF(0);
	
	// key in Boot is rf <==> USB com toss
//	if (IN15 == 0){		  // in key
//		auto_off('Z', TM_1000mS); // Buzzer ON and auto off
//		while (23)
//		{
//			// COM Pass
//			usb2rf();
//			rf2usb();
//		}
//	}

	// TEXT LCD initial
	UART_WRITE(UART_LCD, 0xFB); // First
	UART_WRITE(UART_LCD, 0xD3); // 1101 0011 BackLight ON
	UART_WRITE(UART_LCD, 0xC0); // 1100 0000 command format : all Screen Clear

	//UART_WRITE(UART_LCD, 0x90); // Format : 10yy xxxx(6bit Course position)   x=0, y=1, at position
	strcpy(mast.textlcd, "     Welcome    ");	// max 16 char	
	//Module_LCD_string(0, 1, mast.textlcd);
	
	// version display
	mast.textlcd[13] = 'V';
	mast.textlcd[14] = (CODE_VERSION/10) + 0x30;
	mast.textlcd[15] = (CODE_VERSION - ((mast.textlcd[14]-0x30)*10)) + 0x30;
	Module_LCD_string(0, 1, mast.textlcd);
	
	strcpy(mast.textlcd, "Jokki Jokki Game");	// max 16 char	
	Module_LCD_string(0, 2, mast.textlcd);
	
	time1_delay(TM_3SEC);
	UART_WRITE(UART_LCD, 0xC0); // 1100 0000 command format : all Screen Clear
	
	//textLCD_string(mast.textlcd);
	auto_off('R', TM_100mS); // Buzzer ON and auto off

	// Software ini
	run.gamemode_sw = run.gamemode = GM_DEFAULT * 10; // first is zero

	// Key board Setting
	run.keyboard_adc = 2901;//basic oper adc_read(ADC_KEY_CH, 32) by jtag no KEY
	run_keyboard_define(run.keyboard_adc);
	
	while (2){// make game	
		
		// PC <====> Student
		usb2rf();
		rf2usb();
		
		run_game_mode(); // case run.gamemode_sw
		
		if (isflag(F_IN15_BUTTON)){
			// key board Calibration
			run.keyboard_adc = adc_read(ADC_KEY_CH, 32);// 2901; by jtag no KEY
			run_keyboard_define(run.keyboard_adc);
			
			// Hardware, OUT
			LEDRF(1);
			LEDBF(1);
			BUZZERF(1);
			time0_delay(TM_1000mS);

			LEDRF(0);
			LEDBF(0);
			BUZZERF(0);
			clrflag(F_IN15_BUTTON);
		}
	} // end while
}

void setup(void)
{
	SYS_UnlockReg();
	CLK_EnableXtalRC(CLK_PWRCON_OSC22M_EN_Msk);
	CLK_WaitClockReady(CLK_CLKSTATUS_IRC22M_STB_Msk);
	CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HIRC, CLK_CLKDIV_HCLK(1));

	// Enable IP clock
	CLK_EnableModuleClock(TMR0_MODULE);
	CLK_EnableModuleClock(TMR1_MODULE);
	CLK_EnableModuleClock(UART0_MODULE);
	CLK_EnableModuleClock(UART1_MODULE);
	CLK_EnableModuleClock(UART2_MODULE);
	CLK_EnableModuleClock(ADC_MODULE);

	CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0_S_HIRC, 0);
	CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1_S_HIRC, 0);
	CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HIRC, CLK_CLKDIV_UART(1)); // CLK_CLKSEL1_UART_S_HIRC
	CLK_SetModuleClock(UART1_MODULE, CLK_CLKSEL1_UART_S_HIRC, CLK_CLKDIV_UART(1)); // CLK_CLKSEL1_UART_S_HIRC
	CLK_SetModuleClock(UART2_MODULE, CLK_CLKSEL1_UART_S_HIRC, CLK_CLKDIV_UART(1)); // CLK_CLKSEL1_UART_S_HIRC
	CLK_SetModuleClock(ADC_MODULE, CLK_CLKSEL1_ADC_S_HIRC, CLK_CLKDIV_ADC(10));	   // CLK_CLKSEL1_ADC_S_HIRC

	SystemCoreClockUpdate();
	SYS_LockReg();

	SYS->ALT_MFP = 0x00000000;
	SYS->ALT_MFP1 = 0x00000000;
	SYS->ALT_MFP2 = 0x00000000;

	// Port A
	// SYS->GPA_MFP = 0;
	SYS->GPA_MFP = SYS_GPA_MFP_PA5_ADC5;

	// Port B
	// SYS->GPB_MFP = 0;
	SYS->GPB_MFP |= SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD | SYS_GPB_MFP_PB4_UART1_RXD | SYS_GPB_MFP_PB5_UART1_TXD;

	// Port C
	SYS->GPC_MFP = 0;

	// Port D
	// SYS->GPD_MFP = 0;
	SYS->GPD_MFP = SYS_GPD_MFP_PD14_UART2_RXD | SYS_GPD_MFP_PD15_UART2_TXD;

	// LED Red, blue
	GPIO_SetMode(PA, BIT6 | BIT7, GPIO_PMD_OUTPUT);

	// Buttons
	GPIO_SetMode(PB, BIT15 | BIT8, GPIO_PMD_QUASI);

	// Buzzer
	GPIO_SetMode(PA, BIT12, GPIO_PMD_OUTPUT);

	// RFCON
	GPIO_SetMode(PA, BIT8, GPIO_PMD_OUTPUT);
	RFSETPORT(0); // 1 is Com MODE

	// Timer
	TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 100); // 10mS
	TIMER_EnableInt(TIMER0);
	NVIC_EnableIRQ(TMR0_IRQn); // TMR0_IRQHandler
	TIMER_Start(TIMER0);

	TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 1); //  1 Sec
	TIMER_EnableInt(TIMER1);
	NVIC_EnableIRQ(TMR1_IRQn); // TMR1_IRQHandler
	TIMER_Start(TIMER1);

	// UART
	UART_ENABLE_INT(UART1, (UART_IER_RDA_IEN_Msk | UART_IER_TOUT_IEN_Msk));
	NVIC_EnableIRQ(UART1_IRQn); // UART_IRQHandler

	//	UART_EnableInt(UART0,(UART_IER_RDA_IEN_Msk | UART_IER_TOUT_IEN_Msk));
	// NVIC_EnableIRQ(UART02_IRQn);	//UART_IRQHandler

	UART_EnableInt(UART2, (UART_IER_RDA_IEN_Msk | UART_IER_TOUT_IEN_Msk));
	NVIC_EnableIRQ(UART02_IRQn); // UART_IRQHandler

	// ADC
	ADC_Open(ADC, ADC_ADCR_DIFFEN_SINGLE_END, ADC_ADCR_ADMD_SINGLE, (1 << 5)); // adc5==> KEY BD12
	ADC_POWER_ON(ADC);
}

// Interrupt   ----------------------------------

// UART02_IRQn
void UART02_IRQHandler(void)
{ // UART0_IRQHandler
	//	uint32_t u32IntSts0 = UART0->ISR;
	uint32_t u32IntSts2 = UART2->ISR;

	//	if(u32IntSts0 & UART_ISR_RDA_INT_Msk) {
	//		while(!UART_GET_RX_EMPTY(UART0)){
	//			lcd.buffer[lcd.wp]= UART_READ(UART0);
	//			lcd.wp++;
	//			if(lcd.wp > QSIZE) lcd.wp=0;
	//			}
	//	}

	if (u32IntSts2 & UART_ISR_RDA_INT_Msk)
	{
		while (!UART_GET_RX_EMPTY(UART2))
		{
			usb.buffer[usb.wp] = UART_READ(UART2);
			usb.wp++;
			if (usb.wp > QSIZE)
				usb.wp = 0;
		}
	}
}

// uart1
void UART1_IRQHandler(void)
{ // UART1_IRQHandler

	uint32_t u32IntSts = UART1->ISR;

	if (u32IntSts & UART_ISR_RDA_INT_Msk)
	{
		while (!UART_GET_RX_EMPTY(UART1))
		{
			rf.buffer[rf.wp] = UART_READ(UART1);
			rf.wp++;
			if (rf.wp > QSIZE)
				rf.wp = 0;
		}
	}
}

void TMR0_IRQHandler(void){ // 10ms
	TIMER_ClearIntFlag(TIMER0);
	timer0tick++;
	
	if(isflag(F_STU_READ_TIMECHK)){ // 학생들에게 데이터 요청후 수신 대기 시간
		time.com_check_rf--;
		if(time.com_check_rf == 0){
			clrflag(F_STU_READ_TIMECHK);
			run.student_att_exit = 0; // endless loop exit timer
		}		
	}

	if (IN15 == 0) setflag(F_IN15_BUTTON); // calibration button

	if (isflag(F_KEYBD_READ_OK)){
		run.key_temp = run_keyboard_adc_return(adc_read(ADC_KEY_CH, 32));
		if(run.key_temp){
			run.key_now = run.key_temp;
			setflag(F_ADC_BUTTON);
		}
	}			

	if (isflag(F_BUZZER_AUTOOFF))	{
		time.buzzer_off--;
		if (time.buzzer_off == 0)	{
			BUZZERF(0);
			clrflag(F_BUZZER_AUTOOFF);
		}
	}

	if (isflag(F_LEDB_AUTOOFF))
	{
		time.ledblue_off--;
		if (time.ledblue_off == 0)
		{
			LEDBF(0);
			clrflag(F_LEDB_AUTOOFF);
		}
	}

	if (isflag(F_LEDR_AUTOOFF))
	{
		time.ledred_off--;
		if (time.ledred_off == 0)
		{
			LEDRF(0);
			clrflag(F_LEDR_AUTOOFF);
		}
	}
}
// Interrupt   ----------------------------------

void TMR1_IRQHandler(void){ //  1sec
	TIMER_ClearIntFlag(TIMER1);
	timer1tick++;
	
	GPIO_TOGGLE(LEDBP);
	
	if(isflag(F_TIMEOUT_CHK_ING)){// time out start
		run.timeout_sec--;
		if(run.timeout_sec == 0){
			clrflag(F_TIMEOUT_CHK_ING); // time out start
			setflag(F_TIMEOUT_OVER);
		}
	}
}

void time0_delay(unsigned long T0)
{
	timer0tick = 0;
	while (timer0tick < T0)
		;
}

void time1_delay(unsigned long T1)
{
	timer1tick = 0;
	while (timer1tick < T1)
		;
}

// Utility Function ====================================
unsigned long isflag(unsigned long flag)
{
	return (runflag & flag);
}

void setflag(unsigned long flag)
{
	runflag |= flag;
}

void clrflag(unsigned long flag)
{
	runflag &= ~flag;
}

void auto_off(unsigned char hardware, unsigned long time_autooff)
{

	switch (hardware)
	{
	case 'B': // Blue LED
	case 'b':
		setflag(F_LEDB_AUTOOFF);
		time.ledblue_off = time_autooff;
		LEDBF(1);
		break;

	case 'R': // Red LED
	case 'r':
		setflag(F_LEDR_AUTOOFF);
		time.ledred_off = time_autooff;
		LEDRF(1);
		break;

	case 'Z': // Buzzer
	case 'z':
		setflag(F_BUZZER_AUTOOFF);
		time.buzzer_off = time_autooff;
		BUZZERF(1);
		break;
	}
}
// Utility Function ====================================

// UART 2222222222222222222222222222222222222222222222222222
void ring_ini_rf(void)
{
	rf.rp = 0;
	rf.wp = 0;
}

void ring_ini_usb(void)
{
	usb.rp = 0;
	usb.wp = 0;
}

unsigned char check_poprf(void)
{
	if (rf.wp != rf.rp)
	{
		rf.get_char = (rf.buffer[rf.rp]);
		rf.rp++; // Increment Read Point
		if (rf.rp > QSIZE)
			rf.rp = 0; // Read ��������
		return 1;
	} // end if
	return 0;
}

unsigned char check_popusb(void)
{ // software
	if (usb.wp != usb.rp)
	{
		usb.get_char = (usb.buffer[usb.rp]); // pop get char by check flag
		usb.rp++;							 // Increment Read Point
		if (usb.rp > QSIZE)
			usb.rp = 0; // Read ��������
		return 1;
	}
	return 0;
}

// UART 222222222222222222222222222222222222222222222222

// Project Function ====================================
// project
void usb2rf(void)
{
	if (check_popusb())
	{
		UART_WRITE(UART_RF, usb.get_char);
		auto_off('R', TM_200mS);
	}
}
void rf2usb(void)
{
	if (check_poprf())
	{
		UART_WRITE(UART_USB, rf.get_char);
		auto_off('B', TM_200mS);
	}
}

// LCD Module   ====================================
void Module_LCD_string(unsigned char x, unsigned char y, char *lcdstring16){
	unsigned char ipass;
	
	ipass = 0x80 | x | (y << 4); // 0x90); // Format : 10yy xxxx(6bit Course position)   x=0, y=1, at position
	UART_WRITE(UART_LCD, ipass);
	
	for (ipass = 0; lcdstring16[ipass] != '\0'; ipass++){ // Null('\0')
		UART_WRITE(UART_LCD, lcdstring16[ipass]);
	}
	
}

void Module_LCD_onebyte(unsigned char x, unsigned char y, char lcdbyte){
	unsigned char ipass;
	
	ipass = 0x80 | x | (y << 4); // 0x90); // Format : 10yy xxxx(6bit Course position)   x=0, y=1, at position
	UART_WRITE(UART_LCD, ipass);
	UART_WRITE(UART_LCD, lcdbyte);	
}

// LCD Module   ====================================

// ADC   ()()()()()()())))))))))))))))))))))))
unsigned short adc_read(unsigned char adcchannel, unsigned long avr){

	unsigned long i;
	unsigned long adc_sum = 0;

	ADC_SET_INPUT_CHANNEL(ADC, 1 << adcchannel);

	for (i = 0; i < avr; i++)
	{
		ADC_START_CONV(ADC);
		while (ADC_IS_BUSY(ADC))
			; // 1 ADC is busy
		adc_sum += ADC_GET_CONVERSION_DATA(ADC, adcchannel);
	}

	return (adc_sum / avr);
}
// ADC   ()()()()()()())))))))))))))))))))))))

// Project Function ====================================

// game mode play
void run_game_mode(void){
	
	
	switch(run.gamemode_sw){
		
		case 0 : //default start
			run.gamemode_sw =2;// x0 game
			break;
		
		case 1:// check and key board calibration and next manu
			if(isflag(F_ADC_BUTTON)){
				//if(run.key_now != run.key_old){
					//run.key_old = run.key_now;
					auto_off('Z', TM_200mS);
					Module_LCD_string(0, 2, "");				
					UART_WRITE(UART_LCD, run.key_now);
					//if(run.key_old == '4') run_function(FUC_ATTEND); //void run_function(unsigned char func);
					if(run.key_now == '5') {run_function(FUC_KEY_CAL); run.gamemode_sw=0;}
					if(run.key_now == '#') run.gamemode_sw++;// next// next run_function(FUC_KEY_CAL); 
				//}
				time0_delay(TM_200mS);
				clrflag(F_ADC_BUTTON);				
			}	
			break;
		
		case 2:
			run.gamemode_sw++;// next		
			UART_WRITE(UART_LCD, 0xC0); // 1100 0000 command format : all Screen Clear
			//strcpy(mast.textlcd, "4:Attend 5:KEYBD");		
			  strcpy(mast.textlcd, "4:XO     6:CHO  ");		
			Module_LCD_string(0, 1, mast.textlcd);
			setflag(F_KEYBD_READ_OK); // able read key board
			auto_off('Z', TM_500mS);
		
		// for lCD test
		//UART_WRITE(UART_LCD, 0xC7); //
		
			break;
		
		case 3:// xo game and cho
			if(isflag(F_ADC_BUTTON)){
				//if(run.key_now != run.key_old){
					//run.key_old = run.key_now;
					auto_off('Z', TM_200mS);
					Module_LCD_string(0, 2, "");				
					UART_WRITE(UART_LCD, run.key_now);
					if(run.key_now == '4') run.gamemode_sw = GM_XOGAME * 10;  // XO
					if(run.key_now == '6') run.gamemode_sw = GM_CHOSUNG	* 10; //GM_CHOSUNG * 10;  // cho
				//}
				time0_delay(TM_200mS);
				clrflag(F_ADC_BUTTON);				
			}	
			break;		
			
		// *************   Chosung Game  ****************
		case 30 : //chosung game Start, ini
			run.gamemode_sw++;// next		
			UART_WRITE(UART_LCD, 0xC0); // 1100 0000 command format : all Screen Clear
			//strcpy(mast.textlcd, "4:Attend 5:KEYBD");		
			strcpy(mast.textlcd, "7:Attend #:Next ");		
			Module_LCD_string(0, 1, mast.textlcd);
		
			// send game mode mast ==> student
			run_function(FUC_CHOSUNG);
			time0_delay(TM_500mS);	

			setflag(F_KEYBD_READ_OK); // able read key board
			auto_off('Z', TM_500mS);				
		break;

		case 31:// check
			if(isflag(F_ADC_BUTTON)){
				auto_off('Z', TM_200mS);
				Module_LCD_string(0, 2, "");				
				UART_WRITE(UART_LCD, run.key_now);
				if(run.key_now == '7') {run_function(FUC_ATTEND);}// run.gamemode_sw = 10;}
				if(run.key_now == '#') {run.gamemode_sw++;}// next
				time0_delay(TM_200mS);
				clrflag(F_ADC_BUTTON);				
			}	
			break;
			
		case 32: // chosung game again start 
			run_function(FUC_CHOSUNG); // CMD_GAMEMODE
			run.gamemode_sw++;// next		game mode
		
			UART_WRITE(UART_LCD, LCD_SCREEN_CLR); // all Screen Clear
		
			strcpy(mast.textlcd, "   student:     ");		
					
			ucpass = run.howmanystudent/10;
			mast.textlcd[11] = ucpass + 0x30;			
			ucpass = (run.howmanystudent - (ucpass*10));
			mast.textlcd[12] = ucpass + 0x30;			
			Module_LCD_string(0, 1, mast.textlcd);
		
			strcpy(mast.textlcd, "N:");		
			Module_LCD_string(0, 2, mast.textlcd);				
		
			setflag(F_KEYBD_READ_OK); // able read key board
			auto_off('Z', TM_500mS);				
			break;
		
		
		case 33:// key in ini
			run.gamemode_sw++;// next		game mode
			setflag(F_KEYBD_READ_OK); // able read key board
			run.key_lcd_position = 2;
			run.ucpass=0;
			run.while_event = 1; 
			break;
		
		case 34:// chosung key in and display
			while(run.while_event){	
				cpass = run_processor_keyin_ret_ascii_chosung();
				if(cpass == '#'){ // exit
					run.gamemode_sw++;// next	
					run.while_event = 0;
					break;					
				}
				else if(cpass){ // key in
					if(cpass == '*'){ // key back
						run.key_lcd_position--;						
						run.ucpass--;
					}
					else{
						run.student_chosung_font[run.ucpass++] = run_processor_chosung_to_number(cpass);		
						Module_LCD_onebyte(run.key_lcd_position++, 2, cpass);
						
					}
				}
				
				if (isflag(F_IN15_BUTTON)){		// key board Calibration
					run.keyboard_adc = adc_read(ADC_KEY_CH, 32);// 2901; by jtag no KEY
					run_keyboard_define(run.keyboard_adc);
					// Hardware, OUT
					LEDRF(1);
					LEDBF(1);
					BUZZERF(1);
					time0_delay(TM_1000mS);
					LEDRF(0);
					LEDBF(0);
					BUZZERF(0);
					clrflag(F_IN15_BUTTON);
				}	
			}
		break;
		
		case 35:// display 					
			run.gamemode_sw++;// next		
			UART_WRITE(UART_LCD, 0xC0); // 1100 0000 command format : all Screen Clear
			//strcpy(mast.textlcd, "4:Attend 5:KEYBD");		
			strcpy(mast.textlcd, "7:Start  #:Again");		
			Module_LCD_string(0, 1, mast.textlcd);
			strcpy(mast.textlcd,"*:Main ");
			Module_LCD_string(9, 2, mast.textlcd);
			auto_off('Z', TM_500mS);		
			clrflag(F_ADC_BUTTON);
			setflag(F_KEYBD_READ_OK); // touch ON
			break;
		
		case 36: // send chosung random
					//run.howmanystudent
	//					run.student_chosung_font[run.key_lcd_position] = run_processor_chosung_to_number(cpass);		
				while(1){		
					if(isflag(F_ADC_BUTTON)){
						//run.gamemode_sw++;// result next		
						auto_off('Z', TM_200mS);
						Module_LCD_string(0, 2, "");				
						UART_WRITE(UART_LCD, run.key_now);
						if(run.key_now == '7'){ //  send infor
							run_function(FUC_CHOSUNG_FONT); // 
						}

						if(run.key_now == '#'){// 
							run.gamemode_sw = 30;// again game
							break;
						
						}
						if(run.key_now == '*'){// 
							run.gamemode_sw = 2;// GO main
							break;
						
						}
						time0_delay(TM_200mS);
						clrflag(F_ADC_BUTTON);				
					}	
				}
			break;		
		// END    *************   Chosung Game  ****************			
		
		// *************   X O Game  ****************
		case 10 : //X O Game Start, ini
			run.gamemode_sw++;// next		
			UART_WRITE(UART_LCD, 0xC0); // 1100 0000 command format : all Screen Clear
			//strcpy(mast.textlcd, "4:Attend 5:KEYBD");		
			strcpy(mast.textlcd, "7:Attend #:Next ");		
			Module_LCD_string(0, 1, mast.textlcd);
		
			// send game mode mast ==> student
			run_function(FUC_XOGAME);
			time0_delay(TM_500mS);	

			setflag(F_KEYBD_READ_OK); // able read key board
			auto_off('Z', TM_500mS);
		
			break;

		case 11:// check
			if(isflag(F_ADC_BUTTON)){
				auto_off('Z', TM_200mS);
				Module_LCD_string(0, 2, "");				
				UART_WRITE(UART_LCD, run.key_now);
				if(run.key_now == '7') {run_function(FUC_ATTEND);}// run.gamemode_sw = 10;}
				if(run.key_now == '#') {run.gamemode_sw++;}// next
				time0_delay(TM_200mS);
				clrflag(F_ADC_BUTTON);				
			}	
			break;
			
		case 12: // XO game again start 
			run.gamemode_sw++;// next		
			UART_WRITE(UART_LCD, 0xC0); // 1100 0000 command format : all Screen Clear
			//strcpy(mast.textlcd, "4:Attend 5:KEYBD");		
			strcpy(mast.textlcd, "4:10sec  6:20sec");		
			Module_LCD_string(0, 1, mast.textlcd);
		
			setflag(F_KEYBD_READ_OK); // able read key board
			auto_off('Z', TM_500mS);				
			break;
		
		case 13:// time out send
			if(isflag(F_ADC_BUTTON)){
				auto_off('Z', TM_200mS);			
				Module_LCD_string(0, 2, "");				
				UART_WRITE(UART_LCD, run.key_now);
				
				if(run.key_now == '*'){ // system reset			
					run_function(FUC_REBOOT); // Reset
				}
					
				
				if(run.key_now == '4'){
					run.timeout_sec = run.senddata[0] = 10; //10 sec
					setflag(F_TIMEOUT_CHK_ING); // time out start					
					run_function(FUC_TIMEOUTTIME); // send all
					run.gamemode_sw++;// next
				}
					
				if(run.key_now == '6'){
					run.timeout_sec = run.senddata[0] = 20; //20 sec
					setflag(F_TIMEOUT_CHK_ING); // time out start
					run_function(FUC_TIMEOUTTIME); // send all
					run.gamemode_sw++;// next	
				}
				time0_delay(TM_200mS);
				clrflag(F_ADC_BUTTON);								
			}	
			break;		
			
		case 14:// display remain time
			run.gamemode_sw++;// next		
			UART_WRITE(UART_LCD, 0xC0); // 1100 0000 command format : all Screen Clear
			//strcpy(mast.textlcd, "4:Attend 5:KEYBD");		
			strcpy(mast.textlcd, "7:X 9:O  #:Again");		
			Module_LCD_string(0, 1, mast.textlcd);
		
			//setflag(F_KEYBD_READ_OK); // able read key board
			auto_off('Z', TM_500mS);		
			clrflag(F_KEYBD_READ_OK); // no touch
			
			break;
		
		case 15:
			//run.timeout_sec , unsigned long timeout_sec;// timeout
			

			if(ulpass != run.timeout_sec){
				ulpass = run.timeout_sec; // copy 
				//strcpy(mast.textlcd, "   student:     ");		
				strcpy(mast.textlcd, "   T:           ");		
				Module_LCD_string(0, 2, mast.textlcd);

			
				ucpass = run.timeout_sec/10;
				mast.textlcd[5] = ucpass + 0x30;
				
				ucpass = (run.timeout_sec - (ucpass*10));
				mast.textlcd[6] = ucpass + 0x30;
				
				Module_LCD_string(0, 2, mast.textlcd);
			}		
		
			if(isflag(F_TIMEOUT_OVER)){
				clrflag(F_TIMEOUT_OVER);
				setflag(F_KEYBD_READ_OK); // able read key board
				strcpy(mast.textlcd,"*:Main ");
				Module_LCD_string(9, 2, mast.textlcd);
			}
		
			if(isflag(F_ADC_BUTTON)){
				//run.gamemode_sw++;// result next		
				auto_off('Z', TM_200mS);
				Module_LCD_string(0, 2, "");				
				UART_WRITE(UART_LCD, run.key_now);
				if(run.key_now == '7'){ // X
					//if(run.student_OX_check == STU_X_BUTTON) strcpy(mast.textlcd, "  X:            ");		
					//if(run.student_OX_check == STU_O_BUTTON) strcpy(mast.textlcd, "  O:            ");	
					strcpy(mast.textlcd, "  X:            ");		
					Module_LCD_string(0, 2, mast.textlcd);		
					
					run.student_OX_check = STU_X_BUTTON;// 0x0F; // X
					run_function(FUC_READXO); // CMD_XO_REQ
				}
				if(run.key_now == '9'){ // O
					strcpy(mast.textlcd, "  O:            ");		
					Module_LCD_string(0, 2, mast.textlcd);		
					run.student_OX_check = STU_O_BUTTON;// 0xFB; // O
					run_function(FUC_READXO); // CMD_XO_REQ
				}				
				if(run.key_now == '#'){// again X O game 
					run.gamemode_sw = 12;// again X O 
				}
				if(run.key_now == '*'){// 
					run.gamemode_sw = 2;// GO main
						}
				time0_delay(TM_200mS);
				clrflag(F_ADC_BUTTON);				
			}	
			break;
	}// end switch
}

void run_function(unsigned char func){
	unsigned char number;
	int equal = 0;
	int person = 0;
	int seed= timer0tick;
	srand(seed);
	person =run.howmanystudent;
	int * random;
	random = (int*)malloc(sizeof(int)*person);
	switch(func){	
		case FUC_REBOOT :
			// Hardware Reset
			SYS_UnlockReg();
			SYS_ResetCPU();// Reset CPU 
			free(random);	
		break;
		
		case FUC_TIMEOUTTIME :
			// send time out all student time out time SEC
			run_student_send(RF_ALLSTUDENT, CMD_TIME_OVER, 1); // all send game mode			
			free(random);
			break;
		
		case FUC_CHOSUNG :
			run.senddata[0] = GM_CHOSUNG;//chosung game mode
			run_student_send(RF_ALLSTUDENT, CMD_GAMEMODE, 1); // all send game mode
			free(random);
		break;
		
		case FUC_CHOSUNG_FONT : // 0x77		
			
			for(number = 0; number < run.howmanystudent; number++){
				random[number]=rand()%run.howmanystudent;
				for(equal=0 ; equal<number ; equal++ ){
					if(random[number]==random[equal]) {
						number--;
						break;
					}
				}
				run.senddata[0] = run.student_chosung_font[number]; //숫자값
				run_student_send(run.student_attend[random[number]], CMD_CHOSUNG_FONT, 1); // 1 byte font each address game font
				time0_delay(TM_100mS);
				
			}
		free(random);		
		break;		

		case FUC_XOGAME :
			run.senddata[0] = GM_XOGAME;//0x01;//Xo game mode
			run_student_send(RF_ALLSTUDENT, CMD_GAMEMODE, 1); // all send game mode
			free(random);
		break;

		case FUC_READXO : // X O result
		
			if(run.howmanystudent == 0) run.howmanystudent = MAX_ATTEND;
		
			for(ucpass=0; ucpass < run.howmanystudent; ucpass++){ // ping test and attend
				
				run_student_send(run.student_attend[ucpass], CMD_XO_REQ, 0); // CMD_XO_REQ
				
				time.com_check_rf = TM_200mS; // wait time
				setflag(F_STU_READ_TIMECHK);
				run.student_att_exit = 1; // endless loop exit timer
				while(run.student_att_exit){
					rf_3355_mast_read(); // 학생보드에서 OX결과 등 데이터를 보내면 읽는 함수
					if(isflag(F_RF_COM_OK)){
						clrflag(F_RF_COM_OK);
						if(run.student_OX_check == mast.readdata[0]){ // display
							number = run.student_attend[ucpass]/10;
							mast.textlcd[4] = number + 0x30;
							number = (run.student_attend[ucpass] - (number*10));
							mast.textlcd[5] = number + 0x30;
						
							Module_LCD_string(0, 2, mast.textlcd);		
							
							time0_delay(TM_1000mS); // display time
							time0_delay(TM_1000mS); // display time
						}
					
						clrflag(F_STU_READ_TIMECHK);
						run.student_att_exit = 0;//break;
						
					}
			
				}// end while
			}// end for
			strcpy(mast.textlcd,"*:Main ");
			Module_LCD_string(9, 2, mast.textlcd);	
			free(random);
		break;	
			
		case FUC_ATTEND : // unsigned char student_attend[MAX_ATTEND];// MAX_ATTEND, #define MAX_ATTEND		20// 최대 학생수, 참가자 출석표 작성
			run.howmanystudent =0;
			for(ucpass=0; ucpass < MAX_ATTEND; ucpass++){ // ping test and attend
				run_student_send(ucpass+1, CMD_PING, 0); // 1~20(0~19)
				time.com_check_rf = TM_200mS; // wait time
				setflag(F_STU_READ_TIMECHK);
				run.student_att_exit = 1; // endless loop exit timer
				while(run.student_att_exit){
					rf_3355_mast_read(); // 학생보드에서 OX결과 등 데이터를 보내면 읽는 함수
					if(isflag(F_RF_COM_OK)){
						clrflag(F_RF_COM_OK);
						run.student_attend[run.howmanystudent] = mast.add; // student address copy
						run.howmanystudent++;
						clrflag(F_STU_READ_TIMECHK);
						run.student_att_exit = 0;//break;
						
					}			
				}// end while
			}// end for
			//strcpy(mast.textlcd, "4:Attend 5:KEYBD");		
			  strcpy(mast.textlcd, "   student:     ");		
					
			ucpass = run.howmanystudent/10;
			mast.textlcd[11] = ucpass + 0x30;
			
			ucpass = (run.howmanystudent - (ucpass*10));
			mast.textlcd[12] = ucpass + 0x30;
			
			Module_LCD_string(0, 2, mast.textlcd);		
			free(random);
		break;	
		
		case FUC_KEY_CAL :
			UART_WRITE(UART_LCD, 0xC0); // 1100 0000 command format : all Screen Clear
			//strcpy(mast.textlcd, "4:Attend #:Next ");		
			strcpy(mast.textlcd, "Do not touch KEY");		
			Module_LCD_string(0, 2, mast.textlcd);
			time0_delay(TM_500mS);
			LEDRF(1);
			LEDBF(1);
			BUZZERF(1);
			time0_delay(TM_500mS);
			LEDRF(0);
			LEDBF(0);
			BUZZERF(0);				
		
			run.keyboard_adc = adc_read(ADC_KEY_CH, 32);// 2901; by jtag no KEY
			run_keyboard_define(run.keyboard_adc);
			free(random);
			break;		
		
	}
	
}

// Project Function ====================================
void run_keyboard_define(unsigned short adc_nokey){
	
	//0key 
	run.KEY[1] = (float)adc_nokey * (270.0 / (22.0 + 270.0)); // ex) no key ~ 1 KEY 2933 * 0.9 = 2720(1 key ADC)
	run.KEY[0] = adc_nokey - ((adc_nokey - run.KEY[1]) / 2); // NO KEY 2933 - (2933 - (1 key ADC))/2 
	
	// 1key
	run.KEY[2] = (float)adc_nokey * (200.0 / (22.0 + 200.0)); // ==> 2key ex) 1 Key ~ 2 key
	run.KEY[1] = run.KEY[1] - ((run.KEY[1] - run.KEY[2]) / 2); // (k1-k2)/2

	// 2key
	run.KEY[3] = (float)adc_nokey * (150.0 / (22.0 + 150.0)); // ==> +1
	run.KEY[2] = run.KEY[2] - ((run.KEY[2] - run.KEY[3]) / 2); // over this key
	
	// 3key
	run.KEY[4] = (float)adc_nokey * (100.0 / (22.0 + 100.0)); // ==> +1
	run.KEY[3] = run.KEY[3] - ((run.KEY[3] - run.KEY[4]) / 2); // over this key	
	
	// 4key
	run.KEY[5] = (float)adc_nokey * (66.0 / (22.0 + 66.0)); // ==> +1
	run.KEY[4] = run.KEY[4] - ((run.KEY[4] - run.KEY[5]) / 2); // over this key	
	
	// 5key
	run.KEY[6] = (float)adc_nokey * (47.0 / (22.0 + 47.0)); // ==> +1
	run.KEY[5] = run.KEY[5] - ((run.KEY[5] - run.KEY[6]) / 2); // over this key	
	
	// 6key
	run.KEY[7] = (float)adc_nokey * (32.0 / (22.0 + 32.0)); // ==> +1
	run.KEY[6] = run.KEY[6] - ((run.KEY[6] - run.KEY[7]) / 2); // over this key		
	
	// 7key
	run.KEY[8] = (float)adc_nokey * (22.0 / (22.0 + 22.0)); // ==> +1
	run.KEY[7] = run.KEY[7] - ((run.KEY[7] - run.KEY[8]) / 2); // over this key	

	// 8key
	run.KEY[9] = (float)adc_nokey * (14.7 / (22.0 + 14.7)); // ==> +1
	run.KEY[8] = run.KEY[8] - ((run.KEY[8] - run.KEY[9]) / 2); // over this key		
	
	// 9key
	run.KEY[10] = (float)adc_nokey * (10.0 / (22.0 + 10.0)); // ==> +1
	run.KEY[9] = run.KEY[9] - ((run.KEY[9] - run.KEY[10]) / 2); // over this key			
	
	// 10key
	run.KEY[11] = (float)adc_nokey * (6.7 / (22.0 + 6.7)); // ==> +1
	run.KEY[10] = run.KEY[10] - ((run.KEY[10] - run.KEY[11]) / 2); // over this key			
	
	// 11key
	run.KEY[12] = (float)adc_nokey * (4.7 / (22.0 + 4.7)); // ==> +1
	run.KEY[11] = run.KEY[11] - ((run.KEY[11] - run.KEY[12]) / 2); // over this key			
	
	run.KEY[12] = run.KEY[12] - ((run.KEY[11] - run.KEY[12])/2);
}

char run_processor_keyin_ret_ascii_chosung(void){
	char uc_ascii;
	unsigned char sw_no = 0;
	unsigned long keyin_wait_time;
	unsigned char con_wh;
	//unsigned char check_zero_button;
	
	if(isflag(F_ADC_BUTTON)){
		auto_off('Z', TM_200mS);
		if(run.key_now == '0'){
			time0_delay(TM_300mS);	
			clrflag(F_ADC_BUTTON);
			keyin_wait_time = TM_2000mS;
			timer0tick = 0; // time count ini
			con_wh = 1;
			sw_no = 10;
			while(con_wh){
				if(timer0tick > keyin_wait_time) {// nothing
					con_wh = 0; // exit
					sw_no = 0; 
					break;
				}
				if(isflag(F_ADC_BUTTON)){
					auto_off('Z', TM_200mS);
					if(run.key_now == '*') {
						sw_no = 30;
					}
					else if(run.key_now == '#') {
						sw_no = 40;
					}
					else{
						sw_no += run.key_now - '0'; // 10(0), 11 ~ 19   1    '*' 
					}
					clrflag(F_ADC_BUTTON);
					con_wh = 0; // exit
					break;
				}
			}
			
		}
		else if(('1' <= run.key_now) && (run.key_now <= '9')){
			sw_no = run.key_now - 0x30; // just number  0x30은 아스키코드로 '0'
		}
		else if(run.key_now == '*'){
			sw_no = 30;
		}
		else if(run.key_now == '#'){
			sw_no = 40;
		}

		time0_delay(TM_300mS);	
		clrflag(F_ADC_BUTTON);		
	}
	
	switch(sw_no){
		case 0:	uc_ascii = 0;		break; // no key board
		case 1:	uc_ascii = 'A';	break;		
		case 2:	uc_ascii = 'a';	break;		
		case 3:	uc_ascii = 'B';	break;		
		case 4:	uc_ascii = 'C';	break;		
		case 5:	uc_ascii = 'c';	break;		
		case 6:	uc_ascii = 'D';	break;		
		case 7:	uc_ascii = 'E';	break;		
		case 8:	uc_ascii = 'F';	break;		
		case 9:	uc_ascii = 'f';	break;		
		case 10:	uc_ascii = 'G';	break;		
		case 11:	uc_ascii = 'g';	break;		
		case 12:	uc_ascii = 'H';	break;		
		case 13:	uc_ascii = 'J';	break;		
		case 14:	uc_ascii = 'j';	break;		
		case 15:	uc_ascii = 'K';	break;		
		case 16:	uc_ascii = 'L';	break;		
		case 17:	uc_ascii = 'M';	break;		
		case 18:	uc_ascii = 'N';	break;		
		case 19:	uc_ascii = 'O';	break;		
		case 30:	uc_ascii = '*';	break;		
		case 40:	uc_ascii = '#';	break;		
	}
	return uc_ascii;
}

unsigned char run_processor_chosung_to_number(char chosung_ascii){
	unsigned char ucret_num;
	
	switch(chosung_ascii){
		case 0:		ucret_num = 0;	break; // no key board
		case 'A':	ucret_num = 1;	break;		
		case 'a':	ucret_num = 2;	break;		
		case 'B':	ucret_num = 3;	break;		
		case 'C':	ucret_num = 4;	break;		
		case 'c':	ucret_num = 5;	break;		
		case 'D':	ucret_num = 6;	break;		
		case 'E':	ucret_num = 7;	break;		
		case 'F':	ucret_num = 8;	break;		
		case 'f':	ucret_num = 9;	break;		
		case 'G':	ucret_num = 10;	break;		
		case 'g':	ucret_num = 11;	break;		
		case 'H':	ucret_num = 12;	break;		
		case 'J':	ucret_num = 13;	break;		
		case 'j':	ucret_num = 14;	break;		
		case 'K':	ucret_num = 15;	break;		
		case 'L':	ucret_num = 16;	break;		
		case 'M':	ucret_num = 17;	break;	
		case 'N':	ucret_num = 18;	break;	
		case 'O':	ucret_num = 19;	break;	
	}
	return ucret_num;	
}

char run_keyboard_adc_return(unsigned short adc_key){
	char key_information = 0;

	if (adc_key < run.KEY[0]){ // key in
		if (adc_key > run.KEY[1])		return ('1');
		if (adc_key > run.KEY[2])		return ('2');
		if (adc_key > run.KEY[3])		return ('3');
		if (adc_key > run.KEY[4])		return ('4');
		if (adc_key > run.KEY[5])		return ('5');
		if (adc_key > run.KEY[6])		return ('6');
		
		if (adc_key > run.KEY[7])		return ('7');
		if (adc_key > run.KEY[8])		return ('8');
		if (adc_key > run.KEY[9])		return ('9');
		if (adc_key > run.KEY[10])		return ('*');
		if (adc_key > run.KEY[11])		return ('0');
		if (adc_key > run.KEY[12])		return ('#');
	}
	return key_information;
}

void run_student_send(unsigned char add, unsigned char cmd, unsigned char datalen)
{
	unsigned char senco;
	unsigned char sendsum;

	UART_WRITE(UART_RF, RF_MAST_PREAMBLE); // start  1

	sendsum = add;			  // address
	UART_WRITE(UART_RF, add); // 2

	sendsum += cmd;			  // command
	UART_WRITE(UART_RF, cmd); // 3

	for (senco = 0; senco < datalen; senco++)
	{ // if datalen is zero not use
		sendsum += run.senddata[senco];
		UART_WRITE(UART_RF, run.senddata[senco]); // data
	}
	UART_WRITE(UART_RF, sendsum); // 4

	UART_WRITE(UART_RF, TAIL); // 5
}

unsigned char run_student_receive_checksum(unsigned char datalen)
{
	unsigned char ret_sum;
	unsigned char i;

	ret_sum = mast.add;		 // 1
	ret_sum += mast.command; // 2
	for (i = 0; i < datalen; i++)
	{ // if datalen is zero not use
		ret_sum += mast.readdata[i];
	}
	return ret_sum;
}

unsigned char check_pop(void)
{
	if (rf.wp != rf.rp)
	{
		mast.get_char = (rf.buffer[rf.rp]);
		rf.rp++;
		if (rf.rp > QSIZE)
			rf.rp = 0; // Read ��������
		return 1;
	} // end if
	return 0;
} // end check_pop


void rf_3355_mast_read(void){
	if(check_pop()){
		setflag(F_RF_TIME);
		mast.comtimeout = 0; // @1 sec
		switch (mast.sequence)
		{
		case 0:
			if (mast.get_char == RF_STUDENT_PREAMBLE) { // header 1 // 0x80
				mast.sequence += 2;
			}
			break;
			
		case 2: // 
			mast.add = mast.get_char;
			mast.sequence += 2;	
			break;
		
		case 4:
			mast.command = mast.get_char; // cmd
			if ((mast.command == (CMD_PING | RF_OR0x80)) || (mast.command == (CMD_XO_REQ | RF_OR0x80))){ // 1byte
				mast.len = 1;
				mast.count = 0; // real data Start @sens_data
				mast.sequence += 2;
			}
			else
			{ // no data
				mast.len = 0;
				mast.sequence = 50; // end
			}
			break;

		case 6:
			mast.readdata[mast.count++] = mast.get_char; //
			if (mast.count >= mast.len)
			{						// 1 >=
				mast.sequence = 50; // end
			}
			break;

		case 50: // check sum

			mast.sum = mast.get_char;
			mast.sequence = 54;
			break;
		case 54: // 0D check
			if (mast.get_char == TAIL){
				if (mast.sum == run_student_receive_checksum(mast.len))
				{
					setflag(F_RF_COM_OK);
				}
			}
			clrflag(F_RF_TIME);
			mast.sequence = 0; // home
			break;
		}
	}
}
