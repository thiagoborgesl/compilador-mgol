#include <fstream>
#include <iostream>
#include <list>
#include <ostream>
#include <regex>
#include <sstream>
#include <string.h>
#include <string>

using namespace std;
/*================================ANALISADOR *
 * LEXICO================================*/
int readPos = 0, linePos = 1;
string tokensToSinc;

typedef struct {
  string classe;
  string lexema;
  string tipo;
} TOKEN;

list<TOKEN> tabelaDeSimbolos;

string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz01234567"
                  "89,:!?;[]_.;\\+-*/()\"<>={}";

void iniciaTabelaDeSimbolos(list<TOKEN> &TS) {
  TOKEN inicio = {"inicio", "inicio", "inicio"},
        varinicio = {"varinicio", "varinicio", "varinicio"},
        varfim = {"varfim", "varfim", "varfim"},
        escreva = {"escreva", "escreva", "escreva"},
        leia = {"leia", "leia", "leia"}, se = {"se", "se", "se"},
        entao = {"entao", "entao", "entao"},
        fimse = {"fimse", "fimse", "fimse"},
        repita = {"repita", "repita", "repita"},
        fimrepita = {"fimrepita", "fimrepita", "fimrepita"},
        fim = {"fim", "fim", "fim"},
        inteiro = {"inteiro", "inteiro", "inteiro"},
        literall = {"literal", "literal", "literal"},
        real = {"real", "real", "real"};

  TS.push_front(inicio);
  TS.push_front(varinicio);
  TS.push_front(varfim);
  TS.push_front(escreva);
  TS.push_front(leia);
  TS.push_front(se);
  TS.push_front(entao);
  TS.push_front(fimse);
  TS.push_front(repita);
  TS.push_front(fimrepita);
  TS.push_front(fim);
  TS.push_front(inteiro);
  TS.push_front(literall);
  TS.push_front(real);
}

void inserirNaTabela(list<TOKEN> &TS, TOKEN T) { TS.push_front(T); }

TOKEN buscaNaTabela(list<TOKEN> &TS, string T) {
  list<TOKEN>::iterator it;
  TOKEN tokenFound;

  tokenFound.lexema = "";
  tokenFound.tipo = "";
  tokenFound.classe = "";

  for (it = TS.begin(); it != TS.end(); it++) {
    if (it->lexema == T) {
      tokenFound.classe = it->classe;
      tokenFound.lexema = it->lexema;
      tokenFound.tipo = it->tipo;
    }
  }
  return tokenFound;
}

void atualizarTabela(list<TOKEN> &TS, TOKEN T) {
  list<TOKEN>::iterator it;
  for (it = TS.begin(); it != TS.end(); it++) {
    if (it->lexema == T.lexema) {
      it->classe = T.classe;
      it->lexema = T.lexema;
      it->tipo = T.tipo;
    }
  }
}

