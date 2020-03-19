#include "mmnger_phys.h"


inline void mmap_set (int bit) {
  _mmngr_memory_map[bit / 32] |= (1 << (bit % 32));
}

inline void mmap_unset (int bit) {
  _mmngr_memory_map[bit / 32] &= ~ (1 << (bit % 32));
}

inline char mmap_test (int bit) {
 
 return _mmngr_memory_map[bit / 32] &  (1 << (bit % 32));
}