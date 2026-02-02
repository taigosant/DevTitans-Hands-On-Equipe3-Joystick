#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// ==========================================
// 1. CONFIGURAÇÕES DE HARDWARE (SG90)
// ==========================================
#define SERVOMIN  170 
#define SERVOMAX  550 

// MAPA DE CANAIS
#define SERVO_GARRA    1
#define SERVO_BASE     2
#define SERVO_OMBRO    3
#define SERVO_COTOVELO 4

// SEUS LIMITES DE SEGURANÇA
#define BASE_MIN       0  
#define BASE_MAX       170 

#define GARRA_FECHADA  70  
#define GARRA_ABERTA   100 

#define OMBRO_CIMA     70
#define OMBRO_BAIXO    160 

#define COT_BAIXO      60
#define COT_CIMA       130

#define PASSO_MOVIMENTO 5 

// ==========================================
// 2. VARIÁVEIS GLOBAIS
// ==========================================
int posBase = 90;
int posOmbro = 90;
int posCotovelo = 90;
int posGarra = 90;

bool modoDemoAtivo = false;
int passoDemo = 0;

const char* ssid     = "RobotArm";    
const char* password = "12345678";    

AsyncWebServer server(80);
AsyncWebSocket wsRobotArmInput("/RobotArmInput");

