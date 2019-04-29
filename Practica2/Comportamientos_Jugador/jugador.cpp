#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>
#include <queue>
#include <algorithm>

// David Cabezas Berrido


// Este es el método principal que debe contener los 4 Comportamientos_Jugador
// que se piden en la práctica. Tiene como entrada la información de los
// sensores y devuelve la acción a realizar.
Action ComportamientoJugador::think(Sensores sensores) {
  
  Action sigAccion = actIDLE;
  
  if (sensores.nivel != 4) {    
    if (sensores.mensajeF != -1 and !hayPlan) {
      fil = sensores.mensajeF;
      col = sensores.mensajeC;
      brujula = 0; // 0 corresponde con Norte

      actual.fila = fil;
      actual.columna = col;
      actual.orientacion = brujula;

      destino.fila = sensores.destinoF;
      destino.columna = sensores.destinoC;

      hayPlan=pathFinding (sensores.nivel, actual, destino, mapaResultado, plan);
    }

    switch(ultimaAccion) {
    case actTURN_R: brujula = (brujula+1)%4; break;
    case actTURN_L: brujula = (brujula+3)%4; break;
    case actFORWARD:
      switch(brujula){
      case 0: fil--; break;
      case 1: col++; break;
      case 2: fil++; break;
      case 3: col--; break;
      }
      break;
    }

    if(sensores.destinoF != destino.fila or sensores.destinoC != destino.columna){
      destino.fila = sensores.destinoF;
      destino.columna = sensores.destinoC;
      hayPlan = false;
    }

    if(!hayPlan){
      actual.fila = fil;
      actual.columna = col;
      actual.orientacion = brujula;
      hayPlan = pathFinding(sensores.nivel, actual, destino, mapaResultado, plan);
    }

    if(hayPlan and plan.size()>0){
      sigAccion = plan.front();
      plan.erase(plan.begin());
    }
    else if(sensores.terreno[2]=='P' or sensores.terreno[2]=='M' or sensores.terreno[2]=='D' or sensores.superficie[2]=='a')
      sigAccion = actTURN_R;

    else sigAccion = actFORWARD;

    ultimaAccion = sigAccion;  
  }

  // Nivel 2
  else {    // Comportamiento Reactivo

    switch(ultimaAccion) {
    case actTURN_R: brujula = (brujula+1)%4; break;
    case actTURN_L: brujula = (brujula+3)%4; break;
    case actFORWARD:
      switch(brujula){
      case 0: fil--; break;
      case 1: col++; break;
      case 2: fil++; break;
      case 3: col--; break;
      }
      break;
    }

    if(sensores.mensajeF != -1 and !ubicado){ // Primera casilla amarilla
      
      ubicado = true;
      
      // Copio el mapa que llevaba explorado
      for(int i = 0; i < mapaResultado.size(); i++)
	for(int j = 0; j < mapaResultado.size(); j++)
	  mapaResultado[i][j] = mapa2[i+fil-sensores.mensajeF][j+col-sensores.mensajeC];

      fil = sensores.mensajeF;
      col = sensores.mensajeC;

      actual.fila = fil;
      actual.columna = col;
      actual.orientacion = brujula;

      destino.fila = sensores.destinoF;
      destino.columna = sensores.destinoC;

      hayPlan = hayPlan=pathFinding (sensores.nivel, actual, destino, mapaResultado, plan);
    }

    int k;
    bool horizontal = brujula%2;
    int incrementa = -1 + (int)(brujula<2)*2;
    vector<vector<unsigned char>> *m;
    m = ubicado ? &mapaResultado : &mapa2;

    (*m)[fil][col] = sensores.terreno[0];
    
    switch(horizontal){
       
    case 0:
      for(k = 1; k < 4; k++)
	(*m)[fil-incrementa][col+incrementa*(k-2)] = sensores.terreno[k];
      for(; k < 9; k++)
	(*m)[fil-incrementa*2][col+incrementa*(k-6)] = sensores.terreno[k];
      for(; k < 16; k++)
	(*m)[fil-incrementa*3][col+incrementa*(k-12)] = sensores.terreno[k];
      break;
	
    case 1:
      for(k = 1; k < 4; k++)
	(*m)[fil+incrementa*(k-2)][col+incrementa] = sensores.terreno[k];
      for(; k < 9; k++)
	(*m)[fil+incrementa*(k-6)][col+incrementa*2] = sensores.terreno[k];
      for(; k < 16; k++)
	(*m)[fil+incrementa*(k-12)][col+incrementa*3] = sensores.terreno[k];
      break;     
    }

    // for (int p = 0; p < (*m).size(); p++){
    //   for (int q = 0; q < (*m)[p].size(); q++)
    // 	cout << (*m)[p][q] << " ";
    //   cout << endl;
    // }

    if(!ubicado and !hayPlan){ // Buscar casilla de referencia
      vector<unsigned char>::iterator it = find(sensores.terreno.begin(), sensores.terreno.end(), 'K');
      if(it != sensores.terreno.end()) {
	int pos = distance(sensores.terreno.begin(), it);
	int fK, cK;

	switch(horizontal) { // Posición de la casilla de referencia

	case 0:
	  if(0 < pos and pos < 4){
	    fK = fil-incrementa;  cK = col+incrementa*(pos-2);
	  }
	  else if(4 <= pos and pos < 9){
	    fK = fil-incrementa*2;  cK = col+incrementa*(pos-6);
	  }
	  else { //if(9 <= pos and pos < 16){
	    fK = fil-incrementa*3;  cK = col+incrementa*(pos-12);
	  }
	  break;

	case 1:
	  if(0 < pos and pos < 4){
	    fK=fil+incrementa*(pos-2);  cK = col+incrementa;
	  }
	  else if(4 <= pos and pos < 9){
	    fK=fil+incrementa*(pos-6);  cK = col+incrementa*2;
	  }
	  else { //if(9 <= pos and pos < 16){
	    fK = fil+incrementa*(pos-12); cK = col+incrementa*3;
	  }
	  break;
	}

	actual.fila = fil;
	actual.columna = col;
	actual.orientacion = brujula;

	estado referencia = {fK, cK, -1};
	  
	hayPlan = pathFinding_Nivel2(actual, referencia, mapa2, plan);	
      }
    }

    if(ubicado){
      if(sensores.destinoF != destino.fila or sensores.destinoC != destino.columna){
	destino.fila = sensores.destinoF;
	destino.columna = sensores.destinoC;
	hayPlan = false;
      }

      if(!hayPlan){
	actual.fila = fil;
	actual.columna = col;
	actual.orientacion = brujula;
	hayPlan = pathFinding(sensores.nivel, actual, destino, mapaResultado, plan);
      }
    }

    if(hayPlan and plan.size()>0){
      sigAccion = plan.front();

      if(sigAccion == actFORWARD){

	if(sensores.terreno[2]=='P' or sensores.terreno[2]=='M' or sensores.terreno[2]=='D'){ // El plan ha fallado
	  hayPlan = false;
	  sigAccion = actIDLE;
	}
	
	else if(sensores.superficie[2]=='a'){
	  sigAccion = actIDLE;
	}
	
	else plan.erase(plan.begin());
      }
	
      else plan.erase(plan.begin());
    }
    else{ // No hay plan
      int cual = aleatorio(100);
      
      if(sensores.terreno[2]=='P' or sensores.terreno[2]=='M' or sensores.terreno[2]=='D' or sensores.superficie[2]=='a')
	sigAccion = cual > 6 ? actTURN_R : actTURN_L;

      else{
	if(cual < 3) sigAccion = actTURN_L;
	else if (cual < 8) sigAccion = actTURN_R;
	else sigAccion = actFORWARD;
      }	  
    }
    
    ultimaAccion = sigAccion;
  }
  
  return sigAccion;
}


