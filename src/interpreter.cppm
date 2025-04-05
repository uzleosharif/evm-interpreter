

export module evm_interpreter;

import std;
import magic_enum;

namespace {

enum class OpCode : std::uint8_t {
  kAdd,
  kSub,
  kMul,
  kDiv,
  kPush1,
  kDup1,
  kSwap1,
  kPop,
  kJump,
  kJumpI,
  kJumpDest,
  kMLoad,
  kMStore,
  kStop,
  kInvalid
};

struct InstructionInfo {
  std::uint64_t gas_cost{0};
};

auto const& GetOpcodeTable() {
  static auto const kOpcodeTable{[]() {
    std::unordered_map<OpCode, InstructionInfo> opcode_table{};
    for (auto opcode : magic_enum::enum_values<OpCode>()) {
      opcode_table.insert_or_assign(opcode, InstructionInfo{});
    }

    return opcode_table;
  }()};
  return kOpcodeTable;
}

}  // namespace

namespace uzleo::evm::interpreter {}  // namespace uzleo::evm::interpreter
