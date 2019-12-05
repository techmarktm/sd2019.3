#include <WiFi.h> // Importa a Biblioteca ESP8266WiFi
#include <PubSubClient.h> // Importa a Biblioteca PubSubClient
#include <Adafruit_Sensor.h>
#include <DHT.h>
#define DHTTYPE    DHT11
#define ATRASO_SENSOR (1000UL * 10 * 1)

unsigned long tempoDecorrido = millis() + ATRASO_SENSOR;//começa a contar o tempo


// WIFI
const char* SSID = "WEP"; // SSID / nome da rede WI-FI que deseja se conectar
const char* PASSWORD = "vaidartudocerto"; // Senha da rede WI-FI que deseja se conectar
//const char* SSID = "Repeat"; // SSID / nome da rede WI-FI que deseja se conectar
//const char* PASSWORD = "12345679@Repeat"; // Senha da rede WI-FI que deseja se conectar

/*
// MQTT
const char* BROKER_MQTT = "m12.cloudmqtt.com";
int BROKER_PORT = 10858; // Porta do Broker MQTT
const char* mqttUser = "niwveled";    //user
const char* mqttPassword = "Jhkots1UqEvF";  //password
*/

const char* BROKER_MQTT = "10.5.16.109";
int BROKER_PORT = 1883; // Porta do Broker MQTT
const char* mqttUser = "sdufjf";    //user
const char* mqttPassword = "Sd2019-03";  //password

#define TOPICO_SUBSCRIBE "sd/3504/atuadores"     //tópico MQTT de escuta
#define TOPICO_PUBLISH_TEMP   "sd/3504/sensores/temperatura"    //tópico MQTT de envio de informações para Broker
#define TOPICO_PUBLISH_UMI   "sd/3504/sensores/umidade"    //tópico MQTT de envio de informações para Broker
#define ID_MQTT  "camadaCoisas3"     //id mqtt (para identificação de sessão)

String valorTMP_str;
char valorTMP[4];

//Variáveis e objetos globais
WiFiClient espClient; // Cria o objeto espClient
PubSubClient MQTT(espClient); // Instancia o Cliente MQTT passando o objeto espClient

char EstadoSaida1 = '0';  //variável que armazena o estado atual da saída do Ventilador
char EstadoSaida2 = '0';  //variável que armazena o estado atual da saída da Computador

const int portaLuz1 = GPIO_NUM_19; //porta/pino da luz 1
const int portaLuz2 = GPIO_NUM_18; //porta/pino da luz 2
const int DHTPIN = GPIO_NUM_23; //porta/pino do sensor de temp

DHT dht(DHTPIN, DHTTYPE);

//Declaração das Funções
void initSerial();
void initWiFi();
void initMQTT();
void reconectWiFi(); 
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void VerificaConexoesWiFIEMQTT(void);
void InitOutput(void);

/* 
 *  Implementações das funções
 */
void setup() 
{
    //inicializações:
    InitOutput();
    initSerial();
    initWiFi();
    initMQTT();
    digitalWrite(portaLuz1, LOW);
    digitalWrite(portaLuz2, LOW);
    EstadoSaida1 = '0'; 
    EstadoSaida2 = '0';
}
 
//Função: inicializa comunicação serial com baudrate 115200 (para fins de monitorar no terminal serial 
//        o que está acontecendo.
//Parâmetros: nenhum
//Retorno: nenhum
void initSerial() 
{
    Serial.begin(115200);
}

//Função: inicializa e conecta-se na rede WI-FI desejada
//Parâmetros: nenhum
//Retorno: nenhum
void initWiFi() 
{
    delay(10);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID);
    Serial.println("Aguarde");
    
    reconectWiFi();
}
 
//Função: inicializa parâmetros de conexão MQTT(endereço do 
//        broker, porta e seta função de callback)
//Parâmetros: nenhum
//Retorno: nenhum
void initMQTT() 
{
    MQTT.setServer(BROKER_MQTT, BROKER_PORT);   //informa qual broker e porta deve ser conectado
    MQTT.setCallback(mqtt_callback);            //atribui função de callback (função chamada quando qualquer informação de um dos tópicos subescritos chega)
}
 
//Função: função de callback 
//        esta função é chamada toda vez que uma informação de 
//        um dos tópicos subescritos chega)
//Parâmetros: nenhum
//Retorno: nenhum
void mqtt_callback(char* topic, byte* payload, unsigned int length) 
{
    String msg;

    //obtem a string do payload recebido
    for(int i = 0; i < length; i++) 
    {
       char c = (char)payload[i];
       msg += c;
    }
  
    //toma ação dependendo da string recebida:
    if (msg.equals("desligaVentilador") && EstadoSaida1 == '1')
    {
        digitalWrite(portaLuz1, LOW);
        EstadoSaida1 = '0';
        //EnviaEstadoOutputMQTT();
    }

    //verifica se deve colocar nivel alto de tensão na saída da luz 1:
    if (msg.equals("ligaVentilador") && EstadoSaida1 == '0')
    {
        digitalWrite(portaLuz1, HIGH);
        EstadoSaida1 = '1';
        //EnviaEstadoOutputMQTT();
    }

    //verifica se deve colocar nivel baixo de tensão na saída da luz 1:
    if (msg.equals("desligaPC") && EstadoSaida2 == '1')
    {
        digitalWrite(portaLuz2, LOW);
        EstadoSaida2 = '0';
        //EnviaEstadoOutputMQTT();
    }

    //verifica se deve colocar nivel alto de tensão na saída da luz 1:
    if (msg.equals("ligaPC") && EstadoSaida2 == '0')
    {
        digitalWrite(portaLuz2, HIGH);
        EstadoSaida2 = '1';
        //EnviaEstadoOutputMQTT();
    }
    if (msg.equals("estadoVentilador"))
    {
        if (EstadoSaida1 == '1'){
          delay(200);
          MQTT.publish("sd/3504/atuadores/estado/ventilador", "1");
        }
        else {
          delay(200);
          MQTT.publish("sd/3504/atuadores/estado/ventilador", "0");
        }
    }
    if (msg.equals("estadoPC"))
    {
        if (EstadoSaida2 == '1'){
          delay(200);
          MQTT.publish("sd/3504/atuadores/estado/pc", "1");
        }
        else {
          delay(200);
          MQTT.publish("sd/3504/atuadores/estado/pc", "0");
        }
    }
}
 
