# Disciplina C115 - Conceitos e Tecnologias para Dispositivos Conectados

<h1 align="center">Tomada Inteligente com MQTT e NodeMCU</h1>

<p align="center">
    <img src="https://www.diariodeti.com.br/wp-content/uploads/2017/10/IoT-06-948x640.png">
</p>
Imagem retirada do site Mira Flores: https://www.diariodeti.com.br

### :books: Descrição

<p>Projeto de C115 da disciplina de Conceitos e Tecnologias para Dispositivos Conectados.</p>
<p>Projeto de uma tomada inteligente conectada a internet, que através de um broker MQTT online, se comunica com um aplicativo de celular que por sua vez controla o estado da tomada entre ligado e desligado, possibilitando o controle de qualquer eletrodoméstico, tal como uma cafeteira.</p>
<p>O projeto possui a capacidade de controlar um relé para ligar e desligar cargas em corrente alternada. Seu firmware possui as funções de entrar em modo Access Point (AP), onde é criado a própria rede WI-FI tal como um roteador doméstico. O usuário se conecta a essa rede e ao acessar o endereço 192.168.4.1 pelo navegador, é carregado uma página HTML pedindo o SSID e a senha da rede em que se deseja conectar. Ao preencher esse formulário no HTML carregado, o firmware desliga o modo AP e inicia o modo Station para tentar se conectar a rede indicada. Se a conexão for bem sucedida, o SSID e a senha da rede é gravada na EEPROM do NodeMCU tornando o processo de conexão automático para as próximas vezes que o projeto for ligado. Existe um botão para apagar a EEPROM e poder configurar uma nova rede WI-FI. Essa rede WI-FI onde o projeto se conecta necessita de conexão com a internet pois ela será necessária para comunicar com o Broker MQTT online e poder fazer o controle do eletrodoméstico pelo celular.</p>

#### Projeto
- Esse projeto foi construído usando uma placa NodeMCU equipada com um módulo WI-FI ESP8266, e o aplicativo utilizado foi MQTT Dash.

### :computer: Funcionalidades dos Projetos
#### Funcionalidades
- Access Point para configurar rede WI-FI do modo Station com conexão com a internet;
- Leitura e escrita do SSID e senha da rede WI-FI na memória EEPROM;
- controla um módulo relé ativando ou desativando cargas em corrente alternada ou contínuas;
- Utiliza um Broker MQTT online para qualquer dispositivo, incluindo um celular,poder controlar o relé.

### :hammer_and_wrench: Instalação e Execução
#### Preparação do ambiente no computador para o projeto
- [Git](https://git-scm.com/)
- [IDE Arduíno](https://www.arduino.cc/en/software)
- [Driver Serial](https://www.robocore.net/tutoriais/instalando-driver-do-nodemcu)
- [Instalando dependências do NodeMCU na IDE do Arduíno](https://www.robocore.net/tutoriais/como-programar-nodemcu-arduino-ide)
- [Aplicativo MQTT Dash](https://play.google.com/store/apps/details?id=net.routix.mqttdash&hl=pt_BR&gl=US)

#### compilando o projeto para o NodeMCU
Clone o repositório em seu computador para poder acessar o projeto:
```
git clone https://github.com/flaviobergamini/Projeto_C115_Tomada-Inteligente.git
```
Abra o arquivo na IDE do Arduíno, compila para o NodeMCU, pelo fato da EEPROM estar apagada, o sistema entrará no modo AP de imediato.
O SSID e a senha da rede WI-FI do modo AP é: "Tomada Inteligente" e 12345678. A conexão aparecerá limitada pelo fato de não ter internet, abra o navegador e digite o endereço 192.168.4.1 para cnfigurar a rede WI-FI que você deseja que o projeto se conecte.    

## :gear: Autores

* **Flávio Henrique Madureira Bergamini** - [Flávio](https://github.com/flaviobergamini)
* **Leonardo Martin** - [Leonardo](https://github.com/LeoMokey)
* **Breno Costa Ferreira Alves** - [Breno](https://github.com/Chubanga)

Sob a orientação do professor:
* **Samuel Baraldi Mafra**
