#include "wfc.h"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <random>
#include <queue>
#include <utility>
#include <iostream>

#define DEBUG 0

using namespace std;

Cell::Cell(uint8_t tiles) {
	this->entropy = (1 << tiles) - 1;
}

uint8_t Cell::select_rand() {
	uint8_t hamming = __builtin_popcountll(this->entropy);
	uint8_t bit_to_select = uniform_rand(0, hamming);

	for (uint8_t i = 0; i < 64; i++) {
		if (bit_to_select == 0 && this->entropy & 1 << i) {
			this->entropy = 1 << i;
			return i;
		}
		
		if (this->entropy & (1 << i)) {
			bit_to_select--;
		}
	}
	return 65;
}

WFC::WFC(uint64_t m, uint64_t n, uint8_t tiles, const uint64_t* adj) : m(m), n(n), tiles(tiles) {
	this->possible_adj = new uint64_t[tiles];
	copy(adj, adj + tiles, this->possible_adj);

	this->cells = vector<Cell>(m * n, Cell(tiles));
	this->propagate_set = vector<Cell*>();
}

WFC::~WFC() {
	delete[] this->possible_adj;
}

bool WFC::iter_once() {
	uint32_t mindex = 0;

	for (; mindex < m * n; mindex++) {
		if (cells[mindex].hamming() > 1) break;
	}

	if (mindex == m * n) return false;

	for (uint64_t i = 1; i < m * n; i++) {
		if (cells[mindex] > cells[i] && cells[i].hamming() > 1) {
			mindex = i;
		}
	}

	cells[mindex].select_rand();

	queue<int> q;

	q.push(mindex);


	auto collapse = [&](uint32_t nidx, uint32_t idx) {
		if (cells[nidx].collapsed()) {
			return false;
		}
		uint64_t adj_possible = 0;
		for (uint8_t i = 0; i < 64; i++) {
			if ((1 << i) & cells[idx].entropy) {
				adj_possible |= possible_adj[i];
			}
		}
		uint64_t old_entropy = cells[nidx].entropy;
		cells[nidx].entropy &= adj_possible;
		bool changed = old_entropy != cells[nidx].entropy;
#if DEBUG
		cout << "# " << idx << " , " << nidx << endl;
		cout << cells[idx].entropy << " -> " << cells[nidx].entropy << endl;
#endif
		return changed;
	};

	while (!q.empty()) {
		uint32_t idx = q.front();
		q.pop();

		if (idx >= n && collapse(idx - n, idx)) {
			q.push(idx - n);
		}

		if (idx + n < m * n && collapse(idx + n, idx)) {
			q.push(idx + n);
		}

		if (idx >= 1 && idx / n == (idx - 1) / n && collapse(idx - 1, idx)) {
			q.push(idx - 1);
		}

		if (idx + 1 < m * n && idx / n == (idx + 1) / n && collapse(idx + 1, idx)) {
			q.push(idx + 1);
		}
	}

	return true;
}

void WFC::iter() {

	while (iter_once()) {}

	for (int i = 1; i < m - 1; i++) {
		for (int j = 1; j < n - 1; j++) {
			int mask = 0;
			mask |= cells[pair_to_idx(i-1, j)].entropy;
			mask |= cells[pair_to_idx(i+1, j)].entropy;
			mask |= cells[pair_to_idx(i, j-1)].entropy;
			mask |= cells[pair_to_idx(i, j+1)].entropy;

			int cur = cells[pair_to_idx(i, j)].entropy;

			if (cur == 1 && mask & 2) {
				cout << "ERROR" << endl;
				return;
			}
			if (cur == 2 && mask & 1) {
				cout << "ERROR" << endl;
				return;
			}
		}
	}
}

void WFC::display() {
	auto pretty_entropy = [&](uint64_t ent) {
		cout << ((ent & 1) ? "\033[32m#" : "");
		cout << ((ent & 2) ? "\033[34m^" : "");
		cout << ((ent & 4) ? "\033[33m~" : "");
	};
	cout << "####" << endl;
	for (uint32_t i = 0; i < m; i++) {
		for (uint32_t j = 0; j < n; j++) {
			uint32_t idx = pair_to_idx(i, j);
			pretty_entropy(cells[idx].entropy);
			cout << " ";
		}
		cout << endl;
	}
	cout << "\033[39m####" << endl;
}
