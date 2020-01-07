/*
Programa de seleção de Menu com telas para ESP32, desenvolvido por JailsonBR utilizando a biblioteca u8g2 do olikraus.
   
  PORFAVOR NÃO REMOVA NENHUMA INFORMAÇÃO DESTE CABEÇARIO, ESTE É UM CÓDIGO OPENSOURCE SOB LICENÇA DO CRIADOR
   
  Código adaptado por JailsonBR do site XProjetos.net em 03-12-2019, o uso deste código é livre porem deve respeitar e mante o nome do criador
  e tambêm de quem modificou, o código está sob GNU GENERAL PUBLIC LICENSE Versão 3 (https://www.gnu.org/licenses/quick-guide-gplv3.pt-br.html)

  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

  Copyright(c) sobre o XPsys 2019, XProjetos.net

  XPsys Versão 1.0.2 (Alpha)

Menu selection program with screens for ESP32, developed by JailsonBR using olikraus u8g2 library.
   
   PLEASE DO NOT REMOVE ANY INFORMATION FROM THIS HEADER, THIS IS AN OPENSOURCE CODE UNDER THE CREATOR LICENSE
   
   Code adapted by JailsonBR from XProjeto.net site on 12-03-2019, the use of this code is free but must respect and keep the name of the creator
   and also from those who modified it, the code is under GNU GENERAL PUBLIC LICENSE Version 3 (https://www.gnu.org/licenses/quick-guide-gplv3.en-ml.html)

   Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

   Copyright (c) about XPsys 2019, XProject.net

   XPsys Version 1.0.2 (Alpha)
  
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
#define Bot_PinoADC 34                                        //O compilador substituirá qualquer menção de Bot_PinoADC ao valor 34 no momento da compilação.
#define FontePadrao u8g2_font_5x8_tf                          //Selecione a fonte padrão de todos os menus exceto os ícones
#define T_Teclado -2                                          //Indica que "T_Teclado" será substituído por -1 (Usado no id da tela do teclado)
#define XP_Versao "XP:1.0.2"                                  //Versão do XPsys que é exibido no menu principal
U8G2_ST7920_128X64_1_SW_SPI u8g2(U8G2_R0, 33, 32, 35, 27);    //Enable, RW, RS, RESET  

//int Configura as variaveis de -32,768 a 32,767, ocupa 2 bytes (16 bits)
int Bot_LeituraADC = 0;                                       //Variavel para Leitura da porta analogica
int bot_lido_anterior = 0;                                    //Valor temporario da porta analogica
int ChangeMenu = 0;                                           //Registra a navegação no menu principal
int Tela = -1;                                                //Varíaveis para as telas individuais (-1 é o menu principal
int TecladoBox = 0;                                           //Variável utilizada para navegação do teclado (ID das teclas)
int TecladoCAPS = 0;                                          //Variavel que distingue minuscula para maíuscula e tambem do teclado travado.
int XP_NavCaixas = 0;                                         //Utilizado para navegação de caixas de texto em uma tela qualquer
int UltimaTela = -1;                                          //Utilizado para retorno do teclado para a tela anterior
int ID_CaixaSelecao = 0;                                      //Utilizado para qual caixa de texto(nas Strings XP_Caixa_Tela...) o texto do teclado será colocado 

//bool variável de 0 ou 1, ocupa 1 byte (256 bytes)
bool bot_esquerdo = 0;                                        //Variavel de acionamento do botão (1 = ativado)
bool bot_acima = 0;                                           //Variavel de acionamento do botão (1 = ativado)
bool bot_abaixo = 0;                                          //Variavel de acionamento do botão (1 = ativado)
bool bot_direito = 0;                                         //Variavel de acionamento do botão (1 = ativado)
bool bot_selecionar = 0;                                      //Variavel de acionamento do botão (1 = ativado)
bool bot_voltar = 0;                                          //Variavel de acionamento do botão (1 = ativado)
bool bot_loop = 1;                                            //Variavel para o loop do botão evita que passe mais que uma vez no loop
bool Single_loop = 1;                                         //Variavel para o loop do menu de tela evita que passe mais que uma vez no loop acionado dievrsas vezes o botão;
bool ModoDesenv = 0;                                          //Habilita algumas funções que me auxiliaram na programação.

//unsigned long variável de 0 a 4,294,967,295, ocupa 4 bytes (32 bits)
unsigned long tempo_bot_leitura_ultimo = 0;                   //ultimo tempo que o botão foi pressionado (millis())
unsigned long bot_debounce_tempo = 70;                        //tempo do debounce dos botões em ms
unsigned long diferenca_tempo = 0;                            //diferença dos tempos de acionamento com o millis()

String bufferTeclado;                                         //Usado para o texto digitado pelo teclado virtual

String XP_Caixa_Tela_02_00;                                   //Usado no Formulario teste para teste do teclado
String XP_Caixa_Tela_02_01;                                   //Usado no Formulario teste para teste do teclado
String XP_Caixa_Tela_02_02;                                   //Usado no Formulario teste para teste do teclado

byte ItensMenu = 11;                                          //Coloque o numero de itens do menu principal

char *listaMenu [] = {                                        //Lista de Itens do Menu, sempre começando do item 0
"Relogio",
"Botoes",
"Teste Teclado",
"Configuracoes",
"Wifi",
"Servidor Web",
"Senha",
"LED RBG",
"Bateria",
"Logs",
"Telas"
};

// Variáveis para utilização de acentuação de letras do teclado
int tecladoAcentos [] =        {28,30,44,61,68};              //´ ou `, ' ou ",~,^,¨
int teclaAceitaAcentos [] =    {34,20,25,26,24,71,72};        //a,e,i,o,u,' ',' '

char acento_grave_m [] =       {224,232,236,242,249,96,96};   //'à','è','ì','ò','ù','`','`'
char acento_agudo_m [] =       {225,233,237,243,250,180,180}; //'á','é','í','ó','ú','´','´'
char acento_circunflexo_m [] = {226,234,238,244,251,94,94};   //'â','ê','î','ô','û','^','^'
char acento_til_m [] =         {227,126,126,245,126,126,126}; //'ã','~','~','õ','~','~','~'
char acento_trema_m [] =       {228,235,239,246,252,168,168}; //'ä','ë','ï','ö','ü','¨','¨'

char acento_grave_M [] =       {192,200,204,210,215,96,96};   //'À','È','Ì','Ò','Ù','`','`'
char acento_agudo_M [] =       {193,201,205,211,216,180,180}; //'Á','É','Í','Ó','Ú','´','´'
char acento_circunflexo_M [] = {194,202,206,212,217,94,94};   //'Â','Ê','Î','Ô','Û','^','^'
char acento_til_M [] =         {195,126,126,213,218,126,126}; //'Ã','~','~','Õ','~','~','~'
char acento_trema_M [] =       {196,203,207,214,219,168,168}; //'Ä','Ë','Ï','Ö','Ü','¨','¨'

char tempAcento;

// Matriz com as teclas do teclado tipo minusculo
char teclado_minusculo[5][16] = { 
{'0','1','2','3','4','5','6','7','8','9','*','/','-','+','=',167},
{'@','#','q','w','e','r','t','y','u','i','o','p','\0'/*´*/,123/*{*/,34/*"*/,125/*}*/},
{'$','%','a','s','d','f','g','h','j','k','l',231/*ç*/,'\0'/*~*/,91/*[*/,'_',93/*]*/},
{'\0','\0','z','x','c','v','b','n','m','<','>',';',':','\0'/*^*/,'\0'/*apagar*/,'\0'/*apagar*/},
{'\0','\0',92,124,'\0','&',40/*(*/,' '/*ESPAÇO*/,' '/*ESPAÇO*/,41/*)*/,'.',',','!','?','\0'/*OK*/,'\0'/*OK*/}
};
// Matriz com as teclas do teclado tipo Maiusculo
char teclado_Maiusculo[5][16] = { 
{'0','1','2','3','4','5','6','7','8','9','*','/','-','+','=',167},
{'@','#','Q','W','E','R','T','Y','U','I','O','P','\0'/*´*/,123/*{*/,34/*"*/,125/*}*/},
{'$','%','A','S','D','F','G','H','J','K','L',231/*Ç*/,'\0'/*~*/,91/*[*/,'_',93/*]*/},
{'\0','\0','Z','X','C','V','B','N','M','<','>',';',':','\0'/*^*/,'\0'/*APAGAR*/,'\0'/*APAGAR*/},
{'\0','\0',92,124,'\0','&',40,' ',' ',41,'.',',','!','?','\0','\0'}
};

