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

## Desenvolvedores
<img width="206" height="308" alt="image" src="https://github.com/user-attachments/assets/d7893d29-3348-4ccd-accb-f93c75543fa3" />
<img width="207" height="310" alt="image" src="https://github.com/user-attachments/assets/9ae062f3-9604-4bbe-bcfc-11e2afe092ec" />
<img width="213" height="310" alt="image" src="https://github.com/user-attachments/assets/8c33e761-e184-4555-984b-0dfcda9221bc" />
<img width="203" height="308" alt="image" src="https://github.com/user-attachments/assets/8fc0b9bd-e98f-4f5d-a114-5f4769200092" />
<img width="207" height="308" alt="image" src="https://github.com/user-attachments/assets/78866ae0-bea3-446c-a44e-f0119bb2dbbc" />






