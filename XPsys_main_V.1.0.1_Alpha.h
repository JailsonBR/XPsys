/*
  Programa de seleção de Menu com telas, desenvolvido por JailsonBR utilizando a biblioteca u8g2 do olikraus.
   
  PORFAVOR NÃO REMOVA NENHUMA INFORMAÇÃO DESTE CABEÇALHO, ESTE É UM CÓDIGO OPENSOURCE SOB LICENÇA DO CRIADOR
   
  Codigo adaptado por JailsonBR do site XProjetos.net em 03-06-2019, o uso deste código é livre porem deve respeitar e manter o nome do criador
  e também de quem modificou, o código está sob GNU GENERAL PUBLIC LICENSE Versão 3 (https://www.gnu.org/licenses/quick-guide-gplv3.pt-br.html)

  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

  Copyright(c) sobre o XPsys 2019, XProjetos.net

  XPsys Versão 1.0.1 (Alpha)

============ BSD License for U8g2lib Code ============

Universal 8bit Graphics Library (https://github.com/olikraus/u8g2)

Copyright (c) 2016-2019, olikraus@gmail.com
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list 
  of conditions and the following disclaimer.
  
* Redistributions in binary form must reproduce the above copyright notice, this 
  list of conditions and the following disclaimer in the documentation and/or other 
  materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 

============================================================================================================
                                          END OF THE U8G2 license
============================================================================================================
*/

#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#define Bot_PinoADC 1                                        //O compilador substituirá qualquer menção de Bot_PinoADC ao valor 1 no momento da compilação.
#define FontePadrao u8g2_font_5x7_tf                          //Selecione a fotne padrão de todos os menus exceto os icones

U8G2_ST7920_128X64_1_SW_SPI u8g2(U8G2_R0, 33, 32, 35, 27);    //Enable, RW, RS, RESET  

//int Configura as variaveis de -32,768 a 32,767, ocupa 2 bytes (16 bits)
int Bot_LeituraADC = 0;                                       //Variavel para Leitura da porta analogica
int bot_lido_anterior = 0;                                    //Valor temporario da porta analogica
int ChangeMenu = 0;                                           //Registra a nevegação no menu principal
int Tela = -1;                                                //Varíaveis para as telas individuais (-1 é o menu principal)

//bool variável de 0 ou 1, ocupa 1 byte (256 bytes)
bool bot_esquerdo = 0;                                        //Variavel de acionamento do botão (1 = ativado)
bool bot_acima = 0;                                           //Variavel de acionamento do botão (1 = ativado)
bool bot_abaixo = 0;                                          //Variavel de acionamento do botão (1 = ativado)
bool bot_direito = 0;                                         //Variavel de acionamento do botão (1 = ativado)
bool bot_selecionar = 0;                                      //Variavel de acionamento do botão (1 = ativado)
bool bot_voltar = 0;                                          //Variavel de acionamento do botão (1 = ativado)
bool bot_loop = 1;                                            //Variavel para o loop do botão evita que passe mais que uma vez no loop
bool Single_loop = 1;                                         //Variavel para o loop do menu de tela evita que passe mais que uma vez no loop acionado dievrsas vezes o botão;

//unsigned long variável de 0 a 4,294,967,295, ocupa 4 bytes (32 bits)
unsigned long tempo_bot_leitura_ultimo = 0;                   //ultimo tempo que o botão foi pressionado (millis())
unsigned long bot_debounce_tempo = 70;                        //tempo do debounce dos botões em ms
unsigned long diferenca_tempo = 0;                            //diferença dos tempos de acionamento com o millis()


byte ItensMenu = 11;                                          //Coloque o numero de itens do menu

char *listaMenu [] = {                                        //Lista de Itens do Menu, sempre começando do item 0
"Relogio",
"Botoes",
"Configuracoes",
"Bluetooth",
"Wifi",
"Servidor Web",
"Senha",
"LED RBG",
"Bateria",
"Logs",
"Telas"
};

