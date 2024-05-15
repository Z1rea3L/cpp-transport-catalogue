#pragma once
#include "json.h"
#include "map_renderer.h"
#include "transport_catalogue.h"
#include "json_builder.h"

namespace request_handler {

class RequestHandler {
public:
    RequestHandler(transport_catalogue::TransportCatalogue& transport_catalogue);
    
    void ProcessStatRequests(const std::deque<domain::RequestToStat>& stat_requests, std::ostream& output);
    void FillCatalogue(const std::deque<domain::RequestToAdd>& base_requests_);
    void SetRenderSettings(map_renderer::RendererSettings settings);
private:
    transport_catalogue::TransportCatalogue& transport_catalogue_;
    map_renderer::MapRenderer render_;
    
    std::vector<std::string_view> ParseBusRoute(const std::vector<std::string> &stops, bool is_roundtrip) const;
    
    json::Document ParseBusInfo(const domain::RequestToStat& request)const;
    json::Document ParseStopInfo(const domain::RequestToStat& request)const;
    json::Document RenderMap(const domain::RequestToStat& request);

};
}//namespace request_handler
