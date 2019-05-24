#include <iostream>
#include <cstdlib>
#include <vector>
#include <queue>
#include <algorithm>
#include "player.h"
#include "environment.h"

using namespace std;

const long int masinf=99999999, menosinf=-99999999;


// Constructor
Player::Player(int jug){
  jugador_=jug;
}

// Actualiza el estado del juego para el jugador
void Player::Perceive(const Environment & env){
  actual_=env;
}

long int Puntuacion(int jugador, const Environment &estado){
  long int suma=0;

  for (int i=0; i<7; i++)
    for (int j=0; j<7; j++){
      if (estado.See_Casilla(i,j)==jugador){
	if (j<3)
	  suma += j;
	else
	  suma += (6-j);
      }
    }

  return suma;
}


// Funcion de valoracion para testear Poda Alfabeta
long int ValoracionTest(const Environment &estado, int jugador){
  int ganador = estado.RevisarTablero();

  if (ganador==jugador)
    return masinf; // Gana el jugador que pide la valoracion
  else if (ganador!=0)
    return menosinf; // Pierde el jugador que pide la valoracion
  else if (estado.Get_Casillas_Libres()==0)
    return 0;  // Hay un empate global y se ha rellenado completamente el tablero
  else
    return Puntuacion(jugador,estado);
}

// ------------------- Los tres metodos anteriores no se pueden modificar

bool EnLinea3(const Environment &estado, int fila, int columna) {
  if (estado.See_Casilla(fila,columna)==0)
    return false;
  
  //En la misma Fila
  if (columna<5){
    if ((estado.See_Casilla(fila,columna)%3)   == (estado.See_Casilla(fila,columna+1)%3) and
	(estado.See_Casilla(fila,columna+1)%3) == (estado.See_Casilla(fila,columna+2)%3)){
      //cout << "En la misma fila\n";
      return true;
    }
  }
  //En la misma columna
  if (fila<5){
    if ((estado.See_Casilla(fila,columna)%3)   == (estado.See_Casilla(fila+1,columna)%3) and
	(estado.See_Casilla(fila+1,columna)%3) == (estado.See_Casilla(fila+2,columna)%3)){
      //cout << "En la misma columna\n";

      return true;
    }
  }
  //En la diagonal hacia arriba
  if (fila<5 and columna<5){
    if ((estado.See_Casilla(fila,columna)%3)     == (estado.See_Casilla(fila+1,columna+1)%3) and
	(estado.See_Casilla(fila+1,columna+1)%3) == (estado.See_Casilla(fila+2,columna+2)%3)){
      //cout << "En la diagonal principal\n";

     return true;
    }
  }

  //En la diagonal hacia abajo
  if (fila>1 and columna<5){
    if ((estado.See_Casilla(fila,columna)%3)   == (estado.See_Casilla(fila-1,columna+1)%3) and
	(estado.See_Casilla(fila-1,columna+1)%3) == (estado.See_Casilla(fila-2,columna+2)%3)){
      //cout << "En la diagonal secundaria\n";

      return true;
    }
  }

  return false;
}


bool EnLinea2(const Environment &estado, int fila, int columna) {
  if (estado.See_Casilla(fila,columna)==0)
    return false;
 
  //En la misma Fila
  if (columna<6){
    if ((estado.See_Casilla(fila,columna)%3)   == (estado.See_Casilla(fila,columna+1)%3)){
      //cout << "En la misma fila\n";
      return true;
    }
  }
  //En la misma columna
  if (fila<6){
    if ((estado.See_Casilla(fila,columna)%3)   == (estado.See_Casilla(fila+1,columna)%3)){
      //cout << "En la misma columna\n";

      return true;
    }
  }
  //En la diagonal hacia arriba
  if (fila<6 and columna<6){
    if ((estado.See_Casilla(fila,columna)%3)  == (estado.See_Casilla(fila+1,columna+1)%3)){
      //cout << "En la diagonal principal\n";

      return true;
    }
  }

  //En la diagonal hacia abajo
  if (fila>0 and columna<6){
    if ((estado.See_Casilla(fila,columna)%3)   == (estado.See_Casilla(fila-1,columna+1)%3)){
      //cout << "En la diagonal secundaria\n";

      return true;
    }
  }

  return false;
}