TOKEN SCANNER(ifstream &inFile) {
  TOKEN t, r;
  char character;
  string word = "";
  regex AB_P("\\(");
  regex FC_P("\\)");
  regex OPM("\\+|\\-|\\*|\\/");
  regex RCB("<-");
  regex OPRS("\\<|\\>");
  regex OPRC("\\>=|\\<=|\\=|\\<>");
  regex PT_V(";");
  regex LIT("\"([^\"\\\\]|\\\\.)*\"");
  regex COM("\\{([^\"\\\\]|\\\\.)*\\}");
  regex ID("[A-Za-z][A-Za-z0-9_]*");
  regex NUM(
      "([[:digit:]]+)(\\.(([[:digit:]]+)?))?((e|E)((\\+|-)?)[[:digit:]]+)?");

loop:
  while (inFile.get(character)) {
    readPos++;

    if (character == '\n') {
      linePos++;
      readPos = 0;
    }

    if ((character == ' ' || character == '\n' || character == '\t')) {
      goto loop;
    }

    // detecta caracter fora do alfabeto
    if (!(alphabet.find(character) != std::string::npos)) {
      word.push_back(character);
      t.classe = "ERRO";
      t.lexema = word;
      t.tipo = "NULO";
      return t;
    }

    word.push_back(character);

    if (regex_match(word, AB_P)) {
      t.classe = "ab_p";
      t.lexema = word;
      t.tipo = "NULO";
      return t;
    }

    if (regex_match(word, FC_P)) {
      t.classe = "fc_p";
      t.lexema = word;
      t.tipo = "NULO";
      return t;
    }

    if (regex_match(word, OPM)) {
      t.classe = "opm";
      t.lexema = word;
      t.tipo = "NULO";
      return t;
    }

    if (regex_match(word, OPRS)) {
      // inFile.seekg(+1,ios::cur);
      inFile.get(character);
      word.push_back(character);
      if (regex_match(word, OPRC)) {
        t.classe = "opr";
        t.lexema = word;
        t.tipo = "NULO";
        return t;
      }
      if (regex_match(word, RCB)) {
        t.classe = "rcb";
        t.lexema = word;
        t.tipo = "NULO";
        return t;
      }
      word.pop_back();
      inFile.seekg(-1, ios::cur);
      t.classe = "opr";
      t.lexema = word;
      t.tipo = "NULO";
      return t;
    }

    if (regex_match(word, PT_V)) {
      t.classe = "pt_v";
      t.lexema = word;
      t.tipo = "NULO";
      return t;
    }

    if (regex_match(word, LIT)) {
      t.classe = "lit";
      t.lexema = word;
      t.tipo = "literal";
      return t;
    }

    if (regex_match(word, COM)) {
      t.classe = "comentario";
      t.lexema = word;
      t.tipo = "NULO";
      return t;
    }

    if (regex_match(word, NUM)) {
      inFile.get(character);
      if ((character == 'E' || character == 'e' || character == '.' ||
           isdigit(character)) &&
          !(inFile.eof())) {
        inFile.seekg(-1, ios::cur);
        goto loop;
      }
      inFile.seekg(-1, ios::cur);
      if ((word.find('.') != std::string::npos))
        t.tipo = "real";
      else
        t.tipo = "inteiro";
      t.classe = "num";
      t.lexema = word;
      return t;
    }

    if (regex_match(word, ID)) {
      inFile.get(character);
      if ((isalpha(character) || isdigit(character) || character == '_') &&
          !(inFile.eof())) {
        inFile.seekg(-1, ios::cur);
        goto loop;
      }
      // inFile.unget()
      inFile.seekg(-1, ios::cur);
      t.classe = "id";
      t.lexema = word;
      t.tipo = "NULO";
      r = buscaNaTabela(tabelaDeSimbolos, t.lexema);
      if (r.lexema.empty()) {
        inserirNaTabela(tabelaDeSimbolos, t);
        return t;
      } else
        return r;
    }
  }

  if (inFile.eof()) {
    t.classe = "eof";
    t.lexema = "EOF";
    t.tipo = "EOF";
    inFile.close();
    return t;
  }
}
/*================================ANALISADOR *
 * LEXICO================================*/

/*==============================ANALISADOR *
 * SINTATICO===============================*/
string GOTO[76][20];
string ACTION[76][24];
string FOLLOW[19];

void iniciarFollow(){
  //P
  FOLLOW[0] = "eof";
  //V
  FOLLOW[1] = "leia, escreva, repita, se, id, fim";
  //LV
  FOLLOW[2] = "leia, escreva, repita, se, id, fim";
  //D
  FOLLOW[3] = "inteiro, real, literal, varfim";
  //L
  FOLLOW[4] = "pt_v";
  //TIPO
  FOLLOW[5] = "id";
  //A
  FOLLOW[6] = "eof";
  //ES
  FOLLOW[7] = "leia, escreva, repita, se, id, fim, fimse, fimrepita";
  //ARG
  FOLLOW[8] = "pt_v";
  //CMD
  FOLLOW[9] = "leia, escreva, repita, se, id , fim, fimse, fimrepita";
  //LD
  FOLLOW[10] = "pt_v";
  //OPRD
  FOLLOW[11] = "opm, pt_v, opr, fc_p";
  //COND
  FOLLOW[12] = "leia, escreva, id, se, repita, fim, fimse, fimrepita";
  //CAB
  FOLLOW[13] = "leia, escreva, id, se, fimse";
  //EXP_R
  FOLLOW[14] = "fc_p";
  //CP
  FOLLOW[15] = "leia, escreva, id, se, repita, fim, fimse, fimrepita";
  //R
  FOLLOW[16] = "leia, escreva, id, se, repita, fim";
  //CABR
  FOLLOW[17] = "leia, escreva, id, se, fimrepita";
  //CPR
  FOLLOW[18] = "leia, escreva, id, se, repita, fim";
}

string regrasGramatica[38];

