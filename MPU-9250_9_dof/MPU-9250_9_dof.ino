//Credits  
//REF: original code, http://www.lucidarme.me/?p=5057
//REF: fine tuned and explained code, http://www.brokking.net/imu.html
//REF: data sheets, https://www.invensense.com/products/motion-tracking/9-axis/mpu-9250/


#include <Wire.h>

#define    MPU9250_ADDRESS            0x68
#define    MAG_ADDRESS                0x0C

#define    GYRO_FULL_SCALE_250_DPS    0x00  
#define    GYRO_FULL_SCALE_500_DPS    0x08
#define    GYRO_FULL_SCALE_1000_DPS   0x10
#define    GYRO_FULL_SCALE_2000_DPS   0x18

#define    ACC_FULL_SCALE_2_G        0x00  
#define    ACC_FULL_SCALE_4_G        0x08
#define    ACC_FULL_SCALE_8_G        0x10
#define    ACC_FULL_SCALE_16_G       0x18


int16_t gyro_setting, acc_setting;
float gyro_scale_factor ;
float acc_sensitivity_scale;

int16_t gyro_x, gyro_y, gyro_z;
long gyro_x_cal, gyro_y_cal, gyro_z_cal;

long acc_x, acc_y, acc_z, acc_total_vector;
long acc_x_cal, acc_y_cal, acc_z_cal;

int temperature;

float angle_pitch, angle_roll;
int angle_pitch_buffer, angle_roll_buffer;
boolean set_gyro_angles;
float angle_roll_acc, angle_pitch_acc;
float angle_pitch_output, angle_roll_output;


float angle_in_one_cycle;
float radian_in_one_cycle;

long loop_timer;

int refresh_rate;   // 250Hz will be fine, but seems to short for debug


// Initializations
void setup()
{
  // Arduino initializations
  Wire.begin();
  Serial.begin(115200);

  settings(ACC_FULL_SCALE_8_G, GYRO_FULL_SCALE_500_DPS);

//  Serial.print("Acce Setting : ");
//  Serial.println(acc_setting, HEX);
//
//  Serial.print("Gyro Setting : ");
//  Serial.println(gyro_setting, HEX);

  configure_register();

  Serial.println("Calibrating ...");

  // constants
  // e.g.  integration of angle in 1cycle/4ms (250Hz), in gyro_sensitivity_scale_factor GYRO_FULL_SCALE_500_DPS
  // sampling 0.0000611 = 1 / (250Hz / 65.5)
  angle_in_one_cycle = 1 / ( refresh_rate * gyro_scale_factor);
  //0.000001066 = 0.0000611 * (3.142(PI) / 180degr) The Arduino sin function is in radians
  radian_in_one_cycle = angle_in_one_cycle  * ( 3.1416 / 180.0);

  calibre_gyro_and_accel();

  
  loop_timer = micros();
  Serial.println("Starting ...");
}


