#pragma once

namespace yaccumulate {

    //! http://en.wikipedia.org/wiki/Kahan_summation_algorithm
    template <typename T>
    class KahanAccumulator {
    public:
        using value_type = T;

        explicit KahanAccumulator(const value_type value = {})
            : sum_{value}
            , compensation_{} {
        }

        template <typename Other>
        KahanAccumulator& operator =(const Other value) {
            sum_ = static_cast<value_type>(value);
            compensation_ = {};
            return *this;
        }

        template <typename Other>
        KahanAccumulator& operator+= (const Other value) {
            const value_type xxx = static_cast<value_type>(value) - compensation_;
            const value_type yyy = sum_ + xxx;
            compensation_ = (yyy - sum_) - xxx;
            sum_ = yyy;
            return *this;
        }

        value_type get() const {
            return sum_ + compensation_;
        }

        template <typename Other>
        operator Other() const {
            return get();
        }

    private:
        value_type sum_;
        value_type compensation_;
    };

}  // namespace yaccumulate

