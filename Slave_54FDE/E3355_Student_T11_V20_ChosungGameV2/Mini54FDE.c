/* Purpose : 32Bit Cortex-M0 54FDE Project
		Target Name : 3355-T11
		Compiler : MDK ARM Compiler
		CMSIS : Mini51DE_CMSIS
		Document : xxxx.txt
		Author : Conan KIM
---------------------------------------------
Copyright by FirmwareBank Inc,. All Rights Reserved. */

#include "mini54FDE.h"

struct ring
{
	volatile unsigned long wp;
	volatile unsigned long rp;
	volatile unsigned char buffer[QSIZE];
};
struct ring q; // for ring buffer

struct time_value
{
	volatile unsigned long ledblue_off;
	volatile unsigned long buzzer_off;
};
struct time_value time;

struct com_uart
{
	volatile unsigned long comtimeout;
	volatile unsigned long tail;
	volatile unsigned char get_char;
	volatile unsigned char sequence;
	volatile unsigned char count;
	volatile unsigned char len;
	volatile unsigned char sum;
	unsigned char readdata[10];

	volatile unsigned char trash; // pc에서 시리얼통신시 발생하는 쓰레기값을 처리하기 위함
	volatile unsigned char command;
	volatile unsigned char add;
	volatile unsigned char preamble; // PC와 마스터보드의 헤더파일을 구분하기 위함
};
struct com_uart rf;

struct project{
	volatile unsigned char gamemode, gamemode_sw;
	unsigned char senddata[10];
	
	unsigned char buzzer_count, buzzer_count_temp;
	unsigned char buzzer_switch;
	unsigned long buzzer_ontime, buzzer_ontime_temp;
	unsigned long buzzer_offtime, buzzer_offtime_temp;

	unsigned char dot_count, dot_count_temp;
	unsigned char dot_switch;
	unsigned long dot_ontime, dot_ontime_temp;
	unsigned long dot_offtime, dot_offtime_temp;
	
	unsigned char student_infoANDbutton, student_board_info; 
	volatile unsigned long student_overtime_ms; // time limot
	unsigned char student_touch_toggle;
	unsigned char student_rf_address;
	unsigned char student_font;// display font number 1~13
};
struct project run;
				
// Project
unsigned long runflag;

volatile unsigned long timer0tick, timer1tick; // Timer Value.
uint8_t i;

#define MODULE_COUNT 9		  // Dot LED개수

volatile uint8_t FrameBuffer[MODULE_COUNT][8];

// for test
 unsigned long ulpass;
 unsigned char ucpass;

int main(void)// Start Main.
{ 
	setup();
	runflag = 0; // initial

	//time0_delay(TM_500mS);
	//LEDBF(0);

	// Com ini
	UART_Open(UART, 9600);
	ring_ini();

	// Hardware ini,
	RFCONT(0); // RF SET Pin is HIGH then Communication Mode
	run.student_rf_address = run_read_myaddress();

	// dot ini
	max7219_ini();
	for (i = 0; i < 8; i++)
	{ // all led off
		max7219_ch_write(1, i + 1, 0x00);
	}	
	NULL_Display();// clear

	// Software ini
	//run.gamemode_sw = run.gamemode = GM_XOGAME * 10; // for test
	run.gamemode_sw = run.gamemode = 0; // 
	
	run.student_board_info = 0x0A;// Default, Ping test
	run.student_infoANDbutton = STUDENT_NO;//0xB0;// No button infor
	
//	// font test 
//	ucpass=1;
//	while(233){
//		if(isflag(F_TOUCH_ON)){
//			auto_off('Z', TM_20mS);
//			run_chosung_font_Display(ucpass);
//			time0_delay(TM_500mS);
//			ucpass++;
//			if(ucpass > 19) ucpass=1;
//			clrflag(F_TOUCH_ON);
//		}	
//	}
		
	while (1){
		
		run_game_mode(); // case run.gamemode_sw
			// game mode @switch
			//	0~9: default
			//	10~29: XO 
			//	30~49: KOREAN_GAME_ON

		rf_3355_student_read();
		if (isflag(F_RF_COM_OK)){ //
			auto_off('B', TM_20mS);
			rf_student_processor(rf.command);
			// auto_off('Z', TM_100mS);
			clrflag(F_RF_COM_OK);
		}
	}
} // end main

