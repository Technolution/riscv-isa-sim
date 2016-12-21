#ifndef _RISCV_DEVICES_H
#define _RISCV_DEVICES_H

#include "decode.h"
#include <map>
#include <vector>

class processor_t;

class abstract_device_t {
 public:
  virtual bool load(reg_t addr, size_t len, uint8_t* bytes) = 0;
  virtual bool store(reg_t addr, size_t len, const uint8_t* bytes) = 0;
  virtual ~abstract_device_t() {}
};

class bus_t : public abstract_device_t {
 public:
  bool load(reg_t addr, size_t len, uint8_t* bytes);
  bool store(reg_t addr, size_t len, const uint8_t* bytes);
  void add_device(reg_t addr, abstract_device_t* dev);

 private:
  std::map<reg_t, abstract_device_t*> devices;
};

class rom_device_t : public abstract_device_t {
 public:
  rom_device_t(std::vector<char> data);
  bool load(reg_t addr, size_t len, uint8_t* bytes);
  bool store(reg_t addr, size_t len, const uint8_t* bytes);
  const std::vector<char>& contents() { return data; }
 private:
  std::vector<char> data;
};

class rtc_t : public abstract_device_t {
 public:
  rtc_t(std::vector<processor_t*>&);
  bool load(reg_t addr, size_t len, uint8_t* bytes);
  bool store(reg_t addr, size_t len, const uint8_t* bytes);
  size_t size() { return regs.size() * sizeof(regs[0]); }
  void increment(reg_t inc);
 private:
  std::vector<processor_t*>& procs;
  std::vector<uint64_t> regs;
  uint64_t time() { return regs[0]; }
};

class plic_t : public abstract_device_t {
 public:
  plic_t(std::vector<processor_t*>&);
  bool load(reg_t addr, size_t len, uint8_t* bytes);
  bool store(reg_t addr, size_t len, const uint8_t* bytes);
  size_t size() { return 0x200; }
  void set_interrupt( uint32_t irq );
  void clear_interrupt( uint32_t irq );
 private:
  std::vector<processor_t*>& procs;
  uint32_t ie;
  uint32_t priority[2];
  uint32_t thres;
  uint32_t pending;
  uint32_t claimed;
  uint32_t claim;
};

class uart_t : public abstract_device_t {
 public:
  uart_t(std::vector<processor_t*>&);
  void add_plic(plic_t*);
  bool load(reg_t addr, size_t len, uint8_t* bytes);
  bool store(reg_t addr, size_t len, const uint8_t* bytes);
  size_t size() { return 8; }
  void getchar_int();
 private:
  std::vector<processor_t*>& procs;
  plic_t* plic;
};

#endif
