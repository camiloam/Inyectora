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
enum action {setear, leer, guardar} accion;

uint16_t const negro = 0x0000, blanco = 0xFFFF;
uint8_t const color_1[] = {102, 219, 255};
uint8_t const color_2[] = {54, 208, 255};
uint8_t const color_3[] = {29, 169, 245};

uint8_t* const list_plasticos[] = {"Poliacetal", "Acrilico", "ABS", "Acetato de celulosa", "PTFE", "Nylon-6",
                                   "Policarbonato", "PET", "Polietileno (baja densidad)", "Polietileno (alta densidad)",
                                   "Polipropileno", "Poliestireno", "PVC", "Plasx", "Plasxx", "Plasxxx", "Plasxxxx"
                                  };

uint8_t* const variables[] = {"Velocidad", "Temp 1", "Temp 2", "Temp 3", "Temp 4"};
uint8_t* const simbolos[] = {"1", "2", "3", "+", "4", "5", "6", "-", "7", "8", "9", "0"};

uint16_t const WIDTH = 480;
uint16_t const HEIGHT = 320;

int const tabla_y0 = 80;
int const plastY = 33;
int const plast_vis = 7;
int const plast_tot = sizeof(list_plasticos) / sizeof(list_plasticos[0]);
int const plast_y0 = tabla_y0 + (plastY - 7 * 2) / 2 ;
int const scrollY = plast_vis < plast_tot ? plast_vis * plast_vis * plastY / plast_tot : 0;

int const tabla2_y0 = 100;
int const varY = 40;
int const var_y0 = tabla2_y0 + (varY - 7 * 2) / 2;

int const teclado_x0 = 205;
int const teclado_y0 = 170;
int const tecX = 40;
int const tecY = 40;
int const tec_x0 = teclado_x0 + (tecX - 5 * 2) / 2;
int const tec_y0 = teclado_y0 + (tecY - 7 * 2) / 2;

