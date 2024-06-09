#pragma once
#include "json.h"
#include "map_renderer.h"
#include "transport_catalogue.h"
#include "json_builder.h"
#include "transport_router.h"

namespace request_handler {

class RequestHandler {
public:
    RequestHandler(transport_catalogue::TransportCatalogue& transport_catalogue);
    
    void ProcessStatRequests(const std::deque<domain::RequestToStat>& stat_requests, std::ostream& output); //выводим данные по запросам
    void FillCatalogue(const std::deque<domain::RequestToAdd>& base_requests_); //заполняем транспортный каталог
    void SetRenderSettings(map_renderer::RendererSettings settings); //устанавливаем настройки рендера
    void SetRouterSettings(transport_router::RoutingSettings settings); //устанавливаем настройки пути
    void MakeRouter(); //создаем базу пути //функция отдельно для случая перевызова заполнения транспортного каталога, пути могли измениться
private:
    transport_catalogue::TransportCatalogue& transport_catalogue_; //ссылка на транспотный каталог
    map_renderer::MapRenderer render_; // рендер карты
    transport_router::TransportRouter transport_router_; // база пути
    
    std::vector<std::string_view> ParseBusRoute(const std::vector<std::string> &stops, bool is_roundtrip) const; //парсим путь автобуса
    json::Document ParseBusInfo(const domain::RequestToStat& request)const; //парсим информацию об автобусе
    json::Document ParseStopInfo(const domain::RequestToStat& request)const; //парсим информацию об остановке
    json::Document ParseRouteInfo(const domain::RequestToStat& request); //парсим информацию пути
    json::Document RenderMap(const domain::RequestToStat& request); //рендерим карту
    std::optional<domain::RouteInfo> BuildRoute(std::string_view from, std::string_view to); //строим путь

};
}//namespace request_handler
