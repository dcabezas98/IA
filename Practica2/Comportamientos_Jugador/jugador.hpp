#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>
#include <vector>

struct estado {
  int fila;
  int columna;
  int orientacion;
};


class ComportamientoJugador : public Comportamiento {
public:
  ComportamientoJugador(unsigned int size) : Comportamiento(size), mapa2(2*size+4, vector<unsigned char>(2*size+4, 'O')) {
    // Inicializar Variables de Estado
    fil = col = size-1;
    brujula = 0; // 0: Norte, 1:Este, 2:Sur, 3:Oeste
    destino.fila = -1;
    destino.columna = -1;
    destino.orientacion = -1;
    ultimaAccion = actIDLE;
    hayPlan=false;

    ubicado = false;

    
  }
  ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR), mapa2(2*mapaR.size()+4, vector<unsigned char>(2*mapaR.size()+4, 'O')) {
    // Inicializar Variables de Estado
    fil = col = mapaR.size()-1;
    brujula = 0; // 0: Norte, 1:Este, 2:Sur, 3:Oeste
    destino.fila = -1;
    destino.columna = -1;
    destino.orientacion = -1;
    ultimaAccion = actIDLE;
    hayPlan=false;

    ubicado = false;
  }
  ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
  ~ComportamientoJugador(){}

  Action think(Sensores sensores);
  int interact(Action accion, int valor);
  void VisualizaPlan(const estado &st, const list<Action> &plan);
  ComportamientoJugador * clone(){return new ComportamientoJugador(*this);}

private:

  // Constantes para indicar la penalización del terreno
  const int SandCost = 2, // 2
    WaterCost = 10, // 10 
    ForestCost = 5, // 5
    UnknownCost = 20;
  
  // Declarar Variables de Estado
  int fil, col, brujula;
  estado actual, destino;
  list<Action> plan;

  // Nuevas variables de Estado
  Action ultimaAccion;
  bool hayPlan;

  // Variables para el comportamiento reactivo
  bool ubicado;
  vector<vector<unsigned char>> mapa2;

  // Métodos privados de la clase
  bool pathFinding(int level, const estado &origen, const estado &destino, const vector<vector<unsigned char>> &mapa, list<Action> &plan);
  bool pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan);
  bool pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan);  
  bool pathFinding_CostoUniforme(const estado &origen, const estado &destino, list<Action> &plan);
  bool pathFinding_Nivel2(const estado &origen, const estado &destino, const vector<vector<unsigned char>> &mapa, list<Action> &plan);

  int costo(char casilla);

  void PintaPlan(list<Action> plan);
  bool HayObstaculoDelante(estado &st, const vector<vector<unsigned char>> &mapa);
  bool HayObstaculoDelante(estado &st){
    return HayObstaculoDelante(st, mapaResultado);
  }

};

#endif
