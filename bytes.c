#include <stdint.h>

uint16_t swap_endian(uint16_t val) {
  uint16_t data = (val >> 8) & 0xFF;
  data |= (val & 0xFF) << 8;
  return data;
}
