#pragma once
#include "RelationalEvent.h"
#include <vector>
#include <functional>

namespace rel {

class RelationalSequencer {
public:
    void setLength(int length);
    int length() const noexcept { return length_; }
    std::vector<RelationalEvent>& events() noexcept { return events_; }
    const std::vector<RelationalEvent>& events() const noexcept { return events_; }

    static std::vector<bool> euclideanPattern(int steps, int pulses, int rotation = 0);

private:
    int length_ = 13;
    std::vector<RelationalEvent> events_;
};

} // namespace rel