//Função: reconecta-se ao broker MQTT (caso ainda não esteja conectado ou em caso de a conexão cair)
//        em caso de sucesso na conexão ou reconexão, o subscribe dos tópicos é refeito.
//Parâmetros: nenhum
//Retorno: nenhum
void reconnectMQTT() 
{
    while (!MQTT.connected()) 
    {
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT, mqttUser, mqttPassword)) 
        {
            Serial.println("Conectado com sucesso ao broker MQTT!");
            MQTT.subscribe(TOPICO_SUBSCRIBE); 
        } 
        else 
        {
            Serial.println("Falha ao reconectar no broker.");
            Serial.println("Havera nova tentatica de conexao em 2s");
            delay(2000);
        }
    }
}
 
//Função: reconecta-se ao WiFi
//Parâmetros: nenhum
//Retorno: nenhum
void reconectWiFi() 
{
    //se já está conectado a rede WI-FI, nada é feito. 
    //Caso contrário, são efetuadas tentativas de conexão
    if (WiFi.status() == WL_CONNECTED)
        return;
        
    WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI
    
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(100);
        Serial.print(".");
    }
  
    Serial.println();
    Serial.print("Conectado com sucesso na rede ");
    Serial.print(SSID);
    Serial.println("\nIP obtido: ");
    Serial.println(WiFi.localIP());
}

//Função: verifica o estado das conexões WiFI e ao broker MQTT. 
//        Em caso de desconexão (qualquer uma das duas), a conexão
//        é refeita.
//Parâmetros: nenhum
//Retorno: nenhum
void VerificaConexoesWiFIEMQTT(void)
{
    if (!MQTT.connected()) 
        reconnectMQTT(); //se não há conexão com o Broker, a conexão é refeita
    
     reconectWiFi(); //se não há conexão com o WiFI, a conexão é refeita
}

//Função: inicializa o output em nível lógico baixo
//Parâmetros: nenhum
//Retorno: nenhum
void InitOutput(void)
{
    //IMPORTANTE: o Led já contido na placa é acionado com lógica invertida (ou seja,
    //enviar HIGH para o output faz o Led apagar / enviar LOW faz o Led acender)
    pinMode(portaLuz1, OUTPUT);
    digitalWrite(portaLuz1, HIGH); 
    pinMode(portaLuz2, OUTPUT);
    digitalWrite(portaLuz2, HIGH);
    dht.begin();
    //pinMode(sensorTemp, INPUT);
}


//Função: realiza a leitura do sensor ldr, imprime no terminal e publica no topico do MQTT os dados obtidos pelo sensor
//Parâmetros: nenhum
//Retorno: nenhum
void leituraTemp(){
  
    float temp = dht.readTemperature();
    float umi = dht.readHumidity();
    Serial.println("Temperatura Atual:");
    Serial.println(temp);
    Serial.println("Umidade Atual:");
    Serial.println(umi);
    Serial.println("Estado Ventilador:");
    Serial.println(EstadoSaida1);
    Serial.println("Estado PC:");
    Serial.println(EstadoSaida2);
    
    valorTMP_str = String(temp); //converting ftemp (the float variable above) to a string
    valorTMP_str.toCharArray(valorTMP, valorTMP_str.length() + 1); //packaging up the data to publish to mqtt whoa...
    MQTT.publish(TOPICO_PUBLISH_TEMP, valorTMP);

    valorTMP_str = String(umi); //converting ftemp (the float variable above) to a string
    valorTMP_str.toCharArray(valorTMP, valorTMP_str.length() + 1); //packaging up the data to publish to mqtt whoa...
    MQTT.publish(TOPICO_PUBLISH_UMI, valorTMP);
    /*
    int raw = analogRead(sensorTemp);
    double voltagem = (raw / 2048.0) * 3300;  
    double temp = voltagem * 0.1;
    Serial.println("Temperatura: ");
    Serial.println(temp);

    valorTMP_str = String(temp); //converting ftemp (the float variable above) to a string 
    valorTMP_str.toCharArray(valorTMP, valorTMP_str.length() + 1); //packaging up the data to publish to mqtt whoa...
    
    MQTT.publish(TOPICO_PUBLISH, valorTMP);
    */
}

//programa principal
void loop() 
{   

    // verifica se passou o tempo de atraso estipulado
    if((long)(millis() - tempoDecorrido) >= 0) {
      //chama a funcao para receber os dados do LDR
      leituraTemp();
      tempoDecorrido += ATRASO_SENSOR;
    }
  
    //garante funcionamento das conexões WiFi e ao broker MQTT
    VerificaConexoesWiFIEMQTT();

    //envia o status de todos os outputs para o Broker no protocolo esperado
    //EnviaEstadoOutputMQTT();

    //keep-alive da comunicação com broker MQTT
    MQTT.loop();
}
