#pragma once
#include "json.h"
#include "map_renderer.h"
#include "domain.h"
#include <deque>

namespace json_reader {

    class JsonReader {
    public:
        explicit JsonReader(std::istream& input, std::ostream& output);

    private:
        void ParseRenderSettings(const std::map<std::string,json::Node>& settings);
        void ParseStopRequest(const std::map<std::string,json::Node>& stop_request);
        void ParseBusRequest(const std::map<std::string,json::Node>& bus_request);
        void ParseStatRequests(const json::Node& requests);
        void MakeBase(const json::Node& arr_requests);
        std::vector<std::string_view> ParseBusRoute(const std::vector<std::string>& stops, bool is_roundtrip)const;

        json::Document input_;
        std::ostream& output_;

        std::deque<domain::RequestToAdd> base_requests_;
        std::deque<domain::RequestToStat> stat_requests_;
        map_renderer::RendererSettings render_data_;
    };
}//json_reader namespace
