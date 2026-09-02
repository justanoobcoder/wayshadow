#include "wayshadow/draw.hpp"

#include "wayshadow/icons.hpp"
#include "wayshadow/shm.hpp"

#include <algorithm>
#include <cairo.h>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <numbers>
#include <string>
#include <string_view>
#include <sys/mman.h>
#include <unistd.h>
#include <vector>

namespace wayshadow {

    namespace {

        struct ParsedSegment {
            std::string mods{};
            std::string base_key{};
            std::string count_part{};
            bool is_icon{false};
            double width{0.0};
        };

        // Split segment into modifier prefix and key name
        // e.g. "Ctrl+Alt+Enter" -> mods="Ctrl+Alt+", key="Enter"
        void parse_segment(std::string_view segment, std::string& mods, std::string& key) {
            mods.clear();
            key.clear();

            const auto last_plus = segment.rfind('+');
            if (last_plus != std::string_view::npos) {
                if (last_plus == segment.length() - 1) {
                    key = std::string(segment);
                    return;
                }
                mods = std::string(segment.substr(0, last_plus + 1));
                key = std::string(segment.substr(last_plus + 1));
            } else {
                key = std::string(segment);
            }
        }

        // Split key into base symbol and \xc3\x97N ("×N") suffix if present
        void extract_count(std::string_view key, std::string& base_key, std::string& count_part) {
            const size_t times_pos = key.find("\xc3\x97");
            if (times_pos != std::string_view::npos) {
                base_key = std::string(key.substr(0, times_pos));
                count_part = std::string(key.substr(times_pos));
            } else {
                base_key = std::string(key);
                count_part.clear();
            }
        }

    } // namespace

    void Renderer::redraw(
        ClientState& state, struct wl_surface* surface, struct wl_shm* shm, struct wl_buffer** buffer_out
    ) {
        if (!surface || !state.window_visible) {
            return;
        }

        const int width = state.config.width;
        const int height = state.config.height;
        const int stride = width * 4;
        const size_t size = static_cast<size_t>(stride * height);

        const int fd = ShmPool::allocate_shm_file(size);
        if (fd == -1) {
            return;
        }

        void* data = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (data == MAP_FAILED) {
            close(fd);
            return;
        }

        struct wl_shm_pool* pool = wl_shm_create_pool(shm, fd, static_cast<int32_t>(size));
        struct wl_buffer* new_buffer =
            wl_shm_pool_create_buffer(pool, 0, width, height, stride, WL_SHM_FORMAT_ARGB8888);
        wl_shm_pool_destroy(pool);
        close(fd);

        cairo_surface_t* cs = cairo_image_surface_create_for_data(
            static_cast<unsigned char*>(data), CAIRO_FORMAT_ARGB32, width, height, stride
        );
        cairo_t* cr = cairo_create(cs);

        // Clear buffer
        cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
        cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.0);
        cairo_paint(cr);
        cairo_set_operator(cr, CAIRO_OPERATOR_OVER);

        // Background rectangle
        cairo_new_sub_path(cr);
        cairo_rectangle(cr, 0.0, 0.0, width, height);
        cairo_set_source_rgba(
            cr, state.config.bg_color.r, state.config.bg_color.g, state.config.bg_color.b, state.config.bg_color.a
        );
        cairo_fill(cr);