void preencherGramatica(string gramatica[]) {
  gramatica[0] = "P'->P";
  gramatica[1] = "P->inicio V A";
  gramatica[2] = "V->varinicio LV";
  gramatica[3] = "LV->D LV";
  gramatica[4] = "LV->varfim pt_v";
  gramatica[5] = "D->TIPO L pt_v";
  gramatica[6] = "L->id";
  gramatica[7] = "TIPO->inteiro";
  gramatica[8] = "TIPO->real";
  gramatica[9] = "TIPO->literal";
  gramatica[10] = "A->ES A";
  gramatica[11] = "ES->leia id pt_v";
  gramatica[12] = "ES->escreva ARG pt_v";
  gramatica[13] = "ARG->lit";
  gramatica[14] = "ARG->num";
  gramatica[15] = "ARG->id";
  gramatica[16] = "A->CMD A";
  gramatica[17] = "CMD->id rcb LD pt_v";
  gramatica[18] = "LD->OPRD opm OPRD";
  gramatica[19] = "LD->OPRD";
  gramatica[20] = "OPRD->id";
  gramatica[21] = "OPRD->num";
  gramatica[22] = "A->COND A";
  gramatica[23] = "COND->CAB CP";
  gramatica[24] = "CAB->se ab_p EXP_R fc_p entao";
  gramatica[25] = "EXP_R->OPRD opr OPRD";
  gramatica[26] = "CP->ES CP";
  gramatica[27] = "CP->CMD CP";
  gramatica[28] = "CP->COND CP";
  gramatica[29] = "CP->fimse";
  gramatica[30] = "A->R A";
  gramatica[31] = "R->CABR CPR";
  gramatica[32] = "CABR->repita ab_p EXP_R fc_p";
  gramatica[33] = "CPR->ES CPR";
  gramatica[34] = "CPR->CMD CPR";
  gramatica[35] = "CPR->COND CPR";
  gramatica[36] = "CPR->fimrepita";
  gramatica[37] = "A->fim";
}

void preencherGOTO(ifstream &file, string matriz[76][20]) {
  string line;
  string valor;
  for (int linha = 0; linha < 76; linha++) {

    getline(file, line);
    if (!file.good())
      break;
    stringstream iss(line);
    for (int coluna = 0; coluna < 20; coluna++) {
      getline(iss, valor, ',');
      if (!iss.good())
        break;
      stringstream conversor(valor);
      conversor >> matriz[linha][coluna];
    }
  }
}

void preencherACTION(ifstream &file, string matriz[76][24]) {
  string line;
  string valor;
  for (int linha = 0; linha < 76; linha++) {

    getline(file, line);
    if (!file.good())
      break;
    stringstream iss(line);
    for (int coluna = 0; coluna < 24; coluna++) {
      getline(iss, valor, ',');
      if (!iss.good())
        break;
      stringstream conversor(valor);
      conversor >> matriz[linha][coluna];
    }
  }
}

stack<int> pilha;

int colunaDaClasseEmACTION(TOKEN a) {
  int classe;

  for (int i = 0; i < 24; i++) {
    if (a.classe == ACTION[0][i]) {
      classe = i;
      break;
    }
  }

  return classe;
}

int obterT(string palavra) {
  int t;
  string string_t;

  // pegando o numero em string
  for (int i = 0; i < palavra.length(); i++) {
    if (isdigit(palavra[i]))
      string_t += palavra[i];
  }

  // transformando o numero em inteiro
  stringstream ss;
  ss << string_t;
  ss >> t;

  return t;
}

int cardinalidadeDeB(string regra) {
  int cardinalidade = 0;

  stringstream stream(regra);
  string palavra;

  while (stream >> palavra)
    ++cardinalidade;

  return cardinalidade;
}

void desempilhaCardinalidadeB(int cardinalidade) {
  for (int i = 0; i < cardinalidade; i++)
    pilha.pop();
}

string encontraA(string regra) {
  string A;

  string::size_type posicao = regra.find('-');
  if (posicao != std::string::npos)
    A = regra.substr(0, posicao);

  return A;
}

int colunaDeAemGOTO(string A) {
  int posicaoDeA;

  for (int i = 0; i < 20; i++) {
    if (A == GOTO[0][i]) {
      posicaoDeA = i;
      break;
    }
  }

  return posicaoDeA;
}

void print_stack(stack<int> c) {
    cout<< "pilha";
  while (!c.empty()) {
        std::cout<<c.top()<< ",";
        c.pop();
    }
  cout<<"\n";
}

void sincTokens(int s){
  string allFollows, word;
  for(int i = 0; i < 19; i++){
    if(!(GOTO[s + 1][i].empty())){
      allFollows = allFollows + FOLLOW[i] + ", ";
    }
  }
  istringstream iss(allFollows);
  while(iss >> word){
    if(!(tokensToSinc.find(word) != std::string::npos)){
      tokensToSinc = tokensToSinc + word;
    }
  }
}

/*==============================ANALISADOR *
 * SINTATICO===============================*/

/*==============================ANALISADOR *
 * SEMANTICO===============================*/

void tokensParaSemantico(){
  TOKEN TIPO;
  TOKEN D;
  TOKEN 
}

