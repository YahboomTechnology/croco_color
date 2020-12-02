/*
* @par Copyright (C): 2010-2019, Shenzhen Yahboom Tech
* @file         Color_sorter
* @author       Jessica
* @version      V1.0
* @date         2019.8.21
* @brief        Color sorter
* @details
* @par History  
*
*/
#include <Wire.h>
#include <Servo.h>  
void RGB_Config(uint8_t RGB);


#define ADD true
#define SUB false

int val = 0;
boolean stat = ADD;
int ServoPos = 90;

Servo myservo;      

//Device Address
//ADDR->Low
#define RGB_2           0x00
#define ADDR            0x53
#define W_ADDR_L        0x66
#define R_ADDR_L        0x67
//ADDR->High
#define RGB_1           0x01
#define W_ADDR_H        0x98
#define R_ADDR_H        0x99

//Register Address
#define ControlReg_Addr 0x00
#define RGBCON_Addr     0x04

#define ID_Addr         0x06

#define CDATAL_Addr     0x12
#define CDATAH_Addr     0x13

#define RDATAL_Addr     0x10
#define RDATAM_Addr     0x11
#define RDATAH_Addr     0x12

#define GDATAL_Addr     0x0D
#define GDATAM_Addr     0x0E
#define GDATAH_Addr     0x0F

#define BDATAL_Addr     0x13
#define BDATAM_Addr     0x14
#define BDATAH_Addr     0x15


/*  Confi Arg  */
//Control Register
#define RST             0x00


int ServoPin = 3;

int LED_Red   = 10;
int LED_Blue  = 9; 
int LED_Green = 11; 

unsigned int Red = 0;
unsigned int Green  = 0;
unsigned int Blue = 0;

unsigned int val_red = 0;
unsigned int val_green = 0;
unsigned int val_blue = 0;

/*
* Function       RGB Configuration
* @author        Jessica
* @date          2019.8.21
* @brief         RGB:RGB sensor select
* @param[in]     void
* @retval        void
* @par History   no
*/
void RGB_Config(void)
{
  uint8_t W_ADDR;

  //Reset register, 200ms integration time, all CRGB channels are turned on
  Wire.beginTransmission(ADDR);   //Send Device address
  Wire.write(ControlReg_Addr);
  Wire.write(0x06);
  Wire.endTransmission();         //Send ending signal

  Wire.beginTransmission(ADDR);  //Send Device address
  Wire.write(0x04);
  Wire.write(0x41);              //16bit 50ms
  Wire.endTransmission();        //Send ending signal

  Wire.beginTransmission(ADDR);  //Send Device address
  Wire.write(0x05);
  Wire.write(0x01);
  Wire.endTransmission();        //Send ending signal
}