// project function
void setup(void)
{
	SYS_UnlockReg();
	CLK_EnableXtalRC(CLK_PWRCON_OSC22M_EN_Msk);
	CLK_WaitClockReady(CLK_CLKSTATUS_IRC22M_STB_Msk);
	CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_IRC22M, CLK_CLKDIV_HCLK(1)); //

	// Enable IP clock
	CLK_EnableModuleClock(UART_MODULE);
	CLK_EnableModuleClock(TMR0_MODULE);
	CLK_EnableModuleClock(TMR1_MODULE); //
	// CLK_EnableModuleClock(ADC_MODULE);
	CLK_SetModuleClock(UART_MODULE, CLK_CLKSEL1_UART_S_IRC22M, CLK_CLKDIV_UART(1));
	CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0_S_IRC22M, 0); //
	CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1_S_IRC22M, 0); //
																   // CLK_SetModuleClock(ADC_MODULE, CLK_CLKSEL1_ADC_S_IRC22M,CLK_CLKDIV_ADC(6));

	SystemCoreClockUpdate();
	SYS_LockReg();

	SYS->P0_MFP = 0;
	SYS->P1_MFP = SYS_MFP_P13_TXD | SYS_MFP_P12_RXD;
	SYS->P2_MFP = 0x00000000;
	SYS->P2_MFP = 0;
	SYS->P3_MFP = 0;
	SYS->P4_MFP = SYS_MFP_P47_ICE_DAT | SYS_MFP_P46_ICE_CLK;
	SYS->P5_MFP = 0;

	// LED
	GPIO_SetMode(P0, BIT6, GPIO_PMD_OUTPUT);

	// BUZZER
	GPIO_SetMode(P0, BIT7, GPIO_PMD_OUTPUT);

	// Dot LED
	GPIO_SetMode(P3, BIT5 | BIT4, GPIO_PMD_OUTPUT);
	GPIO_SetMode(P5, BIT1 | BIT0, GPIO_PMD_OUTPUT);

	// RF CONT
	GPIO_SetMode(P1, BIT4, GPIO_PMD_OUTPUT);

	// Button
	GPIO_SetMode(P2, BIT5, GPIO_PMD_QUASI); // Touch
	//GPIO_SetMode(P0, BIT4, GPIO_PMD_INPUT);

	// Address
	GPIO_SetMode(P1, BIT5, GPIO_PMD_QUASI);
	GPIO_SetMode(P3, BIT2, GPIO_PMD_QUASI);
	GPIO_SetMode(P2, BIT4, GPIO_PMD_QUASI);
	GPIO_SetMode(P0, BIT5, GPIO_PMD_QUASI);

	TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 100); // 10mS, 0.01 Sec
	TIMER_EnableInt(TIMER0);
	NVIC_EnableIRQ(TMR0_IRQn); // TMR0_IRQHandler
	TIMER_Start(TIMER0);

	TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 1); // 1S
	TIMER_EnableInt(TIMER1);
	NVIC_EnableIRQ(TMR1_IRQn); // TMR0_IRQHandler
	TIMER_Start(TIMER1);

	// UART Interrupt
	UART_ENABLE_INT(UART, (UART_IER_RDA_IEN_Msk | UART_IER_RTO_IEN_Msk));
	NVIC_EnableIRQ(UART_IRQn);

}

void time0_delay(unsigned long T0)
{ // 1 is 10ms
	timer0tick = 0;
	while (timer0tick < T0);
}
void time1_delay(unsigned long T1)
{ // 1 is 1S
	timer1tick = 0;
	while (timer1tick < T1);
}

// 888888888888888888888888888888888 Interrupt

void UART_IRQHandler(void)
{
	uint32_t u32IntSts = UART->ISR;

	if (u32IntSts & UART_ISR_RDA_INT_Msk)
	{
		while (!UART_GET_RX_EMPTY(UART))
		{
			q.buffer[q.wp] = UART_READ(UART);
			q.wp++;
			if (q.wp > QSIZE)
				q.wp = 0;
		}
	}
}