uint8_t str = 0;


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

  if (pantalla == inicio) {
    my_lcd.Fill_Screen(conv_color(color_2));
    show_string((uint8_t*)"ChemiTech", CENTER, 50, 4, blanco);

    my_lcd.Set_Draw_color(color_1[0], color_1[1], color_1[2]);
    Rectangle(WIDTH / 2 - 100, 120, WIDTH / 2 + 100, 170, (uint8_t*)"INYECTORA", 2, blanco);
    Rectangle(WIDTH / 2 - 100, 220, WIDTH / 2 + 100, 270, (uint8_t*)"BASE DE DATOS", 2, blanco);

    while (pantalla == inicio) {

      pantalla = control;

      str = readS();
      if (str == 'z') {
        pantalla = inicio;
      }
      else if (str == 'x') {
        pantalla = plasticos;
      }
      else if (str == 'c') {
        pantalla = control;
      }
      else if (str == 'v') {
        pantalla = datos;
      }
    }

  }


  /***********************************************************************************************************************/
  if (pantalla == plasticos) {

    int item  = 0;
    int postext = 0;
    int scroll = 0;

    my_lcd.Fill_Screen(blanco);
    Mostrar_ChemiTech();

    show_string("Elija el plastico:", 20, 45, 2, negro);

    //dibujar tabla
    my_lcd.Set_Draw_color(color_2[0], color_2[1], color_2[2]);
    my_lcd.Fill_Rectangle(10, tabla_y0, WIDTH - 10, HEIGHT - 10);

    Actualizar_plasticos(item, postext, scroll);

    while (pantalla == plasticos) {

      str = readS();
      if (str == 'z') {
        pantalla = inicio;
      }
      else if (str == 'x') {
        pantalla = plasticos;
      }
      else if (str == 'c') {
        pantalla = control;
      }
      else if (str == 'v') {
        pantalla = datos;
      }

      if (str == '+') {
        my_lcd.Set_Draw_color(color_2[0], color_2[1], color_2[2]);
        if (item < plast_vis - 1) {
          my_lcd.Fill_Rectangle(10, tabla_y0 + plastY * item, WIDTH - 10, tabla_y0 + plastY * (item + 1) - 1);

          item += 1;
        }
        else if (postext < plast_tot - plast_vis) {
          for (int i = postext; i < plast_vis + postext ; i++) {
            show_string(list_plasticos[i], 20, plast_y0 + plastY * (i - postext), 2, conv_color(color_2));
          }
          my_lcd.Fill_Rectangle(WIDTH - 15, tabla_y0 + scroll, WIDTH - 10, tabla_y0 + scrollY + scroll - 1);

          postext += 1;
          scroll = (plast_vis * plastY - scrollY) * postext / (plast_tot - plast_vis);
        }
        Actualizar_plasticos(item, postext, scroll);
      }

      if (str == '-') {
        my_lcd.Set_Draw_color(color_2[0], color_2[1], color_2[2]);
        if (item > 0) {
          my_lcd.Fill_Rectangle(10, tabla_y0 + plastY * item, WIDTH - 10, tabla_y0 + plastY * (item + 1) - 1);

          item -= 1;
        }
        else if (postext > 0) {
          for (int i = postext; i < plast_vis + postext ; i++) {
            show_string(list_plasticos[i], 20, plast_y0 + plastY * (i - postext), 2, conv_color(color_2));
          }
          my_lcd.Fill_Rectangle(WIDTH - 15, tabla_y0 + scroll, WIDTH - 10, tabla_y0 + scrollY + scroll - 1);

          postext -= 1;
          scroll = plast_vis * plastY * postext / plast_tot;
        }
        Actualizar_plasticos(item, postext, scroll);
      }
    }

  }


  /***********************************************************************************************************************/
  if (pantalla == control) {

    int var = 0;
    accion = setear;

    my_lcd.Fill_Screen(blanco);
    Mostrar_ChemiTech();

    //botones
    my_lcd.Set_Draw_color(color_3[0], color_3[1], color_3[2]);
    Round_Rectangle(50, 40, 150, 80, "SETEAR", 2, blanco);
    Round_Rectangle(240 - 50, 40, 240 + 50, 80, "LEER", 2, blanco);
    Round_Rectangle(330, 40, 430, 80, "GUARDAR", 2, blanco);
    my_lcd.Set_Draw_color(color_1[0], color_1[1], color_1[2]);
    Round_Rectangle(50, 40, 150, 80, "SETEAR", 2, blanco);


    //tabla
    my_lcd.Set_Draw_color(color_3[0], color_3[1], color_3[2]);
    my_lcd.Fill_Rectangle(50, tabla2_y0, 180, HEIGHT - 21);

    my_lcd.Set_Draw_color(color_1[0], color_1[1], color_1[2]);
    my_lcd.Fill_Rectangle(50, tabla2_y0, 180, tabla2_y0 + varY - 1);
    my_lcd.Fill_Rectangle(181, tabla2_y0, 430, HEIGHT - 21);

    for (int i = 0; i < 5; i++) {
      show_string(variables[i], 60, var_y0 + varY * i, 2, blanco);
    }

    //display
    my_lcd.Set_Draw_color(255, 255, 255);
    my_lcd.Fill_Rectangle(200, 115, 410, 155);

    //boton set
    my_lcd.Set_Draw_color(color_2[0], color_2[1], color_2[2]);
    Rectangle(375, 250, 420, 290, "SET", 2, blanco);


    //teclado
    Dibujar_teclado();

    while (pantalla == control) {

      str = readS();
      if (str == 'z') {
        pantalla = inicio;
      }
      else if (str == 'x') {
        pantalla = plasticos;
      }
      else if (str == 'c') {
        pantalla = control;
      }
      else if (str == 'v') {
        pantalla = datos;
      }

      if (str == 's') {
        Actualizar_control(accion);
        accion = setear;   
        my_lcd.Set_Draw_color(color_1[0], color_1[1], color_1[2]);
        Round_Rectangle(50, 40, 150, 80, "SETEAR", 2, blanco);

        my_lcd.Set_Draw_color(color_1[0], color_1[1], color_1[2]);
        my_lcd.Fill_Rectangle(teclado_x0, teclado_y0, 420, 289);
        Dibujar_teclado();
        my_lcd.Set_Draw_color(color_2[0], color_2[1], color_2[2]);
        Rectangle(375, 250, 420, 289, "SET", 2, blanco);

      }
      else if (str == 'l') {
        Actualizar_control(accion);
        accion = leer;
        my_lcd.Set_Draw_color(color_1[0], color_1[1], color_1[2]);
        Round_Rectangle(240 - 50, 40, 240 + 50, 80, "LEER", 2, blanco);
        
        my_lcd.Set_Draw_color(color_1[0], color_1[1], color_1[2]);
        my_lcd.Fill_Rectangle(teclado_x0, teclado_y0, 420, 289);
      }
      else if (str == 'g') {
        Actualizar_control(accion);
        accion = guardar;
        my_lcd.Set_Draw_color(color_1[0], color_1[1], color_1[2]);
        Round_Rectangle(330, 40, 430, 80, "GUARDAR", 2, blanco);

        my_lcd.Set_Draw_color(color_1[0], color_1[1], color_1[2]);
        my_lcd.Fill_Rectangle(teclado_x0, teclado_y0, 420, 289);

        my_lcd.Set_Draw_color(color_2[0], color_2[1], color_2[2]);
        Rectangle(180 + 250 / 2 - 100, 170, 180 + 250 / 2 + 100, 220, "INICIAR", 2, blanco);
        Rectangle(180 + 250 / 2 - 100, 230, 180 + 250 / 2 + 100, 280, "TERMINAR", 2, blanco);
      }


      my_lcd.Set_Draw_color(color_3[0], color_3[1], color_3[2]);
      if (str == 'q') {
        my_lcd.Fill_Rectangle(50, tabla2_y0 + varY * var, 180, tabla2_y0 + varY * (var + 1) - 1);
        var = 0;
        Actualizar_variables(var);
      }
      else if (str == 'w') {
        my_lcd.Fill_Rectangle(50, tabla2_y0 + varY * var, 180, tabla2_y0 + varY * (var + 1) - 1);
        var = 1;
        Actualizar_variables(var);
      }
      else if (str == 'e') {
        my_lcd.Fill_Rectangle(50, tabla2_y0 + varY * var, 180, tabla2_y0 + varY * (var + 1) - 1);
        var = 2;
        Actualizar_variables(var);
      }
      else if (str == 'r') {
        my_lcd.Fill_Rectangle(50, tabla2_y0 + varY * var, 180, tabla2_y0 + varY * (var + 1) - 1);
        var = 3;
        Actualizar_variables(var);
      }
      else if (str == 't') {
        my_lcd.Fill_Rectangle(50, tabla2_y0 + varY * var, 180, tabla2_y0 + varY * (var + 1) - 1);
        var = 4;
        Actualizar_variables(var);
      }



    }

  }


  /***********************************************************************************************************************/
  if (pantalla == datos) {
    my_lcd.Fill_Screen(conv_color(color_1));
    show_string("Les dates", 0, 0, 2, 0);

    while (pantalla == datos) {

      str = readS();
      if (str == 'z') {
        pantalla = inicio;
      }
      else if (str == 'x') {
        pantalla = plasticos;
      }
      else if (str == 'c') {
        pantalla = control;
      }
      else if (str == 'v') {
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
  uint16_t len = (strlen(str) * 6 - 1) * csize;
  int16_t posx = x1 + (x2 - x1 + 1 - len) / 2;
  int16_t posy = y1 + (y2 - y1 + 1 - 7 * csize) / 2;
  show_string(str, posx, posy, csize, cText);
}

void Round_Rectangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t *str, uint8_t csize, uint16_t cText)
{
  my_lcd.Fill_Round_Rectangle(x1, y1, x2, y2, 10);
  uint16_t len = (strlen(str) * 6 - 1) * csize;
  int16_t posx = x1 + (x2 - x1 + 1 - len) / 2;
  int16_t posy = y1 + (y2 - y1 + 1 - 7 * csize) / 2 ;
  show_string(str, posx, posy, csize, cText);
}


void Mostrar_ChemiTech() {
  my_lcd.Set_Draw_color(color_2[0], color_2[1], color_2[2]);
  my_lcd.Fill_Rectangle(0, 0, WIDTH, 25);
  show_string("ChemiTech", WIDTH - 120, 5, 2, blanco);
}

void Actualizar_plasticos(int item, int postext, int scroll) {
  //pintar fila
  my_lcd.Set_Draw_color(color_1[0], color_1[1], color_1[2]);
  my_lcd.Fill_Rectangle(10, tabla_y0 + plastY * item, WIDTH - 10, tabla_y0 + plastY * (item + 1) - 1);
  //pintar scroll
  my_lcd.Set_Draw_color(color_3[0], color_3[1], color_3[2]);
  my_lcd.Fill_Rectangle(WIDTH - 15, tabla_y0 + scroll, WIDTH - 10, tabla_y0 + scrollY + scroll - 1);
  //llenar plasticos
  for (int i = postext; i < plast_vis + postext ; i++) {
    show_string(list_plasticos[i], 20, plast_y0 + plastY * (i - postext), 2, blanco);
  }
}

void Actualizar_variables(int item) {
  //pintar fila
  my_lcd.Set_Draw_color(color_1[0], color_1[1], color_1[2]);
  my_lcd.Fill_Rectangle(50, tabla2_y0 + varY * item, 180, tabla2_y0 + varY * (item + 1) - 1);
  //llenar plasticos
  for (int i = 0; i < 5 ; i++) {
    show_string(variables[i], 60, var_y0 + varY * i, 2, blanco);
  }
}

void Actualizar_control(enum action act) {

  my_lcd.Set_Draw_color(color_3[0], color_3[1], color_3[2]);

  if (act == setear) {
    Round_Rectangle(50, 40, 150, 80, "SETEAR", 2, blanco);
  }
  else if (act == leer) {
    Round_Rectangle(240 - 50, 40, 240 + 50, 80, "LEER", 2, blanco);
  }
  else if (act == guardar) {
    Round_Rectangle(330, 40, 430, 80, "GUARDAR", 2, blanco);
  }
}

void Dibujar_teclado() {
  my_lcd.Set_Draw_color(color_2[0], color_2[1], color_2[2]);
  my_lcd.Fill_Rectangle(teclado_x0, teclado_y0, teclado_x0 + tecX * 4 - 1, teclado_y0 + tecY * 3 - 1);

  my_lcd.Set_Draw_color(255, 255, 255);
  my_lcd.Draw_Line(teclado_x0, teclado_y0 + tecY, teclado_x0 + 4 * tecX - 1, teclado_y0 + tecY);
  my_lcd.Draw_Line(teclado_x0, teclado_y0 + 2 * tecY, teclado_x0 + 4 * tecX - 1, teclado_y0 + 2 * tecY);
  my_lcd.Draw_Line(teclado_x0 + tecX, teclado_y0, teclado_x0 + tecX, teclado_y0 + 3 * tecY - 1);
  my_lcd.Draw_Line(teclado_x0 + 2 * tecX, teclado_y0, teclado_x0 + 2 * tecX, teclado_y0 + 3 * tecY - 1);
  my_lcd.Draw_Line(teclado_x0 + 3 * tecX, teclado_y0, teclado_x0 + 3 * tecX, teclado_y0 + 3 * tecY - 1);

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 4; j++) {
      show_string(simbolos[i * 4 + j], tec_x0 + j * tecX, tec_y0 + i * tecY , 2, blanco);
    }
  }
}
