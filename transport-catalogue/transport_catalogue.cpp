#include "transport_catalogue.h"
#include <algorithm>

namespace transport_catalogue
{
    void TransportCatalogue::AddStop(std::string_view stop_name, const geo::Coordinates& coordinates){
        Stop temp_stop;
        temp_stop.name = stop_name;
        temp_stop.coordinates = coordinates;
        all_stops_.push_back(std::move(temp_stop));

        stopname_to_stop_.insert({all_stops_.back().name, &all_stops_.back()});
        stop_to_busname_[&all_stops_.back()]={};
        
    }

    const Stop* TransportCatalogue::FindStop(std::string_view stop_name)const{
        if(stopname_to_stop_.contains(stop_name)){
            return stopname_to_stop_.at(stop_name);
        }
        return nullptr;
    }

    void TransportCatalogue::FillStopnameToBus(const Bus* bus){
        for(const Stop* stop: bus->stops){
            stop_to_busname_[stop].insert(bus->name);
        }
    }

    void TransportCatalogue::AddBus(std::string_view bus_name, const std::vector<std::string_view>& stops){
        Bus temp_bus;
        temp_bus.name = bus_name;
        for(const std::string_view& stop : stops){
            temp_bus.stops.push_back(stopname_to_stop_.at(stop));
            temp_bus.unique_stops.insert(stopname_to_stop_.at(stop));
        }

        all_buses_.push_back(std::move(temp_bus));
        busname_to_bus_.insert({all_buses_.back().name,&all_buses_.back()});

        FillStopnameToBus(&all_buses_.back());
    }

    const Bus* TransportCatalogue::FindBus(std::string_view bus_name)const{
        if(busname_to_bus_.contains(bus_name)){
            return busname_to_bus_.at(bus_name);
        }
        return nullptr;
    }
    
    std::set<std::string_view> TransportCatalogue::GetBusesOfStop(const Stop* stop)const{
        return stop_to_busname_.at(stop);
    }
    
    void TransportCatalogue::SetStopsDistance(const Stop* stop_lhs, const Stop* stop_rhs, const int distance){
        stops_distances_[{stop_lhs,stop_rhs}]=distance;
    }
    
    int TransportCatalogue::GetStopsDistance(const Stop* stop_lhs, const Stop* stop_rhs)const{
        if(stops_distances_.contains({stop_lhs,stop_rhs})){
            return stops_distances_.at({stop_lhs,stop_rhs});
        }else if (stops_distances_.contains({stop_rhs,stop_lhs})){
            return stops_distances_.at({stop_rhs,stop_lhs});
        }
        return 0;
    }
    
    std::pair<int,double> TransportCatalogue::GetBusDistAndCurvature(const Bus* bus)const{
        double dist_by_coordinates = 0.;
        int distance_by_stops = 0;
        const int counter = (bus->stops.size())-1;
        for(int i = 0; i< counter; ++i){
            dist_by_coordinates += geo::ComputeDistance(bus->stops[i]->coordinates,bus->stops[i+1]->coordinates);
            distance_by_stops += GetStopsDistance(bus->stops[i],bus->stops[i+1]);
        }
        return {distance_by_stops,distance_by_stops/dist_by_coordinates};
    }
}