//Imagem com as letras, simbolos e numeros gerada em XBM para utilização no teclado.
static unsigned char Teclado_Minusculo_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x02, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40,
   0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40,
   0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40,
   0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0xfe, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c,
   0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x3c, 0x82, 0x82, 0x82, 0x82,
   0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x4a,
   0x39, 0x11, 0x39, 0x3d, 0x21, 0x39, 0x71, 0x7d, 0x39, 0x19, 0x11, 0x21,
   0x01, 0x01, 0x01, 0x95, 0x45, 0x11, 0x25, 0x25, 0x31, 0x09, 0x19, 0x25,
   0x65, 0x25, 0x55, 0x21, 0x01, 0x11, 0x01, 0x85, 0x45, 0x19, 0x21, 0x21,
   0x31, 0x09, 0x0d, 0x25, 0x4d, 0x25, 0x39, 0x11, 0x01, 0x11, 0x7d, 0x8d,
   0x45, 0x11, 0x21, 0x39, 0x29, 0x31, 0x3d, 0x31, 0x39, 0x39, 0x11, 0x11,
   0x7d, 0x7d, 0x01, 0x95, 0x45, 0x11, 0x11, 0x21, 0x7d, 0x21, 0x4d, 0x11,
   0x65, 0x21, 0x29, 0x11, 0x01, 0x11, 0x7d, 0x99, 0x45, 0x11, 0x49, 0x25,
   0x21, 0x25, 0x4d, 0x19, 0x45, 0x25, 0x45, 0x09, 0x01, 0x11, 0x01, 0x91,
   0x39, 0x39, 0x7d, 0x19, 0x71, 0x39, 0x39, 0x09, 0x39, 0x19, 0x01, 0x09,
   0x01, 0x01, 0x01, 0x95, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
   0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x4a, 0x7c, 0x7c, 0x7c, 0x7c,
   0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x3c,
   0xba, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
   0x82, 0x82, 0x82, 0x42, 0x45, 0x29, 0x01, 0x01, 0x01, 0x01, 0x09, 0x01,
   0x01, 0x11, 0x01, 0x01, 0x11, 0x39, 0x29, 0x9d, 0x9d, 0x29, 0x39, 0x93,
   0x19, 0x39, 0x09, 0x29, 0x25, 0x01, 0x39, 0x39, 0x09, 0x09, 0x29, 0x91,
   0xa3, 0x7d, 0x25, 0x55, 0x25, 0x09, 0x19, 0x29, 0x25, 0x11, 0x45, 0x49,
   0x01, 0x09, 0x29, 0x91, 0xb3, 0x29, 0x25, 0x55, 0x3d, 0x09, 0x09, 0x29,
   0x25, 0x11, 0x45, 0x49, 0x01, 0x05, 0x01, 0xa1, 0xab, 0x7d, 0x39, 0x29,
   0x05, 0x09, 0x09, 0x11, 0x25, 0x11, 0x45, 0x39, 0x01, 0x09, 0x01, 0x91,
   0xb5, 0x29, 0x21, 0x29, 0x39, 0x09, 0x09, 0x11, 0x39, 0x11, 0x39, 0x09,
   0x01, 0x09, 0x01, 0x91, 0x49, 0x29, 0x21, 0x01, 0x01, 0x01, 0x19, 0x0d,
   0x01, 0x11, 0x01, 0x09, 0x01, 0x39, 0x01, 0x9d, 0xb2, 0x82, 0x82, 0x82,
   0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x42,
   0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c,
   0x7c, 0x7c, 0x7c, 0x3c, 0x92, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
   0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x42, 0x39, 0x81, 0x01, 0x01,
   0x21, 0x19, 0x01, 0x05, 0x11, 0x05, 0x01, 0x19, 0x01, 0x39, 0x01, 0x9d,
   0x15, 0x4f, 0x1d, 0x39, 0x21, 0x09, 0x39, 0x05, 0x01, 0x05, 0x09, 0x25,
   0x01, 0x09, 0x01, 0x91, 0x15, 0x2b, 0x21, 0x05, 0x39, 0x19, 0x25, 0x1d,
   0x11, 0x25, 0x09, 0x05, 0x49, 0x09, 0x01, 0x91, 0x39, 0x1f, 0x39, 0x19,
   0x25, 0x09, 0x25, 0x25, 0x11, 0x15, 0x09, 0x25, 0x55, 0x09, 0x01, 0x91,
   0x51, 0xf1, 0x25, 0x21, 0x25, 0x09, 0x39, 0x25, 0x11, 0x1d, 0x09, 0x19,
   0x25, 0x09, 0x01, 0x91, 0x55, 0xa9, 0x39, 0x1d, 0x39, 0x09, 0x25, 0x25,
   0x15, 0x25, 0x19, 0x11, 0x01, 0x09, 0x7d, 0x91, 0x39, 0xe5, 0x01, 0x01,
   0x01, 0x09, 0x19, 0x25, 0x1d, 0x25, 0x01, 0x19, 0x01, 0x39, 0x01, 0x9d,
   0x92, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
   0x82, 0x82, 0x82, 0x42, 0xfc, 0x7f, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c,
   0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0xfc, 0x3f, 0x02, 0x80, 0x82, 0x82,
   0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x02, 0x40,
   0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x09, 0x01, 0x01, 0x01, 0x01, 0x01,
   0x01, 0x01, 0xc1, 0x80, 0x81, 0x02, 0x3d, 0x25, 0x19, 0x25, 0x09, 0x1d,
   0x29, 0x21, 0x05, 0x11, 0x11, 0x11, 0xa1, 0x80, 0x41, 0x04, 0x11, 0x19,
   0x25, 0x25, 0x39, 0x25, 0x55, 0x19, 0x19, 0x01, 0x01, 0x29, 0x91, 0x87,
   0x21, 0x08, 0x09, 0x19, 0x05, 0x25, 0x49, 0x25, 0x55, 0x05, 0x21, 0x01,
   0x01, 0x45, 0x09, 0x84, 0xe1, 0x0e, 0x05, 0x25, 0x25, 0x19, 0x49, 0x25,
   0x55, 0x19, 0x19, 0x11, 0x11, 0x01, 0x91, 0x87, 0x81, 0x02, 0x3d, 0x01,
   0x19, 0x19, 0x39, 0x25, 0x55, 0x21, 0x05, 0x19, 0x01, 0x01, 0xa1, 0x80,
   0x81, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
   0x01, 0x01, 0xc1, 0x80, 0x02, 0x80, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
   0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x02, 0x40, 0xfc, 0x7f, 0x7c, 0x7c,
   0x7c, 0x7c, 0x7c, 0xfc, 0x7f, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0xfc, 0x3f,
   0x02, 0x80, 0x82, 0x82, 0x82, 0x82, 0x82, 0x02, 0x80, 0x82, 0x82, 0x82,
   0x82, 0x82, 0x02, 0x40, 0x21, 0x38, 0x09, 0x11, 0x01, 0x31, 0x21, 0x01,
   0x00, 0x09, 0x01, 0x01, 0x11, 0x39, 0x01, 0x84, 0x51, 0x44, 0x09, 0x11,
   0x6d, 0x29, 0x11, 0x01, 0x00, 0x11, 0x01, 0x01, 0x11, 0x45, 0x01, 0x84,
   0x89, 0x44, 0x11, 0x11, 0x6d, 0x29, 0x09, 0x01, 0x00, 0x21, 0x01, 0x01,
   0x11, 0x41, 0x21, 0x84, 0x05, 0x7d, 0x11, 0x01, 0x01, 0x19, 0x09, 0x01,
   0x00, 0x21, 0x01, 0x31, 0x11, 0x21, 0x31, 0x84, 0xdd, 0x45, 0x11, 0x11,
   0x01, 0x65, 0x09, 0x01, 0x00, 0x21, 0x01, 0x31, 0x11, 0x11, 0xf9, 0x87,
   0x51, 0x44, 0x21, 0x11, 0x01, 0x25, 0x11, 0x01, 0x00, 0x11, 0x11, 0x11,
   0x01, 0x01, 0x31, 0x80, 0x71, 0x7c, 0x21, 0x11, 0x01, 0x59, 0x21, 0x01,
   0x00, 0x09, 0x01, 0x09, 0x11, 0x11, 0x21, 0x80, 0x02, 0x80, 0x82, 0x82,
   0x82, 0x82, 0x82, 0x02, 0x80, 0x82, 0x82, 0x82, 0x82, 0x82, 0x02, 0x40,
   0xfc, 0x7f, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0xfc, 0x7f, 0x7c, 0x7c, 0x7c,
   0x7c, 0x7c, 0xfc, 0x3f };

