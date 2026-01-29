# Projeto Joystick DevTitans

Este repositório contém o firmware para um joystick baseado em ESP32 e um driver Linux para comunicação com o dispositivo.

O projeto foca no desenvolvimento de baixo nível, transformando sinais elétricos de pinos GPIO em eventos de input padrão do Android, fazendo com que o sistema reconheça o protótipo como um gamepad nativo.

## Funcionalidade Principal

- **Hardware do Joystick:** Um protótipo de joystick será construído utilizando um ESP32 para ler o estado de botões físicos.
- **Comunicação via GPIO:** O ESP32 se comunicará com o Raspberry Pi 4B diretamente através das portas GPIO. Cada botão pressionado no joystick resultará na alteração do estado de um pino GPIO correspondente.
- **Driver de Kernel:** O núcleo do projeto é a implementação de um driver de dispositivo de entrada para o Kernel Linux (AOSP). O próprio driver será responsável por todo o trabalho: ele irá monitorar os pinos GPIO, detectar as mudanças de estado e gerar os eventos de botão (ex: `BTN_A`, `BTN_B`) no formato padrão que o Android entende nativamente.
- **Integração Nativa:** Como o driver cria os eventos de forma padronizada, o Android InputFlinger reconhecerá o dispositivo automaticamente assim que o driver for carregado, permitindo que o joystick seja usado em qualquer aplicativo ou jogo compatível.

## Estrutura

- `firmware/` — Código-fonte do firmware do joystick ESP32 AOSP
- `driver/` — Código-fonte e documentação do driver Linux

## Firmware (ESP32)

### 📋 Requisitos

- ESP-IDF (Framework de desenvolvimento da Espressif)
- Placa ESP32 compatível

### ⚙️ Compilação e Gravação

1. Pegue o codigo do firmware no arquivo `firmware/firmware.ino` e compile usando o [Arduino IDE](https://docs.arduino.cc/software/ide/)
2. Carregue o firmware na placa ESP32 seguindo as instruções do Arduino IDE.

   ```
##  Driver Linux

Consulte o arquivo `driver/README.txt` para instruções de compilação e instalação do driver Linux. 

## Imagem AOSP para Raspberry Pi 4

Consulte o arquivo `driver/README.txt` para instruções de compilação e instalação da Imagem. 

## 🏗️ Garra Robótica Articulada

Abaixo segue o detalhamento, desde a prototipagem com materiais simples até a implementação de uma arquitetura estável com barramento I2C, para ser controlada pelo Joystick GPIO

### 🔗 Links e Recursos
* **Simulação Wokwi:** [Acesse o projeto aqui](https://wokwi.com/projects/454524449047083009)
* **Driver I2C PCA9685:** [Referência do Controlador PWM](https://www.smartprojectsbrasil.com.br/driver-controlador-pwm-servos-16-canais-i2c-pca9684)
* **Estrutura Mecânica:** [Kit Braço Robótico Acrílico](https://www.mercadolivre.com.br/kit-braco-robotico-em-acrilico-preto--4-servos-sg90/up/MLBU1093184659)

---

### 🔬 Histórico de Desenvolvimento

#### 1. Fase de Prototipagem e Simulação
A fase inicial focou na validação da lógica de controle e dos desafios físicos.
* **Simulação:** Realizado esboço básico no Wokwi para validar a lógica de pinagem.
* **Hardware Inicial:** Tentativa de montagem usando palitos de picolé e servos simples.
* **Problemas Detectados:** Movimentos mecanicamente travados e reinicialização constante da ESP32 (Brownout) devido ao consumo dos servos acima de 0.1A quando ligados diretamente no regulador da placa.

#### 2. Gestão de Energia e Potência
Testes com alicate amperímetro confirmaram que o pico de corrente dos servos sob carga exige uma fonte de, no mínimo, **3A**. Tentativas com fontes de 1A mostraram-se insuficientes para manter o torque estável.

#### 3. Versão Final (Mecânica e Eletrônica)
A solução definitiva utilizou a separação de barramentos e um driver dedicado:
* **Driver I2C PCA9685:** Facilitou a organização dos cabos e centralizou a alimentação, fornecendo energia estável tanto para os servos quanto para a ESP32 através do barramento I2C.
* **Montagem:** Kit em acrílico com 4 servos SG90. Nota: Foi necessário centralizar os servos antes da montagem final para evitar danos às engrenagens de plástico.

---

### 📐 Mapeamento de Movimento e Restrições

Foram realizados testes de movimento para definir os ângulos de segurança, evitando que os servos forcem a estrutura ou entrem em stall:

| Articulação | Canal PCA | Ângulo Mín. | Ângulo Máx. | Observação |
| :--- | :---: | :---: | :---: | :--- |
| **Base (Giro)** | 0 | 0° | 180° | Rotação lateral completa |
| **Ombro** | 1 | 15° | 165° | Evita colisão com a base |
| **Cotovelo** | 2 | 30° | 150° | Evita alavanca excessiva |
| **Garra** | 3 | 10° | 75° | 10°=Fechada / 75°=Aberta |

---

### 🛠️ Lições Aprendidas
* **Ajuste Mecânico:** Parafusos excessivamente apertados travam o movimento; o ajuste deve ser firme mas permitir a rotação livre.
* **Centralização:** Sempre calibrar o ponto zero do servo via firmware antes de fixar os braços de acrílico.
* **Estabilidade:** O uso do driver I2C foi o divisor de águas para eliminar ruídos elétricos e quedas de tensão no sistema.

---

### 🚀 Upgrades Futuros e P&D

#### 🛸 Mixagem de Servos (Algoritmo Estilo CCPM)
O próximo desafio técnico é implementar uma lógica de **Mixagem de Servos**, inspirada no sistema **CCPM de helicópteros 6CH**. 
* **Conceito:** Criar uma função onde o movimento do "Ombro" gere uma compensação automática e proporcional no "Cotovelo" e na "Garra".
* **Objetivo:** Manter a orientação da garra constante em relação ao solo ou ao objeto enquanto o braço se desloca, facilitando a operação manual e suavizando a trajetória.

#### Outros Upgrades:
- [ ] Upgrade para servos com engrenagens metálicas (MG90S).
- [ ] Implementação de controle remoto via interface Web (WebSockets) na ESP32.
- [ ] Substituição do chassi por impressão 3D (PETG) para maior rigidez estrutural.

## Desenvolvedores
<img width="206" height="308" alt="image" src="https://github.com/user-attachments/assets/d7893d29-3348-4ccd-accb-f93c75543fa3" />
<img width="207" height="310" alt="image" src="https://github.com/user-attachments/assets/9ae062f3-9604-4bbe-bcfc-11e2afe092ec" />
<img width="213" height="310" alt="image" src="https://github.com/user-attachments/assets/8c33e761-e184-4555-984b-0dfcda9221bc" />
<img width="203" height="308" alt="image" src="https://github.com/user-attachments/assets/8fc0b9bd-e98f-4f5d-a114-5f4769200092" />
<img width="207" height="308" alt="image" src="https://github.com/user-attachments/assets/78866ae0-bea3-446c-a44e-f0119bb2dbbc" />






