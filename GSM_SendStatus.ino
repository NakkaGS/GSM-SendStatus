///////////////////////////////////////////////////////////////////////////////////////////////////////
//PROGRAMA PARA ENVIO SMS COM DADOS DO SENSOR PARA O CELULAR QUE SOLICITAR USANDO A MENSAGEM "STATUS"//
////////////////////////////////////GABRIEL NAKATA - 2016/1 - UDESC////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

#include <SoftwareSerial.h>
#include <DS3231.h>
#include <Wire.h>
SoftwareSerial SIM900(10, 11); // No Arduino Uno os pinos são: SIM900(7, 8)

typedef struct{
    int seg, min, hora, dia, mes, ano, temp;
}sensor;


String read_String; //Variavel que armazena a saida do Serial
String cel; //Armazena numero de celular - tem que ter 8 digitos
String sending_number; //Cria a mensagem AT com o numero do celular

char incoming_char=0;
char number[20]; //Variavel para guardar o número do SMS Recebido


//Configuração da biblioteca do DS3231
DS3231 Clock;
bool Century=false;
bool h12;
bool PM;
byte ADay, AHour, AMinute, ASecond, ABits;
bool ADy, A12h, Apm;

//int second,minute,hour,date,month,year,temperature, day;
int count = 0;


void inicializar_sim900();

void setup()
{
  inicializar_sim900();
  
  //////////////////////////////////////////////////////
  //Iniciando o DS3231 - Apagar depois da primeira vez//
  //////////////////////////////////////////////////////

  Wire.begin();
  Clock.setSecond(50);//Setar os segundos
  Clock.setMinute(54);//Setar os minutos
  Clock.setHour(22);  //Setar as horas 
  Clock.setDate(14);  //Setar o dia do mês
  Clock.setMonth(8);  //Setar o mês
  Clock.setYear(16);  //Setar o ano (os dois ultimos numeros do ano)

  Serial.println("Clock Iniciado");
}
 
void loop()
{
  
  if(Serial.read()== 't'){ //Teste para ver se está enviando mensagem. Para isso, basta escrever "t" no Serial Monitor
    EnviarMensagem();
  }else{
    while(SIM900.available()){ //Guarda os dados da Serial
      delay(3);  
      char c = SIM900.read();
      read_String += c; 
    }
    if (read_String.length() >0){//Caso copia uma linha da serial que seja maior que 1.
      read_String.trim();
      read_String.toUpperCase(); //Deixa tudo em MAIUSCULO
      Serial.println(" ");
      Serial.println(read_String); //Mostra a mensagem Recebido
      cel = read_String.substring(10,18); //Armazena os caracteres que estão entre as posições 10 e 18. Nesta posições está o numero

      if(read_String.indexOf("STATUS") >=0){ //Caso seja igual a "STATUS" ele manda uma mensagem e mostra que está sendo enviado uma mensagem
        Serial.println(" ");
        Serial.println("Reenviando mensagem");
        Serial.println(" ");
        EnviarMensagem();
      }
      read_String="";
    }
  } 
}  
    
///////////////////
//Enviar Mensagem//
///////////////////
  
void EnviarMensagem()
{
  //Manda o SMS para o Celular. +55 - Código do País -=- 47 - Código do Estado -=- Resto - Telefone Desejado
  //Esta é a parte do GSM Shield
  
  sending_number = "AT + CMGS = \"+5547" + cel + "\""; //Cria a mensagem AT com o numero do celular
  
  SIM900.print("AT+CMGF=1\r");    //Ativa o modo texto do SMS
  delay(100);
  //SIM900.println("AT + CMGS = \"+5547000000000\""); //Caso queira mandar para um numero 
  SIM900.println(sending_number);
  delay(100);
  //Serial.println(sending_number); //Verificar a mensagem que está sendo escrita no SIM900

  
  //Dados do sensores que iram para o SMS
  sensor DS31;
  dados_sensor(&DS31);
  
  SIM900.print("Data: ");
  SIM900.print("20");
  SIM900.print(DS31.ano,DEC);
  SIM900.print('-');
  SIM900.print(DS31.mes,DEC);
  SIM900.print('-');
  SIM900.println(DS31.dia,DEC);
  SIM900.print("Horario: ");
  SIM900.print(DS31.hora,DEC);
  SIM900.print(':');
  SIM900.print(DS31.min,DEC);
  SIM900.print(':');
  SIM900.println(DS31.seg,DEC);
  SIM900.print("Temperatura: ");
  SIM900.println(DS31.temp); 

  //Para verificar se está indo para o numero certo
  //SIM900.print("Reenviado para: ");
  //SIM900.print(cel);
  
  delay(100);
  SIM900.println((char)26);//the ASCII code of the ctrl+z is 26 //Este caractere é responsavel de fechar o SMS e enviar a msg desejada
  delay(100);
  mensagem_enviada();
}

////////////////////////////////////////////////
//Guarda os dados do Sensore de Relogio DS3132//
////////////////////////////////////////////////

void dados_sensor(sensor DS31)
{   
  DS31.seg = Clock.getSecond(); //Guarda os segundos
  DS31.min = Clock.getMinute(); //Guarda os minutos
  DS31.hora = Clock.getHour(h12, PM); //Guarda as horas
  DS31.dia = Clock.getDate(); //Guarda o dia do mês
  DS31.mes = Clock.getMonth(Century); //Guarda o mês
  DS31.ano = Clock.getYear(); // Guarda o ano

  DS31.temp = Clock.getTemperature(); //Guarda a temperatura
}

////////////////////////////////////////
//// Mostra os dados no Serial Monitor//
////////////////////////////////////////

void mensagem_enviada()
{
  Serial.println("Mensagem Enviada");

  Serial.print("20");
  Serial.print(DS31.ano,DEC);
  Serial.print('-');
  Serial.print(DS31.mes,DEC);
  Serial.print('-');
  Serial.print(DS31.dia,DEC);
  Serial.print(' ');
  Serial.print(DS31.hora,DEC);
  Serial.print(':');
  Serial.print(DS31.min,DEC);
  Serial.print(':');
  Serial.print(DS31.seg,DEC);
  Serial.print('\n');
  Serial.print("Temperature=");
  Serial.print(DS31.temp); 
  Serial.print('\n');
}

  ///////////////////////////
  // Iniciando o GSM Shield//
  ///////////////////////////
void inicializar_sim900();
{
  Serial.begin(19200); //Serial monitor
  SIM900.begin(19200); //GSM shield
  delay(2000);  //Tempo para entrar na rede GSM
  SIM900.print("AT+CMGF=1\r");  //Coloca em SMS Mode
  delay(100);
  SIM900.print("AT+CNMI=2,2,0,0,0\r"); 
  // blurt out contents of new SMS upon receipt to the GSM shield's serial out
  delay(100);
  
  Serial.println("GSM Iniciado");
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
//PROGRAMA PARA ENVIO SMS COM DADOS DO SENSOR PARA O CELULAR QUE SOLICITAR USANDO A MENSAGEM "STATUS"//
////////////////////////////////////GABRIEL NAKATA - 2016/1 - UDESC////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
