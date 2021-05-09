#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library

//if the IC model is known or the modules is unreadable,you can use this constructed function
LCDWIKI_KBV my_lcd(ILI9486, A3, A2, A1, A0, A4); //model,cs,cd,wr,rd,reset

const int dirPin = 10;
const int stepPin = 11;
const int butPin = 12;
int per;
float per2;
char valor[10];

void setup() {
  // Sets the two pins as Outputs
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(butPin, INPUT);

  digitalWrite(dirPin, HIGH); //Enables the motor to move in a particular direction

  Serial.begin(9600);
  my_lcd.Init_LCD();
  Serial.println(my_lcd.Read_ID(), HEX);
  my_lcd.Fill_Screen(0x0);
  my_lcd.Set_Rotation(1);
}


void loop() {
  my_lcd.Fill_Screen(0xFFFF);
  my_lcd.Set_Draw_color(255, 255, 255);

  while (true) {
    my_lcd.Fill_Rectangle(200, 100, 250, 120);
    MostrarValor();
    for (int i = 0; i < 200; i++) {
      per = actualizarVel();
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(per);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(per);
    }
  }

}
// Function for reading the Potentiometer
int actualizarVel() {
  int customDelay = analogRead(A5); // Reads the potentiometer
  int newCustom = map(customDelay, 0, 1023, 500, 2000); // Convrests the read values of the potentiometer from 0 to 1023 into desireded delay values (300 to 4000)
  return newCustom;
}


void MostrarValor() {
  itoa((int)(150000 / per), valor, 10);
  show_string(valor, 200, 100, 3, 0x0000, 0, 1);
  show_string("rpm", 260, 100, 3, 0x0000, 0, 1);
}

void show_string(uint8_t *str, int16_t x, int16_t y, uint8_t csize, uint16_t fc, uint16_t bc, boolean mode)
{
  my_lcd.Set_Text_Mode(mode);
  my_lcd.Set_Text_Size(csize);
  my_lcd.Set_Text_colour(fc);
  my_lcd.Set_Text_Back_colour(bc);
  my_lcd.Print_String(str, x, y);
}
