#pragma once

#include <algorithm>
#include <array>
#include <atomic>
#include <cstddef>
#include <memory>

namespace rel {

class RelationEngine {
public:
    static constexpr int dimensions = 5;

    enum Dimension {
        Pitch = 0,
        Time = 1,
        FM = 2,
        Resonance = 3,
        Tuning = 4
    };

    using Vector = std::array<double, dimensions>;
    using Matrix = std::array<Vector, dimensions>;

    RelationEngine() noexcept { reset(); }

    void reset() noexcept
    {
        editMatrix_ = {};

        setLocal(Pitch, FM, 0.28);
        setLocal(Pitch, Resonance, 0.16);
        setLocal(Pitch, Tuning, 0.22);
        setLocal(Time, FM, 0.12);
        setLocal(Time, Resonance, 0.24);
        setLocal(FM, Resonance, 0.31);
        setLocal(FM, Tuning, 0.18);
        setLocal(Resonance, FM, 0.12);
        setLocal(Tuning, Pitch, 0.25);
        setLocal(Tuning, FM, 0.20);

        publish();
    }

    void set(int source, int destination, double amount) noexcept
    {
        if (!valid(source) || !valid(destination))
            return;

        setLocal(source, destination, amount);
    }

    double get(int source, int destination) const noexcept
    {
        if (!valid(source) || !valid(destination))
            return 0.0;

        return editMatrix_[static_cast<std::size_t>(destination)]
                          [static_cast<std::size_t>(source)];
    }

    const Matrix& editMatrix() const noexcept
    {
        return editMatrix_;
    }

    void publish() noexcept
    {
        auto snapshot = std::make_shared<Matrix>(editMatrix_);
        std::shared_ptr<const Matrix> immutable = std::move(snapshot);
        std::atomic_store_explicit(
            &audioMatrix_,
            std::move(immutable),
            std::memory_order_release);
    }

    bool copyMatrixForAudio(Matrix& destination) const noexcept
    {
        const auto snapshot = std::atomic_load_explicit(
            &audioMatrix_,
            std::memory_order_acquire);

        if (!snapshot)
            return false;

        destination = *snapshot;
        return true;
    }

    Vector transform(
        const Vector& input,
        double depth,
        const Matrix& matrix) const noexcept
    {
        Vector output = input;
        const double d = std::clamp(depth, 0.0, 1.0);

        for (int destination = 0; destination < dimensions; ++destination)
        {
            double coupled = 0.0;

            for (int source = 0; source < dimensions; ++source)
            {
                coupled +=
                    matrix[static_cast<std::size_t>(destination)]
                          [static_cast<std::size_t>(source)]
                    * input[static_cast<std::size_t>(source)];
            }

            output[static_cast<std::size_t>(destination)] =
                std::clamp(
                    input[static_cast<std::size_t>(destination)]
                        + coupled * d,
                    -1.0,
                    1.0);
        }

        return output;
    }

    Vector transform(
        const Vector& input,
        double depth = 1.0) const noexcept
    {
        return transform(input, depth, editMatrix_);
    }

    Vector transformSlewed(
        const Vector& input,
        double depth,
        const Matrix& matrix,
        double slewFactor = 0.005) noexcept
    {
        const Vector target = transform(input, depth, matrix);
        for (int i = 0; i < dimensions; ++i) {
            lastOutput_[static_cast<std::size_t>(i)] += 
                (target[static_cast<std::size_t>(i)] - lastOutput_[static_cast<std::size_t>(i)]) * slewFactor;
        }
        return lastOutput_;
    }

private:
    static bool valid(int i) noexcept
    {
        return i >= 0 && i < dimensions;
    }

    void setLocal(int source, int destination, double amount) noexcept
    {
        editMatrix_[static_cast<std::size_t>(destination)]
                   [static_cast<std::size_t>(source)] =
            std::clamp(amount, -1.0, 1.0);
    }

    Matrix editMatrix_{};
    std::shared_ptr<const Matrix> audioMatrix_;
    Vector lastOutput_{};
};

} // namespace rel
