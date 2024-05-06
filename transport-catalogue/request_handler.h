#pragma once
#include "json.h"
#include "map_renderer.h"
#include "transport_catalogue.h"
namespace request_handler {

class RequestHandler {
public:
    RequestHandler(const std::deque<domain::RequestToAdd>& base_requests,
                   const std::deque<domain::RequestToStat>& stat_requests,
                   map_renderer::RendererSettings& render_data);

private:
    transport_catalogue::TransportCatalogue transport_catalogue_;
    map_renderer::MapRenderer render_;

    void ProcessStatRequests(const std::deque<domain::RequestToStat>& stat_requests);
    void MakeTransportCatalogueBase(const std::deque<domain::RequestToAdd>& base_requests_);
    std::vector<std::string_view> ParseBusRoute(const std::vector<std::string> &stops, bool is_roundtrip) const;
    json::Node ParseBusInfo(const domain::RequestToStat& request)const;
    json::Node ParseStopInfo(const domain::RequestToStat& request)const;
    json::Node RenderMap(const domain::RequestToStat& request);
};
}//namespace request_handler