void TMR0_IRQHandler(void){ // Timer0, 10ms
	TIMER_ClearIntFlag(TIMER0);
	timer0tick++;
				
	if(isflag(F_OVERTIME_CHECK)){
		run.student_overtime_ms--;
		if(run.student_overtime_ms == 300){
			setflag(F_TIME_WARN);
			
		}
		if(run.student_overtime_ms == 0){
			setflag(F_TIME_OVER);
			clrflag(F_OVERTIME_CHECK);
		}
	}

	if (isflag(F_RUN_DOT_ACTION))
	{ // dot LED ON
		switch (run.dot_switch)
		{
		case 0:						 // ini
			setflag(F_RUN_DOT_ON);	 // dot display ON
			run.dot_ontime_temp = 1; // run.ontime;
			run.dot_count_temp = 1;	 // ini
			run.dot_switch = 1;		 // next
			break;

		case 1: // DOT ON HIGH
			if (run.dot_ontime_temp++ >= run.dot_ontime)
			{							// over
				setflag(F_RUN_DOT_OFF); // off dot
				if (run.dot_count_temp++ >= run.dot_count)
				{
					clrflag(F_RUN_DOT_ACTION);
					break;
				}
				else
				{
					run.dot_switch = 3; // next
					run.dot_offtime_temp = 0;
				}
			}
			break;

		case 3: // DOT Low
			if (run.dot_offtime_temp++ >= run.dot_offtime)
			{
				run.dot_switch = 1;		 // high
				run.dot_ontime_temp = 0; //
				setflag(F_RUN_DOT_ON);	 // dot display ON
			}
			break;
		}
	}

	if (isflag(F_RUN_BUZ_ACTION))
	{ // sound on
		switch (run.buzzer_switch)
		{
		case 0:							// ini
			BUZZER(1);					// ON
			run.buzzer_ontime_temp = 1; // run.buzzer_ontime;
			run.buzzer_count_temp = 1;	// ini
			run.buzzer_switch = 1;		// next
			break;

		case 1: // BUZZER ON HIGH
			if (run.buzzer_ontime_temp++ >= run.buzzer_ontime)
			{			   // over
				BUZZER(0); // OFF
				if (run.buzzer_count_temp++ >= run.buzzer_count)
				{
					clrflag(F_RUN_BUZ_ACTION);
					break;
				}
				else
				{
					run.buzzer_switch = 3; // next
					run.buzzer_offtime_temp = 0;
				}
			}
			break;

		case 3: // Buzzer Low
			if (run.buzzer_offtime_temp++ >= run.buzzer_offtime)
			{
				run.buzzer_switch = 1;		// high
				run.buzzer_ontime_temp = 0; //
				BUZZER(1);					// ON
			}
			break;
		}
	}

	//

	if (TOUCH_BUTTON == 0)	{ // TOUCH
		setflag(F_TOUCH_ON);
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

	if (isflag(F_BUZZER_AUTOOFF))
	{
		time.buzzer_off--;
		if (time.buzzer_off == 0)
		{
			BUZZER(0);
			clrflag(F_BUZZER_AUTOOFF);
		}
	}
}

void TMR1_IRQHandler(void){ // Timer1 1S
	TIMER_ClearIntFlag(TIMER1);
	timer1tick++;
	
	if(isflag(F_GAME_ING)){
		GPIO_TOGGLE(LEDBP);
	}

	if (isflag(F_RF_TIME))
	{
		rf.comtimeout++;
		if (rf.comtimeout > TM_3SEC)
		{					 // over time 3 SEC
			rf.sequence = 0; // ini
			clrflag(F_RF_TIME);
		}
	}

	//	auto_off('B', TM_100mS);
}
// 888888888888888888888888888888888 Interrupt

// Utility
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
	case 'Z': // Buzzer
	case 'z':
		setflag(F_BUZZER_AUTOOFF);
		time.buzzer_off = time_autooff;
		BUZZER(1);
		break;
	}
}

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

// Dot Function  ==========================================
void max7219_ch_write(uint8_t dot9, uint8_t addr, uint8_t value)
{
	switch (dot9)
	{
	case 1:
		MAX7219_CSORG = LOW_GPIO; // ChipSelect to LOW
		break;
	case 2:
		//	MAX7219_CS1 = LOW_GPIO; //ChipSelect to LOW
		break;
	}

	shift8out(addr);
	shift8out(value);

	switch (dot9)
	{
	case 1:
		MAX7219_CSORG = HIGH_GPIO; // ChipSelect to HIGH
		break;
	case 2:
		// MAX7219_CS2 = HIGH_GPIO;//ChipSelect to HIGH
		break;
	}
}

