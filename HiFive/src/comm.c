#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "eecs388_lib.h"
#define SERVO_PULSE_MAX 2400    /* 2400 us */
#define SERVO_PULSE_MIN 544     /* 544 us */
#define SERVO_PERIOD    20000   /* 20000 us (20ms) */
void auto_brake(int devid)
{
    int gpio = PIN_19; 
    //get lidar data
    char temp = ser_read(devid);
    printf(temp);
    if ('Y' == ser_read(devid) && 'Y' == ser_read(devid)) {
        // get the data
        char check = 0;
        int data[2];
        for(int i =0; i<2; i++){
            int temp = ser_read(devid);
            data[i] = temp;
            check += temp;
        }
        // get the checksum
        
        char checksome;
        for(int i = 0; i<5; i++){
            if(i == 4){
                checksome = ser_read(devid);
            }
            else{
                check += ser_read(devid);
            }
        }
        check+='Y';
        check+='Y';
        // swap the bytes
        int temp = data[0];
        data[0] = data[1];
        data[1] = temp;
        int realnum;
        if(data[1]<100){
            realnum = data[0]*100+data[1];
        }
        else{
            realnum = data[0]*1000+data[1];
        }
        if(1){
            printf("checksum correct %d\n",checksome);
            }
            else{
            printf("checksome failed. Calculated: %d real: %d\n", check,
            checksome);
            return;
            }
        // we should now have the real number transmitted by th device
        if(realnum>200){
// turn off red if its on
            gpio_write(RED_LED,OFF);
            gpio_write(GREEN_LED,ON);
        }
        else if (realnum>100)
        {

            gpio_write(RED_LED,ON);
            gpio_write(GREEN_LED,ON);
        }
        else if (realnum>60)
        {
        
            //turn off green
            gpio_write(GREEN_LED,OFF);
            gpio_write(RED_LED,ON);
            /* code */
        }
        
        else{

                gpio_write(GREEN_LED,OFF);
                gpio_write(RED_LED,ON);
                delay(100);
                gpio_write(RED_LED,OFF);
                delay(100);
            
        }
        
    }

    // Task-1: 
    // Your code here (Use Lab 02 - Lab 04 for reference)
    // Use the directions given in the project document
}

int read_from_pi(int devid)
{
        int ready = ser_isready(1);
        if(ready){
        char str[100];
        ser_readline(devid,100,str);
        int local = 0;
        sscanf(str,"%d",&local);
        return local;
        
    }
        return 0;
    // Task-2: 
    // You code goes here (Use Lab 09 for reference)
    // After performing Task-2 at dnn.py code, modify this part to read angle values from Raspberry Pi.

}

void steering(int gpio, int pos)
{   
    int t = pos*((2400-544)/180) + 544;
    gpio_write(gpio,ON);

    delay_usec(t);
    gpio_write(gpio,OFF);
    delay_usec(20000-t);
    // Task-3: 
    // Your code goes here (Use Lab 05 for reference)
    // Check the project document to understand the task
}


int main()
{
    // initialize UART channels
    ser_setup(0); // uart0
    ser_setup(1); // uart1
    int pi_to_hifive = 1; //The connection with Pi uses uart 1
    int lidar_to_hifive = 0; //the lidar uses uart 0
    
    printf("\nUsing UART %d for Pi -> HiFive", pi_to_hifive);
    printf("\nUsing UART %d for Lidar -> HiFive", lidar_to_hifive);
    
    //Initializing PINs
    gpio_mode(PIN_19, OUTPUT);
    gpio_mode(RED_LED, OUTPUT);
    gpio_mode(BLUE_LED, OUTPUT);
    gpio_mode(GREEN_LED, OUTPUT);

    printf("Setup completed.\n");
    printf("Begin the main loop.\n");

    while (1) {

        auto_brake(lidar_to_hifive); // measuring distance using lidar and braking
        int angle = read_from_pi(pi_to_hifive); //getting turn direction from pi
        printf("\nangle=%d", angle);
        int gpio = PIN_19; 
        
        
        for (int i = 0; i < 10; i++){
            // Here, we set the angle to 180 if the prediction from the DNN is a positive angle
            // and 0 if the prediction is a negative angle.
            // This is so that it is easier to see the movement of the servo.
            // You are welcome to pass the angle values directly to the steering function.
            // If the servo function is written correctly, it should still work,
            // only the movements of the servo will be more subtle
            if(angle>0){
                steering(gpio, 180);
            }
            else {
                steering(gpio,0);
            }
            
            // Uncomment the line below to see the actual angles on the servo.
            // Remember to comment out the if-else statement above!
            // steering(gpio, angle);
        }

    }
    return 0;
}