int NenLinea3(const Environment &estado, int jug){

  int n = 0;

  for(int i = 0; i < 7; i++)
    for(int j = 0; j < 7; j++)
      if(estado.See_Casilla(i,j)%3==jug)
	n+=(int) EnLinea3(estado, i, j);

  return n;
}

int NenLinea2(const Environment &estado, int jug){

  int n = 0;

  for(int i = 0; i < 7; i++)
    for(int j = 0; j < 7; j++)
      if(estado.See_Casilla(i,j)%3==jug)
	n+=(int) EnLinea2(estado, i, j);

  return n;
}

int Nfichas(const Environment &estado, int jug){
  int n = 0;

  for(int i = 0; i < 7; i++)
    for(int j = 0; j < 7; j++)
      if(estado.See_Casilla(i,j)%3==jug)
	n++;

  return n;
}

// Un punto por cada ficha que elimine la bomba y otro por cada ficha enemiga en la fila de arriba
int puntosBomba(const Environment &estado, int jug){

  if(!estado.Have_BOOM(jug)) return 0;

  int fil, col;

  for(int i = 0; i < 7; i++)
    for(int j = 0; j < 7; j++)
      if(estado.See_Casilla(i,j)==jug+3){
	fil = i; col = j;
      }

  int n = 0;

  for(int j = 0; j < 7; j++)
    if(estado.See_Casilla(fil,j)%3==jug)
      n++;

  int otro = jug==1 ? 2: 1;
  
  if(fil < 6)
    for(int j = 0; j < 7; j++)
      if(estado.See_Casilla(fil+1,j)%3==otro)
	n++;

  return n;
}


// Funcion heuristica (ESTA ES LA QUE TENEIS QUE MODIFICAR)
long int Valoracion(const Environment &estado, int jugador){

  int ganador = estado.RevisarTablero();
  int otro = jugador==1 ? 2: 1;

  if (ganador==jugador)
    return masinf; // Gana el jugador que pide la valoracion
  else if (ganador!=0)
    return menosinf; // Pierde el jugador que pide la valoracion
  else if (estado.Get_Casillas_Libres()==0)
    return 0;  // Hay un empate global y se ha rellenado completamente el tablero
  else{
    return (NenLinea3(estado, otro)-NenLinea3(estado, jugador))*9 +
      (NenLinea2(estado, otro)-NenLinea2(estado, jugador))*3 +
      Nfichas(estado, otro)-Nfichas(estado, jugador) +
      (puntosBomba(estado, jugador)-puntosBomba(estado, otro))*6;
  }
}


// Esta funcion no se puede usar en la version entregable
// Aparece aqui solo para ILUSTRAR el comportamiento del juego
// ESTO NO IMPLEMENTA NI MINIMAX, NI PODA ALFABETA
void JuegoAleatorio(bool aplicables[], int opciones[], int &j){
  j=0;
  for (int i=0; i<8; i++){
    if (aplicables[i]){
      opciones[j]=i;
      j++;
    }
  }
}