void shift8out(unsigned char data8)
{
	//unsigned long nop_delay = 1000; //원래 코드의 DotLED가 느렸던 이유 shift8out함수의 딜레이가 1초였었다
	//unsigned long nop_delay = 10;
	unsigned long nop_delay = 5; 
	unsigned char clock_count;

	MAX7219_CLK = LOW_GPIO;
	shift_delay(nop_delay);
	for (clock_count = 0; clock_count < 8; clock_count++)
	{
		if (data8 & (0x80 >> clock_count)) // 1000 0000
			MAX7219_DIN = HIGH_GPIO;
		else
			MAX7219_DIN = LOW_GPIO;

		MAX7219_CLK = HIGH_GPIO;
		shift_delay(nop_delay);
		MAX7219_CLK = LOW_GPIO;
		shift_delay(nop_delay);
	}
}

void shift_delay(unsigned long nopdelay)
{
	int i;
	int cal = 1;
	for (i = 0; i < nopdelay; i++)
	{
		cal = cal * 100;
	}
}

void max7219_ini(void)
{
	unsigned char i;

	for (i = 1; i <= 2; i++)
	{
		max7219_ch_write(i, DECODE_MODE_ADDR, DECODE_MODE_VAL);
		max7219_ch_write(i, INTENSITY_ADDR, INTENSITY_VAL);
		max7219_ch_write(i, SCAN_LIMIT_ADDR, SCAN_LIMIT_VAL);
		max7219_ch_write(i, POWER_DOWN_MODE_ADDR, POWER_DOWN_MODE_VAR);
		max7219_ch_write(i, TEST_DISPLAY_ADDR, TEST_DISPLAY_VAL);
	}
}

void max7219_ini_bright(unsigned char bright){
		max7219_ch_write(1, DECODE_MODE_ADDR, DECODE_MODE_VAL);
		max7219_ch_write(1, INTENSITY_ADDR, bright);
		max7219_ch_write(1, SCAN_LIMIT_ADDR, SCAN_LIMIT_VAL);
		max7219_ch_write(1, POWER_DOWN_MODE_ADDR, POWER_DOWN_MODE_VAR);
		max7219_ch_write(1, TEST_DISPLAY_ADDR, TEST_DISPLAY_VAL);
}

void MAX7219_MatrixSetRow64(uint8_t index, uint64_t rows) // 프레임버퍼에 패턴정보를 1바이트씩 저장합니다
{
	int i;
	for (i = 0; i < 8; i++)
	{
		uint8_t row = rows & 0xFF;
		FrameBuffer[index][7 - i] = row;
		rows = rows >> 8;
	}
}
bool MAX7219_MatrixUpdate()
{
	int index, digit;
	for (index = 0; index < MODULE_COUNT; index++) // MODULE_COUNT=9
	{
		for (digit = 0; digit < 8; digit++) // digit는 row줄을 나타냅니다
		{
			uint8_t row = FrameBuffer[index][digit];
			if (!MAX7219_Digit(index, digit, row))
				return false;
		}
	}

	return true;
}
bool MAX7219_Digit(uint8_t index, uint8_t digit, int8_t value)
{
	if (index >= MODULE_COUNT)
		return false;
	if (digit > 0x07)
		return false;

	MAX7219_Write(index, digit + 1, value);

	return true;
}
bool MAX7219_Write(uint8_t index, uint8_t addr, uint8_t data)
{
	// 매트릭스 순서대로 첫번째 매트릭스에 먼저 패턴정보를 띄우고 그외 매트릭스는 NO-OP모드
	// 2번 매트릭스에 패턴정보를 띄우기위해서 1,3,4,...,9번 매트릭스는 NO-OP모드 2번매트릭스는 패턴정보넣기
	// 이런식으로 각 매트릭스에 원하는 패턴정보를 출력합니다.
	int i;
	if (index >= MODULE_COUNT)
		return false;

	MAX7219_CSORG = LOW_GPIO;

	/* NOOP to following ic */
	for (i = index; i < MODULE_COUNT - 1; i++) //
	{
		shift8out(NO_OP_ADDR);

		shift8out(NO_OP_VAL);
	}

	/* Write register */
	shift8out(addr);

	shift8out(data);

	/* NOOP to previous ic */
	for (i = 0; i < index; i++)
	{
		shift8out(NO_OP_ADDR);
		shift8out(NO_OP_VAL);
	}

	MAX7219_CSORG = HIGH_GPIO;

	return true;
}