/*==============================ANALISADOR *
 * SEMANTICO===============================*/

int main() {
  ifstream inFile;
  TOKEN a;
  ifstream gotoFile("GOTO.csv");
  ifstream actionFile("ACTION.csv");
  int s, t, b, posicaoDeA, intGoto, cardinalidade;
  int classe;
  string shift, string_t = "", reduce, reduce_b = "", regra, A, gotoResultado;

  iniciaTabelaDeSimbolos(tabelaDeSimbolos);
  
  iniciarFollow();
  preencherGOTO(gotoFile, GOTO);
  preencherACTION(actionFile, ACTION);
  preencherGramatica(regrasGramatica);

  pilha.push(0);

  inFile.open("Fonte.alg", ios::in);
  if (!inFile) {
    cout << "Arquivo Fonte.alg nao pode ser aberto" << endl;
    abort();
  }

  a = SCANNER(inFile);

  while (1) {
    
    if (a.classe == "ERRO") {
      cout << "ERRO LÉXICO - Caractere " << a.lexema
         << " inválido na linguagem, linha " << linePos << ", coluna "
         << readPos << endl;
      a = SCANNER(inFile);
      
    }
    loop2:
    if (a.classe != "comentario" || a.classe != "ERRO") {
      // estado
      s = pilha.top();
    
      // procura qual coluna a.classe está em action
      classe = colunaDaClasseEmACTION(a);
     
      // consulta na tabela action o estado e a classe
      if (ACTION[s + 1][classe].find("S") != std::string::npos) {
        shift = ACTION[s + 1][classe];
        // pegando o numero do shift
        t = obterT(shift);
       
        // empilhando t na pilha
        pilha.push(t);
        
        // proxima entrada de simbolo
        a = SCANNER(inFile);

        
      } else if (ACTION[s + 1][classe].find("R") != std::string::npos) {
        reduce = ACTION[s + 1][classe];
     
        // pegando o numero do reduce
        b = obterT(reduce);
       
        // recebendo a regra da redução
        regra = regrasGramatica[b-1];
       
        // contando cardinalidade de B
        cardinalidade = cardinalidadeDeB(regra);
      
        // desempilhando a cardinalidade de B da pilha
        desempilhaCardinalidadeB(cardinalidade);
        
        // t recebe o topo da pilha
        t = pilha.top();
       
        // encontrando a palavra A na regra A->B
        A = encontraA(regra);
       
        // encontrando a posicao de A em GOTO
        posicaoDeA = colunaDeAemGOTO(A);
    
        // Pego o estado dentro do GOTO[t,A]
        gotoResultado = GOTO[t + 1][posicaoDeA];
       
        // transformando em int
        stringstream gg;
        gg << gotoResultado;
        gg >> intGoto;
        
        // empilho o GOTO[t,A] na pilha
        pilha.push(intGoto);
        
        cout << regra << endl;
        
        cout << b << endl;

        switch (b) {
          case 5:
            break;
          case 6:
            break;
          case 7:
            break;
          case 8:
            break;
          case 9:
            break;
          case 10:
            break;
          case 11:
            break;
          case 12:
            break;
          case 13:
            break;
          case 14:
            break;
          case 15:
            break;
          case 16:
            break;
          case 17:
            break;
          case 18:
            break;
          case 19:
            break;
          case 20:
            break;
          case 21:
            break;
          case 22:
            break;
          case 23:
            break;
          case 24:
            break;
          case 25:
            break;
          case 26:
            break;
          case 32:
            break;
          case 33:
            break;
          case 34:
            break;
          case 35:
            break;
          case 36:
            break;
          case 37:
            break;
        }

      } else if (ACTION[s + 1][classe] == "Acc") {
        break;
      } else {
        cout << "ERRO SINTATICO - Linha " << linePos << ", Coluna " << readPos << endl;
        
        while(!(pilha.empty())){
          int i = 0;

          s = pilha.top();
         
          while(i < 19){
    
            if(!(GOTO[s + 1][i].empty())){
              //pega o estado s que tem estados valido em goto e junta os follows possiveis
              sincTokens(s);
              
              while (1) {
                
                a = SCANNER(inFile);


                if(FOLLOW[i].find(a.classe) != std::string::npos){
                  gotoResultado = GOTO[s+1][i];
                  stringstream gg;
                  gg << gotoResultado;
                  gg >> intGoto;
                  pilha.push(intGoto);
                  cout << "Espera-se os seguintes tokens: " << tokensToSinc << endl;
                  goto loop2;
                }
              }
            }
            i++;
          }
        pilha.pop();
        }
      }
    }
  }


  return 0;
  
}