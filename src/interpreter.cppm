
// SPDX-License-Identifier: MIT

export module evm_interpreter;

import std;
import magic_enum;
import uzleo_utils;
import fmt;

namespace rng = std::ranges;

namespace {

enum class OpCode : std::uint8_t {
  kAdd = 0x01,
  kSub = 0x03,
  kPush1 = 0x60,
  kDup1 = 0x80,
  kStop = 0x00
};

struct InstructionInfo {
  std::uint64_t gas_cost{0};
};

// constexpr auto const& GetOpcodeTableEntry(OpCode opcode) {
//   static auto const kOpcodeTable{[]() {
//     std::unordered_map<OpCode, InstructionInfo> opcode_table{};
//     for (auto const opcode : magic_enum::enum_values<OpCode>()) {
//       opcode_table.insert_or_assign(opcode, InstructionInfo{});
//     }
//     return opcode_table;
//   }()};
//   return kOpcodeTable.at(opcode);
// }

}  // namespace

namespace uzleo::evm {

export class Machine final {
 public:
  constexpr auto Interpret(std::string_view bytecode_hexstring);

 private:
  using uint256_t = uzleo::utils::UintNBitsType<256>;

  constexpr auto ParseHexString(std::string_view bytecode_hexstring) -> void;
  constexpr auto Execute() -> void;
  constexpr auto ArithmeticOperateTopTwoStackElements(
      std::function<uint256_t(uint256_t const&, uint256_t const&)>&& operation);

  std::vector<std::byte> m_bytecode{};
  std::stack<uint256_t> m_stack{};
};

}  // namespace uzleo::evm

namespace uzleo::evm {

constexpr auto Machine::Interpret(std::string_view bytecode_hexstring) {
  ParseHexString(bytecode_hexstring);
  Execute();
}

constexpr auto Machine::ParseHexString(std::string_view bytecode_hexstring)
    -> void {
  if (bytecode_hexstring.starts_with("0x")) {
    bytecode_hexstring.remove_prefix(2);
  }

  if (rng::size(bytecode_hexstring) % 2 != 0) {
    throw std::invalid_argument{"bytecode hexstring is invalid size wise."};
  }

  m_bytecode.clear();
  m_bytecode.reserve(rng::size(bytecode_hexstring) / 2);
  for (auto iter{rng::begin(bytecode_hexstring)};
       iter != rng::end(bytecode_hexstring); rng::advance(iter, 2)) {
    int result{0};
    auto const from_chars_result =
        std::from_chars(iter, rng::next(iter, 2), result, 16);
    if (from_chars_result.ptr != rng::next(iter, 2) or
        from_chars_result.ec != std::errc{}) {
      throw std::runtime_error{"bytecode hexstring is invalid char wise."};
    }
    m_bytecode.push_back(static_cast<std::byte>(result));
  }
}

constexpr auto Machine::ArithmeticOperateTopTwoStackElements(
    std::function<uint256_t(uint256_t const&, uint256_t const&)>&& operation) {
  auto a{m_stack.top()};
  m_stack.pop();
  auto b{m_stack.top()};
  m_stack.pop();
  m_stack.push(std::move(operation)(a, b));
}

constexpr auto Machine::Execute() -> void {
  for (auto iter{rng::cbegin(m_bytecode)}; iter != rng::cend(m_bytecode);
       rng::advance(iter, 1, rng::cend(m_bytecode))) {
    auto const opcode{static_cast<OpCode>(*iter)};
    switch (opcode) {
      case OpCode::kPush1: {
        rng::advance(iter, 1, rng::cend(m_bytecode));
        m_stack.emplace(uint256_t{std::to_integer<std::uint8_t>(*iter)});
        break;
      }
      case OpCode::kAdd: {
        ArithmeticOperateTopTwoStackElements(
            [](uint256_t const& a, uint256_t const& b) { return a + b; });
        break;
      }
      case OpCode::kDup1: {
        m_stack.push(m_stack.top());
        break;
      }
      case OpCode::kStop: {
        rng::advance(iter, rng::cend(m_bytecode));
        break;
      }
      default: {
        throw std::runtime_error{std::format(
            "OpCode: {:#x} not supported.", static_cast<std::uint8_t>(opcode))};
      }
    }
  }

  while (not m_stack.empty()) {
    fmt::println("{}", m_stack.top());
    m_stack.pop();
  }
}

}  // namespace uzleo::evm