// Llama al algoritmo de busqueda que se usará en cada comportamiento del agente
// Level representa el comportamiento en el que fue iniciado el agente.
bool ComportamientoJugador::pathFinding (int level, const estado &origen, const estado &destino, const vector<vector<unsigned char>> &mapa, list<Action> &plan){
  switch (level){
  case 1: //cout << "Busqueda en Profundad\n";
    return pathFinding_Profundidad(origen,destino,plan);
    break;
  case 2: //cout << "Busqueda en Anchura\n";
    return pathFinding_Anchura(origen,destino,plan);
    break;
  case 3: //cout << "Busqueda Costo Uniforme\n";
    return pathFinding_CostoUniforme(origen, destino, plan);
    break;
  case 4: //cout << "Busqueda para el reto\n";
    return pathFinding_Nivel2(origen, destino, mapa, plan);
    break;
  }
  //cout << "Comportamiento sin implementar\n";
  return false;
}


//---------------------- Implementación de la busqueda en profundidad ---------------------------

// Dado el código en carácter de una casilla del mapa dice si se puede
// pasar por ella sin riegos de morir o chocar.
bool EsObstaculo(unsigned char casilla){
  if (casilla=='P' or casilla=='M' or casilla =='D')
    return true;
  else
    return false;
}


// Comprueba si la casilla que hay delante es un obstaculo. Si es un
// obstaculo devuelve true. Si no es un obstaculo, devuelve false y
// modifica st con la posición de la casilla del avance.
bool ComportamientoJugador::HayObstaculoDelante(estado &st, const vector<vector<unsigned char>> &mapa){
  int fil=st.fila, col=st.columna;

  // calculo cual es la casilla de delante del agente
  switch (st.orientacion) {
  case 0: fil--; break;
  case 1: col++; break;
  case 2: fil++; break;
  case 3: col--; break;
  }

  // Compruebo que no me salgo fuera del rango del mapa
  if (fil<0 or fil>=mapa.size()) return true;
  if (col<0 or col>=mapa[0].size()) return true;

  // Miro si en esa casilla hay un obstaculo infranqueable
  if (!EsObstaculo(mapa[fil][col])){
    // No hay obstaculo, actualizo el parámetro st poniendo la casilla de delante.
    st.fila = fil;
    st.columna = col;
    return false;
  }
  else{
    return true;
  }
}


