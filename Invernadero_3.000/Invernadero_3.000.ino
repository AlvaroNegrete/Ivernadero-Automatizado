  //~~~~~~~~~~~~~~~~~~~~ bmp280 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include <Adafruit_BusIO_Register.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include <Adafruit_SPIDevice.h>

#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp;
int TEMP;                 // se crea una variable para guardar la temperatura
//~~~~~~~~~~~~~~~~~~~~ Controlador y bombas de agua ~~~~~~~~~~~~
int IN1 = 30;      // IN1 a pin digital 30
int IN2 = 32;      // IN2 a pin digital 32
int IN3 = 34;      // IN3 a pin digital 34
int IN4 = 36;      // IN4 a pin digital 36
//~~~~~~~~~~~~~~~~~~~~~~ BLUETOOTH ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include <SoftwareSerial.h>
SoftwareSerial bluetooth (10, 11);
//~~~~~~~~~~~~~~~~~~~~~~ Temperatura de liquidos (DS18B20)~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*#include <OneWire.h>
  #include <DallasTemperature.h>
  OneWire ourWire (38);
  DallasTemperature STA (&ourWire);*/

//~~~~~~~~~~~~~~~~~~~~LDR~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int LDR = A0;
int Valor;
bool inicio = true;
bool fin = true;

//~~~~~~~~~~~~~~~~~~~~  RELE  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# define Rele1 3        //se define un valor constante para el rele 1      
# define Rele2 4          //se define un valor constante para el rele 2
# define Rele3 7
String luzled;
String calefaccion;
//~~~~~~~~~~~~~~~~~~~~  RELOJ DE TIEMPO REAL ~~~~~~~~~~~~~~~~~~~~~
#include <RTClib.h>
RTC_DS3231 rtc;           //se crea un nombre para el RTC
int horas;
int minutos;
int horasLUZ;
//~~~~~~~~~~~~~~~~~~~~  LDC  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd (0x27, 2, 1, 0, 4, 5, 6, 7);  //se asigna nombre a lcd
//~~~~~~~~~~~~~~~~~~~~ SENSOR DE TEMPERATURA Y HUMEDAD  ~~~~~~~~~~~
#include <DHT.h>
#include <DHT_U.h>
#define DATA 2            //se crea una contante para recibir los datos de DHT
DHT dht (DATA, DHT11);    //se crea un nombre para con la dirccion del pin y el tipo de sensor a utilizar
int HUM;                  // se crea una variable para guardar la humedad
//~~~~~~~~~~~~~~~~~~~~  TECLADO MATRICIAL  ~~~~~~~~~~~~~~~~~~~~~~~~
#include <Key.h>
#include <Keypad.h>             //se incluyen las librerias para le teclado
const byte FILAS = 4;           // Se crea una constante para representar las 4 filas del teclado
const byte COLUMNAS = 4;        // Se crea una constante para representar las 4 columnas del teclado
char keys [FILAS][COLUMNAS] = { //se comienza un arreglo llamada "keys" para determinar el orden que tienen las teclas
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'c'},
  {'*', '0', '#', 'D'},
};
byte pinFILAS[FILAS] = {22, 24, 26, 28};  //se define el numero de pin que esta ocupando cada una de las filas (para no poner linea por linea se usa un array)
byte pinCOLUMNAS [COLUMNAS] = {23, 25, 27, 29}; //se define el numero de pin que esta ocupando cada una de las columnas
Keypad teclado = Keypad(makeKeymap (keys), pinFILAS, pinCOLUMNAS, FILAS, COLUMNAS);
// Se crea un objeto llamado "teclado", con una funcion de la libreria para poder llamarlo posteriormente
// Se usa una funcion map para vincular los pines conectados, al arreglo "keys" y fijar un orden entre estos
//para referirnos al teclado, ahora solo se necesita llamar a "teclado" para poder leer su valor.
char tecla; //se crea una variable para guardar la tecla presionada en  le teclado
//~~~~~~~~~~~~~~~~~~~~  NIVEL DE LIQUIDOS  ~~~~~~~~~~~~~~~~~~~~~~~~
int Sbajo = 31;
int Splanta = 33;
int Salto = 35;
int bajo;
int medio;
int alto;


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~






























