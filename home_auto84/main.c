/* AVR Node 84
This is the Node for my simple Home automatization network.

R.Pollak

The softuart code is take from Martin Thomas

*/

#define WITH_STDIO_DEMO   1 /* 1: enable, 0: disable */

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "softuart.h"
#include <math.h>


#if WITH_STDIO_DEMO
#include <stdio.h>

//#define ID 3 will passed over makefile
#define tmp_ID(x) #x
#define STD_ID(x) tmp_ID(x)
#define tmp2_ID(x) \'##x##\'
#define C_ID(x) ((x)+0x30) 
#define PROT_CNT (0)
#if  defined (HAVE_PIR)
#define PROT_CNT ((PROT_CNT)+1)
#endif
#if  defined (HAVE_LDR)
#define PROT_CNT ((PROT_CNT)+1)
#endif
#if  defined (HAVE_DBG)
#define PROT_CNT ((PROT_CNT)+1)
#endif



char line[32];
char dbg[32];
int pir;

void adc_init(void )
{
//outp(0,SFIOR);
DIDR0=0x0c;
ADMUX=2;
ADCSRA=_BV(ADEN)| 7 ;
}


unsigned int adc_get(unsigned char ch )
{
 unsigned int ret;

 ADMUX=ch;
 ADCSRA=_BV(ADEN)| _BV(ADSC) | 7;


 while(bit_is_set(ADCSRA,ADSC));


 ret= ADCW;
 return ret;
}


void send(void)
{       unsigned char test[16]; 
        float volt,res,temp;
        volt=3.3/1024*(float)adc_get(2);
        //res=(10000.0/(3.3-volt))*volt;
	res=10000.0/((3.3/volt)-1.0);
        temp=(1.0/(log(res/10000.0)/3800.0+(1.0/(25.0+273.15))))-273.15;
	//printf("outTopic/sensor/temp/1  %d.%d\n",(int)temp,((int)(temp*10.0)%10));
//	printf_P( PSTR("outTopic/sensor/temp/"STD_ID(ID)"  %d.%d\n"),(int)temp,temp<0.0?((int)(temp*-1.0*10.0)%10):((int)(temp*10.0)%10));
#if defined(WITH_FLOAT)
	printf_P( PSTR("outTopic/sensor/temp/"STD_ID(ID)"  %.1f\n"),temp);
#else
	printf_P( PSTR("outTopic/sensor/temp/"STD_ID(ID)"  %c%d.%d\n"),temp<0.0?'-':' ',temp<-1.0f?(int)(temp*-1.0):(int)temp,temp<0.0f?((int)(temp*-1.0*10.0)%10):((int)(temp*10.0)%10));
#endif
#if defined (HAVE_PIR)
	printf_P( PSTR("outTopic/sensor/pir/"STD_ID(ID)" %d\n"),pir);
#endif
#if defined (HAVE_LDR)
	printf_P( PSTR("outTopic/sensor/light/"STD_ID(ID)" %d\n"),adc_get(3));
#endif
#if defined (HAVE_DBG)
	printf_P(PSTR("outTopic/debug [%s] \n"),dbg );
#endif
        
}


// interface between avr-libc stdio and software-UART
static int my_stdio_putchar( char c, FILE *stream )
{
	if ( c == '\n' ) {
		softuart_putchar( '\r' );
	}
	softuart_putchar( c );

	return 0;
}

FILE suart_stream = FDEV_SETUP_STREAM( my_stdio_putchar, NULL, _FDEV_SETUP_WRITE );

static void stdio_demo_func( void ) 
{
	stdout = &suart_stream;

	//	printf_P( PSTR("This output done with printf_P\n") );
}
#endif /* WITH_STDIO_DEMO */


int main(void)
{
	char c;

	unsigned short cnt = 0;
#if (F_CPU > 4000000UL)
#define CNTHALLO (unsigned int)(0xFFFF)
#else 
#define CNTHALLO (unsigned int)(0xFFFF/3)
#endif
	unsigned short mycnt=0;
	  CLKPR=_BV(CLKPCE);
  	CLKPR=0;
	pir=0;

	DDRA |= 0xe2;
	PORTA |= 0x81;
	adc_init();
	softuart_init();
	TCCR1A=_BV(COM0B1)|_BV(WGM00);
	TCCR1B=_BV(CS00);


	softuart_turn_rx_on(); /* redundant - on by default */
	
	sei();
	
	

#if WITH_STDIO_DEMO
		stdio_demo_func();
#endif
OCR1A=0x10;
OCR1B=0xf0;

	
	for (;;) {
	           if((PINB&4)==4 ) pir=1;
	
		if ( softuart_kbhit() ) {
			c = softuart_getchar();
			//softuart_putchar( '[' );
			//softuart_putchar( c );
			//softuart_putchar( ']' );
			if(c=='g')
			  {
			    c=softuart_getchar();
			    if(c==C_ID(ID))
			      {
				softuart_putchar(3);
				softuart_putchar(2);
				send();
			pir=0;
			      }
			  }
			else if( c=='s') 
			  {
			   unsigned int pwm;
			  c=softuart_getchar();
                            if(c==C_ID(ID))
                              {
			      sprintf(line,"made it");
	//		       char line[32];
				for(cnt=0;cnt<=31;cnt++)
				 {
				  c=softuart_getchar();
				  if(c=='\n') {line[cnt]=0; break;}
				  else line[cnt]=c;
				 }
				int ret=sscanf_P(&line[0],PSTR("inTopic/pwm/"STD_ID(ID)" %d"),&pwm);
				sprintf_P(dbg,PSTR("%dpwm:%d"),ret,pwm);
				OCR1B=pwm;	
			
			  }

			 }
			}
		
		
	}
	
	return 0; /* never reached */
}