struct nodo{
  estado st;
  list<Action> secuencia;
  int coste=0;

  int totalCost(int destinyRow, int destinyColumn)
  {
    int distancia = abs(st.fila-destinyRow)+abs(st.columna-destinyColumn);

    return distancia*coste;
    //return coste;
  }
};


struct ComparaEstados{
  bool operator()(const estado &a, const estado &n) const{
    if ((a.fila > n.fila) or (a.fila == n.fila and a.columna > n.columna) or
	(a.fila == n.fila and a.columna == n.columna and a.orientacion > n.orientacion))
      return true;
    else
      return false;
  }
};


// Implementación de la búsqueda en profundidad.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan) {
  //Borro la lista
  //cout << "Calculando plan\n";
  plan.clear();
  set<estado,ComparaEstados> generados; // Lista de Cerrados
  stack<nodo> pila;											// Lista de Abiertos

  nodo current;
  current.st = origen;
  current.secuencia.empty();

  pila.push(current);

  while (!pila.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){

    pila.pop();
    generados.insert(current.st);

    // Generar descendiente de girar a la derecha
    nodo hijoTurnR = current;
    hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
    if (generados.find(hijoTurnR.st) == generados.end()){
      hijoTurnR.secuencia.push_back(actTURN_R);
      pila.push(hijoTurnR);
    }

    // Generar descendiente de girar a la izquierda
    nodo hijoTurnL = current;
    hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
    if (generados.find(hijoTurnL.st) == generados.end()){
      hijoTurnL.secuencia.push_back(actTURN_L);
      pila.push(hijoTurnL);
    }

    // Generar descendiente de avanzar
    nodo hijoForward = current;
    if (!HayObstaculoDelante(hijoForward.st)){
      if (generados.find(hijoForward.st) == generados.end()){
	hijoForward.secuencia.push_back(actFORWARD);
	pila.push(hijoForward);
      }
    }

    // Tomo el siguiente valor de la pila
    if (!pila.empty()){
      current = pila.top();
    }
  }

  //cout << "Terminada la busqueda\n";
  
  if (current.st.fila == destino.fila and current.st.columna == destino.columna){
    //cout << "Cargando el plan\n";
    plan = current.secuencia;
    //cout << "Longitud del plan: " << plan.size() << endl;
    //PintaPlan(plan);
    // ver el plan en el mapa
    VisualizaPlan(origen, plan);
    return true;
  }
  else {
    //cout << "No encontrado plan\n";
  }


  return false;
}


