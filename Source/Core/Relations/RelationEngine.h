#pragma once
#include <array>
#include <algorithm>
#include <cstddef>

namespace rel {

// Five-dimensional coupling space: pitch, time, FM, resonance and tuning.
// It is intentionally small and deterministic: a musical coupling model rather
// than a general-purpose modulation matrix.
class RelationEngine {
public:
    static constexpr int dimensions = 5;
    enum Dimension { Pitch = 0, Time = 1, FM = 2, Resonance = 3, Tuning = 4 };
    using Vector = std::array<double, dimensions>;
    using Matrix = std::array<Vector, dimensions>;

    RelationEngine() noexcept { reset(); }

    void reset() noexcept {
        matrix_ = {};
        set(Pitch, FM, 0.28);
        set(Pitch, Resonance, 0.16);
        set(Pitch, Tuning, 0.22);
        set(Time, FM, 0.12);
        set(Time, Resonance, 0.24);
        set(FM, Resonance, 0.31);
        set(FM, Tuning, 0.18);
        set(Resonance, FM, 0.12);
        set(Tuning, Pitch, 0.25);
        set(Tuning, FM, 0.20);
    }

    void set(int source, int destination, double amount) noexcept {
        if (source < 0 || source >= dimensions || destination < 0 || destination >= dimensions) return;
        matrix_[static_cast<std::size_t>(destination)][static_cast<std::size_t>(source)] = std::clamp(amount, -1.0, 1.0);
    }

    double get(int source, int destination) const noexcept {
        if (source < 0 || source >= dimensions || destination < 0 || destination >= dimensions) return 0.0;
        return matrix_[static_cast<std::size_t>(destination)][static_cast<std::size_t>(source)];
    }

    Vector transform(const Vector& input, double depth = 1.0) const noexcept {
        Vector output = input;
        const double d = std::clamp(depth, 0.0, 1.0);
        for (int destination = 0; destination < dimensions; ++destination) {
            double coupled = 0.0;
            for (int source = 0; source < dimensions; ++source)
                coupled += matrix_[static_cast<std::size_t>(destination)][static_cast<std::size_t>(source)] * input[static_cast<std::size_t>(source)];
            output[static_cast<std::size_t>(destination)] = std::clamp(input[static_cast<std::size_t>(destination)] + coupled * d, -1.0, 1.0);
        }
        return output;
    }

private:
    Matrix matrix_{};
};

} // namespace rel
