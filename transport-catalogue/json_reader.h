#pragma once
#include "json.h"
#include "map_renderer.h"
#include "domain.h"
#include "transport_router.h"
#include <deque>

namespace json_reader {

    class JsonReader {
    public:
        explicit JsonReader(std::istream& input);
        void ReadJson(std::istream& input); //читаем json файл и сохраняем в виде структур запросов
        const std::deque<domain::RequestToAdd> GetDataBase()const; //получаем базу запросов с данными
        const std::deque<domain::RequestToStat> GetRequestBase()const; //получаем базу запросов вывода
        map_renderer::RendererSettings GetRenderSettings()const; //получаем настройки рендера
        transport_router::RoutingSettings GetRoutingSettings()const; //получаем настройки пути
    private:
        void ParseRenderSettings(const std::map<std::string,json::Node>& settings); //парсим настройки рендера
        void ParseRouteSettings(const std::map<std::string,json::Node>& settings); //парсим настройки пути
        void ParseStopRequest(const std::map<std::string,json::Node>& stop_request); //парсим входные данные с типом Stop
        void ParseBusRequest(const std::map<std::string,json::Node>& bus_request); //парсим входные данные с типом Bus
        void ParseStatRequests(const json::Node& requests); //парсим запросы для вывода и сохраняем для дальнейшей обработки
        void ParseDataRequests(const json::Node& arr_requests); //парсим входные данные и сохраняем для дальнейшей обработки

        std::deque<domain::RequestToAdd> base_requests_; //база запросов на добавление данных
        std::deque<domain::RequestToStat> stat_requests_; //база запросов на вывод данных
        map_renderer::RendererSettings render_data_; //настройки рендера
        transport_router::RoutingSettings routing_settings_; //настройки пути
    };
}//json_reader namespace