// Implementación de la búsqueda en anchura.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan) {
  //Borro la lista
  //cout << "Calculando plan\n";
  plan.clear();
  set<estado,ComparaEstados> generados; // Lista de Cerrados
  queue<nodo> pila;											// Lista de Abiertos
  
  nodo current;
  current.st = origen;
  current.secuencia.empty();

  pila.push(current);

  while (!pila.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){

    pila.pop();
    generados.insert(current.st);

    // Generar descendiente de girar a la derecha
    nodo hijoTurnR = current;
    hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
    if (generados.find(hijoTurnR.st) == generados.end()){
      hijoTurnR.secuencia.push_back(actTURN_R);
      pila.push(hijoTurnR);
    }

    // Generar descendiente de girar a la izquierda
    nodo hijoTurnL = current;
    hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
    if (generados.find(hijoTurnL.st) == generados.end()){
      hijoTurnL.secuencia.push_back(actTURN_L);
      pila.push(hijoTurnL);
    }

    // Generar descendiente de avanzar
    nodo hijoForward = current;
    if (!HayObstaculoDelante(hijoForward.st)){
      if (generados.find(hijoForward.st) == generados.end()){
	hijoForward.secuencia.push_back(actFORWARD);
	pila.push(hijoForward);
      }
    }

    // Tomo el siguiente valor de la pila
    if (!pila.empty()){
      current = pila.front();
    }
  }

  //cout << "Terminada la busqueda\n";
  //cout << pila.empty() << endl;
  if (current.st.fila == destino.fila and current.st.columna == destino.columna){
    //cout << "Cargando el plan\n";
    plan = current.secuencia;
    //cout << "Longitud del plan: " << plan.size() << endl;
    //PintaPlan(plan);
    // ver el plan en el mapa
    VisualizaPlan(origen, plan);
    return true;
  }
  else {
    //cout << "No encontrado plan\n";
  }


  return false;
}


int ComportamientoJugador::costo(char casilla){

  switch(casilla){
  case 'T': return SandCost;
    break;
  case 'A': return WaterCost;
    break;
  case 'B': return ForestCost;
    break;
  case '?': return UnknownCost;
    break;
  default:
    return 1;
    break;
  }
}


// Implementación de la búsqueda en costo uniforme.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_CostoUniforme(const estado &origen, const estado &destino, list<Action> &plan) {
  //Borro la lista
  //cout << "Calculando plan\n";
  plan.clear();
  
  set<estado,ComparaEstados> generados; // Lista de Cerrados
  list<nodo> pila;											// Lista de Abiertos
  
  nodo current;
  current.st = origen;
  current.secuencia.empty();
  
  list<nodo>::iterator it, best = pila.begin();
  int bestCost, c;

  pila.push_front(current);

  do{    
    generados.insert(current.st);

    // Generar descendiente de girar a la derecha
    nodo hijoTurnR = current;
    hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
    if (generados.find(hijoTurnR.st) == generados.end()){
      hijoTurnR.secuencia.push_back(actTURN_R);
      hijoTurnR.coste+=costo(mapaResultado[hijoTurnR.st.fila][hijoTurnR.st.columna]);
      pila.push_front(hijoTurnR);
    }

    // Generar descendiente de girar a la izquierda
    nodo hijoTurnL = current;
    hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
    if (generados.find(hijoTurnL.st) == generados.end()){
      hijoTurnL.secuencia.push_back(actTURN_L);
      hijoTurnL.coste+=costo(mapaResultado[hijoTurnL.st.fila][hijoTurnL.st.columna]);
      pila.push_front(hijoTurnL);
    }

    // Generar descendiente de avanzar
    nodo hijoForward = current;
    if (!HayObstaculoDelante(hijoForward.st)){
      if (generados.find(hijoForward.st) == generados.end()){
	hijoForward.secuencia.push_back(actFORWARD);
	hijoForward.coste+=costo(mapaResultado[hijoForward.st.fila][hijoForward.st.columna]);
	pila.push_front(hijoForward);
      }
    }

    // Tomo el siguiente valor de la pila
    if (!pila.empty()){
      best = pila.begin();
      bestCost = current.totalCost(destino.fila, destino.columna);
      for(it = pila.begin(); it != pila.end(); it++){
	c = (*it).totalCost(destino.fila, destino.columna);
	if(c<bestCost){
	  best = it;
	  bestCost = c;
	}
      }
    }
    current=*best;
    pila.erase(best);
    
  }while (!pila.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna));

  //cout << "Terminada la busqueda\n";
  
  if (current.st.fila == destino.fila and current.st.columna == destino.columna){
    //cout << "Cargando el plan\n";
    plan = current.secuencia;
    //cout << "Longitud del plan: " << plan.size() << endl;
    //PintaPlan(plan);
    // ver el plan en el mapa
    VisualizaPlan(origen, plan);
    return true;
  }
  else {
    //cout << "No encontrado plan\n";
  }


  return false;
}

