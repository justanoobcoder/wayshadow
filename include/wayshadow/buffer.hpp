#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace wayshadow {

    class TextBuffer {
      public:
        static constexpr size_t MAX_DISPLAY_LEN = 256;
        static constexpr size_t MAX_SEGMENTS = 128;

        struct Segment {
            std::string text;
        };

        TextBuffer() = default;

        void append_raw(std::string_view text);
        void append_or_increment(std::string_view key_text);
        void pop_last_segment();
        void backspace();
        void delete_word();
        void clear();

        [[nodiscard]] bool empty() const noexcept { return segments_.empty(); }
        [[nodiscard]] size_t segment_count() const noexcept { return segments_.size(); }
        [[nodiscard]] const std::vector<Segment>& segments() const noexcept { return segments_; }
        [[nodiscard]] const std::string& last_key() const noexcept { return last_key_; }
        [[nodiscard]] int last_key_count() const noexcept { return last_key_count_; }
        [[nodiscard]] std::string full_text() const;

        void reset_repeat_state() noexcept {
            last_key_.clear();
            last_key_count_ = 0;
        }

      private:
        void ensure_capacity(size_t incoming_len);
        void shift_left();

        std::vector<Segment> segments_{};
        std::string last_key_{};
        int last_key_count_{0};
    };

} // namespace wayshadow