        // Font setup
        cairo_select_font_face(cr, "Monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, state.config.font_size);
        cairo_set_source_rgba(
            cr, state.config.text_color.r, state.config.text_color.g, state.config.text_color.b,
            state.config.text_color.a
        );

        cairo_font_extents_t font_extents;
        cairo_font_extents(cr, &font_extents);

        const double icon_size = state.config.font_size;
        const double small_font_size = state.config.font_size * 0.60;
        const double max_width = width - PADDING - RIGHT_PADDING;
        const double y_pos = (height - font_extents.height) / 2.0 + font_extents.ascent + TOP_BOTTOM_PADDING - 7.0;

        const auto& segments = state.buffer.segments();
        std::vector<ParsedSegment> parsed(segments.size());

        // Measurement pass
        for (size_t i = 0; i < segments.size(); ++i) {
            std::string raw_key;
            parse_segment(segments[i].text, parsed[i].mods, raw_key);
            extract_count(raw_key, parsed[i].base_key, parsed[i].count_part);

            cairo_text_extents_t mod_extents;
            cairo_text_extents(cr, parsed[i].mods.c_str(), &mod_extents);
            double w = mod_extents.x_advance;

            if (Icons::is_icon_key(parsed[i].base_key)) {
                parsed[i].is_icon = true;
                if (parsed[i].base_key == "Space") {
                    w += icon_size * 0.35;
                } else {
                    w += icon_size;
                }
            } else {
                parsed[i].is_icon = false;
                cairo_set_font_size(cr, state.config.font_size);
                cairo_text_extents_t key_extents;
                cairo_text_extents(cr, parsed[i].base_key.c_str(), &key_extents);
                w += key_extents.x_advance;
            }

            if (!parsed[i].count_part.empty()) {
                cairo_set_font_size(cr, small_font_size);
                cairo_text_extents_t count_extents;
                cairo_text_extents(cr, parsed[i].count_part.c_str(), &count_extents);
                w += count_extents.x_advance;
                cairo_set_font_size(cr, state.config.font_size);
            }

            parsed[i].width = w;
        }

        // Determine the starting segment to fit within width constraint (right-aligned scrolling)
        size_t start_seg = 0;
        double width_so_far = 0.0;
        for (size_t i = parsed.size(); i > 0; --i) {
            const size_t idx = i - 1;
            if (width_so_far + parsed[idx].width > max_width) {
                start_seg = idx + 1;
                break;
            }
            width_so_far += parsed[idx].width;
        }

        double current_x = width - RIGHT_PADDING - width_so_far;
        if (current_x < PADDING) {
            current_x = PADDING;
        }

        // Render segments
        for (size_t i = start_seg; i < parsed.size(); ++i) {
            const bool is_last = (i == parsed.size() - 1);
            const Color draw_color =
                (is_last && state.use_combo_color) ? state.current_combo_color : state.config.text_color;

            cairo_set_source_rgba(cr, draw_color.r, draw_color.g, draw_color.b, draw_color.a);
            cairo_set_font_size(cr, state.config.font_size);

            if (!parsed[i].mods.empty()) {
                cairo_move_to(cr, current_x, y_pos);
                cairo_show_text(cr, parsed[i].mods.c_str());
                cairo_text_extents_t ext;
                cairo_text_extents(cr, parsed[i].mods.c_str(), &ext);
                current_x += ext.x_advance;
            }

            if (parsed[i].is_icon) {
                Icons::draw(cr, parsed[i].base_key, current_x, y_pos, icon_size, draw_color);
                if (parsed[i].base_key == "Space") {
                    current_x += icon_size * 0.35;
                } else {
                    current_x += icon_size;
                }
            } else {
                cairo_set_font_size(cr, state.config.font_size);
                cairo_move_to(cr, current_x, y_pos);
                cairo_show_text(cr, parsed[i].base_key.c_str());
                cairo_text_extents_t key_ext;
                cairo_text_extents(cr, parsed[i].base_key.c_str(), &key_ext);
                current_x += key_ext.x_advance;
            }

            if (!parsed[i].count_part.empty()) {
                cairo_set_font_size(cr, small_font_size);
                // Slight vertical lift for superscript styling
                const double superscript_offset = state.config.font_size * 0.18;
                cairo_move_to(cr, current_x, y_pos - superscript_offset);
                cairo_show_text(cr, parsed[i].count_part.c_str());
                cairo_text_extents_t count_ext;
                cairo_text_extents(cr, parsed[i].count_part.c_str(), &count_ext);
                current_x += count_ext.x_advance;
                cairo_set_font_size(cr, state.config.font_size);
            }
        }

        // Render mouse indicator bar
        std::string active_buttons;
        if (state.mouse.lmb)
            active_buttons += "LMB ";
        if (state.mouse.rmb)
            active_buttons += "RMB ";
        if (state.mouse.mmb)
            active_buttons += "MMB ";

        const std::string& display_button = !active_buttons.empty() ? active_buttons : state.mouse.last_button;
        if (!display_button.empty()) {
            const std::string mouse_info =
                display_button + "(" + std::to_string(state.mouse.x) + ", " + std::to_string(state.mouse.y) + ")";

            cairo_set_source_rgba(
                cr, state.config.text_color.r, state.config.text_color.g, state.config.text_color.b,
                state.config.text_color.a
            );
            cairo_select_font_face(cr, "Monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
            cairo_set_font_size(cr, state.config.font_size * 0.5);

            cairo_text_extents_t mouse_ext;
            cairo_text_extents(cr, mouse_info.c_str(), &mouse_ext);
            const double mouse_x = (width - mouse_ext.width) / 2.0;
            const double mouse_y = height - 10.0;

            cairo_move_to(cr, mouse_x, mouse_y);
            cairo_show_text(cr, mouse_info.c_str());
        }

        cairo_destroy(cr);
        cairo_surface_destroy(cs);
        munmap(data, size);

        wl_surface_attach(surface, new_buffer, 0, 0);
        wl_surface_damage_buffer(surface, 0, 0, width, height);
        wl_surface_commit(surface);

        if (*buffer_out) {
            wl_buffer_destroy(*buffer_out);
        }
        *buffer_out = new_buffer;
    }

} // namespace wayshadow