static unsigned char Teclado_Maiusculas_bits[] = { //Imagem com as letras, simbolos e numeros gerada em XBM para utilização no teclado.
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x02, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40,
   0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40,
   0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40,
   0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0xfe, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c,
   0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x3c, 0x82, 0x82, 0x82, 0x82,
   0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x4a,
   0x39, 0x11, 0x39, 0x3d, 0x21, 0x39, 0x71, 0x7d, 0x39, 0x19, 0x11, 0x21,
   0x01, 0x01, 0x01, 0x95, 0x45, 0x11, 0x25, 0x25, 0x31, 0x09, 0x19, 0x25,
   0x65, 0x25, 0x55, 0x21, 0x01, 0x11, 0x01, 0x85, 0x45, 0x19, 0x21, 0x21,
   0x31, 0x09, 0x0d, 0x25, 0x4d, 0x25, 0x39, 0x11, 0x01, 0x11, 0x7d, 0x8d,
   0x45, 0x11, 0x21, 0x39, 0x29, 0x31, 0x3d, 0x31, 0x39, 0x39, 0x11, 0x11,
   0x7d, 0x7d, 0x01, 0x95, 0x45, 0x11, 0x11, 0x21, 0x7d, 0x21, 0x4d, 0x11,
   0x65, 0x21, 0x29, 0x11, 0x01, 0x11, 0x7d, 0x99, 0x45, 0x11, 0x49, 0x25,
   0x21, 0x25, 0x4d, 0x19, 0x45, 0x25, 0x45, 0x09, 0x01, 0x11, 0x01, 0x91,
   0x39, 0x39, 0x7d, 0x19, 0x71, 0x39, 0x39, 0x09, 0x39, 0x19, 0x01, 0x09,
   0x01, 0x01, 0x01, 0x95, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
   0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x4a, 0x7c, 0x7c, 0x7c, 0x7c,
   0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x3c,
   0xba, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
   0x82, 0x82, 0x82, 0x42, 0x45, 0x29, 0x39, 0x01, 0x39, 0x3d, 0x01, 0x01,
   0x01, 0x39, 0x39, 0x39, 0x09, 0x39, 0x11, 0x9d, 0x9d, 0x29, 0x45, 0x93,
   0x09, 0x45, 0x7d, 0x45, 0x45, 0x11, 0x45, 0x49, 0x11, 0x09, 0x11, 0x91,
   0xa3, 0x7d, 0x45, 0xab, 0x09, 0x45, 0x11, 0x45, 0x45, 0x11, 0x45, 0x49,
   0x01, 0x09, 0x11, 0x91, 0xb3, 0x29, 0x45, 0xab, 0x39, 0x3d, 0x11, 0x29,
   0x45, 0x11, 0x45, 0x39, 0x01, 0x05, 0x01, 0xa1, 0xab, 0x7d, 0x55, 0xab,
   0x09, 0x25, 0x11, 0x11, 0x45, 0x11, 0x45, 0x09, 0x01, 0x09, 0x01, 0x91,
   0xb5, 0x29, 0x65, 0x45, 0x09, 0x25, 0x11, 0x11, 0x45, 0x11, 0x45, 0x09,
   0x01, 0x09, 0x01, 0x91, 0x49, 0x29, 0x39, 0x45, 0x39, 0x45, 0x11, 0x11,
   0x39, 0x39, 0x39, 0x09, 0x01, 0x39, 0x01, 0x9d, 0xb2, 0x82, 0xc2, 0x82,
   0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x42,
   0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c,
   0x7c, 0x7c, 0x7c, 0x3c, 0x92, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
   0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x42, 0x39, 0x81, 0x11, 0x19,
   0x3d, 0x01, 0x39, 0x45, 0x7d, 0x25, 0x09, 0x19, 0x01, 0x39, 0x01, 0x9d,
   0x15, 0x4f, 0x29, 0x25, 0x45, 0x79, 0x45, 0x45, 0x11, 0x25, 0x09, 0x25,
   0x01, 0x09, 0x01, 0x91, 0x15, 0x2b, 0x29, 0x05, 0x45, 0x09, 0x05, 0x45,
   0x11, 0x15, 0x09, 0x05, 0x49, 0x09, 0x01, 0x91, 0x39, 0x1f, 0x29, 0x19,
   0x45, 0x79, 0x65, 0x7d, 0x11, 0x0d, 0x09, 0x05, 0x55, 0x09, 0x01, 0x91,
   0x51, 0xf1, 0x39, 0x21, 0x45, 0x09, 0x45, 0x45, 0x15, 0x0d, 0x09, 0x25,
   0x25, 0x09, 0x01, 0x91, 0x55, 0xa9, 0x45, 0x25, 0x45, 0x09, 0x45, 0x45,
   0x15, 0x15, 0x09, 0x19, 0x01, 0x09, 0x7d, 0x91, 0x39, 0xe5, 0x45, 0x19,
   0x3d, 0x09, 0x39, 0x45, 0x1d, 0x25, 0x39, 0x09, 0x01, 0x39, 0x01, 0x9d,
   0x92, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x8a,
   0x82, 0x82, 0x82, 0x42, 0xfc, 0x7f, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c,
   0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0xfc, 0x3f, 0x02, 0x80, 0x82, 0x82,
   0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x02, 0x40,
   0x01, 0x01, 0x7d, 0x01, 0x39, 0x45, 0x3d, 0x45, 0x45, 0x01, 0x01, 0x01,
   0x01, 0x01, 0xc1, 0x80, 0x81, 0x03, 0x21, 0x45, 0x45, 0x45, 0x45, 0x4d,
   0x45, 0x21, 0x05, 0x11, 0x11, 0x11, 0xa1, 0x80, 0xc1, 0x07, 0x11, 0x29,
   0x05, 0x45, 0x45, 0x4d, 0x6d, 0x19, 0x19, 0x01, 0x01, 0x29, 0x91, 0x87,
   0xe1, 0x0f, 0x11, 0x11, 0x05, 0x45, 0x3d, 0x55, 0x6d, 0x05, 0x21, 0x01,
   0x01, 0x45, 0x09, 0x84, 0xe1, 0x0f, 0x09, 0x29, 0x05, 0x29, 0x45, 0x65,
   0x55, 0x19, 0x19, 0x11, 0x11, 0x01, 0x91, 0x87, 0x81, 0x03, 0x05, 0x45,
   0x45, 0x29, 0x45, 0x65, 0x55, 0x21, 0x05, 0x19, 0x01, 0x01, 0xa1, 0x80,
   0x81, 0x03, 0x7d, 0x01, 0x39, 0x11, 0x3d, 0x45, 0x55, 0x01, 0x01, 0x01,
   0x01, 0x01, 0xc1, 0x80, 0x02, 0x80, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
   0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x02, 0x40, 0xfc, 0x7f, 0x7c, 0x7c,
   0x7c, 0x7c, 0x7c, 0xfc, 0x7f, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0xfc, 0x3f,
   0x02, 0x80, 0x82, 0x82, 0x82, 0x82, 0x82, 0x02, 0x80, 0x82, 0x82, 0x82,
   0x82, 0x82, 0x02, 0x40, 0x21, 0x38, 0x09, 0x11, 0x01, 0x31, 0x21, 0x01,
   0x00, 0x09, 0x01, 0x01, 0x11, 0x39, 0x01, 0x84, 0x51, 0x40, 0x09, 0x11,
   0x6d, 0x29, 0x11, 0x01, 0x00, 0x11, 0x01, 0x01, 0x11, 0x45, 0x01, 0x84,
   0x89, 0x40, 0x11, 0x11, 0x6d, 0x29, 0x09, 0x01, 0x00, 0x21, 0x01, 0x01,
   0x11, 0x41, 0x21, 0x84, 0x05, 0x7d, 0x11, 0x01, 0x01, 0x19, 0x09, 0x01,
   0x00, 0x21, 0x01, 0x31, 0x11, 0x21, 0x31, 0x84, 0xdd, 0x45, 0x11, 0x11,
   0x01, 0x65, 0x09, 0x01, 0x00, 0x21, 0x01, 0x31, 0x11, 0x11, 0xf9, 0x87,
   0x51, 0x44, 0x21, 0x11, 0x01, 0x25, 0x11, 0x01, 0x00, 0x11, 0x11, 0x11,
   0x01, 0x01, 0x31, 0x80, 0x71, 0x7c, 0x21, 0x11, 0x01, 0x59, 0x21, 0x01,
   0x00, 0x09, 0x01, 0x09, 0x11, 0x11, 0x21, 0x80, 0x02, 0x80, 0x82, 0x82,
   0x82, 0x82, 0x82, 0x02, 0x80, 0x82, 0x82, 0x82, 0x82, 0x82, 0x02, 0x40,
   0xfc, 0x7f, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0xfc, 0x7f, 0x7c, 0x7c, 0x7c,
   0x7c, 0x7c, 0xfc, 0x3f };

   static unsigned char Teclado_Maiusculo_CAPS_bits[] = { //Imagem com as letras, simbolos e numeros gerada em XBM para utilização no teclado.
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x02, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40,
   0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40,
   0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40,
   0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0xfe, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c,
   0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x3c, 0x82, 0x82, 0x82, 0x82,
   0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x4a,
   0x39, 0x11, 0x39, 0x3d, 0x21, 0x39, 0x71, 0x7d, 0x39, 0x19, 0x11, 0x21,
   0x01, 0x01, 0x01, 0x95, 0x45, 0x11, 0x25, 0x25, 0x31, 0x09, 0x19, 0x25,
   0x65, 0x25, 0x55, 0x21, 0x01, 0x11, 0x01, 0x85, 0x45, 0x19, 0x21, 0x21,
   0x31, 0x09, 0x0d, 0x25, 0x4d, 0x25, 0x39, 0x11, 0x01, 0x11, 0x7d, 0x8d,
   0x45, 0x11, 0x21, 0x39, 0x29, 0x31, 0x3d, 0x31, 0x39, 0x39, 0x11, 0x11,
   0x7d, 0x7d, 0x01, 0x95, 0x45, 0x11, 0x11, 0x21, 0x7d, 0x21, 0x4d, 0x11,
   0x65, 0x21, 0x29, 0x11, 0x01, 0x11, 0x7d, 0x99, 0x45, 0x11, 0x49, 0x25,
   0x21, 0x25, 0x4d, 0x19, 0x45, 0x25, 0x45, 0x09, 0x01, 0x11, 0x01, 0x91,
   0x39, 0x39, 0x7d, 0x19, 0x71, 0x39, 0x39, 0x09, 0x39, 0x19, 0x01, 0x09,
   0x01, 0x01, 0x01, 0x95, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
   0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x4a, 0x7c, 0x7c, 0x7c, 0x7c,
   0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x3c,
   0xba, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
   0x82, 0x82, 0x82, 0x42, 0x45, 0x29, 0x39, 0x01, 0x39, 0x3d, 0x01, 0x01,
   0x01, 0x39, 0x39, 0x39, 0x09, 0x39, 0x11, 0x9d, 0x9d, 0x29, 0x45, 0x93,
   0x09, 0x45, 0x7d, 0x45, 0x45, 0x11, 0x45, 0x49, 0x11, 0x09, 0x11, 0x91,
   0xa3, 0x7d, 0x45, 0xab, 0x09, 0x45, 0x11, 0x45, 0x45, 0x11, 0x45, 0x49,
   0x01, 0x09, 0x11, 0x91, 0xb3, 0x29, 0x45, 0xab, 0x39, 0x3d, 0x11, 0x29,
   0x45, 0x11, 0x45, 0x39, 0x01, 0x05, 0x01, 0xa1, 0xab, 0x7d, 0x55, 0xab,
   0x09, 0x25, 0x11, 0x11, 0x45, 0x11, 0x45, 0x09, 0x01, 0x09, 0x01, 0x91,
   0xb5, 0x29, 0x65, 0x45, 0x09, 0x25, 0x11, 0x11, 0x45, 0x11, 0x45, 0x09,
   0x01, 0x09, 0x01, 0x91, 0x49, 0x29, 0x39, 0x45, 0x39, 0x45, 0x11, 0x11,
   0x39, 0x39, 0x39, 0x09, 0x01, 0x39, 0x01, 0x9d, 0xb2, 0x82, 0xc2, 0x82,
   0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x42,
   0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c,
   0x7c, 0x7c, 0x7c, 0x3c, 0x92, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
   0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x42, 0x39, 0x81, 0x11, 0x19,
   0x3d, 0x01, 0x39, 0x45, 0x7d, 0x25, 0x09, 0x19, 0x01, 0x39, 0x01, 0x9d,
   0x15, 0x4f, 0x29, 0x25, 0x45, 0x79, 0x45, 0x45, 0x11, 0x25, 0x09, 0x25,
   0x01, 0x09, 0x01, 0x91, 0x15, 0x2b, 0x29, 0x05, 0x45, 0x09, 0x05, 0x45,
   0x11, 0x15, 0x09, 0x05, 0x49, 0x09, 0x01, 0x91, 0x39, 0x1f, 0x29, 0x19,
   0x45, 0x79, 0x65, 0x7d, 0x11, 0x0d, 0x09, 0x05, 0x55, 0x09, 0x01, 0x91,
   0x51, 0xf1, 0x39, 0x21, 0x45, 0x09, 0x45, 0x45, 0x15, 0x0d, 0x09, 0x25,
   0x25, 0x09, 0x01, 0x91, 0x55, 0xa9, 0x45, 0x25, 0x45, 0x09, 0x45, 0x45,
   0x15, 0x15, 0x09, 0x19, 0x01, 0x09, 0x7d, 0x91, 0x39, 0xe5, 0x45, 0x19,
   0x3d, 0x09, 0x39, 0x45, 0x1d, 0x25, 0x39, 0x09, 0x01, 0x39, 0x01, 0x9d,
   0x92, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x8a,
   0x82, 0x82, 0x82, 0x42, 0xfc, 0x7f, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c,
   0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0xfc, 0x3f, 0x02, 0x80, 0x82, 0x82,
   0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x02, 0x40,
   0x01, 0x01, 0x7d, 0x01, 0x39, 0x45, 0x3d, 0x45, 0x45, 0x01, 0x01, 0x01,
   0x01, 0x01, 0xc1, 0x80, 0x81, 0x03, 0x21, 0x45, 0x45, 0x45, 0x45, 0x4d,
   0x45, 0x21, 0x05, 0x11, 0x11, 0x11, 0xa1, 0x80, 0xc1, 0x07, 0x11, 0x29,
   0x05, 0x45, 0x45, 0x4d, 0x6d, 0x19, 0x19, 0x01, 0x01, 0x29, 0x91, 0x87,
   0xe1, 0x0f, 0x11, 0x11, 0x05, 0x45, 0x3d, 0x55, 0x6d, 0x05, 0x21, 0x01,
   0x01, 0x45, 0x09, 0x84, 0xe1, 0x0f, 0x09, 0x29, 0x05, 0x29, 0x45, 0x65,
   0x55, 0x19, 0x19, 0x11, 0x11, 0x01, 0x91, 0x87, 0x81, 0x03, 0x05, 0x45,
   0x45, 0x29, 0x45, 0x65, 0x55, 0x21, 0x05, 0x19, 0x01, 0x01, 0xa1, 0x80,
   0x81, 0x03, 0x7d, 0x01, 0x39, 0x11, 0x3d, 0x45, 0x55, 0x01, 0x01, 0x01,
   0x01, 0x01, 0xc1, 0x80, 0x02, 0x80, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
   0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x02, 0x40, 0xfc, 0x7f, 0x7c, 0x7c,
   0x7c, 0x7c, 0x7c, 0xfc, 0x7f, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0xfc, 0x3f,
   0x02, 0x80, 0x82, 0x82, 0x82, 0x82, 0x82, 0x02, 0x80, 0x82, 0x82, 0x82,
   0x82, 0x82, 0x02, 0x40, 0x21, 0x38, 0x09, 0x11, 0x01, 0x31, 0x21, 0x01,
   0x00, 0x09, 0x01, 0x01, 0x11, 0x39, 0x01, 0x84, 0x71, 0x44, 0x09, 0x11,
   0x6d, 0x29, 0x11, 0x01, 0x00, 0x11, 0x01, 0x01, 0x11, 0x45, 0x01, 0x84,
   0xf9, 0x44, 0x11, 0x11, 0x6d, 0x29, 0x09, 0x01, 0x00, 0x21, 0x01, 0x01,
   0x11, 0x41, 0x21, 0x84, 0xfd, 0x7d, 0x11, 0x01, 0x01, 0x19, 0x09, 0x01,
   0x00, 0x21, 0x01, 0x31, 0x11, 0x21, 0x31, 0x84, 0xfd, 0x45, 0x11, 0x11,
   0x01, 0x65, 0x09, 0x01, 0x00, 0x21, 0x01, 0x31, 0x11, 0x11, 0xf9, 0x87,
   0x71, 0x44, 0x21, 0x11, 0x01, 0x25, 0x11, 0x01, 0x00, 0x11, 0x11, 0x11,
   0x01, 0x01, 0x31, 0x80, 0x71, 0x7c, 0x21, 0x11, 0x01, 0x59, 0x21, 0x01,
   0x00, 0x09, 0x01, 0x09, 0x11, 0x11, 0x21, 0x80, 0x02, 0x80, 0x82, 0x82,
   0x82, 0x82, 0x82, 0x02, 0x80, 0x82, 0x82, 0x82, 0x82, 0x82, 0x02, 0x40,
   0xfc, 0x7f, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0xfc, 0x7f, 0x7c, 0x7c, 0x7c,
   0x7c, 0x7c, 0xfc, 0x3f };

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
      if(bot_lido_anterior > 0 && bot_lido_anterior < 500)    //Caso o botão Esquerdo seja precionado (Resistor de 1K)
      {
        bot_esquerdo = 1;                                     //Muda o estado da variavel do botão para verdadeiro
      }
      if(bot_lido_anterior > 500 && bot_lido_anterior < 1500) //Caso o botão Acima seja precionado (Resistor de 4K7)
      {
        bot_acima = 1;                                        //Muda o estado da variavel do botão para verdadeiro
      }
      if(bot_lido_anterior > 1500 && bot_lido_anterior < 2000)//Caso o botão Abaxo seja precionado (Resistor de 8K2)
      {
        bot_abaixo = 1;                                       //Muda o estado da variavel do botão para verdadeiro
      }
      if(bot_lido_anterior > 2000 && bot_lido_anterior < 2500)//Caso o botão Direito seja precionado (Resistor de 15K)
      {
        bot_direito = 1;                                      //Muda o estado da variavel do botão para verdadeiro
      }
          if(bot_lido_anterior > 2500 && bot_lido_anterior < 3200)//Caso o botão Selecionar seja precionado (Resistor de 33K)
      {
        bot_selecionar = 1;                                   //Muda o estado da variavel do botão para verdadeiro
      }
          if(bot_lido_anterior > 3200 && bot_lido_anterior < 3500)//Caso o botão Voltar seja precionado (Resistor de 47K)
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
                            Tela FORMULARIO (Sub-Rotina)