// Búsqueda para nivel 2
bool ComportamientoJugador::pathFinding_Nivel2(const estado &origen, const estado &destino, const vector<vector<unsigned char >> &mapa, list<Action> &plan) {

  //Borro la lista
  //cout << "Calculando plan\n";
  plan.clear();
  
  set<estado,ComparaEstados> generados; // Lista de Cerrados
  list<nodo> pila;											// Lista de Abiertos
  
  nodo current;
  current.st = origen;
  current.secuencia.empty();
  
  list<nodo>::iterator it, best = pila.begin();
  int bestCost, c;

  pila.push_front(current);

  do{    
    generados.insert(current.st);

    // Generar descendiente de girar a la derecha
    nodo hijoTurnR = current;
    hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
    if (generados.find(hijoTurnR.st) == generados.end()){
      hijoTurnR.secuencia.push_back(actTURN_R);
      hijoTurnR.coste+=costo(mapa[hijoTurnR.st.fila][hijoTurnR.st.columna]);
      pila.push_front(hijoTurnR);
    }

    // Generar descendiente de girar a la izquierda
    nodo hijoTurnL = current;
    hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
    if (generados.find(hijoTurnL.st) == generados.end()){
      hijoTurnL.secuencia.push_back(actTURN_L);
      hijoTurnL.coste+=costo(mapa[hijoTurnL.st.fila][hijoTurnL.st.columna]);
      pila.push_front(hijoTurnL);
    }

    // Generar descendiente de avanzar
    nodo hijoForward = current;
    if (!HayObstaculoDelante(hijoForward.st, mapa)){
      if (generados.find(hijoForward.st) == generados.end()){
	hijoForward.secuencia.push_back(actFORWARD);
	hijoForward.coste+=costo(mapa[hijoForward.st.fila][hijoForward.st.columna]);
	pila.push_front(hijoForward);
      }
    }

    // Tomo el siguiente valor de la pila
    if (!pila.empty()){
      best = pila.begin();
      bestCost = current.totalCost(destino.fila, destino.columna);
      for(it = pila.begin(); it != pila.end(); it++){
	c = (*it).totalCost(destino.fila, destino.columna);
	if(c<bestCost){
	  best = it;
	  bestCost = c;
	}
      }
    }
    current=*best;
    pila.erase(best);
    
  }while (!pila.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna));

  //cout << "Terminada la busqueda\n";
  
  if (current.st.fila == destino.fila and current.st.columna == destino.columna){
    //cout << "Cargando el plan\n";
    plan = current.secuencia;
    //cout << "Longitud del plan: " << plan.size() << endl;
    PintaPlan(plan);
    // ver el plan en el mapa
    if(mapa==mapaResultado) VisualizaPlan(origen, plan);
    return true;
  }
  else {
    //cout << "No encontrado plan\n";
  }


  return false;
}




// Sacar por la términal la secuencia del plan obtenido
void ComportamientoJugador::PintaPlan(list<Action> plan) {
  auto it = plan.begin();
  while (it!=plan.end()){
    if (*it == actFORWARD){
      cout << "A ";
    }
    else if (*it == actTURN_R){
      cout << "D ";
    }
    else if (*it == actTURN_L){
      cout << "I ";
    }
    else {
      cout << "- ";
    }
    it++;
  }
  cout << endl;
}



void AnularMatriz(vector<vector<unsigned char> > &m){
  for (int i=0; i<m[0].size(); i++){
    for (int j=0; j<m.size(); j++){
      m[i][j]=0;
    }
  }
}


// Pinta sobre el mapa del juego el plan obtenido
void ComportamientoJugador::VisualizaPlan(const estado &st, const list<Action> &plan){
  AnularMatriz(mapaConPlan);
  estado cst = st;

  auto it = plan.begin();
  while (it!=plan.end()){
    if (*it == actFORWARD){
      switch (cst.orientacion) {
      case 0: cst.fila--; break;
      case 1: cst.columna++; break;
      case 2: cst.fila++; break;
      case 3: cst.columna--; break;
      }
      mapaConPlan[cst.fila][cst.columna]=1;
    }
    else if (*it == actTURN_R){
      cst.orientacion = (cst.orientacion+1)%4;
    }
    else {
      cst.orientacion = (cst.orientacion+3)%4;
    }
    it++;
  }
}



int ComportamientoJugador::interact(Action accion, int valor){
  return false;
}
