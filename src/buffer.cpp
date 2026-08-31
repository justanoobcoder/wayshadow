#include "keypop/buffer.hpp"

#include <numeric>

namespace keypop {

    std::string TextBuffer::full_text() const {
        std::string result;
        for (const auto& seg : segments_) {
            result += seg.text;
        }
        return result;
    }

    void TextBuffer::shift_left() {
        if (segments_.empty()) {
            return;
        }
        segments_.erase(segments_.begin());
    }

    void TextBuffer::ensure_capacity(size_t incoming_len) {
        while (segments_.size() >= MAX_SEGMENTS) {
            shift_left();
        }

        size_t current_len = 0;
        for (const auto& seg : segments_) {
            current_len += seg.text.length();
        }

        while (!segments_.empty() && (current_len + incoming_len >= MAX_DISPLAY_LEN)) {
            current_len -= segments_.front().text.length();
            shift_left();
        }
    }

    void TextBuffer::append_raw(std::string_view text) {
        if (text.empty()) {
            return;
        }
        ensure_capacity(text.length());
        segments_.push_back(Segment{std::string(text)});
    }

    void TextBuffer::append_or_increment(std::string_view key_text) {
        if (key_text == last_key_) {
            last_key_count_++;
            pop_last_segment();

            // UTF-8 multiplication sign \xc3\x97 ("×")
            std::string counted_buf = std::string(key_text) + "\xc3\x97" + std::to_string(last_key_count_);
            append_raw(counted_buf);
            return;
        }

        last_key_count_ = 1;
        last_key_ = std::string(key_text);

        if (!segments_.empty()) {
            const auto& last_seg = segments_.back().text;
            // Check if previous segment was a special key name (longer than 1 ASCII char and not repeat marker)
            const bool prev_is_special =
                (last_seg.length() > 1 && last_seg.find("\xc3\x97") == std::string::npos && last_seg.back() != ' ');
            const bool this_is_special = (key_text.length() > 1 && key_text.front() != ' ');
            if (prev_is_special || this_is_special) {
                append_raw(" ");
            }
        }
        append_raw(key_text);
    }

    void TextBuffer::pop_last_segment() {
        if (!segments_.empty()) {
            segments_.pop_back();
        }
    }

    void TextBuffer::backspace() { pop_last_segment(); }

    void TextBuffer::delete_word() {
        if (segments_.empty()) {
            return;
        }

        // Erase trailing space segments first
        while (!segments_.empty()) {
            if (segments_.back().text == " ") {
                segments_.pop_back();
            } else {
                break;
            }
        }

        // Erase word segment tokens until hitting space or empty
        while (!segments_.empty()) {
            if (segments_.back().text == " ") {
                break;
            }
            segments_.pop_back();
        }
    }

    void TextBuffer::clear() {
        segments_.clear();
        last_key_.clear();
        last_key_count_ = 0;
    }

} // namespace keypop