void run_dotDisplay(unsigned char led9, unsigned char allONOFF)
{
	if (allONOFF)
		max7219_ch_write(led9, TEST_DISPLAY_ADDR, 0x01); // Led all ON
	else
		max7219_ch_write(led9, TEST_DISPLAY_ADDR, TEST_DISPLAY_VAL); // led off
}
// Dot Function  ==========================================

// UART ------------------------------------------------------
unsigned char check_pop(void)
{
	if (q.wp != q.rp)
	{
		rf.get_char = (q.buffer[q.rp]);
		q.rp++;
		if (q.rp > QSIZE)
			q.rp = 0; // Read ��������
		return 1;
	} // end if
	return 0;
} // end check_pop

void ring_ini(void)
{
	q.rp = 0;
	q.wp = 0;
}

void putstring(unsigned char *pui8Buffer)
{
	unsigned short rcount;

	for (rcount = 0; pui8Buffer[rcount] != '\0'; rcount++)
	{
		if (pui8Buffer[rcount] == '\n')
		{
			UART_WRITE(UART, 0x0a);
			UART_WRITE(UART, 0x0d);
		}
		else
		{
			UART_WRITE(UART, pui8Buffer[rcount]);
		}
	}
}

void rf_3355_student_read(void){
	if (check_pop()){		
		setflag(F_RF_TIME);
		rf.comtimeout = 0;	// over time check @1 sec
		switch (rf.sequence){
			case 0:
				if (rf.get_char == MAST_PREAMBLE || rf.get_char == PC_PREAMBLE){ // header 1
					rf.preamble = rf.get_char;
					rf.sequence += 2;
				}
				break;
				
			case 2: // Same address
				rf.add = rf.get_char;
				if ((rf.add == run.student_rf_address ) || (rf.add == RF_ALLSTUDENT) ||(rf.add == RF_ALLSTUDENT_PC))
				{ // Student add or ALL ADD
					rf.sequence += 2;
				}
				else
				{
					rf.sequence = 0; // home
				}
				break;
				
			case 4:
				rf.command = rf.get_char; // cmd
				if (rf.command == 0x79)	{ // 2 byte
					rf.len = 2;
					rf.count = 0; // real data Start @sens_data
					rf.sequence += 2;
				}
				else if ((rf.command == 0x73) || (rf.command == 0x77) || (rf.command == 0x78) || (rf.command == 0x7A) || (rf.command == 0x7C) || (rf.command == 0x7D))
				{ // 1byte                                                                                     
					rf.len = 1;
					rf.count = 0; // real data Start @sens_data
					rf.sequence += 2;
				}
				else
				{ // no data
					rf.len = 0;
					rf.sequence = 50; // end
				}
				break;

			case 6: // len 0 is no data
				rf.readdata[rf.count++] = rf.get_char; //
				if (rf.count >= rf.len)
				{					  // 1 >=
					rf.sequence = 50; // end
				}
				break;

			case 50: // check sum
				rf.sum = rf.get_char;
				rf.sequence = 54;
				break;
			
			case 54: // 0D check
				if (rf.get_char == TAIL){
					if (rf.sum == rf_receive_checksum(rf.len)){
						setflag(F_RF_COM_OK);
					}
				}
				clrflag(F_RF_TIME);
				rf.sequence = 0; // home
				break;
			} // end switch
		}	
}

unsigned char rf_receive_checksum(unsigned char datalen)
{
	unsigned char ret_sum;
	unsigned char i;

	ret_sum = rf.add;	   // 1
	ret_sum += rf.command; // 2
	for (i = 0; i < datalen; i++)
	{ // if datalen is zero not use
		ret_sum += rf.readdata[i];
	}

	return ret_sum;
}

