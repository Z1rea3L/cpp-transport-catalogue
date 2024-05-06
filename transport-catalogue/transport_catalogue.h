#pragma once

#include "geo.h"
#include <deque>
#include <unordered_map>
#include <optional>
#include <set>
#include "domain.h"

using namespace domain;

namespace transport_catalogue
{

class TransportCatalogue {
    public:
    
	void AddStop(std::string_view stop_name, const geo::Coordinates& coordinates);
    const Stop* FindStop(std::string_view stop_name)const;
    
    void AddBus(std::string_view bus_name, const std::vector<std::string_view>& stops, bool is_circular);
    const Bus* FindBus(std::string_view bus_name)const;
    
    std::pair<int, double> GetBusDistAndCurvature(const Bus* bus)const;
        
    std::set<std::string_view> GetBusesOfStop(const Stop* stop)const;
    
    void SetStopsDistance(const Stop* stop_lhs, const Stop* stop_rhs, int distance);
    int GetStopsDistance(const Stop* stop_lhs, const Stop* stop_rhs)const;

    std::unordered_map<std::string_view, Stop*> GetStopsMap()const;
    std::unordered_map<std::string_view, Bus*> GetBusesMap()const;

    private:
    
    struct StopsDistanceHasher {
        size_t operator()(const std::pair <const Stop*, const Stop*> stops) const {
            size_t stop_first_hash = std::hash<const void*>{}(stops.first);
            size_t stop_second_hash = std::hash<const void*>{}(stops.second);
            return stop_first_hash + stop_second_hash; 
        }
    };
    
    void FillStopnameToBus(const Bus* bus);
    
    std::deque<Stop> all_stops_;
    std::unordered_map<std::string_view, Stop*, std::hash<std::string_view> > stopname_to_stop_;
    
    std::deque<Bus> all_buses_;
    std::unordered_map<std::string_view, Bus*, std::hash<std::string_view> > busname_to_bus_;

    std::unordered_map<const Stop*, std::set<std::string_view>> stop_to_busname_;
    
    std::unordered_map<std::pair<const Stop*, const Stop*>, int, StopsDistanceHasher> stops_distances_;
};
}
