//Sumopuoli

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
    int16 adcresult = 0;
    float volts = 0.0;
    float voltsBa = 0.0;
    float nopeus = 225; //Säädä tästä robotin nopeus. Skaalaa automaagisesti loput.
    unsigned int IR_val = 0;
    printf("\nBoot\n");

    BatteryLed_Write(1); // Switch led on 
    BatteryLed_Write(0); // Switch led off 
    
    struct sensors_ ref;
    struct sensors_ dig;
    CyGlobalIntEnable; 
    UART_1_Start();
    uint8 button;
    int stopline = 0;
    sensor_isr_StartEx(sensor_isr_handler);
    
    reflectance_start();
    reflectance_read(&ref);
    reflectance_digital(&dig);
    button = SW1_Read(); // read SW1 on pSoC board
    
    IR_led_Write(1);
    CyDelay(5); //Hieman viivettä
    
    while (SW1_Read() == 1)
    {
    }
    
    
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
    motor_forward (255,1000);
    for(;;)
    {
        button = SW1_Read(); // read SW1 on pSoC board
        {
        CyDelay(1);
        motor_start();
        reflectance_digital(&dig);
        reflectance_read(&ref);  
        //Pyöriympyrää
        //kaikki valkoisella
        if ((dig.l3 == 1) && (dig.l1 == 1) && (dig.r1 == 1) && (dig.r3 == 1)){
            motor_turn (nopeus, 0, 0);
        }
        
        // aja oikealle
        // l3 = must, l1 = valk, r1 = valk, r3 = valk
        if ((dig.l3 == 0) && (dig.l1 == 1) && (dig.r1 == 1) && (dig.r3 == 1)) {
            motor_turn (nopeus,0, 500); 
        }//l3 = must, l1 = must, r1 = valk, r3 = valk
        else if ((dig.l3 == 0) && (dig.l1 == 0) && (dig.r1 == 1) && (dig.r3 == 1)) {
            motor_turn (nopeus,0, 500);
        }//l3 = must, l1 = must, r1 = must, r3 = valk
        else if ((dig.l3 == 0) && (dig.l1 == 0) && (dig.r1 == 0) && (dig.r3 == 1)){
            motor_turn (nopeus,0, 500);
        }

        //aja vasemmalle
        //l3 = valk, l1 = must, r1 = must, r3 = must
        if (dig.l3 == 1 && dig.l1 == 0 && dig.r1 == 0 && dig.r3 == 0) {
            motor_turn (0, nopeus, 500);
        }//l3 = valk, l1 = valk, r1 = must, r3 = must
        else if ((dig.l3 == 1) && (dig.l1 == 1) && (dig.r1 == 0) && (dig.r3 == 0)){
            motor_turn (0,nopeus,500);
        }//l3 = valk, l1 = valk, r1 = valk, r3 = must
        else if ((dig.l3 == 1) && (dig.l1 == 1) && (dig.r1 == 1) && (dig.r3 == 0)){
            motor_turn (0, nopeus,500);
        }   
        
        
        // jos kaikki on mustia
        if (dig.l3 == 0 && dig.l1 == 0 && dig.r1 == 0 && dig.r3 == 0) {
            motor_backward (nopeus, 500);
            if ((dig.l3 == 1) && (dig.l1 == 1) && (dig.r1 == 1) && (dig.r3 == 1)){
                motor_turn ( nopeus, 0, 800);
            }
        }//jos uloimmat mustia
        if (dig.l3 == 0 || dig.r3 == 0){
            motor_backward (nopeus, 500);
        }
        }  
    }

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



/*//ultra sonic sensor//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
    Ultra_Start();                          // Ultra Sonic Start function
    while(1) {
        //If you want to print out the value  
        printf("distance = %5.0f\r\n", Ultra_GetDistance());
        CyDelay(1000);
    }
}   
//*/


/*//nunchuk//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
  
    nunchuk_start();
    nunchuk_init();
    
    for(;;)
    {    
        nunchuk_read();
    }
}   
//*/