====================================================================================*/
void TelaFormulario(){                                        //TELA para Testar o Teclado
    if (Tela == 2) {                                          //Caso a variável de tela seja 2
    u8g2.firstPage();                                         //Este comando é parte do loop (imagem) que renderiza o conteúdo da exibição. (Obrigatório)
    do {                                                      //Tudo o que estiver dentro do "do {}" será exibido na tela.

      if (XP_NavCaixas == 0){                                 //Navegação da 1ª Caixa
        u8g2.setFontMode(1);                                  //Ativa o modo transparente de sobreposição do quadrado
        u8g2.setDrawColor(1);                                 //Configura a cor do quadrado a ser desenhado a seguir
        u8g2.drawBox(107,17,20,10);                           //Desenha um quadrado preenchido por cima dos icones
        u8g2.setDrawColor(2);                                 //Permite a sobreposição de itens de forma "transparente"
        u8g2.drawGlyph(8, 25, 187);                           //Mostra dentro das caixas um simbolo >>
      }
      if (XP_NavCaixas == 1){                                 //Navegação da 2ª Caixa
        u8g2.setFontMode(1);                                  //Ativa o modo transparente de sobreposição do quadrado
        u8g2.setDrawColor(1);                                 //Configura a cor do quadrado a ser desenhado a seguir
        u8g2.drawBox(107,35,20,10);                           //Desenha um quadrado preenchido por cima dos icones
        u8g2.setDrawColor(2);                                 //Permite a sobreposição de itens de forma "transparente"
        u8g2.drawGlyph(8, 43, 187);                           //Mostra dentro das caixas um simbolo >>
      }
        if (XP_NavCaixas == 2){                               //Navegação da 3ª Caixa
        u8g2.setFontMode(1);                                  //Ativa o modo transparente de sobreposição do quadrado
        u8g2.setDrawColor(1);                                 //Configura a cor do quadrado a ser desenhado a seguir
        u8g2.drawBox(107,53,20,10);                           //Desenha um quadrado preenchido por cima dos icones
        u8g2.setDrawColor(2);                                 //Permite a sobreposição de itens de forma "transparente"
        u8g2.drawGlyph(8, 62, 187);                           //Mostra dentro das caixas um simbolo >>
      }

      u8g2.drawStr(15,7,"Formulario Teste:");                 //Título da Tela
      u8g2.drawRFrame(1,0,126,9,0);                           //Desenha um retângulo em volta do título      
      u8g2.drawRFrame(5,17,100,10,0);                         //Desenha um retângulo para utilização da 1ª Caixa do texto                                                     
      u8g2.drawRFrame(5,35,100,10,0);                         //Desenha um retângulo para utilização da 2ª Caixa do texto                                                     
      u8g2.drawRFrame(5,53,100,10,0);                         //Desenha um retângulo para utilização da 3ª Caixa do texto

      u8g2.drawRFrame(107,17,20,10,0);                        //Desenha um retângulo em volta ícone depois da 1ª Caixa do Texto                                                     
      u8g2.drawRFrame(107,35,20,10,0);                        //Desenha um retângulo em volta ícone depois da 2ª Caixa do Texto                                                      
      u8g2.drawRFrame(107,53,20,10,0);                        //Desenha um retângulo em volta ícone depois da 3ª Caixa do Texto    
                                                                  
      u8g2.setFont(u8g2_font_open_iconic_all_1x_t);           //Altera a fonte para uma fonte de ícones
      u8g2.drawGlyph(113, 26, 164);                           //Desenha um ícone de Popup depois da 1ª caixa de texto
      u8g2.drawGlyph(113, 44, 164);                           //Desenha um ícone de Popup depois da 2ª caixa de texto
      u8g2.drawGlyph(113, 62, 164);                           //Desenha um ícone de Popup depois da 3ª caixa de texto
      u8g2.setFont(FontePadrao);                              //Volta para fonte Padrão do Projeto
      u8g2.drawStr(8,16,"Caixa 1:");                          //Título da 1ª Caixa de Texto
      u8g2.drawStr(8,34,"Caixa 2:");                          //Título da 2ª Caixa de Texto
      u8g2.drawStr(8,52,"Caixa 3:");                          //Título da 3ª Caixa de Texto
      u8g2.setCursor(14, 25);                                 //Configura a posição do proximo item a ser escrito na tela
      u8g2.print(XP_Caixa_Tela_02_00);                        //Mostra dentro da 1ª Caixa de Texto a String que pode ser alterada no teclado virtual.
      u8g2.setCursor(14, 43);                                 //Configura a posição do proximo item a ser escrito na tela
      u8g2.print(XP_Caixa_Tela_02_01);                        //Mostra dentro da 2ª Caixa de Texto a String que pode ser alterada no teclado virtual.
      u8g2.setCursor(14, 62);                                 //Configura a posição do proximo item a ser escrito na tela
      u8g2.print(XP_Caixa_Tela_02_02);                        //Mostra dentro da 3ª Caixa de Texto a String que pode ser alterada no teclado virtual.

      // ----BOTÂO ACIMA----
      if (bot_acima == 1 && Single_loop == 0) {               //Verifica se o botão Acima foi pressionado e se não passou dentro desse loop
        XP_NavCaixas--;                                       //Pula para caixa de texto que está acima
        if(XP_NavCaixas < 0) {                                //Verifica se o item está menor do que o numero caixas
          XP_NavCaixas = 2;                                   //Muda para a 3ª Caixa
        }
        Single_loop = 1;                                      //Registra que já passou dentro desse loop ou de outros botões(evita varios pulsos de botões)
      } // Fim - Botão Acima

        // ----BOTÂO ABAIXO----
      if (bot_abaixo == 1 && Single_loop == 0) {              //Verifica se o botão Abaixo foi pressionado e se não passou dentro desse loop
        XP_NavCaixas++;                                       //Pula para caixa de texto que está abaixo
        if(XP_NavCaixas > 2) {                                //Verifica se o item está maior do que o numero caixas
          XP_NavCaixas = 0;                                   //Muda para a 1ª Caixa
        }
       Single_loop = 1;                                       //Registra que já passou dentro desse loop ou de outros botões(evita varios pulsos de botões)
      }// Fim - Botão Abaixo

      // ----BOTÂO SELECIONAR----
      if (bot_selecionar == 1 && Single_loop == 0) {          //Verifica se o botão Acima foi pressionado e se não passou dentro desse loop
        if (XP_NavCaixas == 0) ID_CaixaSelecao = 0200;        //Altera o ID_CaixaSeleção deixando-o pronto pra a chamada do teclado
        if (XP_NavCaixas == 1) ID_CaixaSelecao = 0201;        //Altera o ID_CaixaSeleção deixando-o pronto pra a chamada do teclado
        if (XP_NavCaixas == 2) ID_CaixaSelecao = 0202;        //Altera o ID_CaixaSeleção deixando-o pronto pra a chamada do teclado
        UltimaTela = 2;                                       //Guarda a posição da ultima tela antes do teclado ser chamado
        Single_loop = 1;                                      //Registra que já passou dentro desse loop ou de outros botões(evita varios pulsos de botões)
        Tela = T_Teclado;                                     //Muda a tela baseado no numero do item do menu
      } // Fim - Botão Selecionar

      // ----BOTÂO VOLTAR----
      if (bot_voltar == 1 && Single_loop == 0) {              //Caso o botão Voltar for pressionado
        Tela = -1;                                            //Configura para o ID do Menu Principal
        Single_loop = 1;                                      //Registra que já passou dentro desse loop ou de outros botões(evita varios pulsos de botões)     
      }
    }
    while ( u8g2.nextPage() );                                //Este comando é parte do loop (imagem) que renderiza o conteúdo da exibição. (Obrigatório)
  }
}//--------------------------FIM - Tela de FORMULARIO ------------------------------------

