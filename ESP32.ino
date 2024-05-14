/*
Código desenvolvido por:
Felipe Augusto Silva Nascimento
Hortencia Diniz Dultra e Silva
Raimunda Lima Bacelar de Oliveira Neta
Para a disciplina: Sistemas de Aquisição de Dados e Interface
*/
// Autorizações pro Blynk
#define BLYNK_TEMPLATE_ID "TMPL2gS6yBrS0"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "rfTdqTTQeC2ox4wO6sL8ix1numl7dN7y"
// Inclui as bibliotecas necessárias para o desenvovlimento do projeto

#include <EEPROM.h>
#include <DHT.h>
#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Blynk.h>
#include <BlynkSimpleEsp32.h>

// Define o nome da rede e a senha
#define WIFI_SSID "brisa-1197669"
#define WIFI_PASSWORD "b7tucok3"



/* Comente isso para desabilitar impressões e economizar espaço */
#define BLYNK_PRINT Serial

// Define o UTC em -10800 segundos
#define UTC_OFFSET_IN_SECONDS -10800

// Define os espaços de memórias que iremos utilizar da EEPROM
#define HORAMEM1 0
#define MINMEM1 1
#define HORAMEM2 2
#define MINMEM2 3
#define HORAMEM3 4
#define MINMEM3 5

// Define duas variaveis de tempo
#define TEMPO_LONGO 2000
#define TEMPO_CURTO 500

// Define as portas do botões
#define btnVermelho 36
#define btnVerde 34
#define btnAzul 39
#define btnAmarelo 35
#define btnBranco 32

// Define as portas dos LEDs
#define ledVermelho 19
#define ledAzul 18
#define ledVerde 5
#define ledAmarelo 17
#define ledBranco 16

// Define a porta do buzzer
#define buzzer 13

// Define o pino 4 do sensor
#define dhtPin 4
#define DTYPE DHT22  // Configura o uso do código para o sensor DHT11


// Define variáveis auxiliares para os botões verde, vermelho e azul
int L_botao_VERDE = 1;
int LA_botao_VERDE = 1;

int LA_botao_VERMELHO = 1;
int L_botao_VERMELHO = 1;

int LA_botao_AZUL = 1;
int L_botao_AZUL = 1;

// Define AlarmeHora e AlarmeMin como uma variável do tipo byte com valor inicial igual 0
byte AlarmeHora1 = 0;
byte AlarmeMin1 = 0;

byte AlarmeHora2 = 0;
byte AlarmeMin2 = 0;

byte AlarmeHora3 = 0;
byte AlarmeMin3 = 0;

// Define variáveis auxiliares para hora e min
int contaHora1 = 0;
int contaMin1 = 0;

int contaHora2 = 0;
int contaMin2 = 0;

int contaHora3 = 0;
int contaMin3 = 0;

// Define tela 1 igual a 1
int tela = 1;

// Define três variáveis de tempo do tipo long
long tempoInicial1;
long tempoInicial2;
long tempoInicial3;

BlynkTimer timer; //Instancia um objeto chamando time
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(dhtPin, DTYPE);  //Instancia uma função dht do tipo DHT com os parametros sendo o pino do ESP32 e o tipo do sensor

WiFiUDP ntpUDP;  //Instancia um objeto do tipo WifiUDP
NTPClient timeClient(ntpUDP, "pool.ntp.org", UTC_OFFSET_IN_SECONDS);



BLYNK_WRITE(V10)
{
  // Define o valor de entrada do pino V0 para uma variável
  int value = param.asInt();
  //digitalWrite(ledA, value);
  // Update do estado
  Blynk.virtualWrite(V0, value);
}

