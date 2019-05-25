#ifndef PLAYER_H
#define PLAYER_H

#include "environment.h"

class Player{
public:
  Player(int jug);
  Environment::ActionType Think();
  void Perceive(const Environment &env);

  static const int PESO3L = 20,
    PESO2L = 5,
    PESOFICHA = 10,
    PESOBOMBA = 3,
    PESOBAJO = 50,
    PESODESPLAZAR = 3,
    PESOELIMINAR = 4; // Para dar importancia a explotar la bomba para que aparezca la siguiente.
  
private:
  
  int jugador_;
  Environment actual_;

  long int Poda_AlphaBeta(const Environment& state, int depth, long int alpha, long int beta);
};
#endif
