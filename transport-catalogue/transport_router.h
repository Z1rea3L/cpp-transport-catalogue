#pragma once

#include "domain.h"
#include "graph.h"
#include "router.h"
#include "transport_catalogue.h"

#include <memory>
#include <vector>
#include <string_view>
#include <unordered_map>

namespace transport_router {
    
    struct RoutingSettings { //настройки для пути
        double bus_wait_time = 0.0;
        double bus_velocity = 1.0;
    };

    class TransportRouter {
    public:
        void SetRouterSettings(const RoutingSettings& settings); //устанавливаем настройки пути
        std::optional<RouteInfo> BuildRoute(const domain::Stop* from, const domain::Stop* to) const; //строим путь по 2 остановкам
        void FillRouterData(const transport_catalogue::TransportCatalogue& catalogue); //заполняем базу путей
        void ClearRouter();

    private:
        struct EdgeData { //данные ребра графа
            int span_count;
            std::string_view name;
            double time;
        };

        void FillGrafByStops(const transport_catalogue::TransportCatalogue& catalogue); //строим граф по остановкам
        void FillGrafByBuses(const transport_catalogue::TransportCatalogue& catalogue); //строим граф по автобусам

        std::unordered_map<const domain::Stop*, graph::VertexId> stop_to_vertex_; //(остановка - id вершины)
        std::unordered_map<graph::EdgeId, EdgeData> edges_data_; //(id ребра - данные ребра)

        std::unique_ptr<graph::DirectedWeightedGraph<double>> graph_ptr_; //указатель на граф
        std::unique_ptr<graph::Router<double>> router_ptr_; //указатель на роутер

        RoutingSettings settings_; //настройки для базы путей
    };

}//namespace transport_router
