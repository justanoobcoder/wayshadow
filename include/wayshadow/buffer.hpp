#ifndef WAYSHADOW_BUFFER_HPP
#define WAYSHADOW_BUFFER_HPP

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace wayshadow {

class TextBuffer {
 public:
  static constexpr size_t kMaxDisplayLen = 256;
  static constexpr size_t kMaxSegments = 128;

  struct Segment {
    std::string text;
  };

  TextBuffer() = default;

  void AppendRaw(std::string_view text);
  void AppendOrIncrement(std::string_view key_text);
  void PopLastSegment();
  void Backspace();
  void DeleteWord();
  void Clear();

  [[nodiscard]] bool Empty() const noexcept { return segments_.empty(); }
  [[nodiscard]] size_t SegmentCount() const noexcept {
    return segments_.size();
  }
  [[nodiscard]] const std::vector<Segment>& Segments() const noexcept {
    return segments_;
  }
  [[nodiscard]] const std::string& LastKey() const noexcept {
    return last_key_;
  }
  [[nodiscard]] int LastKeyCount() const noexcept { return last_key_count_; }
  [[nodiscard]] std::string FullText() const;

  void ResetRepeatState() noexcept {
    last_key_.clear();
    last_key_count_ = 0;
  }

 private:
  void EnsureCapacity(size_t incoming_len);
  void ShiftLeft();

  std::vector<Segment> segments_;
  std::string last_key_;
  int last_key_count_{0};
};

}  // namespace wayshadow

#endif