/*====================================================================================
                            Navegação de Botões (Sub-Rotina)
====================================================================================*/
void Botoes_Navegacao(){  
    Bot_LeituraADC = analogRead(Bot_PinoADC);                 //Leitura de porta analógica e armazenamento na variável
    
    if(bot_loop == 1) {                                       //Verifica se já passou no Loop dos botões e limpa as variáveis
      bot_esquerdo = 0;
      bot_acima = 0;
      bot_abaixo = 0;
      bot_direito = 0;
      bot_selecionar = 0;
      bot_voltar = 0;
      bot_loop = 0;
      Single_loop = 0;
    }

//Condição bara o Debounce
  if (Bot_LeituraADC < 3500) {                                //Caso algum botão seja apertado (se nenhum for apertado o valor padrão é 4095)
        tempo_bot_leitura_ultimo = millis();                  //Reinicia a contagem do ultimo tempo de leitura
        bot_lido_anterior = Bot_LeituraADC;                   //Armazena o valor da leitura da porta em aoutra variável
  }
  diferenca_tempo = millis() - tempo_bot_leitura_ultimo;      //Faz a diferença do tempo da ultima vez que o botão foi pressionado

  if (diferenca_tempo > bot_debounce_tempo) {                 //Caso o tempo da ultima vez que o botão foi precionado for maior que o Debounce

    if (bot_lido_anterior < 3500) {                           //Caso algum botão seja apertado (se nenhum for apertado o valor padrão é 4095)
      if(bot_lido_anterior > 0 && bot_lido_anterior < 200)    //Caso o botão Esquerdo seja precionado (Resistor de 1K)
      {
        bot_esquerdo = 1;                                     //Muda o estado da variavel do botão para verdadeiro
      }
      if(bot_lido_anterior > 500 && bot_lido_anterior < 1200) //Caso o botão Acima seja precionado (Resistor de 4K7)
      {
        bot_acima = 1;                                        //Muda o estado da variavel do botão para verdadeiro
      }
      if(bot_lido_anterior > 1200 && bot_lido_anterior < 1800)//Caso o botão Abaxo seja precionado (Resistor de 8K2)
      {
        bot_abaixo = 1;                                       //Muda o estado da variavel do botão para verdadeiro
      }
      if(bot_lido_anterior > 1800 && bot_lido_anterior < 2400)//Caso o botão Direito seja precionado (Resistor de 15K)
      {
        bot_direito = 1;                                      //Muda o estado da variavel do botão para verdadeiro
      }
          if(bot_lido_anterior > 2400 && bot_lido_anterior < 2900)//Caso o botão Selecionar seja precionado (Resistor de 33K)
      {
        bot_selecionar = 1;                                   //Muda o estado da variavel do botão para verdadeiro
      }
          if(bot_lido_anterior > 2900 && bot_lido_anterior < 3500)//Caso o botão Voltar seja precionado (Resistor de 47K)
      {
        bot_voltar = 1;                                       //Muda o estado da variavel do botão para verdadeiro
      }
      bot_loop = 1;                                           //Registra que ja foi passado no laço dos botões
      bot_lido_anterior = 4000;                               //limpa a variável temporária dos botões
    }
  }
} //--------------------------FIM - Navegação de Botões ------------------------------------


//Mais informações deste projeto visite: XProjetos.net


/*====================================================================================
                            Tela de Exemplo (Sub-Rotina)
====================================================================================*/
void TelaExemplo(){                                           //Modelo para uma nova tela
    if (Tela == 10) {                                         //Caso a variável de tela seja ...
    u8g2.firstPage();                                         //Este comando é parte do loop (imagem) que renderiza o conteúdo da exibição. (Obrigatório)
    do {                                                      //Tudo o que estiver dentro do "do {}" será exibido na tela.
                                                              // Escreva aqui a tela do programa
    }
    while ( u8g2.nextPage() );                                //Este comando é parte do loop (imagem) que renderiza o conteúdo da exibição. (Obrigatório)
  }
}//--------------------------FIM - Tela de Exemplo ------------------------------------


