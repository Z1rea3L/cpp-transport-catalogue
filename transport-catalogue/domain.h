#pragma once

#include "geo.h"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <map>

namespace domain{

struct Stop {
    std::string name;
    geo::Coordinates coordinates;
};

struct Bus {
    std::string name;
    std::vector<Stop*>stops;
    std::unordered_set<Stop*> unique_stops;
    bool is_circular = false;
};    

enum RequestType {
    null = 0,
    add_stop,
    add_bus,
    find_stop,
    find_bus,
    render_map
};

struct RequestToAdd {
    RequestToAdd() = default;

    RequestType type = RequestType::null; //type of add request
    std::string name = ""; //bus or stop name
    geo::Coordinates coordinates = {0., 0.}; //stop coordinates
    std::vector<std::string> stops = {}; //bus stops
    std::map<std::string, int64_t> distances = {};//stop to stop distances
    bool is_circular = true; //bus route type

};

struct RequestToStat {
    RequestToStat() = default;

    RequestType type = RequestType::null; //type of add request
    std::string name = ""; //bus or stop name
    int id = 0; // stat request id

};

}//domain
