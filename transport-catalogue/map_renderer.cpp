#include "map_renderer.h"
#include <algorithm>
#include <map>

namespace map_renderer {

bool IsZero(double value)
{
    return std::abs(value) < EPSILON;
}

void MapRenderer::SetRenderSettings(RendererSettings& settings)
{
    settings_=settings;
}

void MapRenderer::Render(std::ostream& out, std::vector<domain::Bus*> buses)
{
    std::map<std::string_view, std::pair<geo::Coordinates, geo::Coordinates>> start_end_of_route;
    for (const auto route : buses) {
        auto start_end_stop = std::make_pair(route->stops[0]->coordinates,
                                             route->stops[route->stops.size() - 1]->coordinates);

        if (!route->is_circular) {
            start_end_stop.second = route->stops[route->stops.size() / 2]->coordinates;
        }

        start_end_of_route[route->name] = start_end_stop;
    }

    std::vector<geo::Coordinates> geo_coords;
    std::map<std::string_view, std::vector<domain::Stop*>> map_geo_coords;
    std::map<std::string_view, geo::Coordinates> uniq_stops;
    for (const auto route : buses) {
        if (!route->stops.size()) {
            continue;
        }

        for(auto& stop : route->stops){
            geo_coords.emplace_back(stop->coordinates);
            uniq_stops[stop->name] = stop->coordinates;
        }
        map_geo_coords[route->name] = route->stops;
    }

    projector_ = SphereProjector{ geo_coords.begin(), geo_coords.end(), settings_.width_, settings_.height_, settings_.padding_ };

    AddRouteLine(map_geo_coords);

    AddRouteName(start_end_of_route);

    AddStop(uniq_stops);

    AddStopName(uniq_stops);

    map_.Render(out);
}

void MapRenderer::AddRouteLine(std::map<std::string_view, std::vector<domain::Stop*>>& map_geo_coords)
{
    size_t color_num = 0;
    for (const auto& [name, stop] : map_geo_coords) {
        svg::Polyline route;
        for (const auto coords : stop) {
            route.AddPoint(projector_(coords->coordinates));
        }
        route.SetFillColor(svg::NoneColor);
        route.SetStrokeColor(settings_.color_palette_[color_num]);
        route.SetStrokeWidth(settings_.line_width_);
        route.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
        route.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

        map_.Add(route);

        ++color_num;
        if (color_num > (settings_.color_palette_.size() - 1)) {
            color_num = 0;
        }
    }
}

void MapRenderer::SetBusText(svg::Text& text, svg::Text& subtext, const svg::Point& coordinates, std::string_view name, size_t color_num){
    text.SetPosition(coordinates);
    text.SetOffset(settings_.bus_label_offset_);
    text.SetFontSize(settings_.bus_label_font_size_);
    text.SetFontFamily(std::string("Verdana"));
    text.SetFontWeight(std::string("bold"));
    text.SetData(std::string(name));
    text.SetFillColor(settings_.color_palette_[color_num]);

    subtext.SetPosition(coordinates);
    subtext.SetOffset(settings_.bus_label_offset_);
    subtext.SetFontSize(settings_.bus_label_font_size_);
    subtext.SetFontFamily(std::string("Verdana"));
    subtext.SetFontWeight(std::string("bold"));
    subtext.SetData(std::string(name));
    subtext.SetFillColor(settings_.underlayer_color_);
    subtext.SetStrokeColor(settings_.underlayer_color_);
    subtext.SetStrokeWidth(settings_.underlayer_width_);
    subtext.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
    subtext.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
}

void MapRenderer::AddRouteName(std::map<std::string_view, std::pair<geo::Coordinates, geo::Coordinates>>& start_end_of_route)
{
    size_t color_num = 0;

    for (const auto& [name, coords] : start_end_of_route) {

        svg::Text text;
        svg::Text subtext;

        SetBusText(text, subtext, projector_(coords.first), name, color_num);

        map_.Add(subtext);
        map_.Add(text);

        if (coords.first != coords.second) {
            SetBusText(text, subtext, projector_(coords.second), name, color_num);

            map_.Add(subtext);
            map_.Add(text);
        }

        ++color_num;
        if (color_num > (settings_.color_palette_.size() - 1)) {
            color_num = 0;
        }
    }
}

void MapRenderer::AddStopName(std::map<std::string_view, geo::Coordinates>& stop)
{
    for (const auto& [name, coord] : stop) {

        svg::Text stop_name;
        svg::Text subtext;

        stop_name.SetPosition(projector_(coord));
        stop_name.SetOffset(settings_.stop_label_offset_);
        stop_name.SetFontSize(settings_.stop_label_font_size_);
        stop_name.SetFontFamily(std::string("Verdana"));
        stop_name.SetData(std::string(name));
        stop_name.SetFillColor(std::string("black"));

        subtext.SetPosition(projector_(coord));
        subtext.SetOffset(settings_.stop_label_offset_);
        subtext.SetFontSize(settings_.stop_label_font_size_);
        subtext.SetFontFamily(std::string("Verdana"));
        subtext.SetData(std::string(name));
        subtext.SetFillColor(settings_.underlayer_color_);
        subtext.SetStrokeColor(settings_.underlayer_color_);
        subtext.SetStrokeWidth(settings_.underlayer_width_);
        subtext.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
        subtext.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

        map_.Add(subtext);
        map_.Add(stop_name);
    }
}

void MapRenderer::AddStop(std::map<std::string_view, geo::Coordinates>& stop)
{
    for (const auto& [name, coord] : stop) {

        svg::Circle stop_on_map;

        stop_on_map.SetCenter(projector_(coord));
        stop_on_map.SetRadius(settings_.stop_radius_);
        stop_on_map.SetFillColor(std::string("white"));

        map_.Add(stop_on_map);

    }
}
}//map_renderer
