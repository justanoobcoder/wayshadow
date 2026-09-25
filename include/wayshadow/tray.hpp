#ifndef WAYSHADOW_TRAY_HPP
#define WAYSHADOW_TRAY_HPP

#include <gtk/gtk.h>
#include <libappindicator/app-indicator.h>

#include "wayshadow/state.hpp"
#include "wayshadow/window.hpp"

namespace wayshadow {

class TrayIcon {
 public:
  TrayIcon(ClientState& state, WindowManager& win_mgr, GMainLoop* loop);
  ~TrayIcon() = default;

  TrayIcon(const TrayIcon&) = delete;
  TrayIcon& operator=(const TrayIcon&) = delete;
  TrayIcon(TrayIcon&&) noexcept = default;
  TrayIcon& operator=(TrayIcon&&) noexcept = default;

  [[nodiscard]] bool Init();

 private:
  static void OnToggleActivate(GtkMenuItem* item, void* data);
  static void OnExitActivate(GtkMenuItem* item, void* data);
  void UpdateToggleLabel();

  ClientState* state_{nullptr};
  WindowManager* win_mgr_{nullptr};
  GMainLoop* loop_{nullptr};
  AppIndicator* indicator_{nullptr};
  GtkWidget* menu_{nullptr};
  GtkWidget* toggle_item_{nullptr};
};

}  // namespace wayshadow

#endif