/*====================================================================================
                            MENU PRINCIPAL (Sub-Rotina)
====================================================================================*/
void Menu_Principal() {                                       //Tela do menu Principal
  if (Tela == -1) {                                           //Caso a variável de tela seja -1
    u8g2.firstPage();                                         //Este comando é parte do loop (imagem) que renderiza o conteúdo da exibição. (Obrigatório)
    do {                                                      //Tudo o que estiver dentro do "do {}" será exibido na tela.
      u8g2.setFont(u8g2_font_open_iconic_all_1x_t);           //Configura a fonte tipo icone pequeno dos proximos itens
      u8g2.drawGlyph(2, 9, 199);	                            //Desenha um pequeno icone de lista no topo esquerdo da tela
      u8g2.setFont(FontePadrao);                              //Retorna para a fonte padrão do projeto
      
      //Topo do Menu Principal
      u8g2.setCursor(28, 8);                                  //Configura a posição do proximo item a ser escrito na tela
      u8g2.drawStr(28,8,"Menu Principal");                    //Escreve o titulo o topo da página (semelhante ao print porem não aceita UTF-8)
      u8g2.setDrawColor(2);                                   //Permite a sobreposição de itens de forma "transparente"
      u8g2.drawBox(1,0,126,10);                               //Desenha um retangulo preenchido por cima do nome do Menu Principal

      //Desenho dos quadrados dos icones
      u8g2.drawRFrame(1,11,20,20,0);
      u8g2.drawRFrame(22,11,20,20,0);
      u8g2.drawRFrame(43,11,20,20,0);
      u8g2.drawRFrame(64,11,20,20,0);
      u8g2.drawRFrame(85,11,20,20,0);
      u8g2.drawRFrame(106,11,20,20,0);
      u8g2.drawRFrame(1,32,20,20,0);
      u8g2.drawRFrame(22,32,20,20,0);
      u8g2.drawRFrame(43,32,20,20,0);
      u8g2.drawRFrame(64,32,20,20,0);
      u8g2.drawRFrame(85,32,20,20,0);
      u8g2.drawRFrame(106,32,20,20,0);

      //desenha os icones de cada menu, para personalizar olhe a fonte da biblioteca
      u8g2.setFont(u8g2_font_open_iconic_all_2x_t);           //Configura a fonte tipo icone grande dos proximos itens
      u8g2.drawGlyph(3, 29, 123);	                            //Icone do Relogio
      u8g2.drawGlyph(24, 29, 106);	                          //Icone do Botões 
      u8g2.drawGlyph(45, 29, 129);	                          //Icone do Configurações
      u8g2.drawGlyph(66, 29, 94);	                            //Icone do Bluetooth
      u8g2.drawGlyph(87, 29, 248);	                          //Icone do Wifi 
      u8g2.drawGlyph(108, 29, 175);	                          //Icone do Servidor Web 
      // Segunda Linha
      u8g2.drawGlyph(3, 50, 202);	                            //Icone do Senha 
      u8g2.drawGlyph(24, 50, 259);	                          //Icone do LED RBG
      u8g2.drawGlyph(45, 50, 90);	                            //Icone do Bateria
      u8g2.drawGlyph(66, 50, 122);	                          //Icone do Logs
      u8g2.drawGlyph(87, 50, 222);	                          //Icone do Telas
      //u8g2.drawGlyph(108, 50, nuemro do icone);	            //Icone vazio(basta descomentar)

      //item de navegação dos itens com base na posição do "ChangeMenu"
      int SelecBoxMenux[] = {1, 22, 43, 64, 85, 106, 1, 22, 43, 64, 85, 106};   //Posições do x do quadrado de seleção dos icones
      int SelecBoxMenuy[] = {11, 11, 11, 11, 11, 11, 32, 32, 32, 32, 32, 32};   //Posições do y do quadrado de seleção dos icones

      u8g2.setDrawColor(2);                                   //Permite a sobreposição de itens de forma "transparente"
      u8g2.drawBox(SelecBoxMenux[ChangeMenu],SelecBoxMenuy[ChangeMenu],20,20);  //Desenha um quadrado preenchido por cima dos icones utilizando as posições conforme o numero do ChangeMenu


      u8g2.drawRFrame(0,53,127,10,0);                         //Desenha um retangulo em volta do nome do item do menu selecionado
      u8g2.setFont(FontePadrao);                              //Retorna para a fonte padrão do projeto
      u8g2.drawStr(40,61,listaMenu[ChangeMenu]);              //Escreve na tela o nome da listaMenu que é posicionada atraves do ChangeMenu.

      /*************************
       *       BOTÕES
      **************************/
     // Ações dos botões dentro da tela do menu

     // ----BOTÂO ESQUERDO----
      if (bot_esquerdo == 1 && Single_loop == 0) {            //Verifica se o botão Esquerdo foi pressionado e se não passou dentro desse loop
        ChangeMenu--;                                         //Retrocede um item do menu (quadrado de icone e listaMenu)
        Single_loop = 1;                                      //Registra que já passou dentro desse loop ou de outros botões(evita varios pulsos de botões)
        if(ChangeMenu < 0) {                                  //Verifica se a variável está negativa
          ChangeMenu = ItensMenu - 1;                         //Muda para o ultimo item do menu
        }
      } // Fim - Botão Esquerdo

     // ----BOTÂO ACIMA----
      if (bot_acima == 1 && Single_loop == 0) {               //Verifica se o botão Acima foi pressionado e se não passou dentro desse loop
        ChangeMenu = ChangeMenu - 6;                          //Pula para o item do menu que está acima (quadrado de icone e listaMenu)
        Single_loop = 1;                                      //Registra que já passou dentro desse loop ou de outros botões(evita varios pulsos de botões)
        if(ChangeMenu < 0) {                                  //Verifica se a variável está negativa
          ChangeMenu = 0;                                     //Muda para o primeiro item do menu
        }
      } // Fim - Botão Acima
      
      // ----BOTÂO ABAIXO----
      if (bot_abaixo == 1 && Single_loop == 0) {              //Verifica se o botão Abaixo foi pressionado e se não passou dentro desse loop
        ChangeMenu = ChangeMenu + 6;                          //Pula para o item do menu que está abaixo (quadrado de icone e listaMenu)
        Single_loop = 1;                                      //Registra que já passou dentro desse loop ou de outros botões(evita varios pulsos de botões)
        if(ChangeMenu > ItensMenu - 1) {                      //Verifica se o item do menu está maior do que o numero de itens do menu
          ChangeMenu = ItensMenu - 1;                         //Muda para o ultimo item do menu
        }
      }// Fim - Botão Abaixo

      // ----BOTÂO DIREITO----
      if (bot_direito == 1 && Single_loop == 0) {             //Verifica se o botão Acima foi pressionado e se não passou dentro desse loop
        ChangeMenu++;                                         //Avança um item do menu (quadrado de icone e listaMenu)
        Single_loop = 1;                                      //Registra que já passou dentro desse loop ou de outros botões(evita varios pulsos de botões)
        if(ChangeMenu > ItensMenu - 1) {                      //Verifica se o item do menu está maior do que o numero de itens do menu
          ChangeMenu = 0;                                     //Muda para o primeiro item do menu
        }
      } // Fim - Botão Direito

      // ----BOTÂO SELECIONAR----
      if (bot_selecionar == 1 && Single_loop == 0) {          //Verifica se o botão Acima foi pressionado e se não passou dentro desse loop
        Tela = ChangeMenu;                                    //Muda a tela baseado no numero do item do menu
        Single_loop = 1;                                      //Registra que já passou dentro desse loop ou de outros botões(evita varios pulsos de botões)
      } // Fim - Botão Selecionar
    }
    while ( u8g2.nextPage() );                                //Este comando é parte do loop (imagem) que renderiza o conteúdo da exibição. (Obrigatório)
  }
}//--------------------------FIM - MENU PRINCIPAL ------------------------------------


