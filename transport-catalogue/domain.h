#pragma once

#include "geo.h"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <map>

namespace domain{
//структуры, которые используются больше чем в одном классе, находятся тут
    
//остановка
struct Stop {
    std::string name;
    geo::Coordinates coordinates;
};

//автобус
struct Bus {
    std::string name;
    std::vector<Stop*>stops;
    std::unordered_set<Stop*> unique_stops;
    bool is_circular = false;
};    

//типы запросов к базе данных    
enum RequestType {
    null = 0,
    add_stop,
    add_bus,
    find_stop,
    find_bus,
    render_map,
    route
};

//типы частей пути (ожидание на остановке/поездка на автобусе)    
enum RoutePartType {
    stop,
    bus
};

//часть пути    
struct RoutePart {
    RoutePartType type;
    std::string_view name;
    double time = 0.0;
    int span_count = 0;
};

//информация по полному пути    
struct RouteInfo {
    double total_time = 0.0;
    std::vector<RoutePart> parts;
};

//запрос на добавление в базу    
struct RequestToAdd {
    RequestToAdd() = default;

    RequestType type = RequestType::null; //type of add request
    std::string name = ""; //bus or stop name
    geo::Coordinates coordinates = {0., 0.}; //stop coordinates
    std::vector<std::string> stops = {}; //bus stops
    std::map<std::string, int64_t> distances = {};//stop to stop distances
    bool is_circular = true; //bus route type

};

//запроса на вывод данных    
struct RequestToStat {
    RequestToStat() = default;

    RequestType type = RequestType::null; //type of add request
    std::string name = ""; //bus or stop name
    int id = 0; // stat request id
    std::string from = ""; // route request stop from
    std::string to = ""; // route request stop to

};

}//namepsace domain
