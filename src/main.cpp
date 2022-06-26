/*------------------------------------------------------------------------------------------------------------

    Projeto: Estufa de filamentos; 
    Allan Costa
    Data: 26-04-22
    Ver: 1

--------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------------------*/
/*  BIBLIOTECAS */
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal_I2C.h>
/*------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------------------*/
/*  DEFINIÇÕES */
#define TemperaturaMinima 23
#define TemperaturaMaxima 30
#define tempo 4000
/*------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------------------*/
/*  SENSOR DHT */
#define DHTTYPE DHT11
#define DHTPIN A0
/*------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------------------*/
/*  OBJETOS */
LiquidCrystal_I2C lcd(0x27,20,4);
DHT dht(DHTPIN,DHTTYPE);
/*------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------------------*/
/*  MAPEAMENTO DE HARDWARE  */
#define btnMenu 2
#define btnAumenta 3 
#define btnDiminui 4
#define rele1 8
#define rele2 9
#define rele3 10 
/*------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------------------*/
/*  PROTOTIPAGEM DE FUNÇÕES */
bool BotaoPressionado(int btn);
void EscolheMenu();
void telaInicial(); 
void setpointTemperatura(); 
void execucao(); 
void cronometro();
/*------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------------------*/
/*  VARIAVEIS */
unsigned short contadorMenu=0, 
               tempAux=1,
               acrescimoTemp=0, 
               tempFinal=0,
               ms=0,
               decimos=0, 
               segundos=0, 
               minutos=0, 
               horas=0,
               dias=0; 

unsigned long tempoAntIni=0, 
              tempoAntExec=0;

byte vezes=0; 

int btn; 

float t=0,
      h=0; 
/*------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------------------*/
/*  FUNÇÃO SETUP  */
void setup() 
{  
  pinMode(btnMenu, INPUT_PULLUP);
  pinMode(btnAumenta, INPUT_PULLUP);
  pinMode(btnDiminui, INPUT_PULLUP);
  t = dht.readTemperature();
  h = dht.readHumidity(); 
  Serial.begin(9600);
  dht.begin(); 
  lcd.init(); 
  lcd.backlight(); 
}
/*------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------------------*/
/*  FUNÇÃO LOOP */
void loop() 
{
  EscolheMenu();
}
/*------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------------------*/
/*  FUNÇÃO ESCOLHE MENU */
void EscolheMenu()
{
  static char aux=0;
  if(!digitalRead(btnMenu)) aux = 1;
  if(digitalRead(btnMenu) && aux)
  {
    delay(200);
    aux=0; 
    if(contadorMenu<=3)contadorMenu++;
    if(contadorMenu==3)contadorMenu=0;
  }
  
  switch(contadorMenu)
  {
    case 0: 
    telaInicial(); 
    break; 

    case 1: 
    setpointTemperatura(); 
    break; 

    case 2:
    execucao();
    break;  

    default: 
    break; 
  }
}
/*------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------------------*/
/*  FUNÇÃO TELA INICIAL */
void telaInicial()
{ 
  vezes=0; 
  ms=0, decimos=0, segundos=0, minutos=0, horas = 0;
  if(millis()>=tempoAntIni+tempo)
  {
    lcd.clear(); 
    h = dht.readHumidity(); 
    t = dht.readTemperature();

    lcd.setCursor(0,0);
    lcd.print("Temp: ");
    lcd.print(t);
    lcd.setCursor(0,1);
    lcd.print("Umidade: ");
    lcd.print(h);

    Serial.println(h);
    Serial.println(t);

    tempoAntIni = millis();
  }
}
/*------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------------------*/
/*  FUNÇÃO SET POINT TEMP */ 
void setpointTemperatura()
{ 
  if (acrescimoTemp != tempAux)
  {
    tempFinal = TemperaturaMinima + acrescimoTemp; 
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("SetPoint");
    lcd.setCursor(0,1);
    lcd.print("Temp:");
    lcd.print(tempFinal);
    tempAux = acrescimoTemp;
    Serial.println(tempFinal); 
  }

  static char aux,aux1=0;

  if(!digitalRead(btnAumenta)) aux = 1;
  if(digitalRead(btnAumenta) && aux)
  {
    delay(200);
    aux=0;
    acrescimoTemp++;
    if(tempFinal>TemperaturaMaxima)
    {
      acrescimoTemp=0; 
    }
  }

  if(!digitalRead(btnDiminui)) aux1 = 1;
  if(digitalRead(btnDiminui) && aux1)
  {
    delay(200);
    aux1=0;
    acrescimoTemp--;      
    if(tempFinal<TemperaturaMinima)
    {
      acrescimoTemp = TemperaturaMaxima - TemperaturaMinima; 
    }
  }
}
/*------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------------------*/
/*  FUNÇÃO DE EXECUÇÃO */
void execucao()
{  
  for(vezes; vezes == 0;)
  {
    lcd.clear(); 
    vezes=1; 
  }
  cronometro();
  
  if(millis()>=tempoAntExec+tempo)
  {
    h = dht.readHumidity(); 
    t = dht.readTemperature();
    tempAux=1;
    lcd.setCursor(0,0);
    lcd.print("Exec:"); 
    lcd.setCursor(0,1);
    lcd.print("Set:");
    lcd.print(tempFinal);  
    lcd.print("C");
    lcd.print(" U:");
    lcd.print(h); 

    tempoAntExec = millis();
  }

  if(t<tempFinal)
  {
    digitalWrite(rele1,1);
    digitalWrite(rele2,1);
    digitalWrite(rele3,1); 
  }else if(t>=tempFinal)
  {
    digitalWrite(rele1,0);
    digitalWrite(rele2,0);
    digitalWrite(rele3,0);
  }

}
/*------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------------------*/
/*  FUNÇÃO DE CRONOMETRO */
void cronometro()
{
  ms = millis(); 

  if(ms % 100 == 0)
  {
    decimos++; 
    if(decimos == 10)
    {
      decimos=0; 
      segundos++; 
    }
    if(segundos == 60)
    {
      segundos=0; 
      minutos++; 
    }
    if(minutos == 60)
    {
      minutos=0; 
      horas++; 
    }
    if(horas == 24)
    {
      horas=0; 
      dias++; 
    }

    if(dias<10)
    {
      lcd.setCursor(6,0); 
      lcd.print("0");
    }
    lcd.print(dias); 
    lcd.print("D");
    if(horas<10)
    {
      lcd.setCursor(9,0);
      lcd.print("0");
    }
    lcd.print(horas);
    lcd.print("h");
    if(minutos<10)
    {
      lcd.setCursor(12,0);
      lcd.print("0");
    }
    lcd.print(minutos);
    lcd.print("m");
  }  
}
/*------------------------------------------------------------------------------------------------------------*/