/*
 * File:   I2C.c
 * Author: Katherine Caceros
 *
 * Created on 7 de marzo de 2021, 03:31 PM
 */

// Ajustes de bits de configuración PIC16F887

// Declaraciones de configuración de la línea fuente 'C'

// CONFIG1
# pragma config FOSC = INTRC_NOCLKOUT // Bits de selección del oscilador (oscilador INTOSCIO: función de E / S en el pin RA6 / OSC2 / CLKOUT, función de E / S en RA7 / OSC1 / CLKIN)
# pragma config WDTE = OFF        // Bit de habilitación del temporizador de vigilancia (WDT deshabilitado y puede habilitarse mediante el bit SWDTEN del registro WDTCON)
# pragma config PWRTE = OFF       // Bit de habilitación del temporizador de encendido (PWRT deshabilitado)
# pragma config MCLRE = OFF       // Bit de selección de función de pin RE3 / MCLR (la función de pin RE3 / MCLR es entrada digital, MCLR internamente vinculado a VDD)
# pragma config CP = OFF          // Bit de protección de código (la protección de código de memoria del programa está deshabilitada)
# pragma config CPD = OFF         // Bit de protección de código de datos (la protección de código de memoria de datos está deshabilitada)
# pragma config BOREN = OFF       // Bits de selección de reinicio de Brown Out (BOR deshabilitado)
# pragma config IESO = OFF        // Bit de conmutación interno externo (el modo de conmutación interno / externo está deshabilitado)
# pragma config FCMEN = OFF // Bit habilitado del monitor de reloj a prueba de fallas (el monitor de reloj a prueba de fallas       está deshabilitado)
# pragma config LVP = OFF         // Bit de habilitación de programación de bajo voltaje (el pin RB3 tiene E / S digital, HV en MCLR debe usarse para la programación)

// CONFIG2
# pragma config BOR4V = BOR40V    // Bit de selección de reinicio de bajada (Brown-out Reset establecido en 4.0V)
# pragma config WRT = OFF         // Bits de habilitación de autoescritura de memoria de programa flash (protección contra escritura desactivada)

#define  _XTAL_FREQ    4000000


// Las declaraciones de configuración #pragma deben preceder a los archivos de proyecto incluidos.
// Usa enumeraciones del proyecto en lugar de #define para ON y OFF.

#include  <xc.h>
#include  "I2C.h"

char sensor_dir = 0xEC ; // ID del sensor

char TEMP_LSB = 0 ;
char led = 0 ;       // Variables para almacenar datos de UART y del I2C


void setup(void); //Se crean prototipos
void setup(void) {
    ANSEL = 0;
    ANSELH = 0;         //Puertos en Digital

    TRISA = 0x00;
    TRISB = 0x00;
    TRISC = 0x00;       //Puertos como inputs
    TRISCbits.TRISC3 = 0; //RC3 salida para SCL
    TRISCbits.TRISC7 = 1; //RC7 como entrada para RX
    TRISD = 0x00;
    TRISE = 0x00;
    PORTE = 0x00;       //Se limpia puerto para leds

    I2C_Master_Init(9); //Se configura direccion e I2C

    TXSTAbits.BRGH = 1; //Baudrate 9600 
    BAUDCTLbits.BRG16 = 0;
    SPBRG = 25;

    //Transmision
    TXSTAbits.TXEN = 1; //Se habilita TX
    TXSTAbits.SYNC = 0; //modo Asíncrono
    RCSTAbits.SPEN = 1; //Se habilita RX
    TXSTAbits.TX9 = 0; //Se transmiten 8 bits

    //Lectura
    RCSTAbits.CREN = 1; //Se habilita recibir datos
    RCSTAbits.RX9 = 0; //Se reciben solo 8 bits

    //ENCENDEMOS INTERRUPCIONES
    PIE1bits.RCIE = 1;
    PIE1bits.TXIE = 0; //No se habilitan interrupciones en el envio
    PIR1bits.RCIF = 0; //Se apaga la interrupcion

    INTCONbits.GIE = 1; //Interrupciones del timer
    return;

}

void __interrupt() ISR(void) {
    if (PIR1bits.RCIF == 1) {
        PIR1bits.RCIF = 0; //Resetea bandera RCIF
        led = RCREG;       //Se lee el registro y se guarda
    }
    if (led == 0X0A) {      //Dependiendo del dato que entra en RX enciende o apaga las leds
        PORTE = 0;          //depende de los botones en el IOT cloud
    } else if (led == 0X0B) {
        PORTE = 1;
    } else if (led == 0X0C) {
        PORTE = 0;
    } else if (led == 0X0D) {
        PORTE = 2;
    }
    return;
}

void main(void) {
    setup(); //Se ejecuta el setup
    while (1) {
        TXREG = TEMP_LSB;   //Enviamos por TX los datos del I2C
        I2C_Master_Start(); //Iniciamos I2C
        I2C_Master_Write(0b11101100); //Escribimos el adress de 7 bits y el bit de escritura
        I2C_Master_Write(0xD0); //Se escribe en el dress
        I2C_Master_Stop(); //Se detiene la condicion
        __delay_ms(200);
        I2C_Master_Start(); //Se inicia el I2C
        I2C_Master_Write(0b11101101); //Escribimos el adress de 7 bits y el bit de lectura
        TEMP_LSB = I2C_Master_Read(0); //Se lee y guarda el dato
        I2C_Master_Stop(); //Se detiene la condicion
        __delay_ms(200);
    }
}