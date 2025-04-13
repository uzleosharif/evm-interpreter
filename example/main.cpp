

import evm.interpreter;
import std;

auto main() -> int {
  uzleo::evm::Machine evm{};
  evm.Interpret("0x6005600301806301020304600160021181849200");
  evm.PrintStack();
}
