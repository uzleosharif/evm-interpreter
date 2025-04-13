
// SPDX-License-Identifier: MIT

export module evm.interpreter;

import std;
import magic_enum;
import uzleo.utils;
import fmt;

namespace rng = std::ranges;

namespace {

enum class OpCode : std::uint8_t {
  kAdd = 0x01,
  kSub = 0x03,
  kPush1 = 0x60,
  kPush2 = 0x61,
  kPush3 = 0x62,
  kPush4 = 0x63,
  kPush5 = 0x64,
  kPush6 = 0x65,
  kPush7 = 0x66,
  kPush8 = 0x67,
  kPush9 = 0x68,
  kPush10 = 0x69,
  kPush11 = 0x6a,
  kPush12 = 0x6b,
  kPush13 = 0x6c,
  kPush14 = 0x6d,
  kPush15 = 0x6e,
  kPush16 = 0x6f,
  kPush17 = 0x70,
  kPush18 = 0x71,
  kPush19 = 0x72,
  kPush20 = 0x73,
  kPush21 = 0x74,
  kPush22 = 0x75,
  kPush23 = 0x76,
  kPush24 = 0x77,
  kPush25 = 0x78,
  kPush26 = 0x79,
  kPush27 = 0x7a,
  kPush28 = 0x7b,
  kPush29 = 0x7c,
  kPush30 = 0x7d,
  kPush31 = 0x7e,
  kDup1 = 0x80,
  kDup2 = 0x81,
  kDup3 = 0x82,
  kDup4 = 0x83,
  kDup5 = 0x84,
  kDup6 = 0x85,
  kDup7 = 0x86,
  kDup8 = 0x87,
  kDup9 = 0x88,
  kDup10 = 0x89,
  kDup11 = 0x8a,
  kDup12 = 0x8b,
  kDup13 = 0x8c,
  kDup14 = 0x8d,
  kDup15 = 0x8e,
  kDup16 = 0x8f,
  kSwap1 = 0x90,
  kSwap2 = 0x91,
  kSwap3 = 0x92,
  kSwap4 = 0x93,
  kSwap5 = 0x94,
  kSwap6 = 0x95,
  kSwap7 = 0x96,
  kSwap8 = 0x97,
  kSwap9 = 0x98,
  kSwap10 = 0x99,
  kSwap11 = 0x9a,
  kSwap12 = 0x9b,
  kSwap13 = 0x9c,
  kSwap14 = 0x9d,
  kSwap15 = 0x9e,
  kSwap16 = 0x9f,
  kPop = 0x11,
  kStop = 0x00
};

struct InstructionInfo {
  std::uint64_t gas_cost{0};
};

}  // namespace

namespace uzleo::evm {

export class Machine final {
 public:
  constexpr auto Interpret(std::string_view bytecode_hexstring) {
    ParseHexString(bytecode_hexstring);
    ExecuteBytecode();
  }

  constexpr auto PrintStack() const { m_stack.Print(); }

 private:
  using uint256_t = uzleo::utils::UintNBitsType<256>;

  constexpr auto ParseHexString(std::string_view bytecode_hexstring) -> void {
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

  constexpr auto ExecuteBytecode() -> void {
    for (auto iter{rng::cbegin(m_bytecode)}; iter != rng::cend(m_bytecode);
         rng::advance(iter, 1, rng::cend(m_bytecode))) {
      auto const opcode{static_cast<OpCode>(*iter)};
      switch (opcode) {
        case OpCode::kPush1:
        case OpCode::kPush2:
        case OpCode::kPush3:
        case OpCode::kPush4:
        case OpCode::kPush5:
        case OpCode::kPush6:
        case OpCode::kPush7:
        case OpCode::kPush8:
        case OpCode::kPush9:
        case OpCode::kPush10:
        case OpCode::kPush11:
        case OpCode::kPush12:
        case OpCode::kPush13:
        case OpCode::kPush14:
        case OpCode::kPush15:
        case OpCode::kPush16:
        case OpCode::kPush17:
        case OpCode::kPush18:
        case OpCode::kPush19:
        case OpCode::kPush20:
        case OpCode::kPush21:
        case OpCode::kPush22:
        case OpCode::kPush23:
        case OpCode::kPush24:
        case OpCode::kPush25:
        case OpCode::kPush26:
        case OpCode::kPush27:
        case OpCode::kPush28:
        case OpCode::kPush29:
        case OpCode::kPush30:
        case OpCode::kPush31: {
          auto num_of_bytes{static_cast<unsigned>(opcode) -
                            static_cast<unsigned>(OpCode::kPush1) + 1};
          std::span<std::byte const> span_to_push{std::next(iter),
                                                  num_of_bytes};
          m_stack.Push(uint256_t{span_to_push});
          rng::advance(iter, num_of_bytes, rng::cend(m_bytecode));
          break;
        }
        case OpCode::kAdd: {
          auto sum{rng::fold_left(m_stack.GetElementsSpan(2), uint256_t{},
                                  std::plus<uint256_t>{})};
          m_stack.Pop(2);
          m_stack.Push(sum);
          break;
        }
        case OpCode::kDup1:
        case OpCode::kDup2:
        case OpCode::kDup3:
        case OpCode::kDup4:
        case OpCode::kDup5:
        case OpCode::kDup6:
        case OpCode::kDup7:
        case OpCode::kDup8:
        case OpCode::kDup9:
        case OpCode::kDup10:
        case OpCode::kDup11:
        case OpCode::kDup12:
        case OpCode::kDup13:
        case OpCode::kDup14:
        case OpCode::kDup15:
        case OpCode::kDup16: {
          auto const nth_element{static_cast<unsigned>(opcode) -
                                 static_cast<unsigned>(OpCode::kDup1) + 1};
          m_stack.Push(m_stack.GetElementsSpan(nth_element).front());
          break;
        }
        case OpCode::kSwap1:
        case OpCode::kSwap2:
        case OpCode::kSwap3:
        case OpCode::kSwap4:
        case OpCode::kSwap5:
        case OpCode::kSwap6:
        case OpCode::kSwap7:
        case OpCode::kSwap8:
        case OpCode::kSwap9:
        case OpCode::kSwap10:
        case OpCode::kSwap11:
        case OpCode::kSwap12:
        case OpCode::kSwap13:
        case OpCode::kSwap14:
        case OpCode::kSwap15:
        case OpCode::kSwap16: {
          auto const nth_element{static_cast<unsigned>(opcode) -
                                 static_cast<unsigned>(OpCode::kSwap1) + 2};
          auto tmp{m_stack.GetElementsSpan().front()};
          m_stack.SetElement(m_stack.GetElementsSpan(nth_element).front());
          m_stack.SetElement(tmp, nth_element);
          break;
        }
        case OpCode::kStop: {
          rng::advance(iter, rng::cend(m_bytecode));
          break;
        }
        case OpCode::kPop: {
          m_stack.Pop();
          break;
        };
        default: {
          throw std::runtime_error{
              std::format("OpCode: {:#x} not supported.",
                          static_cast<std::uint8_t>(opcode))};
        }
      }
    }
  }

  std::vector<std::byte> m_bytecode{};
  uzleo::utils::StackType<uint256_t> m_stack{};
};

}  // namespace uzleo::evm
