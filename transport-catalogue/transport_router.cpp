#include "transport_catalogue.h"
#include "transport_router.h"

#include <stdexcept>

using namespace std::literals;

namespace transport_router {
    
    void TransportRouter::SetRouterSettings(const RoutingSettings& settings) {
        settings_ = settings;
    }
    
    void TransportRouter::ClearRouter(){
        stop_to_vertex_.clear();
        edges_data_.clear();
        graph_ptr_.reset();
        router_ptr_.reset();
    }

    void TransportRouter::FillRouterData(const transport_catalogue::TransportCatalogue& catalogue) {
        FillGrafByStops(catalogue);
        FillGrafByBuses(catalogue);
        router_ptr_ = std::make_unique<graph::Router<double>>(*graph_ptr_);
    }

    void TransportRouter::FillGrafByStops(const transport_catalogue::TransportCatalogue& catalogue){
        std::vector<const domain::Stop*> stops = catalogue.GetStopsVec();
        graph_ptr_ = std::make_unique<graph::DirectedWeightedGraph<double>>(stops.size()*2);

        for(size_t i = 0; i < stops.size(); ++i) {
            const domain::Stop* stop = stops[i];
            
            stop_to_vertex_[stop] = i*2;
            graph::VertexId start = i*2;
            graph::VertexId end = i*2+1;
            
            graph::Edge<double> edge{start, end, settings_.bus_wait_time};
            graph::EdgeId edge_id = graph_ptr_->AddEdge(edge);
            
            edges_data_.emplace(edge_id, EdgeData{0, stop->name, settings_.bus_wait_time});
        }
    }

    void TransportRouter::FillGrafByBuses(const transport_catalogue::TransportCatalogue& catalogue){
        std::vector<const domain::Bus*> buses = catalogue.GetBusesVec();
        for (const domain::Bus* bus : buses) {
            std::vector<domain::Stop*> stops = bus->stops;

            if (bus->is_circular == false) {
                stops.insert(stops.end(),stops.rbegin()+1,stops.rend());
            }

            for (int count_from = 0; count_from < stops.size()-1 ; ++count_from) {
                int distance = 0;

                for (int count_to = count_from+1 ; count_to < stops.size(); ++count_to) {
                    if (stops[count_from] == stops[count_to]) {
                        continue;
                    }

                    graph::VertexId start = stop_to_vertex_.at(stops[count_to]);
                    graph::VertexId end = stop_to_vertex_.at(stops[count_from])+1;

                    distance += catalogue.GetStopsDistance(stops[count_to-1], stops[count_to]);
                    double weight = 60.0*(distance/settings_.bus_velocity/1000.0);
                    
                    graph::Edge<double> edge{end, start, weight};
                    graph::EdgeId edge_id = graph_ptr_->AddEdge(edge);
                    
                    int span_count = count_to-count_from;
                    edges_data_.emplace(edge_id, EdgeData{span_count, bus->name, weight});
                }
            }
        }
    }

    std::optional<domain::RouteInfo> TransportRouter::BuildRoute(const domain::Stop* from, const domain::Stop* to) const{
        graph::VertexId vertex_from = stop_to_vertex_.at(from);
        graph::VertexId vertex_to = stop_to_vertex_.at(to);
        
        auto route_info = router_ptr_->BuildRoute(vertex_from, vertex_to);
        if (!route_info) {
            return std::nullopt;
        }

        domain::RouteInfo result;
        result.total_time = route_info->weight;
        
        for (graph::EdgeId id : route_info->edges) {
            const EdgeData& data = edges_data_.at(id);
            RoutePart part;
            if (data.span_count == 0) {
                part.type = domain::RoutePartType::stop;
                part.name = data.name;
                part.time = data.time;
                result.parts.push_back(part);
            } else {
                part.type = domain::RoutePartType::bus;
                part.name = data.name;
                part.span_count = data.span_count;
                part.time = data.time;
                result.parts.push_back(part);
            }
        }
        return result;
    }
    
}//namespace transport_router
