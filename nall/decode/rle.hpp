#pragma once

namespace nall::Decode {

template<uint S = 1, uint M = 4 / S>  //S = word size; M = match length
inline auto RLE(array_view<uint8_t> input) -> vector<uint8_t> {
  vector<uint8_t> output;

  auto load = [&]() -> uint8_t {
    return input ? *input++ : 0;
  };

  uint base = 0;
  uint64_t size = 0;
  /**
   * XXX:
   *  kaishuu0123:
   *    Fixed a bug that prevented Load State from working with Apple Silicon?
   *    Not sure if it is a compiler bug or depends on the CPU architecture it runs on.
   */
  // for(uint byte : range(8)) size |= load() << byte * 8;
  for(uint byte : range(8)) {
    auto v = load();
    auto res = ((uint64_t)v << (byte * 8));
    size = size | res;
  }
  output.resize(size);

  auto read = [&]() -> uint64_t {
    uint64_t value = 0;
    for(uint byte : range(S)) value |= load() << byte * 8;
    return value;
  };

  auto write = [&](uint64_t value) -> void {
    if(base >= size) return;
    for(uint byte : range(S)) output[base++] = value >> byte * 8;
  };

  while(base < size) {
    auto byte = load();
    if(byte < 128) {
      byte++;
      while(byte--) write(read());
    } else {
      auto value = read();
      byte = (byte & 127) + M;
      while(byte--) write(value);
    }
  }

  return output;
}

}
