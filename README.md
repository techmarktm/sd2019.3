# sd2019.3
## Trabalho de Sistemas Distribuídos - UFJF - 2019.3 - Camada das Coisas

#### Grupo: Bruno, Diego, Lucas e Aleksander

Arquivos e Projetos
-------------

Mod1 : Módulo 1, que ficará no fundo da sala. Responsável pela leitura do sensor de porta 
e acionamento das luzes da sala de aula. Este código funcionará no ESP32.

Mod2 : Módulo 2, que ficará na frente sala. Responsável pela leitura do sensor LDR (luminosidade)
e emissao dos sinais infravermelho para ligar e desligar o projetor. Este código funcionará no ESP8266,
pois atualmente o ESP32 não possui suporte direto para as bibliotecas do sensor/emissor IR.

Bibliotecas utilizadas:
-------------

IRremote para ESP8266, Esp8266Wifi, Wifi e PubSubClient(mqtt), bem como as integrações para as placas
ESP32 e ESP8266 para arduino.

Drivers Utilizados:
-------------

CH34x para o ESP8266 e CP210x para o ESP32.

Objetivos:
-------------

- \[x] Comunicar com Broker
- \[x] Obter informações do sensor LDR e enviar para um tópico do broker a cada X minutos
- \[x] Obter informações do sensor de porta e enviar para um tópico do broker a cada evento (abrir ou fechar a porta)
- \[x] Obter informações do sensor infravermelho
- \[x] Acionar relés para ligar luzes (e receber uma mensagem do Broker solicitando tal acionamento)
- \[x] Acionar o infravermelho para ligar/desligar o projetor (e receber uma mensagem do Broker solicitando tal acionamento)
- \[ ] Definir quais tópicos serão utilizados para os sensores e atuadores junto ao grupo do Broker
- \[ ] Definir se vai utilizar sensor de temperatura e o ventilador, e em caso positivo, pedir ao Pagani tais itens
- \[ ] Marcar um dia para verificar a situação do interruptor das luzes na sala de aula (se possível, acompanhado de alguém que faça manutenção e etc)
- \[ ] Obter códigos do controle remoto do projetor

### Informações gerais

Durante a fase de testes, utilizamos o seviço em nuvem do MQTT:

https://customer.cloudmqtt.com/login
