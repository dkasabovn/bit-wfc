#include "wfc.h"
#include <cstdint>
#include <iostream>
#include <random>

using namespace std;

int main(int argc, char** argv) {
  // 97
  srand(time(NULL));
  uint64_t *adj = new uint64_t[3];

  // land 1
  // sea 2
  // beach 4
  adj[0] = 0b101;
  adj[1] = 0b110;
  adj[2] = 0b111;

  WFC solver(atoi(argv[1]), atoi(argv[2]), 3, adj);

  solver.iter();

  solver.display();

  return 0;
}