long int cpt=0;
// Main loop, read and display data
void loop()
{
  read_mpu_9250_data();

  gyro_x -= gyro_x_cal;                                                //Subtract the offset calibration value from the raw gyro_x value
  gyro_y -= gyro_y_cal;                                                //Subtract the offset calibration value from the raw gyro_y value
  gyro_z -= gyro_z_cal;                                                //Subtract the offset calibration value from the raw gyro_z value

//  acc_x -= acc_x_cal;                                                //Subtract the offset calibration value from the raw acc_x value
//  acc_y -= acc_y_cal;                                                //Subtract the offset calibration value from the raw acc_y value
//  acc_z -= acc_z_cal;                    //Subtract the offset calibration value from the raw acc_z value
  // TODO: what if acc_z_cal is zero, how to 

  // angle_in_one_cycle 
  // e.g.  integration of angle in 1cycle/4ms (250Hz), in gyro_sensitivity_scale_factor GYRO_FULL_SCALE_500_DPS
  // sampling 0.0000611 = 1 / (250Hz / 65.5)
    
  // angle in degree
  angle_pitch += gyro_x * angle_in_one_cycle;                                   //Calculate the traveled pitch angle and add this to the angle_pitch variable
  angle_roll += gyro_y * angle_in_one_cycle;                                    //Calculate the traveled roll angle and add this to the angle_roll variable

  //0.000001066 = 0.0000611 * (3.142(PI) / 180degr) The Arduino sin function is in radians
  angle_pitch += angle_roll * sin(gyro_z * radian_in_one_cycle);               //If the IMU has yawed transfer the roll angle to the pitch angel
  angle_roll -= angle_pitch * sin(gyro_z * radian_in_one_cycle);               //If the IMU has yawed transfer the pitch angle to the roll angel

  //Accelerometer angle calculations
  acc_total_vector = sqrt((acc_x*acc_x)+(acc_y*acc_y)+(acc_z*acc_z));  //Calculate the total accelerometer vector
  //57.296 = 1 / (3.1416 / 180) The Arduino asin function is in radians
  angle_pitch_acc = asin((float)acc_x/acc_total_vector)* -57.296;       //Calculate the pitch angle
  angle_roll_acc = asin((float)acc_y/acc_total_vector)* 57.296;       //Calculate the roll angle
  
  //Place the MPU-9250 spirit level and note the values in the following two lines for calibration
  angle_pitch_acc -= 0.8; //0.75;                                              //Accelerometer calibration value for pitch
  angle_roll_acc -= 1.0;//;0.3;   
  
  if(set_gyro_angles){                                                 //If the IMU is already started
    angle_pitch = angle_pitch * 0.9996 + angle_pitch_acc * 0.0004;     //Correct the drift of the gyro pitch angle with the accelerometer pitch angle
    angle_roll = angle_roll * 0.9996 + angle_roll_acc * 0.0004;        //Correct the drift of the gyro roll angle with the accelerometer roll angle
  }
  else{                                                                //At first start
    angle_pitch = angle_pitch_acc;                                     //Set the gyro pitch angle equal to the accelerometer pitch angle 
    angle_roll = angle_roll_acc;                                       //Set the gyro roll angle equal to the accelerometer roll angle 
    set_gyro_angles = false ; // set true if need correcting drift;    //Set the IMU started flag
  }
  
  //To dampen the pitch and roll angles a complementary filter is used
  angle_pitch_output = angle_pitch_output * 0.9 + angle_pitch * 0.1;   //Take 90% of the output pitch value and add 10% of the raw pitch value
  angle_roll_output = angle_roll_output * 0.9 + angle_roll * 0.1;


  Serial.print (angle_pitch_output);
  Serial.print( "\t");
  Serial.print (angle_roll_output);
  Serial.print( "\t");

//  Serial.print(acc_x);
//  Serial.print( "\t");
//  Serial.print(acc_y);
//  Serial.print( "\t");
//  Serial.print(acc_z);
//  Serial.print( "\t");
  // _______________
  // ::: Counter :::
  
//  // Display data counter
//  Serial.print (cpt++,DEC);
//  Serial.print ("\t");
    
    // Display values

  // TODO: auto select the scale depending the SEL bits.
//  // Accelerometer . in G according to the Sensitivity Scale
//  Serial.print (acc_x / 16384.0, 2); 
//  Serial.print( "\t");
//  Serial.print (acc_y / 16384.0, 2);
//  Serial.print ("\t");
//  Serial.print (acc_z / 16384.0, 2);  
//  Serial.print ("\t");
//  
//  // Gyroscope in degree
//  Serial.print (gyro_x / 131.0, 2); 
//  Serial.print ("\t");
//  Serial.print (gyro_y / 131.0 ,2);
//  Serial.print ("\t");
//  Serial.print (gyro_z / 131.0, 2);  
//  Serial.print ("");

  /*  
  // _____________________
  // :::  Magnetometer ::: 

  
  // Read register Status 1 and wait for the DRDY: Data Ready
  
  uint8_t ST1;
  do
  {
    I2Cread(MAG_ADDRESS,0x02,1,&ST1);
  }
  while (!(ST1&0x01));

  // Read magnetometer data  
  uint8_t Mag[7];  
  I2Cread(MAG_ADDRESS,0x03,7,Mag);
  

  // Create 16 bits values from 8 bits data
  
  // Magnetometer
  int16_t mx=-(Mag[3]<<8 | Mag[2]);
  int16_t my=-(Mag[1]<<8 | Mag[0]);
  int16_t mz=-(Mag[5]<<8 | Mag[4]);
  
  
  // Magnetometer
  Serial.print (mx+200,DEC); 
  Serial.print ("\t");
  Serial.print (my-70,DEC);
  Serial.print ("\t");
  Serial.print (mz-700,DEC);  
  Serial.print ("\t");
  
*/
  
  // End of line
  Serial.println("");
//  delay(100);

  while(micros() - loop_timer < 1000000.0/(refresh_rate*1.0));     //Wait until the loop_timer reaches 4000us (250Hz) before starting the next loop
  loop_timer = micros(); 
}

