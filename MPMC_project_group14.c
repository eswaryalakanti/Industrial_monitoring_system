#include <reg51.h>
#include <intrins.h>


#define lcd P2



sbit rs=P0^0;
sbit rw=P0^1;
sbit en=P0^2;/P3^6/
char str;

char Charin=0;


void LCD_cmd(unsigned char);
void LCD_char(unsigned char);
void LCD_str(unsigned char *,unsigned char);
void delay_ms(unsigned int);
void LCD_init();
void LCD_init(void){
	LCD_cmd(0x38);//sets for 
	LCD_cmd(0x10);
	LCD_cmd(0x01);
	LCD_cmd(0x06);
	LCD_cmd(0x0e);
}

void LCD_cmd(unsigned char cmd){
	rw=0;
	rs=0;
	lcd=cmd;
	en=1;
	delay_ms(1000);
	en=0;
}

void LCD_char(unsigned char ch)
{
	rw=0;
	rs=1;
	lcd=ch;
	en=1;
	delay_ms(200);
	en=0;
}

void LCD_str(unsigned char *s,unsigned char r){
	unsigned char w;
	for(w=0;w<r;w++){
		LCD_char(s[w]);
		delay_ms(100);
	}
}

void delay_ms(unsigned int n){
	unsigned int m;
	for(n;n>0;n--)
	for(m=121;m>0;m--);
	nop();
	nop();
	nop();
}
	





sbit DHT11=P1^1;
sbit smoke=P1^3;
sbit red=P1^4;
sbit green=P1^6;
sbit buzzer=P1^7;
sbit IR=P1^2;
sbit touch =P1^5;

unsigned char I_RH,D_RH,I_Temp,D_Temp,Checksum;
void delay_40us(){//each np is 1us
	nop();nop();nop();nop();nop();
	nop();nop();nop();nop();nop();
	nop();nop();nop();nop();nop();
	nop();nop();nop();nop();nop();
	nop();nop();nop();nop();nop();
	nop();nop();nop();nop();nop();
	nop();nop();nop();nop();nop();
	nop();nop();nop();nop();nop();
}

void request(){
	DHT11=0;
	delay_ms(18);
	DHT11=1;
}

void response(){
	while(DHT11==1);
	while(DHT11==0);
	while(DHT11==1);
}

unsigned char Receive_data()
{
	unsigned char i,dataD=0;
	for(i=0;i<8;i++){
		while(DHT11==0);
		delay_40us();
		if(DHT11==1)
		dataD=(dataD<<1)| (0x01);
		else
		dataD=(dataD<<1);
		while(DHT11==1);
	}
	return dataD;
}

void delay(int time)
{

 unsigned int i,j;

 for(i=0;i<time;i++)

 for(j=0;j<1275;j++);

}




void ser_int();
unsigned char rx();
void tx(unsigned char a);


void ser_int()
{
    SCON=0x50;// Sets the serial control _register to enable serial reception
    TMOD=0x20;//Configures the timer mode for the serial communication.
    TH1=TL1=0xFD;//Sets the timer 1 high and low bytes for a specific baud rate 
    TR1=1;//Starts timer 1 to enable serial communication.
}

unsigned char rx()
{
    while(RI==0);//Waits until the receive interupt flag (RI) is set, indicating that there is _data to be read.
    RI=0;//Resets the receive_interrupt flag.
    return SBUF;//Returns the received byte from the serial buffer.
}

void tx(unsigned char a)
{
    SBUF=a;// Places the byte 'a' into the serial buffer.
    while(TI==0);//Waits until the transmit _interrupt flag (TI) is set, indicating that the buffer is ready for the next byte.
    TI=0;//Resets the transmit _interrupt flag after transmission.
}


	//2 for off the buzzer
	//1 for the red led
	//0 for the night times
	//3 for reset mode
	//4 for displaying the temperature and humidity and verifying.

void main(){
	extern unsigned char a,b,c,dd;
	LCD_init();
  red=0;
	green=0;
	smoke=0;
	buzzer=1;//not work
  touch=0;
  ser_int();
  

	
	LCD_cmd(0x80);
	LCD_str("Welcome to zenesis",12);
	LCD_cmd(0xc0);
	LCD_str("Please enter code:",12);
	while(1){  
	unsigned char gets;				
		gets=rx();
    tx(gets);
		if(touch==1){
			buzzer=1;}
		if(gets=='0'){
			while(1){
					if(IR==0){
						LCD_cmd(0x01);
		  LCD_cmd(0x80);
	    LCD_str("detected",8);
						delay_ms(100);
			buzzer=0;}else{
								LCD_cmd(0x01);
		  LCD_cmd(0x80);
	    LCD_str("not detected",12);
				delay_ms(100);
				buzzer=1;}
			if(smoke==1){
				red=1;
				green=0;}else{
					red=0;
					green=1;}
							request();
		response();
		I_RH=Receive_data();
		D_RH=Receive_data();
		I_Temp=Receive_data();
		D_Temp=Receive_data();
		Checksum=Receive_data();
					
		if(I_Temp>30){
			buzzer=0;
			red=1;
			green=0;
		}
		if(I_RH>50){
			buzzer=0;
			red=1;
			green=0;}
			}
				
		}else if(gets=='1'){
			red=0;
			green=1;
			LCD_cmd(0x01);
		  LCD_cmd(0x80);
	    LCD_str("Hii 1",12);
		}else if(gets=='2'){
			buzzer=1;
			red=0;
			green=1;
	    }
		else if(gets=='3'){	
	LCD_cmd(0x80);
	LCD_str("Welcome to zenesis",12);
	LCD_cmd(0xc0);
			LCD_str("enter code:",11);
			delay_ms(100);
			LCD_cmd(0xcd);
			LCD_char(gets);

		}else if (gets=='4'){
			LCD_cmd(0x01);
			LCD_cmd(0x80);
	LCD_str("Humidity=",12);
	LCD_cmd(0xc0);
	LCD_str("Temperature=",12);
		request();
		response();
		I_RH=Receive_data();
		D_RH=Receive_data();
		I_Temp=Receive_data();
		D_Temp=Receive_data();
		Checksum=Receive_data();
		if(smoke==1){
			red=1;
			green=0;
		}
		else{
			red=0;
			green=1;
		}
		
		if((I_RH+D_RH+I_Temp+D_Temp)!=Checksum){
			LCD_str("Error",5);
		}
		else{
			a=(I_RH/10)+48;
			b=(I_RH%10)+48;
			LCD_cmd(0X89);
			LCD_char(a);
			LCD_cmd(0X8a);
			LCD_char(b);
			LCD_cmd(0X8b);
			LCD_char('%');
			
			c=(I_Temp/10)+48;
			dd=(I_Temp%10)+48;
			LCD_cmd(0xcc);
			LCD_char(c);
			LCD_cmd(0xcd);
			LCD_char(dd);
			delay_ms(300);
		}		
	}
	}
}