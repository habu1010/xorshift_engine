#pragma once

#include <cstdlib>
#include <limits>
#include <array>

namespace xorshift
{
  namespace detail
  {
    struct xorshift
    {
      typedef uint_fast32_t result_type;
      static constexpr size_t STATE_SIZE = 4;

      static void init_state(std::array<result_type, STATE_SIZE>& state)
      {
        state[0] = 123456789;
        state[1] = 362436069;
        state[2] = 521288629;
        state[3] = 88675123;
      }

      static result_type next_state_and_get(std::array<result_type, STATE_SIZE>& state)
      {
        result_type t = state[0] ^ (state[0] << 11);

        state[0] = state[1];
        state[1] = state[2];
        state[2] = state[3];

        return state[3] = (state[3] ^ (state[3] >> 19)) ^ (t ^ (t >> 8));
      }
    };

    struct xorwow
    {
      typedef uint_fast32_t result_type;
      static constexpr size_t STATE_SIZE = 6;

      static void init_state(std::array<result_type, STATE_SIZE>& state)
      {
        state[0] = 123456789;
        state[1] = 362436069;
        state[2] = 521288629;
        state[3] = 88675123;
        state[4] = 5783321;
        state[5] = 6615241;
      }

      static result_type next_state_and_get(std::array<result_type, STATE_SIZE>& state)
      {
        result_type t = state[0] ^ (state[0] >> 2);

        state[0] = state[1];
        state[1] = state[2];
        state[2] = state[3];
        state[3] = state[4];
        state[4] = (state[4] ^ (state[4] << 4)) ^ (t ^ (t << 1));

        return (state[5] += 362437) + state[4];
      }
    };
  }

  template <class VARIANT>
  class xorshift_engine
  {
  public:
    typedef typename VARIANT::result_type result_type;
    static constexpr size_t STATE_SIZE = VARIANT::STATE_SIZE;

    xorshift_engine()
    { VARIANT::init_state(state); }

    explicit xorshift_engine(result_type s)
    { seed(s); }

    void
    seed(result_type s)
    {
      for (size_t i = 0; i < STATE_SIZE; i++) {
        state[i] = s = 1812433253UL * (s ^ (s >> 30)) + i;
      }
    }

    static constexpr result_type
    min()
    { return std::numeric_limits<result_type>::min(); }

    static constexpr result_type
    max()
    { return std::numeric_limits<result_type>::max(); }

    void
    discard(unsigned long long z)
    {
      for (; z != 0ULL; --z)
        (*this)();
    }

    result_type
    operator()()
    {
      return VARIANT::next_state_and_get(state);
    }

    friend bool
    operator==(const xorshift_engine& lhs, const xorshift_engine& rhs)
    {
      return std::equal(lhs.state.begin(), lhs.state.end(), rhs.state.begin());
    }

  private:
    std::array<result_type, STATE_SIZE> state;
  };

  template<class T>
  inline bool
  operator!=(const xorshift_engine<T>& lhs, const xorshift_engine<T>& rhs)
  {
    return !(lhs == rhs);
  }

  typedef xorshift_engine<detail::xorshift>
  xorshift;

  typedef xorshift_engine<detail::xorwow>
  xorwow;
}
