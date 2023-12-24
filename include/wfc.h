#pragma once

#include <cstdint>
#include <vector>
#include <random>

using namespace std;

static inline uint8_t uniform_rand(uint64_t a, uint64_t b) {
	return rand()%(b-a) + a;
}

static inline uint64_t parent(uint64_t idx) {
	return (idx - 1) / 2;
}

class Cell {
	public:
		uint64_t entropy;
		Cell(uint8_t tiles);
		inline bool collapsed() {
			return this->hamming() == 1;
		}
		inline uint8_t hamming() {
			return __builtin_popcountll(this->entropy);
		}
		uint8_t select_rand();
		bool operator>(const Cell& other) const {
			return __builtin_popcountll(this->entropy) > __builtin_popcountll(other.entropy);
		}
};

class WFC {
	private:
		vector<Cell> cells;
		vector<Cell*> propagate_set; // TODO
		uint64_t m, n;
		uint64_t* possible_adj;
		uint64_t* constraint_adj; // TODO
		uint8_t tiles;
		inline uint32_t pair_to_idx(uint32_t x, uint32_t y) {
			return x * (m) + y;
		}
	public:
		WFC(uint64_t m, uint64_t n, uint8_t tiles, const uint64_t* adj);
		~WFC();
		void iter();
		bool iter_once();
		void display();
};


