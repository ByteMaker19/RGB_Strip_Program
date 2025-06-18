#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20,4);

#define R 3
#define G 5
#define B 6
#define CAMBIO_MODO 10
#define CAMBIO_INTENSIDAD 9

bool modo[5]{1,0,0,0,0};
bool seleccion[3]{1,0,0};
bool nuevoModo;

bool pantallaR;
bool pantallaB;
bool pantallaG;
bool pantallaMulti;
bool pantallasb;

int intensidad_r = 130;
int intensidad_g = 130;
int intensidad_b = 130;
int velocidad = 50;

bool selred;
bool selgren;
bool selblue;

// Variables para antirrebote
bool estadoAnteriorModo = HIGH;
unsigned long ultimaLecturaModo = 0;
const unsigned long debounceTiempo = 50;

bool estadoAnteriorIntensidad = HIGH;
unsigned long ultimaLecturaIntensidad = 0;
bool presionadoIntensidad = false;

void setup(){
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(CAMBIO_MODO, INPUT_PULLUP);
  pinMode(CAMBIO_INTENSIDAD, INPUT_PULLUP);
  lcd.init();
  lcd.backlight();
  pantallaPrincipal();
  Serial.begin(9600);
}

void loop(){
  actualizarBotones();  // actualiza estados de los botones

  enviarColores();
  cambiarColores();
  cambiarIntensidad();
  cambiarPantalla();
 if(pantallasb)pantallaSB();
 if(modo[4] && presionadoIntensidad){
  Serial.print("Seleccion actual: ");
  if(seleccion[0]) Serial.println("RED");
  if(seleccion[1]) Serial.println("GREEN");
  if(seleccion[2]) Serial.println("BLUE");
}

  //char buffer[50];
  //sprintf(buffer, "LEDR = %d, LEDG = %d, LEDB = %d", intensidad_r, intensidad_g, intensidad_b);
  //Serial.println(buffer);
}

void pantallaPrincipal(){
  lcd.setCursor(0,0);
  lcd.print("CONTROL LEDS");
  lcd.setCursor(0,1);
  lcd.print("Intensidad RED: ");
  lcd.print(intensidad_r);
}

void enviarColores() {
  static unsigned long previousMillis = 0;
  static int estado = 0;
  unsigned long currentMillis = millis();

  if (modo[0]) {
    analogWrite(R, intensidad_r);
    analogWrite(G, 0);
    analogWrite(B, 0);
  }
  else if (modo[1]) {
    analogWrite(R, 0);
    analogWrite(G, intensidad_g);
    analogWrite(B, 0);
  }
  else if (modo[2]) {
    analogWrite(R, 0);
    analogWrite(G, 0);
    analogWrite(B, intensidad_b);
  }
  else if (modo[3]) {
    if (currentMillis - previousMillis >= velocidad) {
      previousMillis = currentMillis;
      analogWrite(R, 0);
      analogWrite(G, 0);
      analogWrite(B, 0);

      switch (estado) {
        case 0: analogWrite(R, intensidad_r); break;
        case 1: analogWrite(G, intensidad_g); break;
        case 2: analogWrite(B, intensidad_b); break;
      }
      estado = (estado + 1) % 3;
    }
  }
}

void cambiarColores(){
  if(nuevoModo && modo[4]== 1 ){
    modo[0] = 1; modo[1] = 0; modo[2] = 0; modo[3] = 0; modo[4] = 0;
    pantallaR = 1; nuevoModo = 0;
  }

  if(nuevoModo && modo[0]== 1 ){
    modo[0] = 0; modo[1] = 1; modo[2] = 0; modo[3] = 0; modo[4] = 0;
    pantallaG = 1; nuevoModo = 0;
  }

  if(nuevoModo && modo[1]== 1 ){
    modo[0] = 0; modo[1] = 0; modo[2] = 1; modo[3] = 0; modo[4] = 0;
    pantallaB = 1; nuevoModo = 0;
  }

  if(nuevoModo && modo[2]== 1 ){
    modo[0] = 0; modo[1] = 0; modo[2] = 0; modo[3] = 1; modo[4] = 0;
    pantallaMulti = 1; nuevoModo = 0;
  }

  if(nuevoModo && modo[3]== 1 ){
    modo[0] = 0; modo[1] = 0; modo[2] = 0; modo[3] = 0; modo[4] = 1;
    pantallaSB(); pantallasb = 1; nuevoModo = 0;
  }
}

