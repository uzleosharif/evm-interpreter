

import evm_interpreter;
import std;

auto main() -> int {
  uzleo::evm::Machine evm{};
  evm.Interpret("0x60056003018000");
}
