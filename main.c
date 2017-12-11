

#include <project.h>
#include <stdio.h>
#include "Motor.h"
#include "Ultra.h"
#include "Nunchuk.h"
#include "Reflectance.h"
#include "I2C_made.h"
#include "Gyro.h"
#include "Accel_magnet.h"
#include "IR.h"
#include "Ambient.h"
#include "Beep.h"

int rread(void);

//battery level//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
    ADC_Battery_Start();        
    int16 adcresult =0;
    float volts = 0.0;
    float voltsBa = 0.0;
    float nopeus = 225; //Säädä tästä robotin nopeus. Skaalaa automaagisesti loput.
    int stopline = 0;
    unsigned int IR_val = 0;
    printf("\nBoot\n");
    int blackline = 0;
    
    BatteryLed_Write(1); // Switch led on 
    BatteryLed_Write(0); // Switch led off 
    //uint8 button;
    //button = SW1_Read(); // read SW1 on pSoC board
    
    struct sensors_ ref;
    struct sensors_ dig;
    CyGlobalIntEnable; 
    UART_1_Start();
    uint8 button;
    //int stopline = 0;
    sensor_isr_StartEx(sensor_isr_handler);
    
    reflectance_start();

    IR_led_Write(1);
    CyDelay(5); //Hieman viivettä
    while (SW1_Read() == 1)
    {
    }
    CyDelay(500);
    
        //Go to stopline.
    motor_start();
    while (stopline < 1){
        reflectance_read(&ref);
        reflectance_digital(&dig);
        motor_forward (nopeus/2,0);
        if ((ref.l3 > 10000) && (ref.l1 > 10000) && (ref.r1 > 10000) && (ref.r3 > 10000)) {
            motor_stop ();
            stopline++;
        }
    }  
    
    //Wait for controller signal
    while (IR_val == 0){
            IR_val = get_IR ();
    }
    
    motor_start();
    motor_forward (nopeus, 200);
    stopline = 0;
    
    while (stopline < 1){
        
        button = SW1_Read(); // read SW1 on pSoC board
        {
        //CyDelay(2000);
        reflectance_read(&ref);
        reflectance_digital(&dig);
        CyDelay(1);
        //motor_start();
        
        
        if(dig.l3 == 0 && dig.l1 == 0 && dig.r1 == 0 && dig.r3 == 0){
            blackline = 1;
        }
        if (blackline == 1) {
            while (dig.l3 == 0 && dig.r3 == 0) {
                motor_forward (nopeus,0);
                reflectance_read(&ref);
                reflectance_digital(&dig);
                //Beep (3,44);
            }
            while (dig.l3 == 1 && dig.r3 == 1){
                motor_forward (nopeus,0);
                Beep( 8,20);
                reflectance_read(&ref);
                reflectance_digital(&dig);
                if (dig.l3 == 0 && dig.r3 == 0){
                    stopline++;
                    //motor_stop ();
                    break;
                }
            }
        }
        
        //Aja suoraan
        if (ref.l1 > 9000 && ref.r1 > 9000){
            //blackline = 0;  
            motor_forward (nopeus, 0);
        }
        

        //Aja oikealle
        //l3 = valk, l1 = valk, r1 = must, r3 =valk
        if ((ref.r1 > 15000) && (ref.l1 < 9000) && (ref.r3 < 9000) && (ref.l3 < 9000)) {
            motor_turn (nopeus,0, 0); 
        }//l3 = valk, l1 = valk, r1 = must, r3 =must
        else if ((ref.r1 > 15000) && (ref.r3 > 15000) && (ref.l1 < 9000) && (ref.l3 < 9000)) {
            motor_turn (nopeus,0, 0);
        }//l3 = valk, l1 = valk, r1 = valk, r3 =must
        else if ((ref.r3 > 15000) && (ref.r1 < 9000) && (ref.l1 < 9000) && (ref.l3 < 9000)){
            motor_turn (nopeus,0, 0);
        }
        
        //Aja vasemmalle
         //l3 = valk, l1 = must, r1 = valk, r3 =valk
        if ((ref.l1 > 15000) && (ref.r1 < 9000) && (ref.l3 < 9000) && (ref.r3 <9000)){
            motor_turn (0, nopeus, 0);
        }//l3 = must, l1 = must, r1 = valk, r3 =valk
        else if ((ref.l1 > 15000) && (ref.l3 > 15000) && (ref.r1 < 9000) && (ref.r3 < 9000)){
            motor_turn (0,nopeus,0);
        }//l3 = must, l1 = valk, r1 = valk, r3 =valk
        else if ((ref.l3 > 15000) && (ref.l1 < 9000) && (ref.r1 < 9000) && (ref.r3 < 9000)){
            motor_turn (0, nopeus,0);
        }  
        }
    }
    //Pysäytä moottori kolmannella mustalla viivalla
    motor_stop();
   

    for(;;)
    {
        
        ADC_Battery_StartConvert();
        if(ADC_Battery_IsEndConversion(ADC_Battery_WAIT_FOR_RESULT)) {   // wait for get ADC converted value
            adcresult = ADC_Battery_GetResult16();
            volts = ADC_Battery_CountsTo_Volts(adcresult);                  // convert value to Volts
            voltsBa = volts * 1.5;
            if (voltsBa < 4) {
                BatteryLed_Write(1);
                CyDelay(500);
                BatteryLed_Write(0);
            }
            // If you want to print value
            printf("%d %f\r\n",adcresult, voltsBa);
        }
        CyDelay(500);
        
    }
 }   



#if 0
int rread(void)
{
    SC0_SetDriveMode(PIN_DM_STRONG);
    SC0_Write(1);
    CyDelayUs(10);
    SC0_SetDriveMode(PIN_DM_DIG_HIZ);
    Timer_1_Start();
    uint16_t start = Timer_1_ReadCounter();
    uint16_t end = 0;
    while(!(Timer_1_ReadStatusRegister() & Timer_1_STATUS_TC)) {
        if(SC0_Read() == 0 && end == 0) {
            end = Timer_1_ReadCounter();
        }
    }
    Timer_1_Stop();
    
    return (start - end);
}
#endif

/* Don't remove the functions below */
int _write(int file, char *ptr, int len)
{
    (void)file; /* Parameter is not used, suppress unused argument warning */
	int n;
	for(n = 0; n < len; n++) {
        if(*ptr == '\n') UART_1_PutChar('\r');
		UART_1_PutChar(*ptr++);
	}
	return len;
}

int _read (int file, char *ptr, int count)
{
    int chs = 0;
    char ch;
 
    (void)file; /* Parameter is not used, suppress unused argument warning */
    while(count > 0) {
        ch = UART_1_GetChar();
        if(ch != 0) {
            UART_1_PutChar(ch);
            chs++;
            if(ch == '\r') {
                ch = '\n';
                UART_1_PutChar(ch);
            }
            *ptr++ = ch;
            count--;
            if(ch == '\n') break;
        }
    }
    return chs;
}
/* [] END OF FILE */