// ==========================================
// 3. INTERFACE WEB (COM SUPORTE A GAMEPAD)
// ==========================================
const char* htmlHomePage PROGMEM = R"HTMLHOMEPAGE(
<!DOCTYPE html>
<html lang="pt-BR">
  <head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no">
  <title>Robô Gamepad</title>
    <style>
    body { font-family: 'Segoe UI', sans-serif; text-align: center; background-color: #f4f4f9; margin: 0; padding: 10px; }
    h1 { color: #E03C31; margin: 5px; }
    #statusgp { color: #555; font-weight: bold; margin-bottom: 10px; font-size: 14px; }
    .connected { color: #28a745 !important; }
    
    .control-group { background: white; border-radius: 15px; padding: 10px; margin: 10px auto; max-width: 400px; box-shadow: 0 4px 10px rgba(0,0,0,0.1); }
    .btn { background-color: #007bff; color: white; border: none; padding: 15px 0; width: 80px; font-size: 16px; font-weight: bold; border-radius: 10px; margin: 4px; touch-action: manipulation; }
    .btn:active { background-color: #0056b3; transform: translateY(2px); }
    .btn-red { background-color: #dc3545; width: 45%; }
    .btn-green { background-color: #28a745; width: 45%; }
    .btn-reset { background-color: #6c757d; width: 90%; margin-top: 5px; }
    .row { display: flex; justify-content: center; align-items: center; }
    
    .legend { font-size: 12px; color: #666; margin-top: 5px; text-align: left; padding-left: 20px;}
    </style>
  </head>
  <body>
    <h1>Braço Robótico</h1>
    <div id="statusgp">Conecte o Controle e aperte um botão...</div>

    <div class="control-group">
      <h3>Mapeamento (XYAB)</h3>
      <div class="legend">
        🕹️ <b>Analógico:</b> Base e Ombro<br>
        🔼 <b>Y (Cima):</b> Cotovelo Sobe<br>
        🔽 <b>A (Baixo):</b> Cotovelo Desce<br>
        ◀️ <b>X (Esq):</b> Garra Fecha<br>
        ▶️ <b>B (Dir):</b> Garra Abre
      </div>
    </div>

    <div class="control-group">
      <div class="row">
        <button class="btn btn-green" onclick="sendCommand('Demo', 'ON')">DEMO</button>
        <button class="btn btn-red" onclick="sendCommand('Demo', 'OFF')">PARAR</button>
      </div>
      <button class="btn btn-reset" onclick="sendCommand('Home', 'GO')">RESET</button>
    </div>

    <div class="control-group">
      <h3>Manual / Touch</h3>
      <div class="row"><button class="btn" onclick="sendCommand('Gripper','OPEN')">Abrir</button><button class="btn" onclick="sendCommand('Gripper','CLOSE')">Fechar</button></div>
      <div class="row"><button class="btn" onclick="sendCommand('Shoulder','UP')">Sobe</button></div>
      <div class="row"><button class="btn" onclick="sendCommand('Elbow','DOWN')">Recolhe</button><button class="btn" onclick="sendCommand('Elbow','UP')">Estica</button></div>
      <div class="row"><button class="btn" onclick="sendCommand('Shoulder','DOWN')">Desce</button></div>
      <div class="row"><button class="btn" onclick="sendCommand('Base','LEFT')"><</button><button class="btn" onclick="sendCommand('Base','RIGHT')">></button></div>
    </div> 
  
    <script>
      var wsUrl = "ws:\/\/" + window.location.hostname + "/RobotArmInput";      
      var websocket;
      var lastUpdate = 0; 
      
      function initWebSocket() {
        websocket = new WebSocket(wsUrl);
        websocket.onclose = function(event){ setTimeout(initWebSocket, 1000); };
      }
      
      function sendCommand(parte, acao) {
        if(websocket.readyState === WebSocket.OPEN) {
           websocket.send(parte + "," + acao);
        }
      }

      window.addEventListener("gamepadconnected", function(e) {
        var gpStatus = document.getElementById("statusgp");
        gpStatus.innerHTML = "🎮 Conectado: " + e.gamepad.id;
        gpStatus.className = "connected";
        loopGamepad(); 
      });

      function loopGamepad() {
        var gamepads = navigator.getGamepads ? navigator.getGamepads() : [];
        var gp = gamepads[0]; 

        if (gp) {
          var agora = Date.now();
          if (agora - lastUpdate > 80) { // 80ms delay para não travar
            
            // --- 1. ANALÓGICO ESQUERDO ---
            if (gp.axes[0] < -0.5) sendCommand('Base', 'LEFT');
            if (gp.axes[0] > 0.5)  sendCommand('Base', 'RIGHT');
            if (gp.axes[1] < -0.5) sendCommand('Shoulder', 'UP');
            if (gp.axes[1] > 0.5)  sendCommand('Shoulder', 'DOWN');

            // --- 2. BOTÕES (XYAB) ---
            if (gp.buttons[3].pressed) sendCommand('Elbow', 'UP');    // Y
            if (gp.buttons[0].pressed) sendCommand('Elbow', 'DOWN');  // A
            if (gp.buttons[2].pressed) sendCommand('Gripper', 'CLOSE'); // X
            if (gp.buttons[1].pressed) sendCommand('Gripper', 'OPEN');  // B
            
            if (gp.buttons[9] && gp.buttons[9].pressed) sendCommand('Demo', 'ON'); // Start

            lastUpdate = agora;
          }
        }
        requestAnimationFrame(loopGamepad);
      }
      
      window.onload = initWebSocket;
    </script>
  </body>    
</html>
)HTMLHOMEPAGE";

// ==========================================
// 4. FUNÇÕES DE MOVIMENTO
// ==========================================

void aplicarServo(int canal, int angulo) {
  int pulso = map(angulo, 0, 180, SERVOMIN, SERVOMAX);
  pwm.setPWM(canal, 0, pulso);
}

// ----------------------------------------------------
// Função 1: Movimento Suave APENAS para o Demo
// ----------------------------------------------------
void moverSuaveDemo(int canal, int anguloAlvo, int delayMs) {
  int *posAtual;
  if (canal == SERVO_BASE) posAtual = &posBase;
  else if (canal == SERVO_OMBRO) posAtual = &posOmbro;
  else if (canal == SERVO_COTOVELO) posAtual = &posCotovelo;
  else if (canal == SERVO_GARRA) posAtual = &posGarra;
  else return;

  if (*posAtual == anguloAlvo) return;
  int step = (*posAtual < anguloAlvo) ? 1 : -1;

  while (*posAtual != anguloAlvo) {
    wsRobotArmInput.cleanupClients(); 
    if (modoDemoAtivo == false) return; 
    *posAtual += step;
    aplicarServo(canal, *posAtual);
    delay(delayMs); 
  }
}

// ----------------------------------------------------
// Função 2: Movimento Suave para o HOME (RESET)
// ----------------------------------------------------
void moverParaHome(int canal, int anguloAlvo, int delayMs) {
  int *posAtual;
  if (canal == SERVO_BASE) posAtual = &posBase;
  else if (canal == SERVO_OMBRO) posAtual = &posOmbro;
  else if (canal == SERVO_COTOVELO) posAtual = &posCotovelo;
  else if (canal == SERVO_GARRA) posAtual = &posGarra;
  else return;

  if (*posAtual == anguloAlvo) return;
  int step = (*posAtual < anguloAlvo) ? 1 : -1;

  while (*posAtual != anguloAlvo) {
    wsRobotArmInput.cleanupClients(); 
    *posAtual += step;
    aplicarServo(canal, *posAtual);
    delay(delayMs); 
  }
}

void processarManual(String parte, String acao) {
  if (modoDemoAtivo) {
    modoDemoAtivo = false;
  }

  if (parte == "Base") {
    if (acao == "LEFT")  posBase += PASSO_MOVIMENTO;
    if (acao == "RIGHT") posBase -= PASSO_MOVIMENTO;
    posBase = constrain(posBase, BASE_MIN, BASE_MAX);
    aplicarServo(SERVO_BASE, posBase);
  }
  else if (parte == "Shoulder") {
    if (acao == "UP")    posOmbro -= PASSO_MOVIMENTO; 
    if (acao == "DOWN")  posOmbro += PASSO_MOVIMENTO;
    posOmbro = constrain(posOmbro, OMBRO_CIMA, OMBRO_BAIXO);
    aplicarServo(SERVO_OMBRO, posOmbro);
  }
  else if (parte == "Elbow") {
    if (acao == "UP")    posCotovelo += PASSO_MOVIMENTO;
    if (acao == "DOWN")  posCotovelo -= PASSO_MOVIMENTO;
    posCotovelo = constrain(posCotovelo, COT_BAIXO, COT_CIMA);
    aplicarServo(SERVO_COTOVELO, posCotovelo);
  }
  else if (parte == "Gripper") {
    if (acao == "OPEN")  posGarra = GARRA_ABERTA;
    if (acao == "CLOSE") posGarra = GARRA_FECHADA;
    aplicarServo(SERVO_GARRA, posGarra);
  }
}

// ==========================================
// 5. EVENTOS DO SERVIDOR
// ==========================================

void handleRoot(AsyncWebServerRequest *request) {
  request->send_P(200, "text/html", htmlHomePage);
}

void handleNotFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "File Not Found");
}

void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {                      
  if (type == WS_EVT_DATA) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
      std::string myData = "";
      myData.assign((char *)data, len);
      int commaIndex = myData.find(',');
      if (commaIndex == -1) return;
      
      String key = String(myData.substr(0, commaIndex).c_str());
      String val = String(myData.substr(commaIndex + 1).c_str());
      
      if (key == "Demo") {
        if (val == "ON") {
          modoDemoAtivo = true;
          passoDemo = 0; 
        } else {
          modoDemoAtivo = false;
        }
      } 
      // === BOTÃO HOME ===
      else if (key == "Home") {
        modoDemoAtivo = false; 
        Serial.println("Resetando...");
        moverParaHome(SERVO_GARRA, 90, 8);
        moverParaHome(SERVO_OMBRO, 90, 8);
        moverParaHome(SERVO_COTOVELO, 90, 8);
        moverParaHome(SERVO_BASE, 90, 8);
      }
      else {
        processarManual(key, val);
      }
    }
  }
}

// ==========================================
// 6. SETUP
// ==========================================

void setup(void) {
  Serial.begin(115200);
  pwm.begin();
  
  // FREQUENCIA CRITICA PARA SG90 = 50Hz
  pwm.setPWMFreq(50); 
  delay(100);

  // Posição Inicial
  aplicarServo(SERVO_BASE, posBase);
  aplicarServo(SERVO_OMBRO, posOmbro);
  aplicarServo(SERVO_COTOVELO, posCotovelo);
  aplicarServo(SERVO_GARRA, posGarra);

  WiFi.softAP(ssid, password);
  Serial.print("IP: "); Serial.println(WiFi.softAPIP());

  server.on("/", HTTP_GET, handleRoot);
  server.onNotFound(handleNotFound);
  wsRobotArmInput.onEvent(onWebSocketEvent);
  server.addHandler(&wsRobotArmInput);
  server.begin();
}

// ==========================================
// 7. LOOP
// ==========================================

void loop() {
  wsRobotArmInput.cleanupClients();

  if (modoDemoAtivo) {
    int velLenta  = 8;  
    int velMedia  = 5;  
    int velRapida = 3;  

    int ombroChao = 150; 
    int cotoveloChao = COT_BAIXO; 

    switch (passoDemo) {
      case 0: // Preparar
        moverSuaveDemo(SERVO_GARRA, GARRA_ABERTA, velRapida);
        moverSuaveDemo(SERVO_OMBRO, 90, velLenta);
        moverSuaveDemo(SERVO_COTOVELO, 90, velLenta);
        moverSuaveDemo(SERVO_BASE, 90, velLenta);
        delay(100); break;
        
      case 1: // Esq + Chão
        moverSuaveDemo(SERVO_BASE, BASE_MAX, velLenta);
        moverSuaveDemo(SERVO_COTOVELO, cotoveloChao, velMedia);
        moverSuaveDemo(SERVO_OMBRO, ombroChao, velLenta); 
        delay(200); break;
        
      case 2: // Pegar
        moverSuaveDemo(SERVO_GARRA, GARRA_FECHADA, velRapida);
        delay(200); break;

      case 3: // Levantar
        moverSuaveDemo(SERVO_OMBRO, OMBRO_CIMA, velLenta);
        moverSuaveDemo(SERVO_COTOVELO, 90, velMedia);
        delay(100); break;
        
      case 4: // Levar Direita
        moverSuaveDemo(SERVO_BASE, BASE_MIN, velLenta);
        delay(100); break;
        
      case 5: // Baixar Direita
        moverSuaveDemo(SERVO_COTOVELO, cotoveloChao, velMedia);
        moverSuaveDemo(SERVO_OMBRO, ombroChao, velLenta); 
        delay(200); break;

      case 6: // Soltar
        moverSuaveDemo(SERVO_GARRA, GARRA_ABERTA, velRapida);
        delay(200); break;

      case 7: // Afastar
        moverSuaveDemo(SERVO_OMBRO, 110, velLenta);
        delay(200); break;

      case 8: // Voltar p/ Pegar
        moverSuaveDemo(SERVO_OMBRO, ombroChao, velLenta);
        delay(200); break;

      case 9: // Re-Pegar
        moverSuaveDemo(SERVO_GARRA, GARRA_FECHADA, velRapida);
        delay(200); break;

      case 10: // Levantar
        moverSuaveDemo(SERVO_OMBRO, OMBRO_CIMA, velLenta);
        moverSuaveDemo(SERVO_COTOVELO, 90, velMedia);
        delay(100); break;

      case 11: // Voltar Esq
        moverSuaveDemo(SERVO_BASE, BASE_MAX, velLenta);
        delay(100); break;

      case 12: // Baixar Origem
        moverSuaveDemo(SERVO_COTOVELO, cotoveloChao, velMedia);
        moverSuaveDemo(SERVO_OMBRO, ombroChao, velLenta);
        delay(200); break;

      case 13: // Soltar Final
        moverSuaveDemo(SERVO_GARRA, GARRA_ABERTA, velRapida);
        delay(200); break;

      case 14: // Reiniciar
        moverSuaveDemo(SERVO_OMBRO, 90, velLenta);
        moverSuaveDemo(SERVO_COTOVELO, 90, velLenta);
        delay(400); break;
    }
    
    if (modoDemoAtivo) {
      passoDemo++;
      if (passoDemo > 14) passoDemo = 1; 
    }
  }
}