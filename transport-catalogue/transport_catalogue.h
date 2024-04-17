#pragma once

#include "geo.h"
#include <deque>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <set>

namespace transport_catalogue
{

struct Stop {
    std::string name;     
    geo::Coordinates coordinates;
};
 
struct Bus {
    std::string name;
    std::vector<const Stop*>stops;
    std::unordered_set<const Stop*> unique_stops;
};

class TransportCatalogue {
    public:
    
	void AddStop(std::string_view stop_name, const geo::Coordinates& coordinates);
    const Stop* FindStop(std::string_view stop_name)const;
    
    void AddBus(std::string_view bus_name, const std::vector<std::string_view>& stops);
    const Bus* FindBus(std::string_view bus_name)const;
    
    double GetBusDistance(const Bus* bus)const;
    
    std::set<std::string_view> GetBusesOfStop(const Stop* stop)const;
    
    private:
    
    void FillStopname_To_Bus(const Bus* bus);
    
    std::deque<Stop> all_stops_;
    std::unordered_map<std::string_view, const Stop*, std::hash<std::string_view> > stopname_to_stop_;
    
    std::deque<Bus> all_buses_;
    std::unordered_map<std::string_view, const Bus*, std::hash<std::string_view> > busname_to_bus_;

    std::unordered_map<const Stop*, std::set<std::string_view>> stop_to_busname_;
};
}
