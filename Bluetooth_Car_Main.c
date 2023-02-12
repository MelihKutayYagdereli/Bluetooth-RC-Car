
#include <stdio.h>
#include <MKL25Z4.h>

static volatile char c;

//INITILAZE ALL THE PORTS AND TIMERS
void UART0_INIT(void);
void MOTOR_INIT(void);
void pwm_INIT(void);  


void UART0_IRQHandler(void);
void motor_pwm (int servo_angle, int LEFT, int RIGHT); //wheel rotation, left right motor power
void Delay(volatile unsigned int time_del);


int main (void) {
	
	pwm_INIT();
	UART0_INIT();
	MOTOR_INIT();
	
	Delay(3000000);
	motor_pwm(60,0,0); // the stop position. wheels are facing forward, no power is given to the motors.
	Delay(3000000);
	
	while(1){
	//the controls acording to the recived char
		while(c!='S'){
			
		if (c=='F'){
	// the right dc motor damaged so giving more power to the right motor makes it balanced.
		motor_pwm(60,85,99);
		PTB->PCOR |= 0x01; //PORT5
		PTB->PSOR |= 0x02; //PORT4
		
		PTB->PSOR |= 0x04; //PORT3
		PTB->PCOR |= 0x08; //PORT2
		Delay(50000);
		
		}		
		else if (c=='B'){
		
		motor_pwm(60,80,90);
		PTB->PSOR |= 0x01; //PORT5
		PTB->PCOR |= 0x02; //PORT4
		
		PTB->PCOR |= 0x04; //PORT3
		PTB->PSOR |= 0x08; //PORT2
		Delay(5000);
		
		}
		
		else if (c=='L'){
		motor_pwm(100,0,0);
			Delay(5000);
		}
		
		else if (c=='R'){
		motor_pwm(25,0,0);
			Delay(5000);
		}
		//BACK LEFT
		else if (c=='H'){
		motor_pwm(100,30,75);
		PTB->PSOR |= 0x01; //PORT5
		PTB->PCOR |= 0x02; //PORT4
		
		PTB->PCOR |= 0x04; //PORT3
		PTB->PSOR |= 0x08; //PORT2
			Delay(5000);
		}
		
		else if (c=='J'){
		
		motor_pwm(20,30,75);
		PTB->PSOR |= 0x01; //PORT5
		PTB->PCOR |= 0x02; //PORT4
		
		PTB->PCOR |= 0x04; //PORT3
		PTB->PSOR |= 0x08; //PORT2
			Delay(5000);
		}
		
		else if (c=='I'){
		
		motor_pwm(20,65,80);
		PTB->PCOR |= 0x01; //PORT5
		PTB->PSOR |= 0x02; //PORT4
		
		PTB->PSOR |= 0x04; //PORT3
		PTB->PCOR |= 0x08; //PORT2
			Delay(5000);
		}
		
		else if (c=='G'){
		
		motor_pwm(100,65,80);
		PTB->PCOR |= 0x01; //PORT5
		PTB->PSOR |= 0x02; //PORT4
		
		PTB->PSOR |= 0x04; //PORT3
		PTB->PCOR |= 0x08; //PORT2
			Delay(5000);
		}
		
		
		
		}
			
		
		PTB->PCOR |= 0x01; //PORT5
		PTB->PCOR |= 0x02; //PORT4
		
		PTB->PCOR |= 0x04; //PORT3
		PTB->PCOR |= 0x08; //PORT2
		Delay(500);
		motor_pwm(60,0,0); // the stop position. wheels are facing forward, no power is given to the motors.

	}
		
	}
	

void MOTOR_INIT(void){
	SIM->SCGC5 |= 0x400; /*Enable clock to port E*/ 
	
	PORTB->PCR[0] = 0x100; /*Select Porte 5 as GPIO*/ 
	PORTB->PCR[1] = 0x100; /*Select PortE 4 as GPIO*/ 
	PORTB->PCR[2] = 0x100; /*Select Porte 5 as GPIO*/ 
	PORTB->PCR[3] = 0x100; /*Select PortE 4 as GPIO*/ 
	
	PTB->PDDR |= 0x01; /*Select Porte 3 as output*/ 
	PTB->PDDR |= 0x02; /*Select Porte 2 as output*/ 
	PTB->PDDR |= 0x04; /*Select Porte 5 as output*/ 
	PTB->PDDR |= 0x08; /*Select Porte 4 as output*/ 
}

/* initialize UART0 to receive at 115200 Baud */
void UART0_INIT(void){

	
	__disable_irq(); /* global disable IRQs */
	SIM->SCGC4 |= 0x0400; // enable clock for UART0 / 
	SIM->SOPT2 |= 0x04000000; // use FLL output for UART Baud rategenerator / 
	UART0->C2 = 0; // turn off UART0 while changing configurations / 
	UART0->BDH = 0x00; 
	UART0->BDL = 137; // 115200 Baud / 
	UART0->C4 = 0x0F; // Over Sampling Ratio 16 / 
	UART0->C1 = 0x00; // 8-bit data / 
	UART0->C2 = 0x24; // enable receive and receive interrupt/
	NVIC->ISER[0] |= 0x00001000; // enable INT12 (bit 12 of ISER[0]) // 
	SIM->SCGC5 |= 0x02000; // enable clock for PORTA / 
	PORTE->PCR[21] = 0x400; // make PTA1 UART0_Rx pin 
	__enable_irq();

}

void motor_pwm (int servo_angle, int LEFT, int RIGHT){
		int MODD = 81914/50,servo_duty = (10*servo_angle / 180 ) + 2.5;
		
		TPM0->SC = 0; /* disable timer */
	
		TPM0->MOD = MODD; /* max modulo value; */
	
		TPM0->CONTROLS[1].CnV = (MODD*(servo_duty))/100; //SERVO DUTY
	
		TPM0->CONTROLS[2].CnV = (MODD*(LEFT))/100; /* Set up channel value for LEFT MOTOR% dutycycle */
		TPM0->CONTROLS[3].CnV = (MODD*(RIGHT))/100; /* Set up channel value for RIGHT MOTOR% dutycycle */
		
		TPM0->SC = 0xAF; /* enable TPM0 with prescaler /128 */
}

void UART0_IRQHandler(void){
	c = UART0->D; // recive the char coming from the user
	Delay(5000);
}

	void pwm_INIT(void){
		SIM->SCGC5 |= 0x2000; /* enable clock to Port E */
		SIM->SCGC5 |= 0x200; /* enable clock to Port A */
		
		PORTA->PCR[4] = 0x0300;  
		PORTE->PCR[29] = 0x0300;  
		PORTE->PCR[30] = 0x0300;
		
		SIM->SCGC6 |= 0x01000000; /* enable clock to TPM0 */
		SIM->SOPT2 |= 0x01000000; /* use MCGFLLCLK as timer counter clock */
		TPM0->SC = 0; /* disable timer */
		TPM0->CONTROLS[1].CnSC = 0x28; /* edge-aligned, pulse low */
		TPM0->CONTROLS[2].CnSC = 0x28; /* edge-aligned, pulse low */
		TPM0->CONTROLS[3].CnSC = 0x28; /* edge-aligned, pulse low */
	
	}


void Delay(volatile unsigned int time_del) {
  while (time_del--) 
		{
  }
}