/*====================================================================================
                            Tela de Teclado (Sub-Rotina)
====================================================================================*/
void Teclado(){                                               //Modelo para uma nova tela
    if (Tela == T_Teclado) {                                          //Caso a variável de tela seja ...
    u8g2.firstPage();                                         //Este comando é parte do loop (imagem) que renderiza o conteúdo da exibição. (Obrigatório)
    do {                                                      //Tudo o que estiver dentro do "do {}" será exibido na tela.                            
        switch (TecladoCAPS) {                                //Verifica o valor da variavel TecladoCAPS que seleciona o maiusculo do teclado
          case 0:                                             //Caso seja 0
          u8g2.drawXBM(0, 0, 128, 64, Teclado_Minusculo_bits);//Exibe o Teclado Minusculo
          break;                                              //Fim do Caso 0
          case 1:                                             //Caso seja 1
          u8g2.drawXBM(0, 0, 128, 64, Teclado_Maiusculas_bits);//Exibe o Teclado Maiusculo
          break;                                              //Fim do Caso 1
          case 2:                                             //Caso seja 2
          u8g2.drawXBM(0, 0, 128, 64, Teclado_Maiusculo_CAPS_bits);//Exibe o Teclado Maiusculo Travado
          break;                                              //Fim do Caso 2
        }
        if (ModoDesenv == 1) {                                     //Mostra duas varíaveis que utilizei para localizar os botões
        u8g2.setCursor(100, 10);                                   //Configura a posição do proximo item a ser escrito na tela
        u8g2.print(TecladoCAPS);                                   //Mostra a varíavel que utilizo para minusculo e maiusculo
        u8g2.setCursor(112, 10);                                   //Configura a posição do proximo item a ser escrito na tela
        u8g2.print(TecladoBox);                                    //Mostra o ID do botão selecionado
        }

        //    -----Navegação do Teclado-----
        int tecladox = 0;                                        //Variavel para Coordenada X Temporária do TecladoBox
        int tecladoy = 0;                                        //Variavel para Coordenada Y Temporária do TecladoBox
                
        if(TecladoBox >= 0 && TecladoBox <= 15){                 //Verifica o valor de TecladoBox se está na 1ª Linha
          tecladoy = 0;                                          //Coordenada da Linha
          tecladox = TecladoBox;                                 //Coordenada da Coluna
        } 
        if(TecladoBox >= 16 && TecladoBox <= 31){                //Verifica o valor de TecladoBox se está na 2ª Linha
          tecladoy = 1;                                          //Coordenada da Linha
          tecladox = TecladoBox - 16;                            //Coordenada da Coluna subtraindo valor de quadrados da linha anterior
        }
        if(TecladoBox >= 32 && TecladoBox <= 47){                //Verifica o valor de TecladoBox se está na 3ª Linha
          tecladoy = 2;                                          //Coordenada da Linha
          tecladox = TecladoBox - 32;                            //Coordenada da Coluna subtraindo valor de quadrados da linha anterior
        }
       if(TecladoBox >= 48 && TecladoBox <= 63){                 //Verifica o valor de TecladoBox se está na 4ª Linha
          tecladoy = 3;                                          //Coordenada da Linha
          tecladox = TecladoBox - 48;                            //Coordenada da Coluna subtraindo valor de quadrados da linha anterior
        } 
      if(TecladoBox >= 64 && TecladoBox <= 79){                  //Verifica o valor de TecladoBox se está na 5ª Linha
          tecladoy = 4;                                          //Coordenada da Linha
          tecladox = TecladoBox - 64;                            //Coordenada da Coluna subtraindo valor de quadrados da linha anterior
        }
        u8g2.setCursor(4, 10);                                   //Configura a posição do proximo item a ser escrito na tela
        u8g2.setFont(FontePadrao);                               //Retorna para a fonte padrão do projeto
        u8g2.print(bufferTeclado);                               //Mostra os caracteres selecionados no teclado

        //item de navegação dos itens com base na posição do "TecladoBox"
        int SelecBoxMenux[] = { 0, 8 ,16 ,24 ,32 ,40 ,48 ,56 ,64 ,72 ,80 ,88 ,96 ,104 ,112 ,120,
                                0, 8 ,16 ,24 ,32 ,40 ,48 ,56 ,64 ,72 ,80 ,88 ,96 ,104 ,112 ,120,
                                0, 8 ,16 ,24 ,32 ,40 ,48 ,56 ,64 ,72 ,80 ,88 ,96 ,104 ,112 ,120,
                                0, 8 ,16 ,24 ,32 ,40 ,48 ,56 ,64 ,72 ,80 ,88 ,96 ,104 ,112 ,120,
                                0, 8 ,16 ,24 ,32 ,40 ,48 ,56 ,64 ,72 ,80 ,88 ,96 ,104 ,112 ,120};   //Posições do x do quadrado de seleção dos caracteres
        int SelecBoxMenuy[] = { 13 ,13 ,13 ,13 ,13 ,13 ,13 ,13 ,13 ,13 ,13 ,13 ,13 ,13 ,13 ,13,
                                23 ,23 ,23 ,23 ,23 ,23 ,23 ,23 ,23 ,23 ,23 ,23 ,23 ,23 ,23 ,23,
                                33 ,33 ,33 ,33 ,33 ,33 ,33 ,33 ,33 ,33 ,33 ,33 ,33 ,33 ,33 ,33,
                                43 ,43 ,43 ,43 ,43 ,43 ,43 ,43 ,43 ,43 ,43 ,43 ,43 ,43 ,43 ,43,
                                53 ,53 ,53 ,53 ,53 ,53 ,53 ,53 ,53 ,53 ,53 ,53 ,53 ,53 ,53 ,53};   //Posições do y do quadrado de seleção dos caracteres

        u8g2.setDrawColor(2);                                                    //Permite a sobreposição de itens de forma "transparente"
        u8g2.drawBox(SelecBoxMenux[TecladoBox],SelecBoxMenuy[TecladoBox],8,10);  //Desenha um quadrado preenchido por cima dos ícones utilizando as posições conforme o numero do TecladoBox
        
        switch (TecladoBox) {                               //Verifica o valor da variavel TecladoBox esta função é para os ícones grandes do teclado
        case 48:                                            //Caso esteja na posição 48
          u8g2.drawBox(8,43,8,10);                          //Estende o quadrado de seleção desenhando outro ao lado.
          break;
        case 49:                                            //Caso esteja na posição 49
          u8g2.drawBox(0,43,8,10);                          //Estende o quadrado de seleção desenhando outro ao lado.
          break;
        case 62:                                            //Caso esteja na posição 62
          u8g2.drawBox(120,43,8,10);                        //Estende o quadrado de seleção desenhando outro ao lado.
          break;
        case 63:                                            //Caso esteja na posição 63
          u8g2.drawBox(112,43,8,10);                        //Estende o quadrado de seleção desenhando outro ao lado.
          break;
        case 64:                                            //Caso esteja na posição 64
          u8g2.drawBox(8,53,8,10);                          //Estende o quadrado de seleção desenhando outro ao lado.
          break;
        case 65:                                            //Caso esteja na posição 48
          u8g2.drawBox(0,53,8,10);                          //Estende o quadrado de seleção desenhando outro ao lado.
          break;
        case 71:                                            //Caso esteja na posição 71
          u8g2.drawBox(64,53,8,10);                         //Estende o quadrado de seleção desenhando outro ao lado.
          break;
        case 72:                                            //Caso esteja na posição 72
          u8g2.drawBox(56,53,8,10);                         //Estende o quadrado de seleção desenhando outro ao lado.
          break;
        case 78:                                            //Caso esteja na posição 78
          u8g2.drawBox(120,53,8,10);                        //Estende o quadrado de seleção desenhando outro ao lado.
          break;
        case 79:                                            //Caso esteja na posição 79
          u8g2.drawBox(112,53,8,10);                        //Estende o quadrado de seleção desenhando outro ao lado.
          break;
        }
        //bufferTeclado = ;
        // ----BOTÂO ESQUERDO----
        if (bot_esquerdo == 1 && Single_loop == 0) {          //Verifica se o botão Esquerdo foi pressionado e se não passou dentro desse loop
          TecladoBox--;                                       //Retrocede um item do teclado
          Single_loop = 1;                                    //Registra que já passou dentro desse loop ou de outros botões(evita varios pulsos de botões)
          if(TecladoBox < 0) {                                //Verifica se o item do teclado está maior do que o numero de teclas
            TecladoBox = 79;                                  //Muda para o ultimo item do teclado
        }
        } // Fim - Botão Esquerdo

        // ----BOTÂO ACIMA----
      if (bot_acima == 1 && Single_loop == 0) {               //Verifica se o botão Acima foi pressionado e se não passou dentro desse loop
        TecladoBox = TecladoBox - 16;                         //Pula para o item do menu que está acima
        Single_loop = 1;                                      //Registra que já passou dentro desse loop ou de outros botões(evita varios pulsos de botões)
        if(TecladoBox < 0) {                                  //Verifica se o item do menu está maior do que o numero de teclas
          TecladoBox = 79;                                    //Muda para o ultimo item do menu
        }
      } // Fim - Botão Acima

        // ----BOTÂO ABAIXO----
      if (bot_abaixo == 1 && Single_loop == 0) {              //Verifica se o botão Abaixo foi pressionado e se não passou dentro desse loop
        TecladoBox = TecladoBox + 16;                         //Pula para o item do menu que está abaixo
        Single_loop = 1;                                      //Registra que já passou dentro desse loop ou de outros botões(evita varios pulsos de botões)
        if(TecladoBox > 79) {                                 //Verifica se o item do menu está maior do que o numero de teclas
          TecladoBox = 0;                                     //Muda para o primeiro item do menu
        }
      }// Fim - Botão Abaixo

      // ----BOTÂO DIREITO----
      if (bot_direito == 1 && Single_loop == 0) {             //Verifica se o botão Acima foi pressionado e se não passou dentro desse loop
        TecladoBox++;                                         //Avança um item do menu
        Single_loop = 1;                                      //Registra que já passou dentro desse loop ou de outros botões(evita varios pulsos de botões)
        if(TecladoBox > 79) {                                 //Verifica se o item do menu está maior do que o numero teclas
          TecladoBox = 0;                                     //Muda para o primeiro item do menu
        }
      } // Fim - Botão Direito

      // ----BOTÂO SELECIONAR----
      if (bot_selecionar == 1 && Single_loop == 0) {          //Verifica se o botão Acima foi pressionado e se não passou dentro desse loop
        bool switchTemp = 0;                                  //Variável local para mudaça dos tipos do Teclado (Minusculo, Maiusculo ou Maiusculo Travado)
        bool caractereSpecial = 0;                            //Variável local para guardar que tipo de tecla com acento foi pressionada

        // Alterador de Minuscula para Maiuscula ou Maiuscula Travada
        switch (TecladoBox) {                                 //Casos para mudaça do Layout do Teclado (Minusculo, Maiusculo ou Maiusculo Travado)
        case 48:                                              //Caso a tecla SHIFT for pressionada
          if (TecladoCAPS == 2 && switchTemp == 0) {          //Verifica se está na posição do teclado Travado e se não foi alterado recentemente
          TecladoCAPS = 0;                                    //Muda para Minusculo
          switchTemp = 1;                                     //Informa que já foi alterado recentemente o layout
        }
          if (TecladoCAPS == 1 && switchTemp == 0)  {         //Verifica se está na posição do teclado Maiusculo e se não foi alterado recentemente
          TecladoCAPS = 0;                                    //Muda para Minusculo
          switchTemp = 1;                                     //Informa que já foi alterado recentemente o layout
        }
          if (TecladoCAPS == 0 && switchTemp == 0){           //Verifica se está na posição do teclado Minusculo e se não foi alterado recentemente
          TecladoCAPS = 1;                                    //Muda para Maiusculo
          switchTemp = 1;                                     //Informa que já foi alterado recentemente o layout
        }
          break;
        case 49:                                              //Caso a tecla SHIFT for pressionada
          if (TecladoCAPS == 2 && switchTemp == 0) {          //Verifica se está na posição do teclado Travado e se não foi alterado recentemente
          TecladoCAPS = 0;                                    //Muda para Minusculo
          switchTemp = 1;                                     //Informa que já foi alterado recentemente o layout
        }
          if (TecladoCAPS == 1 && switchTemp == 0) {          //Verifica se está na posição do teclado Maiusculo e se não foi alterado recentemente
          TecladoCAPS = 0;                                    //Muda para Minusculo
          switchTemp = 1;                                     //Informa que já foi alterado recentemente o layout
        }
          if (TecladoCAPS == 0 && switchTemp == 0) {          //Verifica se está na posição do teclado Minusculo e se não foi alterado recentemente
          TecladoCAPS = 1;                                    //Muda para Maiusculo
          switchTemp = 1;                                     //Informa que já foi alterado recentemente o layout
        }
          break;
        case 64:                                              //Caso a tecla SHIFT com Cadeado for pressionada
          if (TecladoCAPS == 0 && switchTemp == 0) {          //Verifica se está na posição do teclado Minusculo e se não foi alterado recentemente
            TecladoCAPS = 2;                                  //Muda para Maiusculo Travado
            switchTemp = 1;                                   //Informa que já foi alterado recentemente o layout
          }
          if (TecladoCAPS == 1 && switchTemp == 0) {          //Verifica se está na posição do teclado Maiusculo e se não foi alterado recentemente
            TecladoCAPS = 2;                                  //Muda para Maiusculo Travado
            switchTemp = 1;                                   //Informa que já foi alterado recentemente o layout
          }
          if (TecladoCAPS == 2 && switchTemp == 0) {          //Verifica se está na posição do teclado Travado e se não foi alterado recentemente
            TecladoCAPS = 0;                                  //Muda para Minusculo
            switchTemp = 1;                                   //Informa que já foi alterado recentemente o layout
          }
          break;
        case 65:                                              //Caso a tecla SHIFT com Cadeado for pressionada
          if (TecladoCAPS == 0 && switchTemp == 0) {          //Verifica se está na posição do teclado Minusculo e se não foi alterado recentemente
            TecladoCAPS = 2;                                  //Muda para Maiusculo Travado
            switchTemp = 1;                                   //Informa que já foi alterado recentemente o layout
          }
          if (TecladoCAPS == 1 && switchTemp == 0) {          //Verifica se está na posição do teclado Maiusculo e se não foi alterado recentemente
            TecladoCAPS = 2;                                  //Muda para Maiusculo Travado
            switchTemp = 1;                                   //Informa que já foi alterado recentemente o layout
          }
          if (TecladoCAPS == 2 && switchTemp == 0) {          //Verifica se está na posição do teclado Travado e se não foi alterado recentemente
            TecladoCAPS = 0;                                  //Muda para Minusculo
            switchTemp = 1;                                   //Informa que já foi alterado recentemente o layout
          }
          break;
        }

        for(int XP = 0; XP <= 4 ; XP++) {                     //Pequeno Loop para geração dos acentos das teclas duplas
          if(TecladoBox == tecladoAcentos[XP]){               //Verifica se a tecla pressionada é uma das teclas de acentuação (veja: tecladoAcentos)
            tempAcento = tecladoAcentos[XP];                  //Guarda a ID da tecla pressionada na variavel temporária
            if (TecladoCAPS == 1 || TecladoCAPS == 2){        //Caso o teclado esteja em Maiuscula ou Maiuscula Travada
              if (TecladoBox == 28) tempAcento = 180;         // Se a tecla ' ` '(acento agudo) for pressionada guarda a ID 180
            }
        } // Fim do loop
       }
        if(tempAcento > 0){                                             //Caso algum acento seja pressionado anteriormente
          if(TecladoCAPS == 0){                                         //Se o teclado estiver no Layout MINUSCULO
            for(int XP = 0; XP < 7 ; XP++) {                            //Faz um pequeno loop para todas as 7 possibilidades
              if(TecladoBox == teclaAceitaAcentos[XP]){                 //Verifica se está em alguma tecla que aceita acentuação como a,e,i,o,u,'espaço','espaço'
                if(tempAcento == 28){                                   //Verifica se a tecla '´' foi pressionada anteriormente
                  bufferTeclado = (bufferTeclado + acento_agudo_m[XP]); //Caso sim, adiciona ao buffer a letra correspondente com o acento
                  tempAcento = 0;                                       //Apaga a ultima tecla pressionada
                  caractereSpecial = 1;                                 //Informa que já foi inserida uma letra no buffer do teclado
                }
                if(tempAcento == 44){                                   //Verifica se a tecla '~' foi pressionada anteriormente
                  bufferTeclado = (bufferTeclado + acento_til_m[XP]);   //Caso sim, adiciona ao buffer a letra correspondente com o acento
                  tempAcento = 0;                                       //Apaga a ultima tecla pressionada
                  caractereSpecial = 1;                                 //Informa que já foi inserida uma letra no buffer do teclado
                }
                if(tempAcento == 61){                                   //Verifica se a tecla '^' foi pressionada anteriormente
                  bufferTeclado = (bufferTeclado + acento_circunflexo_m[XP]);   //Caso sim, adiciona ao buffer a letra correspondente com o acento
                  tempAcento = 0;                                       //Apaga a ultima tecla pressionada
                  caractereSpecial = 1;                                 //Informa que já foi inserida uma letra no buffer do teclado
                }
                if(tempAcento == 68){                                   //Verifica se a tecla '¨' foi pressionada anteriormente
                  bufferTeclado = (bufferTeclado + acento_trema_m[XP]); //Caso sim, adiciona ao buffer a letra correspondente com o acento
                  tempAcento = 0;                                       //Apaga a ultima tecla pressionada
                  caractereSpecial = 1;                                 //Informa que já foi inserida uma letra no buffer do teclado
                }
               if(tempAcento == 180){                                   //Verifica se a tecla '´' foi pressionada anteriormente
                  bufferTeclado = (bufferTeclado + acento_grave_m[XP]); //Caso sim, adiciona ao buffer a letra correspondente com o acento
                  tempAcento = 0;                                       //Apaga a ultima tecla pressionada
                  caractereSpecial = 1;                                 //Informa que já foi inserida uma letra no buffer do teclado
                }
              }
            }
          }// Fim do if tempAcento = 0

          if(TecladoCAPS == 1 || TecladoCAPS == 2){                     //Se o teclado estiver no Layout MAIUSCULO ou MAIUSCULO TRAVADO
            for(int XP = 0; XP < 7 ; XP++) {                            //Faz um pequeno loop para todas as 7 possibilidades
              if(TecladoBox == teclaAceitaAcentos[XP]){                 //Verifica se está em alguma tecla que aceita acentuação como a,e,i,o,u,'espaço','espaço'
                if(tempAcento == 28){                                   //Verifica se a tecla '´' foi pressionada anteriormente
                  bufferTeclado = (bufferTeclado + acento_agudo_M[XP]); //Caso sim, adiciona ao buffer a letra correspondente com o acento
                  tempAcento = 0;                                       //Apaga a ultima tecla pressionada
                  caractereSpecial = 1;                                 //Informa que já foi inserida uma letra no buffer do teclado
                }
                if(tempAcento == 44){                                   //Verifica se a tecla '~' foi pressionada anteriormente
                  bufferTeclado = (bufferTeclado + acento_til_M[XP]);   //Caso sim, adiciona ao buffer a letra correspondente com o acento
                  tempAcento = 0;                                       //Apaga a ultima tecla pressionada
                  caractereSpecial = 1;                                 //Informa que já foi inserida uma letra no buffer do teclado
                }
                if(tempAcento == 61){                                   //Verifica se a tecla '^' foi pressionada anteriormente
                  bufferTeclado = (bufferTeclado + acento_circunflexo_M[XP]);   //Caso sim, adiciona ao buffer a letra correspondente com o acento
                  tempAcento = 0;                                       //Apaga a ultima tecla pressionada
                  caractereSpecial = 1;                                 //Informa que já foi inserida uma letra no buffer do teclado
                }
                if(tempAcento == 68){                                   //Verifica se a tecla '¨' foi pressionada anteriormente
                  bufferTeclado = (bufferTeclado + acento_trema_M[XP]); //Caso sim, adiciona ao buffer a letra correspondente com o acento
                  tempAcento = 0;                                       //Apaga a ultima tecla pressionada
                  caractereSpecial = 1;                                 //Informa que já foi inserida uma letra no buffer do teclado
                }
                if(tempAcento == 180){                                  //Verifica se a tecla '´' foi pressionada anteriormente
                  bufferTeclado = (bufferTeclado + acento_grave_M[XP]); //Caso sim, adiciona ao buffer a letra correspondente com o acento
                  tempAcento = 0;                                       //Apaga a ultima tecla pressionada
                  caractereSpecial = 1;                                 //Informa que já foi inserida uma letra no buffer do teclado
                }
              }
            }
          }// Fim do if tempAcento = 1 e 2
        }// Fim do if tempAcento > 0

        if (TecladoBox == 62 || TecladoBox == 63){                      //Verifica se a tecla Apagar foi pressionada
          bufferTeclado.remove(bufferTeclado.length() - 1);             //Apaga a ultima letra baseado no tamanho da string
        }

        if(TecladoBox == 30){                                           // Caso a Tecla Dupla (' ou ") for pressionada
          if (TecladoCAPS == 0) bufferTeclado = bufferTeclado + '\"';   // Se estiver minusculo adiciona "
          if (TecladoCAPS == 1 || TecladoCAPS == 2) bufferTeclado = bufferTeclado + '\''; // Se estiver maiusculo adiciona '
        }
        
        // Regra principal para evitar adicionar letras duplicadas ou adicionar strings vazias no bufferTeclado
        if (TecladoBox != 28 &&       //Tecla ´
            TecladoBox != 30 &&       //Tecla " ou '
            TecladoBox != 44 &&       //Tecla ~
            TecladoBox != 48 &&       //Tecla Shift
            TecladoBox != 49 &&       //Tecla Shift
            TecladoBox != 61 &&       //Tecla ^
            TecladoBox != 62 &&       //Tecla Apagar
            TecladoBox != 63 &&       //Tecla Apagar
            TecladoBox != 64 &&       //Tecla Shit Travada
            TecladoBox != 65 &&       //Tecla Shit Travada
            TecladoBox != 68 &&       //Tecla Trema
            TecladoBox != 78 &&       //Tecla Enviar
            TecladoBox != 79 &&       //Tecla Enviar
            caractereSpecial == 0){   //Se algum carater especial foi selecionado anteriormente
              //Caso TecladoCAPS seja minusculo adiciona ao bufferTeclado a palavra da matriz do teclado minusculo
              if (TecladoCAPS == 0) bufferTeclado = bufferTeclado + (teclado_minusculo[tecladoy][tecladox]); 
              //Caso TecladoCAPS seja Maiusculo adiciona ao bufferTeclado a palavra da matriz do teclado Maiusculo
              if (TecladoCAPS == 1 || TecladoCAPS == 2) bufferTeclado = bufferTeclado + (teclado_Maiusculo[tecladoy][tecladox]);
              if (TecladoCAPS == 1) TecladoCAPS = 0;          //Depois de adicionar uma letra Maiuscula qualquer volta para o layout minusculo         
        }

        //Gravação das STRINGS, caso você queira que o teclado escreva em uma string qualquer adicione a linha aqui usado o ID_CaixaSelecao
        if (TecladoBox == 78 || TecladoBox == 79) {                          //Caso esteja sobre o Botão Enviar
          if (ID_CaixaSelecao == 0200) XP_Caixa_Tela_02_00 = bufferTeclado;  //Copia a String do bufferTeclado para outra
          if (ID_CaixaSelecao == 0201) XP_Caixa_Tela_02_01 = bufferTeclado;  //Copia a String do bufferTeclado para outra
          if (ID_CaixaSelecao == 0202) XP_Caixa_Tela_02_02 = bufferTeclado;  //Copia a String do bufferTeclado para outra
          bufferTeclado = "";                                                //Limpa a String do bufferTeclado
          Tela = UltimaTela;                                                 //Finaliza voltando para a tela anterior
        }
        Single_loop = 1;                                      //Registra que já passou dentro desse loop ou de outros botões(evita varios pulsos de botões)
      } // Fim - Botão Selecionar

      // ----BOTÂO Voltar----
      if (bot_voltar == 1 && Single_loop == 0) {              //Caso o botão Voltar for pressionado
        Tela = UltimaTela;                                    //Configura para o ID do Menu Principal
        Single_loop = 1;                                      //Registra que já passou dentro desse loop ou de outros botões(evita varios pulsos de botões)     
      }// Fim - Botão Voltar
    }
    while ( u8g2.nextPage() );                                //Este comando é parte do loop (imagem) que renderiza o conteúdo da exibição. (Obrigatório)
  }
}//--------------------------FIM - Tela de Teclado ------------------------------------

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
      u8g2.drawGlyph(45, 29, 257);	                          //Icone do Formulário
      u8g2.drawGlyph(66, 29, 129);	                          //Icone do Configurações
      u8g2.drawGlyph(87, 29, 248);	                          //Icone do Wifi 
      u8g2.drawGlyph(108, 29, 175);	                          //Icone do Servidor Web 
      // Segunda Linha
      u8g2.drawGlyph(3, 50, 202);	                            //Icone do Senha 
      u8g2.drawGlyph(24, 50, 259);	                          //Icone do LED RBG
      u8g2.drawGlyph(45, 50, 90);	                            //Icone do Bateria
      u8g2.drawGlyph(66, 50, 122);	                          //Icone do Logs
      u8g2.drawGlyph(87, 50, 222);	                          //Icone do Telas
      //u8g2.drawGlyph(108, 50, numero do icone);	            //Icone vazio(basta descomentar)

      //item de navegação dos itens com base na posição do "ChangeMenu"
      int SelecBoxMenux[] = {1, 22, 43, 64, 85, 106, 1, 22, 43, 64, 85, 106};   //Posições do x do quadrado de seleção dos icones
      int SelecBoxMenuy[] = {11, 11, 11, 11, 11, 11, 32, 32, 32, 32, 32, 32};   //Posições do y do quadrado de seleção dos icones

      u8g2.setDrawColor(2);                                                     //Permite a sobreposição de itens de forma "transparente"
      u8g2.drawBox(SelecBoxMenux[ChangeMenu],SelecBoxMenuy[ChangeMenu],20,20);  //Desenha um quadrado preenchido por cima dos icones utilizando as posições conforme o numero do ChangeMenu


      u8g2.drawRFrame(0,53,127,10,0);                         //Desenha um retangulo em volta do nome do item do menu selecionado
      u8g2.setFont(FontePadrao);                              //Retorna para a fonte padrão do projeto
      u8g2.drawStr(55,61,listaMenu[ChangeMenu]);              //Escreve na tela o nome da listaMenu que é posicionada atraves do ChangeMenu.
      u8g2.drawStr(2,61,XP_Versao);                           //Escreve na tela o a versão do XPsys


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
    //u8g2.enableUTF8Print();                                   //Habilita a codificação UTF8 que aceita acentos
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
  Teclado();
  TelaFormulario();
}//--------------------------FIM - LOOP ------------------------------------
