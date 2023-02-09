//CONTROLADOR MIDI_BLUETOOTH

#include "BluetoothSerial.h"
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
BluetoothSerial SerialBT;// Terminal Bluetooth

    //Botones
const int nBotones = 5; // número de botones
const int calibracion=130; //delay (es el número de loops minimo de espera que debe haber entre cada repetición de nota)
boolean notaioff[nBotones];
int contador[nBotones];    
byte notas[] = {57,60,64,67,69}; //estás son las notas que se dispararán los botones
byte botones[] = {18,19,12,13,23}; //aqui se ponen los GPIO de cada boton, el numero de elementos debe ser nBotones

   //Potenciómetros
const int nPots=4; //número de potenciómetros
byte analogs[] = {36,39,35,34} ; //GPIO que se usaran como entradas analógicas. //byte_dato que representa un entero sin signo de 0 a 255
int lecturas[nPots]; 
int lecturasAnteriores[nPots];

bool hayip=false;




void setup() {
  Serial.begin(115200);
  SerialBT.begin("MIDI-JossMa"); //Nombre del dispositivo
  Serial.println("The device started, now you can pair it with bluetooth!");  
  
  for (int i=0;i<nBotones;i++)
  {
   pinMode(botones[i], INPUT_PULLUP);//con la resistencia integrada
  }
  analogReadResolution(10);  
}

      //Función para configurar loa mensajes MIDIy enviarlos a la terminal BT
void midi(unsigned char comando, unsigned char nota,unsigned char velocity) //unsigned char- variable sin signo, que tiene un rango de [0,255]
{
    SerialBT.write(comando);
    SerialBT.write(nota);
    SerialBT.write(velocity);
}


void loop() {
  
  for (int i=0; i<nBotones; i++) 
  {
      if (digitalRead(botones[i]) == LOW) //botón sin presionado
      {
        if (contador[i]==0)//cuenta regresiva terminada ?
        {
          if (notaioff[i]== 1) //¿la nota esta apagada?
          {
            contador[i]=calibracion; //valor de cuenta regresiva 
            midi(144,notas[i],100); //se envía la nota
            notaioff[i] = 0; //la nota no esta apagada (esta encendida)
          }
        }
        
      }
      else //botón sin presionar (posible envio de Note Off)
      {
        if (contador[i]==0) //cuenta regresiva terminada ?
        {
          if (notaioff[i] ==0) //¿La nota esta esta activada?
          {
            contador[i]=calibracion; //valor de cuenta regresiva 
            midi(144,notas[i],0); //envio de note off
            notaioff[i] = 1;  //la nota ya no está encendida
          }
        }
      }
  }

  
for (int i=0; i<nBotones;i++)
{
if (contador[i]>0) contador[i]--;
}

for (int k=0; k<nPots; k++) //se hace un recorrido desde k=0 hasta que k sea menor a nPots
{
  int lec = analogRead(analogs[k]);
  lecturas[k] = map(lec,0,1023,0,255); //mapeamos al rango al doble del CC
}

for (int k=0; k<nPots;k++)
{    //vemos si hubo un cambio en el valor de los potenciómetros
      if (lecturas[k] > (lecturasAnteriores[k]+1) || lecturas[k] < (lecturasAnteriores[k]-1) )
      {                   
          midi(176,k+30,lecturas[k]/2); //mándamos a partir del CC 30
          lecturasAnteriores[k] = lecturas[k];
      } 
}
delay(1); 
}
