# RenaultSWC2USB
Uma interface open-source para comandos de rádio renault dos anos 2000.

Caso esteja usando uma multimidia chinesa(ANDROID), com uma interface zendel ou parecida e está bugando tudo, aqui está a sua solução.

Basicamente, este código interpreta os comandos do controle e converte para um comando de teclado normal e aproveita os controles de midia.

Ahhh mas porque não converter para o padrão SWC????
Porque o output terá que ser analógico, e um arduino mini de menos de 50 reais não tem capacidade de fazer isso, teria que partir pra uma solução mais cara que a interface zendel que você encontra no mercado.

Antes de começar, verifique se sua multimidia funciona com um teclado padrão, funcionando, é só implementar.

Neste caso foi usado uma Arduino Esp32 S2 4mb Flash 2mb Psram D1 Minm, mas qualquer outra parecida que tenha usb-otg vai te atender.

O DebugButtons pode te ajudar caso não funcionar algum botão, você terá que pegar o hex que vai estar no console e adicionar na função handleEvent do USB-HID.