void cambiarIntensidad(){
  if(modo[0] && presionadoIntensidad){
    intensidad_r += 10;
    pantallaR = 1;
  }

  if(modo[1] && presionadoIntensidad){
    intensidad_g += 10;
    pantallaG = 1;
  }

  if(modo[2] && presionadoIntensidad){
    intensidad_b += 10;
    pantallaB = 1;
  }

  if(modo[3] && presionadoIntensidad){
    velocidad += 50;
    pantallaMulti = 1;
  }

  if(seleccion[2]==1 && presionadoIntensidad){
    seleccion[0] = 1; seleccion[1] = 0; seleccion[2] = 0;
  }

  if(seleccion[0]==1 && presionadoIntensidad){
    seleccion[0] = 0; seleccion[1] = 1; seleccion[2] = 0;
  }

  if(seleccion[1]==1 && presionadoIntensidad){
    seleccion[0] = 0; seleccion[1] = 0; seleccion[2] = 1;
  }

  if(modo[4] && seleccion[2] && presionadoIntensidad){
    seleccion[0] = 1;
    seleccion[1] = 0;
    seleccion[2] = 0;
    selred = 1;
  }
    if(modo[4] && seleccion[0] && presionadoIntensidad){
    seleccion[0] = 0;
    seleccion[1] = 1;
    seleccion[2] = 0;
    selgren = 1;
  }
    if(modo[4] && seleccion[1] && presionadoIntensidad){
    seleccion[0] = 0;
    seleccion[1] = 0;
    seleccion[2] = 1;
    selblue = 1;
  }

  if(intensidad_r>250) intensidad_r = 0;
  if(intensidad_g>250) intensidad_g = 0;
  if(intensidad_b>250) intensidad_b = 0;
  if(velocidad>250) velocidad = 100;
}

void cambiarPantalla(){
  if(pantallaR){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("CONTROL LEDS");
    lcd.setCursor(0,1);
    lcd.print("Intensidad RED: ");
    lcd.print(intensidad_r);
    pantallaR = 0;
  } 

  if(pantallaG){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("CONTROL LEDS");
    lcd.setCursor(0,1);
    lcd.print("Intensidad GREN: ");
    lcd.print(intensidad_g);
    pantallaG = 0;
  }

  if(pantallaB){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("CONTROL LEDS");
    lcd.setCursor(0,1);
    lcd.print("Intensidad BLUE: ");
    lcd.print(intensidad_b);
    pantallaB = 0;
  }

  if(pantallaMulti){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("CONTROL LEDS");
    lcd.setCursor(0,1);
    lcd.print("Multicolor");
    lcd.setCursor(0,2);
    lcd.print("Velocidad:" );
    lcd.print(velocidad);
    lcd.print("s");
    pantallaMulti = 0;
  }

    if(selred){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("CONTROL LEDS");
      lcd.setCursor(0,1);
      lcd.print("Sube y baja");
      lcd.setCursor(0,2);
      lcd.print("Color: RED ");
      selred = 0;
    }
    if(selgren){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("CONTROL LEDS");
      lcd.setCursor(0,1);
      lcd.print("Sube y baja");
      lcd.setCursor(0,2);
      lcd.print("Color: GREN ");
      selgren = 0;
    }
  
    if(selblue){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("CONTROL LEDS");
      lcd.setCursor(0,1);
      lcd.print("Sube y baja");
      lcd.setCursor(0,2);
      lcd.print("Color: BLUE ");
      selblue = 0;
    }

}

void pantallaSB(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("CONTROL LEDS");
  lcd.setCursor(0,1);
  lcd.print("Sube y baja");
  lcd.setCursor(0,2);
  lcd.print("Color: ");



  pantallasb = 0;
}

// ✅ NUEVA función que gestiona el antirrebote para ambos botones
void actualizarBotones() {
  // CAMBIO_MODO
  bool lecturaModo = digitalRead(CAMBIO_MODO);
  if (lecturaModo != estadoAnteriorModo) {
    if (millis() - ultimaLecturaModo > debounceTiempo) {
      if (lecturaModo == LOW) {
        nuevoModo = true;
      }
      ultimaLecturaModo = millis();
    }
  }
  estadoAnteriorModo = lecturaModo;

  // CAMBIO_INTENSIDAD
  bool lecturaIntensidad = digitalRead(CAMBIO_INTENSIDAD);
  if (lecturaIntensidad != estadoAnteriorIntensidad) {
    if (millis() - ultimaLecturaIntensidad > debounceTiempo) {
      if (lecturaIntensidad == LOW) {
        presionadoIntensidad = true;
      }
      ultimaLecturaIntensidad = millis();
    }
  }
  else {
    presionadoIntensidad = false;  // solo se activa una vez por pulsación
  }
  estadoAnteriorIntensidad = lecturaIntensidad;
}