/*====================================================================================
                           1 - TELA DE TESTE DE BOTÃO (Sub-Rotina)
====================================================================================*/
// Exibe as informações importantes para a calibração dos botões
void Tela1() {
    if (Tela == 1) {                                          //Caso a Tela seja 1
    u8g2.firstPage();                                         //Este comando é parte do loop (imagem) que renderiza o conteúdo da exibição. (Obrigatório)
    do {                                                      //Tudo o que estiver dentro do "do {}" será exibido na tela.
      u8g2.drawRFrame(0,0,127,10,2);                          //Desenha um retangulo em volta do titulo
      u8g2.drawStr(28,8,"Teste de Botao");                    //Escreve o titulo o topo da página (semelhante ao print porem não aceita UTF-8)
      u8g2.drawRFrame(13,16,104,16,2);                        //Desenha um retangulo em volta do nome do botão
      u8g2.setCursor(2, 40);                                  //Configura a posição do proximo item a ser escrito na tela
      u8g2.print("ADC:");                                     //Escreve na tela baseado na posição informada anteriormente..
      u8g2.setCursor(30, 40);                                 //Configura a posição do proximo item a ser escrito na tela
      u8g2.print(bot_lido_anterior);                          //Escreve na tela a variavel temporaria do botão.
      u8g2.setCursor(60, 40);                                 //Configura a posição do proximo item a ser escrito na tela
      u8g2.print("GPIO:");                                    //Escreve na tela baseado na posição informada anteriormente..
      u8g2.setCursor(92, 40);                                 //Configura a posição do proximo item a ser escrito na tela
      u8g2.print(Bot_LeituraADC);                             //Escreve na tela a variavel de leitura analógica em tempo real do botão.
      u8g2.setCursor(2, 50);                                  //Configura a posição do proximo item a ser escrito na tel
      u8g2.print("Pino:");                                    //Escreve na tela baseado na posição informada anteriormente..
      u8g2.setCursor(30, 50);                                 //Configura a posição do proximo item a ser escrito na tela
      u8g2.print(Bot_PinoADC);                                //Escreve na tela o numero do GPIO configurado do botão.
      
      
      /*************************
       *       BOTÕES
      **************************/
     // Ações dos botões dentro da tela de teste de botões

      if (bot_esquerdo == 1) {                                //Caso o botão Esquerdo for pressionado
      u8g2.setCursor(15, 29);                                 //Configura a posição do proximo item a ser escrito na tela
      u8g2.setFont(u8g2_font_unifont_t_latin);                //Configura a fonte para uma fonte diferente do padrão
      u8g2.print("ESQUERDO");                                 //Escreve na tela baseado na posição informada anteriormente.
      u8g2.setFont(FontePadrao);                              //Retorna para a fonte padrão do projeto
      }
      if (bot_acima == 1) {                                   //Caso o botão Acima for pressionado
      u8g2.setCursor(15, 29);                                 //Configura a posição do proximo item a ser escrito na tela
      u8g2.setFont(u8g2_font_unifont_t_latin);                //Configura a fonte para uma fonte diferente do padrão
      u8g2.print("ACIMA");                                    //Escreve na tela baseado na posição informada anteriormente.
      u8g2.setFont(FontePadrao);  
      }
      if (bot_abaixo == 1) {                                  //Caso o botão Abaixo for pressionado
      u8g2.setCursor(15, 29);                                 //Configura a posição do proximo item a ser escrito na tela
      u8g2.setFont(u8g2_font_unifont_t_latin);                //Configura a fonte para uma fonte diferente do padrão
      u8g2.print("ABAIXO");                                   //Escreve na tela baseado na posição informada anteriormente.
      u8g2.setFont(FontePadrao);                              //Retorna para a fonte padrão do projeto
      }
      if (bot_direito == 1) {                                 //Caso o botão Direito for pressionado
      u8g2.setCursor(15, 29);                                 //Configura a posição do proximo item a ser escrito na tela
      u8g2.setFont(u8g2_font_unifont_t_latin);                //Configura a fonte para uma fonte diferente do padrão
      u8g2.print("DIREITO");                                  //Escreve na tela baseado na posição informada anteriormente.
      u8g2.setFont(FontePadrao);                              //Retorna para a fonte padrão do projeto
      }
      if (bot_selecionar == 1) {                              //Caso o botão Selecionar for pressionado
      u8g2.setCursor(15, 29);                                 //Configura a posição do proximo item a ser escrito na tela
      u8g2.setFont(u8g2_font_unifont_t_latin);                //Configura a fonte para uma fonte diferente do padrão
      u8g2.print("SELECIONAR");                               //Escreve na tela baseado na posição informada anteriormente.
      u8g2.setFont(FontePadrao);                              //Retorna para a fonte padrão do projeto
      }
      if (bot_voltar == 1) {                                  //Caso o botão Voltar for pressionado
      u8g2.setCursor(15, 29);                                 //Configura a posição do proximo item a ser escrito na tela
      u8g2.setFont(u8g2_font_unifont_t_latin);                //Configura a fonte para uma fonte diferente do padrão
      u8g2.print("VOLTAR");                                   //Escreve na tela baseado na posição informada anteriormente.
      u8g2.setFont(FontePadrao);                              //Retorna para a fonte padrão do projeto
      }
      if (bot_voltar == 1 && Single_loop == 0) {              //Caso o botão Voltar for pressionado
        Tela = -1;                                            //Configura para o ID do Menu Principal
        Single_loop = 1;                                      //Registra que já passou dentro desse loop ou de outros botões(evita varios pulsos de botões)     
      }
    }
    while ( u8g2.nextPage() );                                //Este comando é parte do loop (imagem) que renderiza o conteúdo da exibição. (Obrigatório)
  }
}//--------------------------FIM - 1 - TELA DE TESTE DE BOTÃO ------------------------------------

/*====================================================================================
                            SETUP (Configurações)
====================================================================================*/


void setup() {
    u8g2.begin();                                             //Inicia a Biblioteca do Display
    u8g2.setFont(FontePadrao);                                //Configura o tipo de Fonte u8g2_font_6x12_tr
    pinMode(23, OUTPUT);                                      //Configura o pino do LED de luz de fundo do LCD (opcional)
    digitalWrite(23, HIGH);                                   //Liga o pino do LED de luz de fundo do LCD (opcional)
}//--------------------------FIM - SETUP ------------------------------------

/*====================================================================================
                            LOOP (Programa)
====================================================================================*/

void loop() {
  Botoes_Navegacao(); //Chama a subrotina dos botões do teclado
  Tela1();            //Chama a subrotina da tela de teste de botão para estar pronta caso a variável Tela seja 1
  Menu_Principal();   //Chama a subrotina da tela de menu Principal para estar pronta caso a variável Tela seja -1
}//--------------------------FIM - LOOP ------------------------------------
