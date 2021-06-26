// Copyright (c) 2000-2021, Heiko Bauke
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//   * Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//
//   * Redistributions in binary form must reproduce the above
//     copyright notice, this list of conditions and the following
//     disclaimer in the documentation and/or other materials provided
//     with the distribution.
//
//   * Neither the name of the copyright holder nor the names of its
//     contributors may be used to endorse or promote products derived
//     from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

#if !(defined TRNG_NEGATIVE_BINOMIAL_DIST_HPP)

#define TRNG_NEGATIVE_BINOMIAL_DIST_HPP

#include <trng/limits.hpp>
#include <trng/utility.hpp>
#include <trng/math.hpp>
#include <trng/special_functions.hpp>
#include <cstddef>
#include <ostream>
#include <istream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <numeric>
#include <ciso646>

namespace trng {

  // non-uniform random number generator class
  class negative_binomial_dist {
  public:
    using result_type = int;

    class param_type {
    private:
      double p_{0};
      double r_{0};
      std::vector<double> P_;

      // probability density function
      double pdf(int x) const {
        if (x < 0)
          return 0;
        std::vector<double> terms{math::ln_Gamma(r_ + x), -math::ln_Gamma(r_),
                                  -math::ln_Gamma(static_cast<double>(x + 1)),
                                  +math::ln(p_) * r_, math::ln1p(-p_) * x};
        std::sort(terms.begin(), terms.end(),
                  [](double a, double b) { return math::abs(a) < math::abs(b); });
        return math::exp(std::accumulate(terms.begin(), terms.end(), 0.0));
      }

      // cumulative density function
      double cdf(int x) const {
        if (x < 0)
          return 0;
        return math::Beta_I(p_, r_, static_cast<double>(x + 1));
      }

      void calc_probabilities() {
        P_ = std::vector<double>();
        int x{0};
        double p{0.0};
        while (p < 1.0 - 1.0 / 4096.0) {
          p = cdf(x);
          P_.push_back(p);
          ++x;
        }
        P_.push_back(1);
      }

    public:
      double p() const { return p_; }
      void p(double p_new) {
        p_ = p_new;
        calc_probabilities();
      }
      double r() const { return r_; }
      void r(double r_new) {
        r_ = r_new;
        calc_probabilities();
      }
      param_type() = default;
      explicit param_type(double p, double r) : p_{p}, r_{r} { calc_probabilities(); }
      friend class negative_binomial_dist;
    };

  private:
    param_type P;

  public:
    // constructor
    explicit negative_binomial_dist(double p, double r) : P{p, r} {}
    explicit negative_binomial_dist(const param_type &P) : P{P} {}
    // reset internal state
    void reset() {}
    // random numbers
    template<typename R>
    int operator()(R &r) {
      double p{utility::uniformco<double>(r)};
      const std::size_t x{utility::discrete(p, P.P_.begin(), P.P_.end())};
      int x_i{static_cast<int>(x)};
      if (x + 1 == P.P_.size()) {
        p -= cdf(x_i);
        while (p > 0) {
          ++x_i;
          p -= pdf(x_i);
        }
      }
      return x_i;
    }
    template<typename R>
    int operator()(R &r, const param_type &p) {
      negative_binomial_dist g(p);
      return g(r);
    }
    // property methods
    int min() const { return 0; }
    int max() const { return math::numeric_limits<int>::max(); }
    const param_type &param() const { return P; }
    void param(const param_type &P_new) { P = P_new; }
    double p() const { return P.p(); }
    void p(double p_new) { P.p(p_new); }
    double r() const { return P.r(); }
    void r(double r_new) { P.r(r_new); }
    // probability density function
    double pdf(int x) const { return P.pdf(x); }
    // cumulative density function
    double cdf(int x) const { return P.cdf(x); }
  };

  // -------------------------------------------------------------------

  // EqualityComparable concept
  bool operator==(const negative_binomial_dist::param_type &P1,
                  const negative_binomial_dist::param_type &P2) {
    return P1.p() == P2.p() and P1.r() == P2.r();
  }

  bool operator!=(const negative_binomial_dist::param_type &P1,
                  const negative_binomial_dist::param_type &P2) {
    return not(P1 == P2);
  }

  // Streamable concept
  template<typename char_t, typename traits_t>
  std::basic_ostream<char_t, traits_t> &operator<<(
      std::basic_ostream<char_t, traits_t> &out, const negative_binomial_dist::param_type &P) {
    std::ios_base::fmtflags flags(out.flags());
    out.flags(std::ios_base::dec | std::ios_base::fixed | std::ios_base::left);
    out << '(' << std::setprecision(math::numeric_limits<double>::digits10 + 1) << P.p() << ' '
        << P.r() << ')';
    out.flags(flags);
    return out;
  }

  template<typename char_t, typename traits_t>
  std::basic_istream<char_t, traits_t> &operator>>(std::basic_istream<char_t, traits_t> &in,
                                                   negative_binomial_dist::param_type &P) {
    double p;
    double r;
    std::ios_base::fmtflags flags(in.flags());
    in.flags(std::ios_base::dec | std::ios_base::fixed | std::ios_base::left);
    in >> utility::delim('(') >> p >> utility::delim(' ') >> r >> utility::delim(')');
    if (in)
      P = negative_binomial_dist::param_type(p, r);
    in.flags(flags);
    return in;
  }

  // -------------------------------------------------------------------

  // EqualityComparable concept
  bool operator==(const negative_binomial_dist &g1, const negative_binomial_dist &g2) {
    return g1.param() == g2.param();
  }
  bool operator!=(const negative_binomial_dist &g1, const negative_binomial_dist &g2) {
    return g1.param() != g2.param();
  }

  // Streamable concept
  template<typename char_t, typename traits_t>
  std::basic_ostream<char_t, traits_t> &operator<<(std::basic_ostream<char_t, traits_t> &out,
                                                   const negative_binomial_dist &g) {
    std::ios_base::fmtflags flags(out.flags());
    out.flags(std::ios_base::dec | std::ios_base::fixed | std::ios_base::left);
    out << "[negative_binomial " << g.param() << ']';
    out.flags(flags);
    return out;
  }

  template<typename char_t, typename traits_t>
  std::basic_istream<char_t, traits_t> &operator>>(std::basic_istream<char_t, traits_t> &in,
                                                   negative_binomial_dist &g) {
    negative_binomial_dist::param_type P;
    std::ios_base::fmtflags flags(in.flags());
    in.flags(std::ios_base::dec | std::ios_base::fixed | std::ios_base::left);
    in >> utility::ignore_spaces() >> utility::delim("[negative_binomial ") >> P >>
        utility::delim(']');
    if (in)
      g.param(P);
    in.flags(flags);
    return in;
  }

}  // namespace trng

#endif