/*
* Function       ReadColor
* @author        Jessica
* @date          2019.8.21
* @brief         Get color
* @param[in]     void
* @retval        void
* @par History   no
*/
void ReadColor()                 //Sensor gets color
{
  int index = 0;
  char ColorData[6] = {0};
  //Red
  Wire.beginTransmission(ADDR);  //Send Device address
  Wire.write(0x10);
  Wire.endTransmission();     //Send ending signal
  Wire.beginTransmission(ADDR);  //Send Device address
  Wire.requestFrom(ADDR, 2);    // request 1 bytes from slave device #2
  while (Wire.available())   // slave may send less than requested
  {
    char ff = Wire.read();    // receive a byte as character
    ColorData[index] = ff;
    index++;
    //printf("RED:%x\n", ff);
  }
  Wire.endTransmission();     //Send ending signal

  //Green
  Wire.beginTransmission(ADDR);  //Send Device address
  Wire.write(0x0D);
  Wire.endTransmission();     //Send ending signal
  Wire.beginTransmission(ADDR);  //Send Device address
  Wire.requestFrom(ADDR, 2);    // request 1 bytes from slave device #2
  while (Wire.available())   // slave may send less than requested
  {
    char ff = Wire.read();    // receive a byte as character
    ColorData[index] = ff;
    index++;
    //printf("Green:%x\n", ff);
  }
  Wire.endTransmission();     //Send ending signal

  //Blue
  Wire.beginTransmission(ADDR);  //Send Device address
  Wire.write(0x13);
  Wire.endTransmission();     //Send ending signal
  Wire.beginTransmission(ADDR);  //Send Device address
  Wire.requestFrom(ADDR, 2);    // request 1 bytes from slave device #2
  while (Wire.available())   // slave may send less than requested
  {
    char ff = Wire.read();    // receive a byte as character
    ColorData[index] = ff;
    index++;
    //printf("Blue:%x\n", ff);
  }
  Wire.endTransmission();     //Send ending signal
  Red = ((unsigned int)(ColorData[1] & 0xff) << 8 | (ColorData[0] & 0xff)); //2.06
  Green = ((unsigned int)(ColorData[3] & 0xff) << 8 | (ColorData[2] & 0xff));
  Blue = ((unsigned int)(ColorData[5] & 0xff) << 8 | (ColorData[4] & 0xff));


  if (Red > 4500) Red = 4500;    //4500.7600.4600 are the calibration values detected when pure white paper is detected. 
                                   //If the color recognition is not accurate, you can modify the calibration values here.
  if (Green > 7600) Green = 7600;
  if (Blue > 4600) Blue = 4600;
  val_red = map(Red, 0, 4500, 0, 255);
  val_blue = map(Blue, 0, 4600, 0, 255);
  val_green = map(Green, 0, 7600, 0, 255);


  if (val_red > val_green && val_red > val_blue)
  {
    val_red = 255;
    val_green = 0;
    val_blue = 0;
    myservo.write(10);
    delay(1500);
  }
  else if (val_green > val_red && val_green > val_blue)
  {

    val_green = 255;
    val_red = 0;
    val_blue = 0;
    myservo.write(160);
    delay(1500);
  }
  else if (val_blue > val_red && val_blue >  val_green)
  {
    val_blue = 255;
    val_red = 0;
    val_green = 0;
  }

  printf("R:%d, G:%d, B:%d\n", Red, Green, Blue);  //Serial print data
  printf("val_red:%d, val_green:%d, val_blue:%d\n", val_red, val_green, val_blue);//Serial print data

}

/*
* Function       RGB_light
* @author        Jessica
* @date          2019.8.21
* @brief         Light up RGB
* @param[in]     void
* @retval        void
* @par History   NO
*/
void RGB_light()
{
  analogWrite(LED_Red, val_red);
  analogWrite(LED_Green, val_green);
  analogWrite(LED_Blue, val_blue);
}

int serial_putc( char c, struct __file * )
{
  Serial.write( c );
  return c;
}
void printf_begin(void)
{
  fdevopen( &serial_putc, 0 );
}

/*
* Function       setup
* @author        Jessica
* @date          2019.8.21
* @brief         Initial configuration
* @param[in]     void
* @retval        void
* @par History   NO
*/
void setup()
{
  Wire.begin();
  Wire.setClock(100000);
  Serial.begin(9600);  

  pinMode(LED_Red,   OUTPUT);
  pinMode(LED_Blue,  OUTPUT);
  pinMode(LED_Green, OUTPUT);

  myservo.attach(ServoPin);

  myservo.write(ServoPos);
  delay(1000);

  printf_begin();
  Serial.println("\nI2C Scanner");
  RGB_Config();
}

/*
* Function       loop
* @author        Jessica
* @date          2019.8.21
* @brief         mian
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   no
*/
void loop()
{
  ReadColor();
  RGB_light();
//  servo_control();
  delay(500);
}

void serialEvent()
{
  while (Serial.available())
  {
    char IncomingByte = Serial.read();
    Serial.print(IncomingByte);
  }
}