// Esta função é chamada toda vez que o dispositivo é conectado ao Blynk.Cloud
BLYNK_CONNECTED()
{
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

// Esta função envia o tempo de atividade do Arduino a cada segundo para o Pino Virtual 2.
/*void myTimerEvent()
{
  Blynk.virtualWrite(V2, millis() / 1000);
}*/

void leitura_dht22() 
{

  float temperatura = dht.readTemperature(false); //Lê a temperatura ambiente
  float umidade = dht.readHumidity(); //Lê a umidade do ar
  
  Blynk.virtualWrite(V6, temperatura); //Associa o pino virtual V6 do Blynk à variável temperatura.
  Blynk.virtualWrite(V2, umidade); //Associa o pino virtual V2 do Blynk à variável umidade.
  
}
void IRAM_ATTR desliga_alarme(){ //interrupção para desligar o alarme quando o botao vermelho for acionado
  digitalWrite(ledVermelho, LOW);
  digitalWrite(ledVerde, LOW);
  digitalWrite(ledAmarelo, LOW);
  digitalWrite(ledAzul, LOW);
  noTone(buzzer);
}

void setup() {

  attachInterrupt(digitalPinToInterrupt(btnVermelho), desliga_alarme, FALLING);

  EEPROM.begin(255);  //Aloca 26 espaços na memoria EEPROM

  pinMode(buzzer, OUTPUT);  //Define o buzzer como saida

  //Define todos os LEDs como saida
  pinMode(ledVermelho, OUTPUT);
  pinMode(ledAzul, OUTPUT);
  pinMode(ledVerde, OUTPUT);
  pinMode(ledAmarelo, OUTPUT);
  pinMode(ledBranco, OUTPUT);

  //Define todos os botões como entrada
  pinMode(btnVermelho, INPUT);
  pinMode(btnVerde, INPUT);
  pinMode(btnAzul, INPUT);
  pinMode(btnAmarelo, INPUT);
  pinMode(btnBranco, INPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  //Inicia a comunicação Wifi passando nome da rede e senha
  Serial.print("Conectando ao WiFi");
  while (WiFi.status() != WL_CONNECTED) {  //Enquanto o Wifi estiver desconectado ele printa "."
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Conectado com IP: ");  //Printa o IP
  Serial.println(WiFi.localIP());
  Serial.println();

  timeClient.begin();   //Inicia o objeto timeClient
  timeClient.update();  //Atualiza seus valores

  lcd.init();       //Inicia o display LCD
  lcd.clear();      //Limpa o display LCD
  lcd.backlight();  //Acende o display do LED

  dht.begin();  //Inicia as leituras do sensor de temperatura e umidade

  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASSWORD); //Inicia o blynk
  timer.setInterval(1000L, leitura_dht22);
}

void loop() {
 
  // Lê os valores do espaço 0, 1, 2, 3, 4 e 5 da EEPROM e armazena em uma variável respectivamente
  AlarmeHora1 = EEPROM.read(HORAMEM1);
  AlarmeMin1 = EEPROM.read(MINMEM1);

  AlarmeHora2 = EEPROM.read(HORAMEM2);
  AlarmeMin2 = EEPROM.read(MINMEM2);


  AlarmeHora3 = EEPROM.read(HORAMEM3);
  AlarmeMin3 = EEPROM.read(MINMEM3);

  Serial.begin(115200);

  //Chama as funções principais
  telas();
  botoes();
  alarmes();
  beber_agua();
  banheiro();
  Blynk.run();
  timer.run();

}


//Cria uma função que retorna a data atualizada
String getTimeStampString() {
  time_t rawtime = timeClient.getEpochTime();
  struct tm* ti;
  ti = localtime(&rawtime);
  //String que dá o ano
  uint16_t ano = ti->tm_year + 1900;
  String anoStr = String(ano);
  //String que dá o mês
  uint8_t mes = ti->tm_mon + 1;
  String mesStr = mes < 10 ? "0" + String(mes) : String(mes);
  //String que dá o dia
  uint8_t dia = ti->tm_mday;
  String diaStr = dia < 10 ? "0" + String(dia) : String(dia);
  //Retorna o dia, mês e ano
  return diaStr + "/" + mesStr + "/" + anoStr + " ";
}

//Cria a função telas
void telas() {
 detachInterrupt(0);
  if (tela == 1) {  // Verifica se tela é igual a 1
    lcd.setCursor(0, 0);
    lcd.print("Hora: ");
    lcd.print(timeClient.getFormattedTime());  // Printa a hora no LCD


    lcd.setCursor(0, 1);
    lcd.print("Data: ");
    lcd.print(getTimeStampString());  //Printa a data no LCD
  }

  else if (tela == 2) {                              //Verifica se tela é igual a 2
    float temperatura = dht.readTemperature(false);  //Lê a temperatura e armazena em uma variável
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temperatura);  //Printa a temperatura no LCD
    lcd.print("C");
    float umidade = dht.readHumidity();  //Lê a umidade e armazena em uma variável
    lcd.setCursor(0, 1);
    lcd.print("Umid: ");
    lcd.print(umidade);  //Printa a umidade
    lcd.print("%");
  }

  if (tela == 3) {  //Se tela for igual a 3
    lcd.setCursor(0, 0);
    lcd.print("Alarme 1:");
    lcd.setCursor(0, 1);
    lcd.print(AlarmeHora1);  //Printa o valor da hora do alarme 1 que está armazenado na EEPROM
    lcd.setCursor(2, 1);
    lcd.print(":");
    lcd.setCursor(3, 1);
    lcd.print(AlarmeMin1);  //Printa o valor dos minutos do alarme 1 que está armazenado na EEPROM
  }

  if (tela == 4) {  //Se tela for igual a 4
    lcd.setCursor(0, 0);
    lcd.print("Alarme 2:");
    lcd.setCursor(0, 1);
    lcd.print(AlarmeHora2);  //Printa o valor da hora do alarme 2 que está armazenado na EEPROM
    lcd.setCursor(2, 1);
    lcd.print(":");
    lcd.setCursor(3, 1);
    lcd.print(AlarmeMin2);  //Printa o valor dos minutos do alarme 2 que está armazenado na EEPROM
  }

  if (tela == 5) {  //Se tela for igual a 5
    lcd.setCursor(0, 0);
    lcd.print("Alarme 3:");
    lcd.setCursor(0, 1);
    lcd.print(AlarmeHora3);  //Printa o valor da hora do alarme 3 que está armazenado na EEPROM
    lcd.setCursor(2, 1);
    lcd.print(":");
    lcd.setCursor(3, 1);
    lcd.print(AlarmeMin3);  //Printa o valor dos minutos do alarme 3 que está armazenado na EEPROM
  }

  if (tela == 30) {  // Se tela for igual a 30

    lcd.setCursor(0, 0);
    lcd.print("Alarme 1:");
    lcd.setCursor(0, 1);
    lcd.print(contaHora1);  //Printa o valor da hora do alarme 1
    lcd.setCursor(2, 1);
    lcd.print(":");
    lcd.setCursor(3, 1);
    lcd.print(contaMin1);  //Printa o valor dos minutos do alarme 1

    if (digitalRead(btnVermelho) == 1) {              // Verifica se o botão vermelho foi pressionado
      if (millis() - tempoInicial2 >= TEMPO_LONGO) {  //Se o tempo de execução do sistema menos o tempo inicial for maior ou igual ao tempo longo
        contaHora1 = contaHora1 + 1;                  // Acrescenta mais 1 na hora
        delay(400);
        lcd.setCursor(0, 1);
        lcd.print(contaHora1);  // Printa a hora
        lcd.setCursor(2, 1);
        lcd.print(":");
        if (contaHora1 == 24) {  // Se a hora for igual a 24
          lcd.setCursor(0, 1);
          lcd.print("   ");
          contaHora1 = 0;  //O valor da hora vai pra 0
        }
      }
    }
    while (digitalRead(btnVermelho) == HIGH)
      ;
    if (digitalRead(btnVerde) == HIGH) {              //Verifica se o botão verde foi pressionado
      if (millis() - tempoInicial3 >= TEMPO_LONGO) {  //Se o tempo de execução do sistema menos o tempo inicial for maior ou igual ao tempo longo
        contaMin1 = contaMin1 + 5;                    //Acrescenta mais 5 nos minutos
        delay(400);
        lcd.setCursor(3, 1);
        lcd.print(contaMin1);   //Printa os minutos
        if (contaMin1 == 60) {  //Verfica se os minutos são igauais a 60
          lcd.setCursor(3, 1);
          lcd.print("   ");
          contaMin1 = 0;  // Minutos vai pra 0
        }
      }
    }
    while (digitalRead(btnVerde) == HIGH)
      ;

    if (digitalRead(btnAzul) == 1) {                  //Verifica se o botão azul foi pressionado
      if (millis() - tempoInicial1 >= TEMPO_LONGO) {  //Se o tempo de execução do sistema menos o tempo inicial for maior ou igual ao tempo longo
        EEPROM.write(HORAMEM1, contaHora1);           //Escreve na EEPROM a hora
        EEPROM.write(MINMEM1, contaMin1);             //Escreve na EEPROM os minutos
        EEPROM.commit();                              //Confirma a escrita dos dados na EEPROM
        lcd.clear();                                  //Limpa o display
        lcd.setCursor(0, 0);
        lcd.print("Alarme 1 salvo!");
        delay(2000);
        lcd.clear();
        tela = 1;  // Volta pra tela 1
      }
    }
    while (digitalRead(btnAzul) == HIGH)
      ;
  }

  if (tela == 40) {  // Se tela for igual a 40
    lcd.setCursor(0, 0);
    lcd.print("Alarme 2:");
    lcd.setCursor(0, 1);
    lcd.print(contaHora2);  //Printa o valor da hora do alarme 2
    lcd.setCursor(2, 1);
    lcd.print(":");
    lcd.setCursor(3, 1);
    lcd.print(contaMin2);  //Printa o valor dos minutos do alarme 2

    if (digitalRead(btnVermelho) == 1) {              // Verifica se o botão vermelho foi pressionado
      if (millis() - tempoInicial2 >= TEMPO_LONGO) {  //Se o tempo de execução do sistema menos o tempo inicial for maior ou igual ao tempo longo
        contaHora2 = contaHora2 + 1;                  // Acrescenta mais 1 na hora
        delay(400);
        lcd.setCursor(0, 1);
        lcd.print(contaHora2);  // Printa a hora
        lcd.setCursor(2, 1);
        lcd.print(":");
        if (contaHora2 == 24) {  //Se a hora for igual a 24
          lcd.setCursor(0, 1);
          lcd.print("   ");
          contaHora2 = 0;  // A hora passa a ser 0
        }
      }
    }
    while (digitalRead(btnVermelho) == HIGH)
      ;
    if (digitalRead(btnVerde) == HIGH) {              //Verifica se o botão verde foi pressionado
      if (millis() - tempoInicial3 >= TEMPO_LONGO) {  //Se o tempo de execução do sistema menos o tempo inicial for maior ou igual ao tempo longo
        contaMin2 = contaMin2 + 5;                    //Acrescenta mais 5 nos minutos
        delay(400);
        lcd.setCursor(3, 1);
        lcd.print(contaMin2);
        if (contaMin2 == 60) {  //Verfica se os minutos são igauais a 60
          lcd.setCursor(3, 1);
          lcd.print("   ");
          contaMin2 = 0;  // Minutos vai pra 0
        }
      }
    }
    while (digitalRead(btnVerde) == HIGH)
      ;

    if (digitalRead(btnAzul) == 1) {                  //Verifica se o botão azul foi pressionado
      if (millis() - tempoInicial1 >= TEMPO_LONGO) {  //Se o tempo de execução do sistema menos o tempo inicial for maior ou igual ao tempo longo
        EEPROM.write(HORAMEM2, contaHora2);           //Escreve na EEPROM a hora
        EEPROM.write(MINMEM2, contaMin2);             //Escreve na EEPROM os minutos
        EEPROM.commit();                              //Confirma a escrita dos dados na EEPROM
        lcd.clear();                                  //Limpa o display
        lcd.setCursor(0, 0);
        lcd.print("Alarme 2 salvo!");
        delay(2000);
        lcd.clear();
        tela = 1;  //Volta pra tela 1
      }
    }
    while (digitalRead(btnAzul) == HIGH)
      ;
  }
  //Da mesma forma que foi para a tela 30 e 40, será a tela 50
  if (tela == 50) {
    lcd.setCursor(0, 0);
    lcd.print("Alarme 3:");
    lcd.setCursor(0, 1);
    lcd.print(contaHora3);
    lcd.setCursor(2, 1);
    lcd.print(":");
    lcd.setCursor(3, 1);
    lcd.print(contaMin3);

    if (digitalRead(btnVermelho) == 1) {
      if (millis() - tempoInicial2 >= TEMPO_LONGO) {
        contaHora3 = contaHora3 + 1;
        delay(400);
        lcd.setCursor(0, 1);
        lcd.print(contaHora3);
        lcd.setCursor(2, 1);
        lcd.print(":");
        if (contaHora3 == 24) {
          lcd.setCursor(0, 1);
          lcd.print("   ");
          contaHora3 = 0;
        }
      }
    }
    while (digitalRead(btnVermelho) == HIGH)
      ;
    if (digitalRead(btnVerde) == HIGH) {
      if (millis() - tempoInicial3 >= TEMPO_LONGO) {
        contaMin3 = contaMin3 + 5;
        delay(400);
        lcd.setCursor(3, 1);
        lcd.print(contaMin3);
        if (contaMin3 == 60) {
          lcd.setCursor(3, 1);
          lcd.print("   ");
          contaMin3 = 0;
        }
      }
    }
    while (digitalRead(btnVerde) == HIGH)
      ;

    if (digitalRead(btnAzul) == 1) {
      if (millis() - tempoInicial1 >= TEMPO_LONGO) {
        EEPROM.write(HORAMEM3, contaHora3);
        EEPROM.write(MINMEM3, contaMin3);
        EEPROM.commit();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Alarme 3 salvo!");
        delay(2000);
        lcd.clear();
        tela = 1;
      }
    }
    while (digitalRead(btnAzul) == HIGH)
      ;
  }
  attachInterrupt(digitalPinToInterrupt(btnVermelho), desliga_alarme, FALLING);
}

//Cria a função dos botões
void botoes() {
  L_botao_VERDE = digitalRead(btnVerde);            //Lê o estado do botão verde e atribui a uma variável
  if (L_botao_VERDE == 0 && LA_botao_VERDE == 1) {  //Verifica o estado do botão e da variavel auxiliar
    tempoInicial3 = millis();                       //Atribui o tempo de execução do sistema a uma variável
    if (tela == 1) {                                //Se tela for igual 1
      lcd.clear();
      tela = 2;              // Tela vai ser igual 2
    } else if (tela == 2) {  //Se tela for igual a 2
      lcd.clear();
      tela = 3;              //Tela vai ser igual a 3
    } else if (tela == 3) {  //Se tela for igual a 3
      lcd.clear();
      tela = 4;              //tela vai ser igual a 4
    } else if (tela == 4) {  //Se tela for igual a 4
      lcd.clear();
      tela = 5;  //Tela vai ser igual a 4
    }
  }
  LA_botao_VERDE = L_botao_VERDE;  // Estado do botão igual a variavel auxiliar


  L_botao_VERMELHO = digitalRead(btnVermelho);            //Lê o estado do botão vermelho
  if (L_botao_VERMELHO == 0 && LA_botao_VERMELHO == 1) {  //Verifica o estado do botão e da variável auxiliar
    tempoInicial2 = millis();                             //Atribui o tempo de execução do sistema a uma variável
    if (tela == 2) {                                      //Se tela for igual 2
      lcd.clear();
      tela = 1;              //Tela vai ser igual a 1
    } else if (tela == 3) {  // Se tela for igual a 3
      lcd.clear();
      tela = 2;              //Tela vai ser igual a 2
    } else if (tela == 4) {  //Se tela for igual a 4
      lcd.clear();
      tela = 3;              //Tela vai ser igual a 3
    } else if (tela == 5) {  //Se tela vai ser igual a 5
      lcd.clear();
      tela = 4;              //Tela vai ser igual a 4
    } else if (tela == 6) {  // Se tela for igual a 6
      lcd.clear();
      tela = 5;               //Tela vai ser igual a 5
    } else if (tela == 30) {  //Se tela for igual a 30
      lcd.clear();
      tela = 3;               //Tela vai ser igual a 3
    } else if (tela == 40) {  //Se tela for igual a 40
      lcd.clear();
      tela = 4;               //Tela vai ser igual a 4
    } else if (tela == 50) {  //Se tela for igual a 50
      lcd.clear();
      tela = 5;  //Tela vai ser igual a 5
    }
  }
  LA_botao_VERMELHO = L_botao_VERMELHO;  // Estado do botão igual a variavel auxiliar


  L_botao_AZUL = digitalRead(btnAzul);            //Lê o estado do botão azul e atribui a uma variável
  if (L_botao_AZUL == 1 && LA_botao_AZUL == 1) {  //Verifica o estado do botão e da variável auxiliar
    tempoInicial1 = millis();                     //Atribui o tempo de execução do sistema a uma variável
    if (tela == 3) {                              //Se tela for igual a 3
      lcd.clear();
      tela = 30;             //Tela passa a ser igual a 30
    } else if (tela == 4) {  //Se tela for 4
      lcd.clear();
      tela = 40;  //Tela passa a ser igual a 40
    }
    if (tela == 5) {  //Se tela for igual a 5
      lcd.clear();
      tela = 50;  //Tela passa a ser igual a 50
    }
  }
}
//Função dos três alarmes definidos pelo usuário no menu do display
void alarmes() {
  //Verifica se as horas e os minutos correspondem ao valores setados no alarme 1
  if (timeClient.getHours() == AlarmeHora1 && timeClient.getMinutes() == AlarmeMin1) {
    for (int i = 0; i < 5; i++) {
      digitalWrite(ledVermelho, HIGH);  //Liga o LED vermelho
      tone(buzzer, 1500);               //Liga o buzzer
      delay(1000);
      digitalWrite(ledVermelho, LOW);  //Desliga o LED
      noTone(buzzer);                  //Desliga o buzzer
      delay(1000);
    }
  }
  while (digitalRead(btnVermelho) == HIGH)
    ;
  //Verifica se as horas e os minutos correspondem ao valores setados no alarme 2
  if (timeClient.getHours() == AlarmeHora2 && timeClient.getMinutes() == AlarmeMin2) {
    for (int i = 0; i < 5; i++) {
      digitalWrite(ledAzul, HIGH);  //Liga o LED azul
      tone(buzzer, 1500);           //Liga o buzzer
      delay(1000);
      digitalWrite(ledAzul, LOW);  //Desliga o LED azul
      noTone(buzzer);              //Desliga o buzzer
      delay(1000);
    }
  }

  //Verifica se as horas e os minutos correspondem ao valores setados no alarme 3
  if (timeClient.getHours() == AlarmeHora3 && timeClient.getMinutes() == AlarmeMin3) {
    for (int i = 0; i < 5; i++) {
      digitalWrite(ledVerde, HIGH);  //Liga o LED verde
      tone(buzzer, 1500);            //Liga o buzzer
      delay(1000);
      digitalWrite(ledVerde, LOW);  //Desliga o LED verde
      noTone(buzzer);               //Desliga o buzzer
      delay(1000);
    }
    lcd.clear();
  }
}

//Alarme que tem o intuito de lembra ao usuario de beber agua
void beber_agua() {
  float temperatura = dht.readTemperature(false);  //Lê os dados de temeperatura
  float umidade = dht.readHumidity();              //Lê a umidade do ar


  if (temperatura >= 30 && umidade <= 50) {  //Verifica se a tempratura é maior ou igual a 30 e se a umidade é menor ou igual a 50
                                             //Se essa condição for verdadeira o alarme irá soar a cada 2 horas

    //Alarme soa as 8h30, ligando o LED branco e o buzzer
    if (timeClient.getHours() == 8 && timeClient.getMinutes() == 30) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Hora da agua!");
      digitalWrite(ledBranco, HIGH);
      tone(buzzer, 1500);
      delay(1000);
      digitalWrite(ledBranco, LOW);
      noTone(buzzer);
      delay(1000);
    }

    //Alarme soa as 10h30, ligando o LED branco e o buzzer
    if (timeClient.getHours() == 10 && timeClient.getMinutes() == 30) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Hora da agua!");
      digitalWrite(ledBranco, HIGH);
      tone(buzzer, 1500);
      delay(1000);
      digitalWrite(ledBranco, LOW);
      noTone(buzzer);
      delay(1000);
    }

    //Alarme soa as 12h30, ligando o LED branco e o buzzer
    if (timeClient.getHours() == 12 && timeClient.getMinutes() == 30) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Hora da agua!");
      digitalWrite(ledBranco, HIGH);
      tone(buzzer, 1500);
      delay(1000);
      digitalWrite(ledBranco, LOW);
      noTone(buzzer);
      delay(1000);
    }

    //Alarme soa as 14h30, ligando o LED branco e o buzzer
    if (timeClient.getHours() == 14 && timeClient.getMinutes() == 30) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Hora da agua!");
      digitalWrite(ledBranco, HIGH);
      tone(buzzer, 1500);
      delay(1000);
      digitalWrite(ledBranco, LOW);
      noTone(buzzer);
      delay(1000);
    }

    //Alarme soa as 16h30, ligando o LED branco e o buzzer
    if (timeClient.getHours() == 16 && timeClient.getMinutes() == 30) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Hora da agua!");
      digitalWrite(ledBranco, HIGH);
      tone(buzzer, 1500);
      delay(1000);
      digitalWrite(ledBranco, LOW);
      noTone(buzzer);
      delay(1000);
    }

    //Alarme soa as 18h30, ligando o LED branco e o buzzer
    if (timeClient.getHours() == 18 && timeClient.getMinutes() == 30) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Hora da agua!");
      digitalWrite(ledBranco, HIGH);
      tone(buzzer, 1500);
      delay(1000);
      digitalWrite(ledBranco, LOW);
      noTone(buzzer);
      delay(1000);
    }

    //Alarme soa as 20h30, ligando o LED branco e o buzzer
    if (timeClient.getHours() == 20 && timeClient.getMinutes() == 30) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Hora da agua!");
      digitalWrite(ledBranco, HIGH);
      tone(buzzer, 1500);
      delay(1000);
      digitalWrite(ledBranco, LOW);
      noTone(buzzer);
      delay(1000);
    }
    //Alarme soa as 22h30, ligando o LED branco e o buzzer
    if (timeClient.getHours() == 22 && timeClient.getMinutes() == 30) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Hora da agua!");
      digitalWrite(ledBranco, HIGH);
      tone(buzzer, 1500);
      delay(1000);
      digitalWrite(ledBranco, LOW);
      noTone(buzzer);
      delay(1000);
    }

  } else {  // //Verifica se a tempratura é diferente de  30 e se a umidade é diferente de 50
    //Caso seja verdade, o alarme soa a cada 3 horas

    //Alarme soa as 08h30, ligando o LED branco e o buzzer
    if (timeClient.getHours() == 8 && timeClient.getMinutes() == 30) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Hora da agua!");
      digitalWrite(ledBranco, HIGH);
      tone(buzzer, 1500);
      delay(1000);
      digitalWrite(ledBranco, LOW);
      noTone(buzzer);
      delay(1000);
    }
    //Alarme soa as 11h30, ligando o LED branco e o buzzer
    if (timeClient.getHours() == 11 && timeClient.getMinutes() == 30) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Hora da agua!");
      digitalWrite(ledBranco, HIGH);
      tone(buzzer, 1500);
      delay(1000);
      digitalWrite(ledBranco, LOW);
      noTone(buzzer);
      delay(1000);
    }
    //Alarme soa as 14h30, ligando o LED branco e o buzzer
    if (timeClient.getHours() == 14 && timeClient.getMinutes() == 30) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Hora da agua!");
      digitalWrite(ledBranco, HIGH);
      tone(buzzer, 1500);
      delay(1000);
      digitalWrite(ledBranco, LOW);
      noTone(buzzer);
      delay(1000);
    }
    //Alarme soa as 17h30, ligando o LED branco e o buzzer
    if (timeClient.getHours() == 17 && timeClient.getMinutes() == 30) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Hora da agua!");
      digitalWrite(ledBranco, HIGH);
      tone(buzzer, 1500);
      delay(1000);
      digitalWrite(ledBranco, LOW);
      noTone(buzzer);
      delay(1000);
    }
    //Alarme soa as 20h30, ligando o LED branco e o buzzer
    if (timeClient.getHours() == 20 && timeClient.getMinutes() == 30) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Hora da agua!");
      digitalWrite(ledBranco, HIGH);
      tone(buzzer, 1500);
      delay(1000);
      digitalWrite(ledBranco, LOW);
      noTone(buzzer);
      delay(1000);
    }
  }
}

