#pragma once
#include "geo.h"
#include "svg.h"
#include "domain.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <map>
#include <optional>
#include <vector>

namespace map_renderer{

inline const double EPSILON = 1e-6;
bool IsZero(double value);

struct RendererSettings { //настройки рендера
    RendererSettings() = default;

    double width_ = 0.0;
    double height_ = 0.0;
    double padding_ = 0.0;
    double line_width_ = 0.0;
    double stop_radius_ = 0.0;
    int bus_label_font_size_ = 0;
    svg::Point bus_label_offset_ = { 0.0, 0.0 };
    int stop_label_font_size_ = 0;
    svg::Point stop_label_offset_ = { 0.0, 0.0 };
    svg::Color underlayer_color_ = {};
    double underlayer_width_ = 0.0;
    std::vector<svg::Color> color_palette_ = {};
};


class SphereProjector {
public:

    SphereProjector() = default;

    template <typename PointInputIt>
    SphereProjector(PointInputIt points_begin, PointInputIt points_end,
                    double max_width, double max_height, double padding)
        : padding_(padding)
    {
        if (points_begin == points_end) {
            return;
        }

        const auto [left_it, right_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
        min_lon_ = left_it->lng;
        const double max_lon = right_it->lng;

        const auto [bottom_it, top_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
        const double min_lat = bottom_it->lat;
        max_lat_ = top_it->lat;

        std::optional<double> width_zoom;
        if (!IsZero(max_lon - min_lon_)) {
            width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
        }

        std::optional<double> height_zoom;
        if (!IsZero(max_lat_ - min_lat)) {
            height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
        }

        if (width_zoom && height_zoom) {
            zoom_coeff_ = std::min(*width_zoom, *height_zoom);
        }
        else if (width_zoom) {
            zoom_coeff_ = *width_zoom;
        }
        else if (height_zoom) {
            zoom_coeff_ = *height_zoom;
        }
    }

    svg::Point operator()(geo::Coordinates coords) const {
        return {
            (coords.lng - min_lon_) * zoom_coeff_ + padding_,
            (max_lat_ - coords.lat) * zoom_coeff_ + padding_
        };
    }

private:
    double padding_ = 0.0;
    double min_lon_ = 0.0;
    double max_lat_ = 0.0;
    double zoom_coeff_ = 0.0;
};



class MapRenderer{
public:
    explicit MapRenderer() = default;
    void SetRenderSettings(RendererSettings& settings);
    void Render(std::ostream& out, std::vector<domain::Bus*> route);
    RendererSettings GetRenderSettings()const;

private:
    void AddRouteLine(std::map<std::string_view, std::vector<domain::Stop*>>& map_geo_coords, svg::Document& map_);
    void AddRouteName(std::map<std::string_view, std::pair<geo::Coordinates, geo::Coordinates>>& start_end_of_route, svg::Document& map_);
    void AddStop(std::map<std::string_view, geo::Coordinates>& stop, svg::Document& map_);
    void AddStopName(std::map<std::string_view, geo::Coordinates>& stop, svg::Document& map_);

    RendererSettings settings_ {};
    SphereProjector projector_{};
    void SetBusText(svg::Text &text, svg::Text &subtext, const svg::Point &coordinates, std::string_view name, size_t color_num);
};

}   // map_renderer
