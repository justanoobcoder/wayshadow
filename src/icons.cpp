#include "keypop/icons.hpp"

#include <cmath>
#include <cstdlib>
#include <numbers>
#include <string>

namespace keypop {

    bool Icons::is_icon_key(std::string_view key) noexcept {
        if (key == "Enter"
            || key == "Left"
            || key == "Right"
            || key == "Up"
            || key == "Down"
            || key == "Tab"
            || key == "Space"
            || key == "Del"
            || key == "Delete"
            || key == "Caps"
            || key == "Home"
            || key == "End"
            || key == "PgUp"
            || key == "PgDn"
            || key == "Play"
            || key == "Pause"
            || key == "Prev"
            || key == "Next"
            || key == "Vol+"
            || key == "Vol-"
            || key == "Bri+"
            || key == "Bri-"
            || key == "Mute") {
            return true;
        }
        if (key.starts_with('F') && key.length() >= 2 && key.length() <= 3) {
            const int fnum = std::atoi(key.data() + 1);
            if (fnum >= 1 && fnum <= 12) {
                return true;
            }
        }
        return false;
    }

    void
    Icons::draw(cairo_t* cr, std::string_view key_name, double x, double y, double size, const Color& color) noexcept {
        cairo_save(cr);
        cairo_set_source_rgba(cr, color.r, color.g, color.b, color.a);
        cairo_set_line_width(cr, size * 0.08);
        cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
        cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);