void setup() {
  bluetooth.begin(38400);
  Serial.begin (9600);
  //~~~~~~~~~~~~~~~~~~~~  RELE  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  pinMode (Rele1, OUTPUT);          //define como salida el rele1
  digitalWrite (Rele1, HIGH);       //se pone en un estado alto (apagado)el rele para evitar errores

  pinMode (Rele2, OUTPUT);          //define como salida el rele1
  digitalWrite (Rele2, HIGH);       //se pone en un estado alto (apagado)el rele para evitar errores

  //~~~~~~~~~~~~~~~~~~~~  RELOJ DE TIEMPO REAL ~~~~~~~~~~~~~~~~~~~~~
  if (!rtc.begin()) {               //si no se iniciliza el RTC...
    lcd.print ("no encontrado");    //...avisar al usuario que algo esta mal con el RTC
    while (1);
  }
  //
  // rtc.adjust (DateTime (__DATE__ , __TIME__));  // ajuste de la fecha sacandola desde el sistema de la pc
  //~~~~~~~~~~~~~~~~~~~~  LDC  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  lcd.setBacklightPin(3, POSITIVE); //Define cual es el pin y el estado para controlar el contraste
  lcd.setBacklight(HIGH);           //Se define como va a permanecer el fondo (encendido o apagado)
  lcd.begin(16, 2);                 //inicializa el lcd y define que lcd se va a utilizar
  lcd.clear();                      //vacía el lcd
  //~~~~~~~~~~~~~~~~~~~~ SENSOR DE TEMPERATURA Y HUMEDAD  ~~~~~~~~~~~
  dht.begin();
  //~~~~~~~~~~~~~~~~~~~~  NIVEL DE LIQUIDOS  ~~~~~~~~~~~~~~~~~~~~~~~~
  pinMode (Sbajo, INPUT);
  pinMode (Salto, INPUT);
  pinMode (Splanta, INPUT);
  //~~~~~~~~~~~~~~~~~~~~ Temperatura de liquidos ~~~~~~~~~~~~~~~~~~~~~
  //STA.begin();
  //~~~~~~~~~~~~~~~~~~~~ Brienvenida ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  lcd.setCursor(2, 0);
  lcd.print ("INVERNADERO");
  lcd.setCursor(1, 1);
  lcd.print ("AUTOMATIZADO");
  delay (3000);
  lcd.clear();
  //~~~~~~~~~~~~~~~~~~~~ Controlador y bombas de agua~~~~~~~~~~~~~~~~~~~~~~
  pinMode(IN1, OUTPUT);   // IN1 como salida
  pinMode(IN2, OUTPUT);   // IN2 como salida
  pinMode(IN3, OUTPUT);   // IN3 como salida
  pinMode(IN4, OUTPUT);   // IN4 como salida
  //~~~~~~~~~~~~~~~~~~~~  BMP280  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (!bmp.begin()) {
    lcd.clear();
    lcd.print("FALLA conexion");
    while (1);
  }
}


