// send mast or PC command code and run function
void rf_student_processor(unsigned char comm)
{
	switch (comm){
		case CMD_PING://0x71: // ping
		run.senddata[0] = run.student_board_info; // my board information
		run_send_mast(comm, 1); // OX게임 button 결과 보냄
		break;
		
		case CMD_XO_REQ://0x72: // button information
		run.senddata[0] = run.student_infoANDbutton; // button omformation
		run_send_mast(comm, 1); // key board infor
		//setflag(F_SEND_INFOBUT);// 	run.student_infoANDbutton 
		break; // 
	
		case CMD_GAMEMODE://0x73: // Game mode Command
		run.gamemode = rf.readdata[0]; // rf.readdata[rf.count++]
		run.gamemode_sw = run.gamemode * 10;//  case @ run_gamemode
		run_send_mast(comm, 0); // send 
		break;
		
		case CMD_HW_RESET:// 0x74 hardware RESET
			 SYS_ResetCPU();
		break;
		//#define CMD_CHOSUNG_FONT	0x77
		case CMD_CHOSUNG_FONT://0x77: //#define CMD_CHOSUNG_FONT
			run.student_font = rf.readdata[0]; // display font
			setflag(F_RUN_FONT_ACTION);
			run_send_mast(comm, 0);
		break;		
		
		case CMD_BUZZER://0x78: // buzzer run
			run.buzzer_count = (rf.readdata[0] & 0xF0) >> 4; // CCCC
			run.buzzer_ontime = rf.readdata[0] & 0x0F;		 //@sec
			run.buzzer_ontime *= 100;						 // sec *100 ==> 100 @ 10ms ==> 1000ms
			run.buzzer_offtime = TM_1000mS;					 // 1 Sec
			run.buzzer_switch = 0;							 // ini
			setflag(F_RUN_BUZ_ACTION);

			run_send_mast(comm, 0);
		break;		
		
		case CMD_DOT_LED://0x79:
		// Sample
		// rf.readdata[0] = 0x30 | (0x03 << 1); // cccc ttt9(000 1)
		// rf.readdata[1] = 0x01; // 8765 4321 // JUST 1
//		if ((rf.readdata[0] & 0xF0) && (rf.readdata[0] & 0x0E))
	//	{
	
			run.dot_count = (rf.readdata[0] & 0xF0) >> 4;  // CCCC
			run.dot_ontime = (rf.readdata[0] & 0x0E) >> 1; // ttt9 @sec
			run.dot_ontime *= 100;						   // sec *100 ==> 100 @ 10ms ==> 1000ms
			run.dot_offtime = TM_1000mS;				   // 1 Sec
			run.dot_switch = 0;							   // ini
			setflag(F_RUN_DOT_ACTION);
			run_send_mast(comm, 0);
		break;

		case CMD_DOT_LIGHT://0x7A: // bright 
			max7219_ini_bright(rf.readdata[0]);
			run_send_mast(comm, 0);
		break;

		case CMD_STUD_TEST://0x7B: // dot test

		break;

		case CMD_TIME_OVER://0x7C: // signal return not use or use, time over
		run.student_overtime_ms = rf.readdata[0]; // 0~255 Sec,  copy over time 
		if(run.student_overtime_ms ==0){ // key board input over time
			
		}
		else{
			if(run.gamemode == GM_XOGAME){
				setflag(F_GAME_TOUCH_START); 
				run.student_overtime_ms = run.student_overtime_ms * TM_1000mS; // Sec * ms
				setflag(F_OVERTIME_CHECK); // if time over F_STOP_INPUT_OVER
				auto_off('Z', TM_500mS);	
			}
			
			if(run.gamemode == GM_CHOSUNG){
				run.student_overtime_ms = run.student_overtime_ms * TM_1000mS; // Sec * ms
				setflag(F_OVERTIME_CHECK); // if time over F_STOP_INPUT_OVER
				auto_off('Z', TM_500mS);	
			}			
		}
		break;
		case CMD_STUD_OUTE://0x7D:

		break;
	}
}

