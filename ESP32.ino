// Adafruit IO Publish Example
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.

/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
#include "config.h" //Se ejecuta la configuracion del ESP32, Red y claves del IOT Cloud

/************************ Programa principal *******************************/
int raw_temp = 0;
int incomingByte = 0;
String led_state;     //Se crean variables para recibir el Tx del PIC, y del estado de la led del servidor

AdafruitIO_Feed *temp = io.feed("Temp");
AdafruitIO_Feed *Led_Verde = io.feed("Led_Verde");
AdafruitIO_Feed *Led_Roja = io.feed("Led_Roja");  //Se crean las instancias para los feeds


void handleMessage(AdafruitIO_Data *data) { //Funcion que se encarga de recibir los datos que se envian
    //Serial.print("Recibiendo <- ");       //desde la IOT cloud y se almacenan para posteriormente compararlos
    //Serial.println(data->value());        //en un if que enviara datos al PIC
    led_state = data->value();
    //Serial.println(led_state);
}


void setup() 
{
  pinMode(2, OUTPUT); //Se establece led 2, BUILDIN, para debugueo
  Serial.begin(9600); //Se inicia la comunicacion serial a 9600 baudios

  //Serial.print("Connecting to Adafruit IO");
  // connect to io.adafruit.com
  io.connect();       //Se inicia la conexion a IOT de adafruit

  Led_Verde->onMessage(handleMessage);
  Led_Roja->onMessage(handleMessage); //Se llama a la funcion cuando se reciben datos de los feeds
  
  // wait for a connection
  while(io.status() < AIO_CONNECTED) { //While para que no avanze si no conecta a la nube
    digitalWrite(2,HIGH);             //Enciende mientras espera la conexion exitosa de la nube
    delay(500);
  }
  digitalWrite(2,LOW);                //Apaga una vez conectado

  Led_Verde->get();
  Led_Roja->get();                  //Obtenemos datos de la Feed siempre que existan cambios
}

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();

  if (Serial.available() > 0) {       //Si recibe datos desde UART
    // read the incoming byte:  
    incomingByte = Serial.read();     //Almacena la lectura serial
    //Serial.print("Entrada Uart: ");
    //Serial.println(incomingByte, HEX);
    //Serial.print("Enviando -> ");
    //Serial.println((incomingByte, HEX));
    temp->save((incomingByte, DEC)); //La envia a la nube, al feed
  }


  switch (led_state.toInt()) { //Switch case para los datos recibidos de los botones de la nube
  case 00:
    //Serial.println("Led Roja apagada");
    Serial.write(0X0A);       //escribe el valor por uart que se envia al pic para encender o apagar leds
    break;
  case 01:
    //Serial.println("Led Roja Encendida");
    Serial.write(0X0B);
    break;
    case 10:
    //Serial.println("Led Verde Apagada");
    Serial.write(0X0C);
    break;
    case 11:
    //Serial.println("Led Verde Encendida");
    Serial.write(0X0D);
    break;
  default:
    break;
}

  delay(3000);

}
