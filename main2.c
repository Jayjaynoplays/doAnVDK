#device PIC16F877A
#INCLUDE <16F877A.h>                                                        
#fuses NOWDT 
#fuses NOBROWNOUT
#fuses NOLVP
#device ADC = 10
#use delay(crystal=20M)

unsigned CHAR  SevenSegments [10] = {0xC0,0XF9,0XA4,0XB0,0X99,0X92,0X82,0XF8,0X80,0X90};  //numbers of a digit
unsigned INT32  BDT;
unsigned INT32 GasValue,count ;


#INT_AD
void  AD_isr(void){ //Ngat adc
  GasValue = GasValue + read_adc(ADC_READ_ONLY);
}

#INT_TIMER1
void  TIMER1_isr(void){ //Ngat timer
  set_timer1(3036); //set 100 ms 
  BDT++; 
  if (BDT>=10){ //dem 10 lan se ngat, tuc la 1s 
    BDT=0;
    output_toggle(pin_b3);
  }
}

void alarm(){
  if(GasValue <= 30){
    output_high(pin_b0);
    output_low(pin_b1);
    output_low(pin_b2);
  }else {
    if(GasValue <= 60){
      output_low(pin_b0);
      output_high(pin_b1);
      output_low(pin_b2);
    }else{
      output_low(pin_b0);
      output_low(pin_b1);
      output_high(pin_b2);
    }
  }
}

void show(){
  OUTPUT_D (SevenSegments [GasValue%10]);
  OUTPUT_C (SevenSegments [GasValue/10]);
}

void main(){
   //GasValue = 0;
   //count = 0;
  SET_TRIS_A (0x01); //set this port to be input FOR gas sensor
  SET_TRIS_B (0x00); //set this port to be ouput FOR warning LEDs
  SET_TRIS_C (0x00); //set this port to be ouput FOR 7 segments module
  SET_TRIS_D (0x00); //set this port to be ouput FOR 7 segments module
  
  

  SETUP_ADC_PORTS(ALL_ANALOG);
  setup_adc(ADC_CLOCK_DIV_32);
  SET_ADC_CHANNEL(0);
  
  setup_timer_1(T1_INTERNAL|T1_DIV_BY_8);      //104 ms overflow
  set_timer1(3036);

  enable_interrupts(INT_AD);  //lenh cho phep ngat
  enable_interrupts(INT_TIMER1);
  enable_interrupts(GLOBAL);
  
  
  while(true){
    GasValue = 0;
    for (count=0; count<100; count++){
      read_adc(ADC_START_ONLY);
      delay_ms(1);
    }
    //GasValue = GasValue / 2.046;
    //GasValue = GasValue / 100;
    GasValue = GasValue /1020;
    output_high(pin_e0);
    alarm();
    show();
    //
  }
}
