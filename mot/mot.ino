#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library
//Librerías obtenidas de http://www.lcdwiki.com/3.95inch_Arduino_Display-UNO

//if the IC model is known or the modules is unreadable,you can use this constructed function
LCDWIKI_KBV my_lcd(ILI9486, A3, A2, A1, A0, A4); //model,cs,cd,wr,rd,reset

const int dirPin = 10;
const int stepPin = 11;
const int butPin = 12;
int per;
float per2;
char valor[10];
enum screen {inicio, plasticos, control, datos} pantalla = inicio;
uint8_t const color_1[] = {102, 219, 255};
uint8_t const color_2[] = {29, 169, 245};
uint8_t const color_3[] = {54, 208, 255};

uint8_t* const list_plasticos[] = {"Poliacetal", "Acrilico", "ABS", "Acetato de celulosa", "PTFE", "Nylon-6",
                                   "Policarbonato", "PET", "Polietileno (baja densidad)", "Polietileno (alta densidad)",
                                   "Polipropileno", "Poliestireno", "PVC", "Plasx","Plasxx","Plasxxx","Plasxxxx"
                                  };
//uint16_t const negro = 0, blanco = 0xFFFF;
enum colores {negro = 0x0000, blanco = 0xFFFF};

uint16_t WIDTH;
uint16_t HEIGHT;
uint8_t str = 0;

int const tabla_y0 = 80;
int const plastY = 33;
int const plast_vis = 7;
int const plast_tot = sizeof(list_plasticos) / sizeof(list_plasticos[0]);
int const plast_y0 = tabla_y0 + (plastY - 7 * 2) / 2 ;
int const scrollY = plast_vis < plast_tot ? plast_vis * plast_vis * plastY / plast_tot : 0;




void setup() {
  // Sets the two pins as Outputs
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(butPin, INPUT);

  digitalWrite(dirPin, HIGH); //Enables the motor to move in a particular direction

  my_lcd.Init_LCD();
  my_lcd.Set_Rotation(1);
  Serial.begin(9600);
  Serial.println(my_lcd.Read_ID(), HEX);

}





void loop() {
  WIDTH = my_lcd.Get_Display_Width();
  HEIGHT = my_lcd.Get_Display_Height();

  if (pantalla == inicio) {
    my_lcd.Fill_Screen(conv_color(color_2));
    show_string((uint8_t*)"ChemiTech", CENTER, 50, 4, blanco);

    my_lcd.Set_Draw_color(color_1[0], color_1[1], color_1[2]);
    Rectangle(WIDTH / 2 - 100, 120, WIDTH / 2 + 100, 170, (uint8_t*)"INYECTORA", 2, blanco);
    Rectangle(WIDTH / 2 - 100, 220, WIDTH / 2 + 100, 270, (uint8_t*)"BASE DE DATOS", 2, blanco);

    while (pantalla == inicio) {

      pantalla = plasticos;

      str = readS();
      if (str == '0') {
        pantalla = inicio;
      }
      else if (str == '1') {
        pantalla = plasticos;
      }
      else if (str == '2') {
        pantalla = control;
      }
      else if (str == '3') {
        pantalla = datos;
      }
    }

  }

  if (pantalla == plasticos) {

    int item  = 0;
    int postext = 0;
    int scroll = 0;

    my_lcd.Fill_Screen(blanco);
    my_lcd.Set_Draw_color(color_1[0], color_1[1], color_1[2]);
    my_lcd.Fill_Rectangle(0, 0, WIDTH, 25);
    show_string("ChemiTech", WIDTH - 120, 5, 2, blanco);

    show_string("Elija el plastico:", 20, 45, 2, negro);

    my_lcd.Set_Draw_color(color_3[0], color_3[1], color_3[2]);
    my_lcd.Fill_Rectangle(10, tabla_y0, WIDTH - 10, HEIGHT - 10);

    my_lcd.Set_Draw_color(color_3[0], color_3[1], color_3[2]);
    my_lcd.Fill_Rectangle(10, tabla_y0 + plastY * item, WIDTH - 10, tabla_y0 + plastY * (item + 1) - 1);
    Actualizar_plasticos(item, postext, scroll);

    while (pantalla == plasticos) {

      str = readS();
      if (str == '0') {
        pantalla = inicio;
      }
      else if (str == '1') {
        pantalla = plasticos;
      }
      else if (str == '2') {
        pantalla = control;
      }
      else if (str == '3') {
        pantalla = datos;
      }

      if (str == '+') {
        my_lcd.Set_Draw_color(color_3[0], color_3[1], color_3[2]);
        if (item < plast_vis - 1) {
          my_lcd.Fill_Rectangle(10, tabla_y0 + plastY * item, WIDTH - 10, tabla_y0 + plastY * (item + 1) - 1);
          item += 1;
        }
        else if (postext < plast_tot - plast_vis) {

          for (int i = postext; i < plast_vis + postext ; i++) {
            show_string(list_plasticos[i], 20, plast_y0 + plastY * (i - postext), 2, conv_color(color_3));
          }
          my_lcd.Fill_Rectangle(WIDTH - 15, tabla_y0 + scroll, WIDTH - 10, tabla_y0 + scrollY + scroll - 1);
          postext += 1;
          scroll = (plast_vis * plastY - scrollY) * postext / (plast_tot - plast_vis);
        }
        Actualizar_plasticos(item, postext, scroll);
      }

      if (str == '-') {
        my_lcd.Set_Draw_color(color_3[0], color_3[1], color_3[2]);
        if (item > 0) {
          my_lcd.Fill_Rectangle(10, tabla_y0 + plastY * item, WIDTH - 10, tabla_y0 + plastY * (item + 1) - 1);
          item -= 1;
        }
        else if (postext > 0) {
          for (int i = postext; i < plast_vis + postext ; i++) {
            show_string(list_plasticos[i], 20, plast_y0 + plastY * (i - postext), 2, conv_color(color_3));
          }
          my_lcd.Fill_Rectangle(WIDTH - 15, tabla_y0 + scroll, WIDTH - 10, tabla_y0 + scrollY + scroll - 1);
          postext -= 1;
          scroll = plast_vis * plastY * postext / plast_tot;
        }
        Actualizar_plasticos(item, postext, scroll);
      }

    }

  }

  if (pantalla == control) {
    my_lcd.Fill_Screen(blanco);
    show_string("Le control", 0, 0, 2, 0);

    while (pantalla == control) {

      str = readS();
      if (str == '0') {
        pantalla = inicio;
      }
      else if (str == '1') {
        pantalla = plasticos;
      }
      else if (str == '2') {
        pantalla = control;
      }
      else if (str == '3') {
        pantalla = datos;
      }
    }

  }

  if (pantalla == datos) {
    my_lcd.Fill_Screen(conv_color(color_1));
    show_string("Les dates", 0, 0, 2, 0);

    while (pantalla == datos) {

      str = readS();
      if (str == '0') {
        pantalla = inicio;
      }
      else if (str == '1') {
        pantalla = plasticos;
      }
      else if (str == '2') {
        pantalla = control;
      }
      else if (str == '3') {
        pantalla = datos;
      }
    }

  }


  /*while (true) {
    my_lcd.Fill_Rectangle(200, 100, 250, 120);
    MostrarValor();
    for (int i = 0; i < 200; i++) {
      per = actualizarVel();
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(per);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(per);
    }
    }*/

}