// game mode play
void run_game_mode(void){
	switch(run.gamemode_sw){
		case 0 : //ini //0~9
			LEDBF(1); // default LED ON
			run.gamemode_sw++;// next
		
			auto_off('Z', TM_500mS);
			Point_Display(); time0_delay(TM_500mS); NULL_Display(); time0_delay(TM_100mS);		
			Point_Display(); time0_delay(TM_500mS); NULL_Display(); time0_delay(TM_100mS);				
			break;
		
		case 1:// 
			// running, dot display
			// all Blue LED ON 
		
			if(isflag(F_TOUCH_ON)){
				auto_off('Z', TM_20mS);
				run_chosung_font_Display(ucpass);
				time0_delay(TM_300mS);
				ucpass++;
				if(ucpass > 20) ucpass=0;
				clrflag(F_TOUCH_ON);
			}	
		
			break;
		
		
		// *************   Chosung Game  ****************
		case 30 : // chosung Game Start	 ini		//run.gamemode_sw = run.gamemode * 10;//  case @ run_gamemode
			setflag(F_GAME_ING); // led on off
			run.gamemode_sw++;// next
		
			// ini
			run.student_font = 0; // null font
		
			auto_off('Z', TM_500mS);
			// 3 time display
			A_Display(); time0_delay(TM_500mS); NULL_Display(); time0_delay(TM_100mS);	// 
			Ff_Display(); time0_delay(TM_500mS); NULL_Display(); time0_delay(TM_100mS);	// 
			H_Display(); time0_delay(TM_500mS); NULL_Display(); 
			break;
		
		case 31 : //  going game
			// run.gamemode = GM_CHOSUNG
			//		setflag(F_RUN_FONT_ACTION);
		
		
			if(isflag(F_RUN_FONT_ACTION)){
				run_chosung_font_Display(run.student_font);
				clrflag(F_RUN_FONT_ACTION);				
			}			
			
			if(isflag(F_TIME_OVER)){
				run.gamemode_sw++;// next
				
				clrflag(F_TIME_OVER);
			}				
			break;
		
		// END   *************   Chosung Game  ****************
		
		// *************   X O Game  ****************
		case 10 : //X O Game Start
			setflag(F_GAME_ING); // led on off

			run.gamemode_sw++;// next
			run.student_touch_toggle = 0;
			run.student_infoANDbutton = STUDENT_NO;//	0xB0		
		
			auto_off('Z', TM_500mS);
			// 3 time display
			X_Display(); time0_delay(TM_500mS); NULL_Display(); time0_delay(TM_100mS);		
			O_Display(); time0_delay(TM_500mS); NULL_Display(); time0_delay(TM_100mS);		
			X_Display(); time0_delay(TM_500mS); NULL_Display(); time0_delay(TM_100mS);				
			O_Display(); time0_delay(TM_500mS); NULL_Display(); 		
		
			clrflag(F_TOUCH_ON);// clear button
			break;
		
		case 11:// // x o touch display
			if(isflag(F_TOUCH_ON)){ // change X 0 with buzzer
				auto_off('Z', TM_500mS);
				run.student_touch_toggle ^= 1;
				if(run.student_touch_toggle){
					O_Display();
					run.student_infoANDbutton = STUDENT_O;//0xFB;
				}
				else{
					X_Display();
					run.student_infoANDbutton = STUDENT_X;
				}
				time0_delay(TM_200mS);
				clrflag(F_TOUCH_ON);
			}
			if(isflag(F_TIME_WARN)){
				run_soundfunction('C', 6);// time over sound
				
				clrflag(F_TIME_WARN);
			}
			if(isflag(F_TIME_OVER)){
				run.gamemode_sw++;// next
				run_soundfunction('T',0);
				clrflag(F_TIME_OVER);
				clrflag(F_GAME_TOUCH_START);
			}			
			break;		
			
		case 12 : //  time over and send button infor
			// waitting next send infor
			if(run.gamemode == GM_XOGAME){
				if(isflag(F_GAME_TOUCH_START)){
					clrflag(F_GAME_TOUCH_START);
					// XO game again
					run.gamemode_sw =11; // key in again
					run.student_touch_toggle = 0;
					run.student_infoANDbutton = STUDENT_NO;//	0xB0		
					//auto_off('Z', TM_500mS);
					run_soundfunction('C', 2);	// BUZZER count  			
					Point_Display();
				}
			}	
			break;		
		// END *************   X O Game  ****************
		case 50 :
			setflag(F_GAME_ING); // led on off
			run.gamemode_sw++;// next
			auto_off('Z', TM_500mS);
			// 3 time display
			B_Display(); time0_delay(TM_500mS); NULL_Display(); time0_delay(TM_100mS);	// 
			G_Display(); time0_delay(TM_500mS); NULL_Display(); time0_delay(TM_100mS);	// 
			K_Display(); time0_delay(TM_500mS); NULL_Display(); 
			break;

		case 51:
			if(isflag(F_RUN_FONT_ACTION)){
				run_chosung_font_Display(run.student_font);
				clrflag(F_RUN_FONT_ACTION);				
			}	
			break;

	}// end switch
}