// Função que tem o intuito de ser uma alarme para lembrar o usuário de ir ao banheiro
void banheiro() {

  //Alarme soa as 8h00, ligando o LED amarelo e o buzzer
  if (timeClient.getHours() == 8 && timeClient.getMinutes() == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Hora do banheiro!");
    digitalWrite(ledAmarelo, HIGH);
    tone(buzzer, 1500);
    delay(1000);
    digitalWrite(ledAmarelo, LOW);
    noTone(buzzer);
    delay(1000);
  }

  //Alarme soa as 11h00, ligando o LED amarelo e o buzzer
  if (timeClient.getHours() == 11 && timeClient.getMinutes() == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Hora do banheiro!");
    digitalWrite(ledAmarelo, HIGH);
    tone(buzzer, 1500);
    delay(1000);
    digitalWrite(ledAmarelo, LOW);
    noTone(buzzer);
    delay(1000);
  }


  //Alarme soa as 14h00, ligando o LED amarelo e o buzzer
  if (timeClient.getHours() == 14 && timeClient.getMinutes() == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Hora do banheiro!");
    digitalWrite(ledAmarelo, HIGH);
    tone(buzzer, 1500);
    delay(1000);
    digitalWrite(ledAmarelo, LOW);
    noTone(buzzer);
    delay(1000);
  }


  //Alarme soa as 17h00, ligando o LED amarelo e o buzzer
  if (timeClient.getHours() == 17 && timeClient.getMinutes() == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Hora do banheiro!");
    digitalWrite(ledAmarelo, HIGH);
    tone(buzzer, 1500);
    delay(1000);
    digitalWrite(ledAmarelo, LOW);
    noTone(buzzer);
    delay(1000);
  }

  //Alarme soa as 20h00, ligando o LED amarelo e o buzzer
  if (timeClient.getHours() == 20 && timeClient.getMinutes() == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Hora do banheiro!");
    digitalWrite(ledAmarelo, HIGH);
    tone(buzzer, 1500);
    delay(1000);
    digitalWrite(ledAmarelo, LOW);
    noTone(buzzer);
    delay(1000);
  }

  //Alarme soa as 23h00, ligando o LED amarelo e o buzzer
  if (timeClient.getHours() == 23 && timeClient.getMinutes() == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Hora do banheiro!");
    digitalWrite(ledAmarelo, HIGH);
    tone(buzzer, 1500);
    delay(1000);
    digitalWrite(ledAmarelo, LOW);
    noTone(buzzer);
    delay(1000);
  }
}