// Function for reading the Potentiometer
int actualizarVel() {
  int customDelay = analogRead(A5); // Reads the potentiometer
  int newCustom = map(customDelay, 0, 1023, 500, 2000); // Convrests the read values of the potentiometer from 0 to 1023 into desireded delay values (300 to 4000)
  return newCustom;
}


void MostrarValor() {
  itoa((int)(150000 / per), valor, 10);
  show_string((uint8_t*)valor, 200, 100, 3, 0x0000);
  show_string("rpm", 260, 100, 3, 0x0000);
}

uint16_t conv_color(uint8_t col[]) {
  return my_lcd.Color_To_565(col[0], col[1], col[2]);
}


void show_string(uint8_t *str, int16_t x, int16_t y, uint8_t csize, uint16_t fc)
{
  my_lcd.Set_Text_Mode(1);
  my_lcd.Set_Text_Size(csize);
  my_lcd.Set_Text_colour(fc);
  my_lcd.Set_Text_Back_colour(0);
  my_lcd.Print_String(str, x, y);
}


uint8_t readS() {
  uint8_t c = 0;
  if (Serial.available()) {
    c = Serial.read();
  }
  if (c != 0) {
    Serial.println(c);
  }
  return c;
}

void Rectangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t *str, uint8_t csize, uint16_t cText)
{
  my_lcd.Fill_Rectangle(x1, y1, x2, y2);
  uint16_t len = strlen((const char *)str) * 6 * csize;
  int16_t posx = x1 + (x2 - x1 + 1 - len) / 2;
  int16_t posy = y1 + (y2 - y1 + 1 - 7 * csize) / 2;
  show_string(str, posx, posy, csize, cText);
}

void Actualizar_plasticos(int item, int postext, int scroll) {
  //pintar fila
  my_lcd.Set_Draw_color(color_1[0], color_1[1], color_1[2]);
  my_lcd.Fill_Rectangle(10, tabla_y0 + plastY * item, WIDTH - 10, tabla_y0 + plastY * (item + 1) - 1);
  //pintar scroll
  my_lcd.Set_Draw_color(color_2[0], color_2[1], color_2[2]);
  my_lcd.Fill_Rectangle(WIDTH - 15, tabla_y0 + scroll, WIDTH - 10, tabla_y0 + scrollY + scroll - 1);
  //llenar plasticos
  for (int i = postext; i < plast_vis + postext ; i++) {
    show_string(list_plasticos[i], 20, plast_y0 + plastY * (i - postext), 2, blanco);
  }
}
