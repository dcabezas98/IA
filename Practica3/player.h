#ifndef PLAYER_H
#define PLAYER_H

#include "environment.h"

class Player{
public:
  Player(int jug);
  Environment::ActionType Think();
  void Perceive(const Environment &env);
private:
  
  int jugador_;
  Environment actual_;

  long int Poda_AlphaBeta(const Environment& state, int depth, long int alpha, long int beta);
};
#endif
