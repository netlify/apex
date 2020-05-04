#ifndef APEX_CONCURRENCY_SPIN_HPP
#define APEX_CONCURRENCY_SPIN_HPP

#include <atomic>

#include <immintrin.h>

namespace apex::concurrency {

// Allows us to create a spin lock via `std::unique_lock` or `std::scoped_lock`
// This implementation *also* currently uses _mm_pause, so it's intel cpus only
// This will need to be replaced with the equivalent instruction for other
// arches we wish to support.
// The goal behind this is to
// 1) Use exponential back/off to save electricity
// 2) Try to stay away from the OS scheduler until we 'back off' enough
// 3) Be usable in 'realtime' situations.
// TODO: consider putting this inside an implementation file. However,
// the actual cost vs inline *does* need to be measured via assembly output.
//

struct spin_mutex {
  bool try_lock () noexcept { return not this->flag.test_and_set(std::memory_order_acquire); }
  void unlock () noexcept { this->flag.clear(std::memory_order_release); }
  void lock () noexcept {
    // These numbers are chosen at random. They *could* technically be template
    // parameters or generated via CTAD. THis would make for a more powerful
    // API and usage
    // XXX: in actuality the numbers are taken from
    // https://timur.audio/using-locks-in-real-time-audio-processing-safely
    constexpr std::array iterations { 5, 10, 3000 };

    for (auto idx = 0; idx < iterations[0]; ++idx) {
      if (this->try_lock()) { return; }
    }

    for (auto idx = 0; idx < iterations[1]; ++idx) {
      if (this->try_lock()) { return; }
      _mm_pause();
    }
  }
  // FIXME: This initializer is deprecated in C++20
  std::atomic_flag flag = ATOMIC_FLAG_INIT;

};

} /* namespace apex::concurrency */

#endif /* APEX_CONCURRENCY_SPIN_HPP */
