#pragma once
#include "json.h"
#include "map_renderer.h"
#include "domain.h"
#include <deque>

namespace json_reader {

    class JsonReader {
    public:
        explicit JsonReader(std::istream& input);
        void ReadJson(std::istream& input);
        const std::deque<domain::RequestToAdd> GetDataBase()const;
        const std::deque<domain::RequestToStat> GetRequestBase()const;
        map_renderer::RendererSettings GetRenderSettings()const;
    private:
        void ParseRenderSettings(const std::map<std::string,json::Node>& settings);//парсим настройки рендера
        void ParseStopRequest(const std::map<std::string,json::Node>& stop_request);//парсим входные данные с типом Stop
        void ParseBusRequest(const std::map<std::string,json::Node>& bus_request);//парсим входные данные с типом Bus
        void ParseStatRequests(const json::Node& requests); //парсим запросы для вывода и сохраняем для дальнейшей обработки
        void ParseDataRequests(const json::Node& arr_requests); //парсим входные данные и сохраняем для дальнейшей обработки

        std::deque<domain::RequestToAdd> base_requests_;
        std::deque<domain::RequestToStat> stat_requests_;
        map_renderer::RendererSettings render_data_;
    };
}//json_reader namespace