void calibre_gyro_and_accel()
{
  for (int cal_int = 0; cal_int < 2000 ; cal_int ++){                  //Run this code 2000 times
    if(cal_int % 125 == 0) Serial.print(".");                              //Print a dot on the LCD every 125 readings
    read_mpu_9250_data();                                              //Read the raw acc and gyro data from the MPU-6050
    gyro_x_cal += gyro_x;                                              //Add the gyro x-axis offset to the gyro_x_cal variable
    gyro_y_cal += gyro_y;                                              //Add the gyro y-axis offset to the gyro_y_cal variable
    gyro_z_cal += gyro_z;                                              //Add the gyro z-axis offset to the gyro_z_cal variable

    
    acc_x_cal += acc_x;                                              //Add the gyro x-axis offset to the gyro_x_cal variable
    acc_y_cal += acc_y;                                              //Add the gyro y-axis offset to the gyro_y_cal variable
    acc_z_cal += acc_z;                                              //Add the gyro z-axis offset to the gyro_z_cal variable

    delay(3);                                                          //Delay 3us to simulate the 250Hz program loop
  }
  gyro_x_cal /= 2000.0;                                                  //Divide the gyro_x_cal variable by 2000 to get the avarage offset
  gyro_y_cal /= 2000.0;                                                  //Divide the gyro_y_cal variable by 2000 to get the avarage offset
  gyro_z_cal /= 2000.0;  

  acc_x_cal /= 2000.0;                                                  //Divide the gyro_x_cal variable by 2000 to get the avarage offset
  acc_y_cal /= 2000.0;                                                  //Divide the gyro_y_cal variable by 2000 to get the avarage offset
  acc_z_cal /= 2000.0;  
}

void settings(int16_t acc_full_scale, int16_t gyro_full_scale)
{
//  Serial.println("MPU is configured as following, ");
//  Serial.print("acc_full_scale :  ");
//  Serial.print(acc_full_scale, HEX);
//
//  Serial.print(" gyro_full_scale :  ");
//  Serial.println(gyro_full_scale, HEX);

  delay(5000);
  // 
  refresh_rate = 100; //Hz
  // 
  gyro_setting = gyro_full_scale;
  acc_setting = acc_full_scale;
  //
  switch (gyro_full_scale) {
    case GYRO_FULL_SCALE_250_DPS:
      gyro_scale_factor = 131.0;
      break;
    case GYRO_FULL_SCALE_500_DPS:
      gyro_scale_factor = 65.5;
      break;
    case GYRO_FULL_SCALE_1000_DPS:
      gyro_scale_factor = 32.8;
      break;
    case GYRO_FULL_SCALE_2000_DPS:
      gyro_scale_factor = 16.4;
      break;
    default: 
      gyro_scale_factor = 16.4;
    break;
  }

  switch (acc_full_scale) {
    case ACC_FULL_SCALE_2_G:
      acc_sensitivity_scale = 16384.0;
      break;
    case ACC_FULL_SCALE_4_G:
      acc_sensitivity_scale = 8192.0;
      break;
    case ACC_FULL_SCALE_8_G:
      acc_sensitivity_scale = 4096.0;
      break;
    case ACC_FULL_SCALE_16_G:
      acc_sensitivity_scale = 2048.0;
      break;
    default: 
      acc_sensitivity_scale = 2048.0;
    break;
  }
}

void configure_register()
{
  // Configure gyroscope range
  I2CwriteByte(MPU9250_ADDRESS,27,gyro_setting);
  // Configure accelerometers range
  I2CwriteByte(MPU9250_ADDRESS,28,acc_setting);
  // Set by pass mode for the magnetometers
  I2CwriteByte(MPU9250_ADDRESS,0x37,0x02);
  
  // Request first magnetometer single measurement
  I2CwriteByte(MAG_ADDRESS,0x0A,0x01);  
}


void read_mpu_9250_data(){                                             //Subroutine for reading the raw gyro and accelerometer data
                                                                   
  //Add the low and high byte to the gyro_z variable
  // ____________________________________
  // :::  accelerometer and gyroscope ::: 

  // Read accelerometer and gyroscope
  uint8_t Buf[14];
  I2Cread(MPU9250_ADDRESS,0x3B,14,Buf);                                //Send the requested starting register and request 14 bytes from the MPU-6050
  
  // Create 16 bits values from 8 bits data
  
  // Accelerometer
  // NOTE: the direction may be dependent on your target applications of different world reference.
  acc_x=-(Buf[0]<<8 | Buf[1]);
  acc_y=-(Buf[2]<<8 | Buf[3]);
  acc_z=-(Buf[4]<<8 | Buf[5]);

  // Temperature
  temperature = Buf[6]<<8 | Buf[7];
  
  // Gyroscope
  gyro_x= Buf[8]<<8 | Buf[9];
  gyro_y= Buf[10]<<8 | Buf[11];
  gyro_z= Buf[12]<<8 | Buf[13];
}


// This function read Nbytes bytes from I2C device at address Address. 
// Put read bytes starting at register Register in the Data array. 
void I2Cread(uint8_t Address, uint8_t Register, uint8_t Nbytes, uint8_t* Data)
{
  // Set register address
  Wire.beginTransmission(Address);
  Wire.write(Register);
  Wire.endTransmission();
  
  // Read Nbytes
  Wire.requestFrom(Address, Nbytes); 
  uint8_t index=0;
  while (Wire.available())
    Data[index++]=Wire.read();
}


// Write a byte (Data) in device (Address) at register (Register)
void I2CwriteByte(uint8_t Address, uint8_t Register, uint8_t Data)
{
  // Set register address
  Wire.beginTransmission(Address);
  Wire.write(Register);
  Wire.write(Data);
  Wire.endTransmission();
}