/*//IR receiver//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
    
    unsigned int IR_val; 
    
    for(;;)
    {
       IR_val = get_IR();
       printf("%x\r\n\n",IR_val);
    }    
 }   
//*/


/*//Ambient light sensor//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
    
    I2C_Start();
    
    I2C_write(0x29,0x80,0x00);          // set to power down
    I2C_write(0x29,0x80,0x03);          // set to power on
    
    for(;;)
    {    
        uint8 Data0Low,Data0High,Data1Low,Data1High;
        Data0Low = I2C_read(0x29,CH0_L);
        Data0High = I2C_read(0x29,CH0_H);
        Data1Low = I2C_read(0x29,CH1_L);
        Data1High = I2C_read(0x29,CH1_H);
        
        uint8 CH0, CH1;
        CH0 = convert_raw(Data0Low,Data0High);      // combine Data0
        CH1 = convert_raw(Data1Low,Data1High);      // combine Data1

        double Ch0 = CH0;
        double Ch1 = CH1;
        
        double data = 0;
        data = getLux(Ch0,Ch1);
        
        // If you want to print out data
        //printf("%lf\r\n",data);    
    }    
 }   
//*/


/*//accelerometer//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
  
    I2C_Start();
  
    uint8 X_L_A, X_H_A, Y_L_A, Y_H_A, Z_L_A, Z_H_A;
    int16 X_AXIS_A, Y_AXIS_A, Z_AXIS_A;
    
    I2C_write(ACCEL_MAG_ADDR, ACCEL_CTRL1_REG, 0x37);           // set accelerometer & magnetometer into active mode
    I2C_write(ACCEL_MAG_ADDR, ACCEL_CTRL7_REG, 0x22);
    
    
    for(;;)
    {
        //print out accelerometer output
        X_L_A = I2C_read(ACCEL_MAG_ADDR, OUT_X_L_A);
        X_H_A = I2C_read(ACCEL_MAG_ADDR, OUT_X_H_A);
        X_AXIS_A = convert_raw(X_L_A, X_H_A);
        
        Y_L_A = I2C_read(ACCEL_MAG_ADDR, OUT_Y_L_A);
        Y_H_A = I2C_read(ACCEL_MAG_ADDR, OUT_Y_H_A);
        Y_AXIS_A = convert_raw(Y_L_A, Y_H_A);
        
        Z_L_A = I2C_read(ACCEL_MAG_ADDR, OUT_Z_L_A);
        Z_H_A = I2C_read(ACCEL_MAG_ADDR, OUT_Z_H_A);
        Z_AXIS_A = convert_raw(Z_L_A, Z_H_A);
        
        printf("ACCEL: %d %d %d %d %d %d \r\n", X_L_A, X_H_A, Y_L_A, Y_H_A, Z_L_A, Z_H_A);
        value_convert_accel(X_AXIS_A, Y_AXIS_A, Z_AXIS_A);
        printf("\n");
        
        CyDelay(50);
    }
}   
//*/


//reflectance//
/*int main()
{
    struct sensors_ ref;
    struct sensors_ dig;
    CyGlobalIntEnable; 
    UART_1_Start();
  
    sensor_isr_StartEx(sensor_isr_handler);
    
    reflectance_start();

    IR_led_Write(1);
    CyDelay(5); //Hieman viivettä
    for(;;)
    {
        reflectance_read(&ref);
        printf("%d %d %d %d \r\n", ref.l3, ref.l1, ref.r1, ref.r3);       //print out each period of reflectance sensors
        reflectance_digital(&dig);      //print out 0 or 1 according to results of reflectance period
        printf("%d %d %d %d \r\n", dig.l3, dig.l1, dig.r1, dig.r3);        //print out 0 or 1 according to results of reflectance period
        
        CyDelay(500);
    }
}   
//*/

