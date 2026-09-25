#ifndef WAYSHADOW_APP_HPP
#define WAYSHADOW_APP_HPP

#include "wayshadow/config.hpp"
#include "wayshadow/input.hpp"
#include "wayshadow/state.hpp"
#include "wayshadow/tray.hpp"
#include "wayshadow/window.hpp"
#include "wayshadow/wl_setup.hpp"
#include "wayshadow/xkb_handler.hpp"

namespace wayshadow {

class Application {
 public:
  explicit Application(Config config);
  ~Application();

  Application(const Application&) = delete;
  Application& operator=(const Application&) = delete;
  Application(Application&&) noexcept = default;
  Application& operator=(Application&&) noexcept = default;

  int Run();

 private:
  static gboolean OnWaylandEvent(GIOChannel* source, GIOCondition condition,
                                 gpointer data);
  static gboolean OnInputEvent(GIOChannel* source, GIOCondition condition,
                               gpointer data);
  static gboolean OnTimerTick(gpointer data);
  static gboolean OnRepeatDelayDone(gpointer data);
  static gboolean OnRepeatRateTick(gpointer data);
  static gboolean OnHoldTimer(gpointer data);

  void SetupRepeatTimer();

  ClientState state_{};
  WaylandContext wl_ctx_;
  XkbHandler xkb_;
  std::unique_ptr<InputManager> input_{nullptr};
  std::unique_ptr<WindowManager> win_mgr_{nullptr};
  std::unique_ptr<TrayIcon> tray_{nullptr};
  GMainLoop* loop_{nullptr};
  unsigned int hold_timer_id_{0};
};

}  // namespace wayshadow

#endif