// Invoca el siguiente movimiento del jugador
Environment::ActionType Player::Think(){
  
  const int PROFUNDIDAD_MINIMAX = 6;  // Umbral maximo de profundidad para el metodo MiniMax
  const int PROFUNDIDAD_ALFABETA = 7; // Umbral maximo de profundidad para la poda Alfa_Beta
  
  Environment::ActionType accion; // acción que se va a devolver
  bool aplicables[8]; // Vector bool usado para obtener las acciones que son aplicables en el estado actual. La interpretacion es
  // aplicables[0]==true si PUT1 es aplicable
  // aplicables[1]==true si PUT2 es aplicable
  // aplicables[2]==true si PUT3 es aplicable
  // aplicables[3]==true si PUT4 es aplicable
  // aplicables[4]==true si PUT5 es aplicable
  // aplicables[5]==true si PUT6 es aplicable
  // aplicables[6]==true si PUT7 es aplicable
  // aplicables[7]==true si BOOM es aplicable

  int n_act; //Acciones posibles en el estado actual


  n_act = actual_.possible_actions(aplicables); // Obtengo las acciones aplicables al estado actual en "aplicables"

  // Muestra por la consola las acciones aplicable para el jugador activo
  //actual_.PintaTablero();
  cout << " Acciones aplicables ";
  (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
  for (int t=0; t<8; t++)
    if (aplicables[t])
      cout << " " << actual_.ActionStr( static_cast< Environment::ActionType > (t)  );
  cout << endl;


  /*
  //--------------------- COMENTAR Desde aqui
  int opciones[10];
  
  cout << "\n\t";
  int n_opciones=0;
  JuegoAleatorio(aplicables, opciones, n_opciones);

  if (n_act==0){
  (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
  cout << " No puede realizar ninguna accion!!!\n";
  //accion = Environment::actIDLE;
  }
  else if (n_act==1){
  (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
  cout << " Solo se puede realizar la accion "
  << actual_.ActionStr( static_cast< Environment::ActionType > (opciones[0])  ) << endl;
  accion = static_cast< Environment::ActionType > (opciones[0]);

  }
  else { // Hay que elegir entre varias posibles acciones
  int aleatorio = rand()%n_opciones;
  cout << " -> " << actual_.ActionStr( static_cast< Environment::ActionType > (opciones[aleatorio])  ) << endl;
  accion = static_cast< Environment::ActionType > (opciones[aleatorio]);
  }

  //--------------------- COMENTAR Hasta aqui
  */
    

  //--------------------- AQUI EMPIEZA LA PARTE A REALIZAR POR EL ALUMNO ------------------------------------------------
  
  Environment next;

  int mejor = -1;

  long int e, mejorE = menosinf;

  int i;
  
  for(int j = 0; j < 8; j++){
    if(aplicables[j]){

      if(mejor == -1) mejor = j;
      
      i = j-1;
      next = actual_.GenerateNextMove(i);
      
      e = Poda_AlphaBeta(next, PROFUNDIDAD_ALFABETA, menosinf, masinf);

      if(e > mejorE){
	mejor = j;
	mejorE = e;
      }
    }
  }

  accion = static_cast< Environment::ActionType > (mejor);

  // Opcion: Poda AlfaBeta
  // NOTA: La parametrizacion es solo orientativa
  // valor = Poda_AlfaBeta(actual_, jugador_, 0, PROFUNDIDAD_ALFABETA, accion, alpha, beta);
  //cout << "Valor MiniMax: " << valor << "  Accion: " << actual_.ActionStr(accion) << endl;

  return accion;
}


long int Player::Poda_AlphaBeta(const Environment& state, int depth, long int alpha, long int beta){
  
  if(depth == 0 or state.JuegoTerminado()){
    return Valoracion(state, jugador_);
  }

  int n;
  long int e;

  Environment* V = new Environment[8];
  
  if(state.JugadorActivo()==jugador_){ // Si el que juega soy yo
    long int maxE = menosinf;
    n = state.GenerateAllMoves(V);
    for(int i = 0; i < n; i++){
      e = Poda_AlphaBeta(V[i], depth-1, alpha, beta);
      maxE = max(maxE, e);
      alpha = max(alpha, e);

      if(beta <= alpha)
	break;
    }
    delete [] V;
    return maxE;
  }
  else{ // Si el que juega es el otro
    long int minE=masinf;
    n = state.GenerateAllMoves(V);

    for(int i = 0; i < n; i++){
      e = Poda_AlphaBeta(V[i], depth-1, alpha, beta);
      minE = min(minE, e);
      beta = min(beta, e);
      
      if(beta <= alpha)
	break;
    }
    delete [] V;
    return minE;
  }
}