//motor//
/*int main()
{
    uint8 button;
    CyGlobalIntEnable; 
    UART_1_Start();
    
    button = SW1_Read(); // read SW1 on pSoC board
    while (SW1_Read() == 1)
        motor_start();              // motor start

        motor_forward(200,2200);     // moving forward
        motor_turn(107,5,800);     // turn
        motor_forward(200,1600);
        motor_turn(100,5,800);     // turn
        motor_forward(200,1500);
        motor_turn(100,5,800);
        motor_turn(150,125,1350);
        motor_turn(150,100,1350);
        //motor_backward(100,2000);    // movinb backward
           
        motor_stop();               // motor stop
    
    
    for(;;)
    {

    }
}
*/
    

/*//gyroscope//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
  
    I2C_Start();
  
    uint8 X_L_G, X_H_G, Y_L_G, Y_H_G, Z_L_G, Z_H_G;
    int16 X_AXIS_G, Y_AXIS_G, Z_AXIS_G;
    
    I2C_write(GYRO_ADDR, GYRO_CTRL1_REG, 0x0F);             // set gyroscope into active mode
    I2C_write(GYRO_ADDR, GYRO_CTRL4_REG, 0x30);             // set full scale selection to 2000dps    
    
    for(;;)
    {
        //print out gyroscope output
        X_L_G = I2C_read(GYRO_ADDR, OUT_X_AXIS_L);
        X_H_G = I2C_read(GYRO_ADDR, OUT_X_AXIS_H);
        X_AXIS_G = convert_raw(X_H_G, X_L_G);
        
        
        Y_L_G = I2C_read(GYRO_ADDR, OUT_Y_AXIS_L);
        Y_H_G = I2C_read(GYRO_ADDR, OUT_Y_AXIS_H);
        Y_AXIS_G = convert_raw(Y_H_G, Y_L_G);
        
        
        Z_L_G = I2C_read(GYRO_ADDR, OUT_Z_AXIS_L);
        Z_H_G = I2C_read(GYRO_ADDR, OUT_Z_AXIS_H);
        Z_AXIS_G = convert_raw(Z_H_G, Z_L_G);
     
        // If you want to print value
        printf("%d %d %d \r\n", X_AXIS_G, Y_AXIS_G, Z_AXIS_G);
        CyDelay(50);
    }
}   
//*/


/*//magnetometer//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
  
    I2C_Start();
   
    uint8 X_L_M, X_H_M, Y_L_M, Y_H_M, Z_L_M, Z_H_M;
    int16 X_AXIS, Y_AXIS, Z_AXIS;
    
    I2C_write(GYRO_ADDR, GYRO_CTRL1_REG, 0x0F);             // set gyroscope into active mode
    I2C_write(GYRO_ADDR, GYRO_CTRL4_REG, 0x30);             // set full scale selection to 2000dps
    I2C_write(ACCEL_MAG_ADDR, ACCEL_CTRL1_REG, 0x37);           // set accelerometer & magnetometer into active mode
    I2C_write(ACCEL_MAG_ADDR, ACCEL_CTRL7_REG, 0x22);
    
    
    for(;;)
    {
        X_L_M = I2C_read(ACCEL_MAG_ADDR, OUT_X_L_M);
        X_H_M = I2C_read(ACCEL_MAG_ADDR, OUT_X_H_M);
        X_AXIS = convert_raw(X_L_M, X_H_M);
        
        Y_L_M = I2C_read(ACCEL_MAG_ADDR, OUT_Y_L_M);
        Y_H_M = I2C_read(ACCEL_MAG_ADDR, OUT_Y_H_M);
        Y_AXIS = convert_raw(Y_L_M, Y_H_M);
        
        Z_L_M = I2C_read(ACCEL_MAG_ADDR, OUT_Z_L_M);
        Z_H_M = I2C_read(ACCEL_MAG_ADDR, OUT_Z_H_M);
        Z_AXIS = convert_raw(Z_L_M, Z_H_M);
        
        heading(X_AXIS, Y_AXIS);
        printf("MAGNET: %d %d %d %d %d %d \r\n", X_L_M, X_H_M, Y_L_M, Y_H_M, Z_L_M, Z_H_M);
        printf("%d %d %d \r\n", X_AXIS,Y_AXIS, Z_AXIS);
        CyDelay(50);      
    }
}   
//*/


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
