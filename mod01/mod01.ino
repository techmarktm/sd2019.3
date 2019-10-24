#include <WiFi.h> // Importa a Biblioteca ESP8266WiFi
#include <PubSubClient.h> // Importa a Biblioteca PubSubClient



// WIFI
const char* SSID = "Repeat"; // SSID / nome da rede WI-FI que deseja se conectar
const char* PASSWORD = "12345679@Repeat"; // Senha da rede WI-FI que deseja se conectar



// MQTT
const char* BROKER_MQTT = "m12.cloudmqtt.com";
int BROKER_PORT = 10858; // Porta do Broker MQTT
const char* mqttUser = "niwveled";    //user
const char* mqttPassword = "Jhkots1UqEvF";  //password

#define TOPICO_SUBSCRIBE "atuadores"     //tópico MQTT de escuta
#define TOPICO_PUBLISH   "sensores"    //tópico MQTT de envio de informações para Broker
#define ID_MQTT  "camadaCoisas"     //id mqtt (para identificação de sessão)



//Variáveis e objetos globais
WiFiClient espClient; // Cria o objeto espClient
PubSubClient MQTT(espClient); // Instancia o Cliente MQTT passando o objeto espClient

char EstadoSaida = '0';  //variável que armazena o estado atual da saída

const int portaLed = GPIO_NUM_18;
const int portaLDR = 34;

String valorLDR_str;
char valorLDR[10];
 
//Prototypes
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
    pinMode(portaLDR, INPUT);
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
    //verifica se deve colocar nivel alto de tensão na saída D4:
    //IMPORTANTE: o Led já contido na placa é acionado com lógica invertida (ou seja,
    //enviar HIGH para o output faz o Led apagar / enviar LOW faz o Led acender)
    if (msg.equals("Desliga") && EstadoSaida == '0')
    {
        digitalWrite(portaLed, LOW);
        EstadoSaida = '1';
        EnviaEstadoOutputMQTT();
    }

    //verifica se deve colocar nivel alto de tensão na saída D4:
    if (msg.equals("Liga") && EstadoSaida == '1')
    {
        digitalWrite(portaLed, HIGH);
        EstadoSaida = '0';
        EnviaEstadoOutputMQTT();
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

//Função: envia ao Broker o estado atual do output 
//Parâmetros: nenhum
//Retorno: nenhum
void EnviaEstadoOutputMQTT(void)
{
    if (EstadoSaida == '0')
      MQTT.publish(TOPICO_PUBLISH, "LedsLigados");

    if (EstadoSaida == '1')
      MQTT.publish(TOPICO_PUBLISH, "LedsDesligados");

    Serial.println("- Estado da saida enviado ao broker!");
    //delay(1000);
}

//Função: inicializa o output em nível lógico baixo
//Parâmetros: nenhum
//Retorno: nenhum
void InitOutput(void)
{
    //IMPORTANTE: o Led já contido na placa é acionado com lógica invertida (ou seja,
    //enviar HIGH para o output faz o Led apagar / enviar LOW faz o Led acender)
    pinMode(portaLed, OUTPUT);
    digitalWrite(portaLed, HIGH); 
}


//programa principal
void loop() 
{   
    int lumi = analogRead(portaLDR);   
    Serial.println("Nível de Luminosidade: ");
    Serial.println(lumi);

    valorLDR_str = String(lumi); //converting ftemp (the float variable above) to a string 
    valorLDR_str.toCharArray(valorLDR, valorLDR_str.length() + 1); //packaging up the data to publish to mqtt whoa...
    
    MQTT.publish(TOPICO_PUBLISH, valorLDR);
    
    delay(1000);
  
    //garante funcionamento das conexões WiFi e ao broker MQTT
    VerificaConexoesWiFIEMQTT();

    //envia o status de todos os outputs para o Broker no protocolo esperado
    //EnviaEstadoOutputMQTT();

    //keep-alive da comunicação com broker MQTT
    MQTT.loop();
}