void loop() {
  TEMP = bmp.readTemperature();
  HUM = dht.readHumidity();
  DateTime Fecha = rtc.now();
  tecla = teclado.getKey();                   //lee lo que se presione en el teclado, y lo guarda en la variable tecla
  bajo = (digitalRead (Sbajo));               //guardar el estado del pin 31 en una variable "bajo"
  medio = (digitalRead (Splanta));            //guardar el estado del pin 33 en una variable "medio"
  alto = (digitalRead (Salto));               //guardar el estado del pin 35 en una variable "alto"
  Valor = analogRead(LDR);
  horas = Fecha.hour();
  minutos = Fecha.minute();

  lcd.setCursor(2, 0);
  lcd.print ("Seleccionar");
  lcd.setCursor(4, 1);
  lcd.print ("Opcion");
  if (Rele1 == 0)
    luzled = "encendido" ;
  else (luzled = "apagado") ;
  if (Rele2 == 0)
    calefaccion = "encendido";
  else (calefaccion = "apagado");
  bluetooth.print(TEMP);
  bluetooth.print(",");
  bluetooth.print(HUM);
  bluetooth.print(",");
  bluetooth.print(calefaccion);
  bluetooth.print(",");
  //~~~~~~~~~~~~~~~~~~ENCENDER LUZ LED SI NO HAY LUZ DE DIA~~~~~~~~~~~~~~~~~~
  Serial.print ("Valor0=");
  Serial.println (Valor);

  if (Valor > 500 && horas > 11 && horas < 23) {
      digitalWrite(Rele1, LOW);
      delay (200);
      
    }
  
  else {
  digitalWrite(Rele1, HIGH);
    delay (200);
  }

  //~~~~~~~~~~~~~~~~~~ENCENDER LAMPARA INCANDECENTE SI LA TEMPERATURA ES MENOR A 18~~~~~~~~~~~~~~~~~~
  if (TEMP < 18) {
    digitalWrite (Rele2, HIGH);
    if (TEMP <= 23)
      digitalWrite (Rele2, HIGH);
    if (TEMP == 24)
      digitalWrite (Rele2, LOW);
  }

  //~~~~~~~~~~~~~~~~~~VENTILADORES~~~~~~~~~~~~~~~~~~
  if ( TEMP >= 20) {
    digitalWrite (Rele3, LOW);
  }
  if (TEMP < 18) {
  }

  switch (tecla) {              //comprobar el estado de la variable "tecla"...

    //~~~~~~~~~~~~~~~~~~~~  CASO NUMERO UNO  ~~~~~~~~~~~~~~~~~~~~~~~~
    //~~~~~~~~~~~~~~~~~~~~  MOSTRAR HORA ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    case '1' :                    //si la tecla es igual a 1
      lcd.clear();                //limpia el lcd
      hora();                     //llama al programa que muestra la hora
      delay (3000);               //esperar 3 segundos
      lcd.clear();                //limpiar lcd
      break;                      //salir del caso 1
    //~~~~~~~~~~~~~~~~~~~~  CASO NUMERO DOS  ~~~~~~~~~~~~~~~~~~~~~~~~
    //~~~~~~~~~~~~~~~~~~~~  MOSTRAR TEMPERATURA Y HUMEDAD ~~~~~~~~~~~
    case '2':
      lcd.clear();                //limpiar lcd
      Temp_Hum();                 //llama al programa de muestra la temperatura y la humedad
      delay (3000);               //esperar 3 segundos
      lcd.clear();                //limpiar lcd
      break;                      //salir del caso 2
    //~~~~~~~~~~~~~~~~~~~~  CASO NUMERO TRES  ~~~~~~~~~~~~~~~~~~~~~~~~
    //~~~~~~~~~~~~~~~~~~~~  MOSTRAR NIVEL DE LIQUIDO ~~~~~~~~~~~~~~~~~
    case '3':
      lcd.clear();                                //limpiar el ldc
      if (bajo == 1 && alto == 0) {   //Si el agua llega solo al primer cables escirbir "nivel de liquido bajo"
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("NIVEL DE LIQUIDO");
        lcd.setCursor(6, 1);
        lcd.print("BAJO");
        delay (3000);
        lcd.clear();
      }

      else if  (bajo == 1 && medio == 1 && alto == 1) {   //Si el agua llega solo a todos los cables escirbir "nivel de liquido alto"
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("NIVEL DE LIQUIDO");
        lcd.setCursor(6, 1);
        lcd.print("ALTO");
        delay (3000);
        lcd.clear();
      }
      else {  //si el agua no llega a ningun cable escribir que no hay liquidos en el tanque
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("SIN LIQUIDOS");
        delay (3000);
        lcd.clear();
      }
      /*
         //~~~~~~~~~~~~~~Temperatura de liquidos~~~~~~~~~~~~~~~~~~~~~
           case '4':
             STA.requestTemperatures();
             float TempA=STA.getTempCByIndex(0);
             lcd.clear();
             lcd.setCursor(0,0);
             lcd.print(TempA);
             delay (3000);
             break;
      */
  }
}


//############################################################################################
void hora() {
  DateTime Fecha = rtc.now(); //guarda en "fecha" todos los datos de la fehca y hora actual.
  lcd.setCursor(0, 0);        //se poiciona en la fila 0 y columna 0...
  lcd.print (Fecha.hour());   //...la hora actual
  lcd.setCursor(3, 0);        //se pociciona en la fila 0 y columna 3...
  lcd.print (":");            // unos ":" para separar la hora de los minutos
  lcd.setCursor(4, 0);        //se posiciona en la fila 0 y la columna 4...
  lcd.print (Fecha.minute()); //...los minutos actuales
}
//############################################################################################
void Temp_Hum() {
  //~~~~~~~~~~~~~~~~~~~~  Lectura de temperatura  ~~~~~~~~~~~~~~~~~~
  //leer la temperatura, y guardarla en una variable
  lcd.setCursor(13, 0);
  lcd.print (TEMP);               //escibir el valor guardado en el ldc
  lcd.setCursor(0, 0);
  lcd.print("TEMPERATURA:");
  //~~~~~~~~~~~~~~~~~~~~  Lectura de humedad  ~~~~~~~~~~~~~~~~~~~~~~
  //Leer la humedad, y guardarla en una variable
  lcd.setCursor(13, 1);
  lcd.print (HUM);                //escribir el valor guardado en el lcd
  lcd.setCursor(0, 1);
  lcd.print("HUMEDAD:");
}
//############################################################################################