        if (key_name == "Enter") {
            const double cx = x + size * 0.5;
            const double cy = y - size * 0.3;
            const double w = size * 0.4;
            const double h = size * 0.3;

            cairo_move_to(cr, cx + w, cy - h);
            cairo_line_to(cr, cx, cy - h);
            cairo_line_to(cr, cx, cy);
            cairo_line_to(cr, cx - w / 2.0, cy);

            cairo_move_to(cr, cx - w / 2.0 + size * 0.1, cy - size * 0.1);
            cairo_line_to(cr, cx - w / 2.0, cy);
            cairo_line_to(cr, cx - w / 2.0 + size * 0.1, cy + size * 0.1);
            cairo_stroke(cr);
        } else if (key_name == "Left") {
            const double cx = x + size * 0.5;
            const double cy = y - size * 0.3;
            const double w = size * 0.3;

            cairo_move_to(cr, cx + w, cy);
            cairo_line_to(cr, cx - w, cy);
            cairo_move_to(cr, cx - w + size * 0.15, cy - size * 0.15);
            cairo_line_to(cr, cx - w, cy);
            cairo_line_to(cr, cx - w + size * 0.15, cy + size * 0.15);
            cairo_stroke(cr);
        } else if (key_name == "Right") {
            const double cx = x + size * 0.5;
            const double cy = y - size * 0.3;
            const double w = size * 0.3;

            cairo_move_to(cr, cx - w, cy);
            cairo_line_to(cr, cx + w, cy);
            cairo_move_to(cr, cx + w - size * 0.15, cy - size * 0.15);
            cairo_line_to(cr, cx + w, cy);
            cairo_line_to(cr, cx + w - size * 0.15, cy + size * 0.15);
            cairo_stroke(cr);
        } else if (key_name == "Up") {
            const double cx = x + size * 0.5;
            const double cy = y - size * 0.3;
            const double h = size * 0.3;

            cairo_move_to(cr, cx, cy + h);
            cairo_line_to(cr, cx, cy - h);
            cairo_move_to(cr, cx - size * 0.15, cy - h + size * 0.15);
            cairo_line_to(cr, cx, cy - h);
            cairo_line_to(cr, cx + size * 0.15, cy - h + size * 0.15);
            cairo_stroke(cr);
        } else if (key_name == "Down") {
            const double cx = x + size * 0.5;
            const double cy = y - size * 0.3;
            const double h = size * 0.3;

            cairo_move_to(cr, cx, cy - h);
            cairo_line_to(cr, cx, cy + h);
            cairo_move_to(cr, cx - size * 0.15, cy + h - size * 0.15);
            cairo_line_to(cr, cx, cy + h);
            cairo_line_to(cr, cx + size * 0.15, cy + h - size * 0.15);
            cairo_stroke(cr);
        } else if (key_name == "Tab") {
            const double cx = x + size * 0.5;
            const double cy = y - size * 0.3;
            const double w = size * 0.3;

            cairo_move_to(cr, cx - w, cy);
            cairo_line_to(cr, cx + w, cy);
            cairo_move_to(cr, cx + w, cy - size * 0.15);
            cairo_line_to(cr, cx + w, cy + size * 0.15);
            cairo_move_to(cr, cx + w - size * 0.15, cy - size * 0.15);
            cairo_line_to(cr, cx + w, cy);
            cairo_line_to(cr, cx + w - size * 0.15, cy + size * 0.15);
            cairo_stroke(cr);
        } else if (key_name == "Space") {
            const double icon_w = size * 0.25;
            const double icon_h = size * 0.1;

            const double start_x = x + (size * 0.35 - icon_w) / 2.0;
            const double bottom_y = y;

            cairo_move_to(cr, start_x, bottom_y - icon_h);
            cairo_line_to(cr, start_x, bottom_y);
            cairo_line_to(cr, start_x + icon_w, bottom_y);
            cairo_line_to(cr, start_x + icon_w, bottom_y - icon_h);
            cairo_stroke(cr);
        } else if (key_name == "Del" || key_name == "Delete") {
            const double cx = x + size * 0.5;
            const double cy = y - size * 0.3;
            const double w = size * 0.25;

            cairo_move_to(cr, cx - w, cy);
            cairo_line_to(cr, cx + w, cy);
            cairo_move_to(cr, cx + w - size * 0.12, cy - size * 0.12);
            cairo_line_to(cr, cx + w, cy);
            cairo_line_to(cr, cx + w - size * 0.12, cy + size * 0.12);

            cairo_move_to(cr, cx - w, cy - size * 0.08);
            cairo_line_to(cr, cx - w + size * 0.1, cy + size * 0.08);
            cairo_move_to(cr, cx - w + size * 0.1, cy - size * 0.08);
            cairo_line_to(cr, cx - w, cy + size * 0.08);
            cairo_stroke(cr);
        } else if (key_name.starts_with('F') && key_name.length() >= 2 && key_name.length() <= 3) {
            const double cx = x + size * 0.5;
            const double cy = y - size * 0.3;
            const double box_w = size * 0.45;
            const double box_h = size * 0.35;

            cairo_set_line_width(cr, size * 0.05);
            cairo_rectangle(cr, cx - box_w / 2.0, cy - box_h / 2.0, box_w, box_h);
            cairo_stroke(cr);

            cairo_set_font_size(cr, size * 0.22);

            cairo_text_extents_t f_ext;
            cairo_text_extents(cr, std::string(key_name).c_str(), &f_ext);

            const double text_x = cx - f_ext.width / 2.0 - f_ext.x_bearing;
            const double text_y = cy - f_ext.height / 2.0 - f_ext.y_bearing;

            cairo_move_to(cr, text_x, text_y);
            cairo_show_text(cr, std::string(key_name).c_str());

            cairo_restore(cr);
            return;
        } else if (key_name == "Caps") {
            const double cx = x + size * 0.5;
            const double cy = y - size * 0.3;
            const double h = size * 0.25;

            cairo_move_to(cr, cx, cy - h);
            cairo_line_to(cr, cx - size * 0.1, cy - h + size * 0.12);
            cairo_move_to(cr, cx, cy - h);
            cairo_line_to(cr, cx + size * 0.1, cy - h + size * 0.12);

            cairo_move_to(cr, cx - size * 0.15, cy + h);
            cairo_line_to(cr, cx, cy);
            cairo_line_to(cr, cx + size * 0.15, cy + h);
            cairo_move_to(cr, cx - size * 0.08, cy + h * 0.4);
            cairo_line_to(cr, cx + size * 0.08, cy + h * 0.4);
            cairo_stroke(cr);
        } else if (key_name == "Home") {
            const double cx = x + size * 0.5;
            const double cy = y - size * 0.3;
            const double w = size * 0.25;

            cairo_move_to(cr, cx - w, cy);
            cairo_line_to(cr, cx, cy - w);
            cairo_line_to(cr, cx + w, cy);
            cairo_move_to(cr, cx - w * 0.8, cy);
            cairo_line_to(cr, cx - w * 0.8, cy + w);
            cairo_line_to(cr, cx + w * 0.8, cy + w);
            cairo_line_to(cr, cx + w * 0.8, cy);
            cairo_stroke(cr);
        } else if (key_name == "End") {
            const double cx = x + size * 0.5;
            const double cy = y - size * 0.3;
            const double w = size * 0.25;

            cairo_move_to(cr, cx - w, cy - w);
            cairo_line_to(cr, cx - w, cy + w);
            cairo_line_to(cr, cx + w, cy + w);
            cairo_move_to(cr, cx + w - size * 0.12, cy + w - size * 0.12);
            cairo_line_to(cr, cx + w, cy + w);
            cairo_move_to(cr, cx + w, cy + w);
            cairo_line_to(cr, cx + w - size * 0.12, cy + w + size * 0.12);
            cairo_stroke(cr);
        } else if (key_name == "PgUp") {
            const double cx = x + size * 0.5;
            const double cy = y - size * 0.3;
            const double h = size * 0.15;

            cairo_move_to(cr, cx, cy - h);
            cairo_line_to(cr, cx - size * 0.12, cy - h + size * 0.12);
            cairo_move_to(cr, cx, cy - h);
            cairo_line_to(cr, cx + size * 0.12, cy - h + size * 0.12);
            cairo_move_to(cr, cx, cy + h);
            cairo_line_to(cr, cx - size * 0.12, cy + h + size * 0.12);
            cairo_move_to(cr, cx, cy + h);
            cairo_line_to(cr, cx + size * 0.12, cy + h + size * 0.12);
            cairo_stroke(cr);
        } else if (key_name == "PgDn") {
            const double cx = x + size * 0.5;
            const double cy = y - size * 0.3;
            const double h = size * 0.15;

            cairo_move_to(cr, cx, cy - h);
            cairo_line_to(cr, cx - size * 0.12, cy - h - size * 0.12);
            cairo_move_to(cr, cx, cy - h);
            cairo_line_to(cr, cx + size * 0.12, cy - h - size * 0.12);
            cairo_move_to(cr, cx, cy + h);
            cairo_line_to(cr, cx - size * 0.12, cy + h - size * 0.12);
            cairo_move_to(cr, cx, cy + h);
            cairo_line_to(cr, cx + size * 0.12, cy + h - size * 0.12);
            cairo_stroke(cr);
        } else if (key_name == "Esc") {
            const double cx = x + size * 0.5;
            const double cy = y - size * 0.3;
            const double r = size * 0.25;

            cairo_arc(cr, cx, cy, r, 0, 2 * std::numbers::pi);
            cairo_stroke(cr);
            cairo_set_line_width(cr, size * 0.06);
            cairo_move_to(cr, cx - r * 0.5, cy - r * 0.5);
            cairo_line_to(cr, cx + r * 0.5, cy + r * 0.5);
            cairo_move_to(cr, cx + r * 0.5, cy - r * 0.5);
            cairo_line_to(cr, cx - r * 0.5, cy + r * 0.5);
            cairo_stroke(cr);
        } else if (key_name == "Play") {
            const double cx = x + size * 0.5;
            const double cy = y - size * 0.3;
            const double w = size * 0.2;

            cairo_move_to(cr, cx - w, cy - w);
            cairo_line_to(cr, cx + w, cy);
            cairo_line_to(cr, cx - w, cy + w);
            cairo_close_path(cr);
            cairo_stroke(cr);
        } else if (key_name == "Pause") {
            const double cx = x + size * 0.5;
            const double cy = y - size * 0.3;
            const double h = size * 0.3;
            const double w = size * 0.06;

            cairo_rectangle(cr, cx - size * 0.12, cy - h / 2.0, w, h);
            cairo_rectangle(cr, cx + size * 0.06, cy - h / 2.0, w, h);
            cairo_stroke(cr);
        } else if (key_name == "Vol+" || key_name == "Bri+") {
            const double cx = x + size * 0.5;
            const double cy = y - size * 0.3;

            cairo_move_to(cr, cx - size * 0.15, cy);
            cairo_line_to(cr, cx + size * 0.15, cy);
            cairo_move_to(cr, cx, cy - size * 0.15);
            cairo_line_to(cr, cx, cy + size * 0.15);
            cairo_stroke(cr);
        } else if (key_name == "Vol-" || key_name == "Bri-") {
            const double cx = x + size * 0.5;
            const double cy = y - size * 0.3;

            cairo_move_to(cr, cx - size * 0.2, cy);
            cairo_line_to(cr, cx + size * 0.2, cy);
            cairo_stroke(cr);
        } else if (key_name == "Mute") {
            const double cx = x + size * 0.5;
            const double cy = y - size * 0.3;

            cairo_move_to(cr, cx - size * 0.2, cy - size * 0.1);
            cairo_line_to(cr, cx - size * 0.05, cy - size * 0.2);
            cairo_line_to(cr, cx - size * 0.05, cy + size * 0.2);
            cairo_line_to(cr, cx - size * 0.2, cy + size * 0.1);
            cairo_close_path(cr);
            cairo_stroke(cr);

            cairo_move_to(cr, cx + size * 0.05, cy - size * 0.15);
            cairo_line_to(cr, cx + size * 0.2, cy + size * 0.15);
            cairo_move_to(cr, cx + size * 0.2, cy - size * 0.15);
            cairo_line_to(cr, cx + size * 0.05, cy + size * 0.15);
            cairo_stroke(cr);
        }

        cairo_restore(cr);
    }

} // namespace keypop