void run_nop_delay(void){
	unsigned long Microseconds;
	
	for(Microseconds=0; Microseconds < 900; Microseconds++){
		__nop();
	}
}

void run_soundfunction(char option, unsigned long buzcount){
	unsigned long outi, ini;//, midi;
	
	 switch(option){
		case 'G': // good
		case 'g':
			for(outi = 0; outi < 20; outi++){
				BUZZER(1);
				time0_delay(TM_20mS);
				for(ini = 0; ini < outi; ini++){
					BUZZER(0);
					run_nop_delay();
				}
			}
			BUZZER(0);			
			break;

		case 'T': // time over
		case 't':
			for(outi = 0; outi < 10; outi++){
				BUZZER(1);
				time0_delay(TM_20mS);
				for(ini = 0; ini < outi; ini++){
					BUZZER(0);
					time0_delay(TM_20mS);
				}
			}
			BUZZER(0);			
			break;
			
		case 'C': // Buzzer Count mode
		case 'c':
			for(outi = 0; outi < buzcount; outi++){
				BUZZER(1);
				time0_delay(TM_100mS);
				BUZZER(0);
				time0_delay(TM_50mS);
			}
			break;
	 }
	
}


void run_send_mast(unsigned char cmd, unsigned char datalen)
{
	unsigned char senco;
	unsigned char sendsum;

	if (rf.add == RF_ALLSTUDENT)
		return; // ALL address is not send

	UART_WRITE(UART, RF_STUDENT); // start  1

	sendsum = rf.add;		  // address
	UART_WRITE(UART, rf.add); // 2

	sendsum += cmd | 0x80;		  // command | 80
	UART_WRITE(UART, cmd | 0x80); // 3

	for (senco = 0; senco < datalen; senco++)
	{ // if datalen is zero not use
		UART_WRITE(UART, run.senddata[senco]); // data
		sendsum += run.senddata[senco];		
	}
	UART_WRITE(UART, sendsum); // 4

	UART_WRITE(UART, TAIL); // 5
}

void run_action(unsigned char hardware, unsigned long soundontime)
{
	switch (hardware)
	{
	case 'Z': // Buzzer
	case 'z':
		setflag(F_RUN_BUZ_ACTION); // sound on
		run.buzzer_ontime = soundontime;
		BUZZER(1);
		break;
	}
}

unsigned char run_read_myaddress(void){
unsigned char readadd =0;
	
	if(ADDRESS8 == 0) readadd |=0x08;
	if(ADDRESS4 == 0) readadd |=0x04;
	if(ADDRESS2 == 0) readadd |=0x02;
	if(ADDRESS1 == 0) readadd |=0x01;	
	return readadd;
}

void run_chosung_font_Display(unsigned char chosung){    
    switch (chosung) {
		case 0:        Point_Display();        break; //
		case 1:        A_Display();        break; //
		case 2:        Aa_Display();        break; //
		case 3:        B_Display();        break;//
		case 4:        C_Display();        break;//
		case 5:        Cc_Display();        break;//
		case 6:        D_Display();        break;//
		case 7:        E_Display();        break;//
		case 8:        F_Display();        break;//
		case 9:        Ff_Display();        break;//
		
		case 10:        G_Display();        break;//
		case 11:        Gg_Display();       break;//
		case 12:        H_Display();        break;//O Display();        break;
		case 13:		J_Display();        break;// ziun
		
		case 14:        Jj_Display();        break;
		case 15:        K_Display();        break;
		case 16:        L_Display();        break;
		case 17:        M_Display();        break;
		case 18:        N_Display();        break;
		case 19:        OP_Display();        break;
		case 20:        NULL_Display();        break;
    }
}
