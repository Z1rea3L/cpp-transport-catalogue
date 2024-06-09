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
    
	void AddStop(std::string_view stop_name, const geo::Coordinates& coordinates); //добавляем остановку
    const Stop* FindStop(std::string_view stop_name)const; //ищем остановку
    
    void AddBus(std::string_view bus_name, const std::vector<std::string_view>& stops, bool is_circular); //добавляем автобус
    const Bus* FindBus(std::string_view bus_name)const; //ищем автобус
    
    std::pair<int, double> GetBusDistAndCurvature(const Bus* bus)const; //получаем дистанцию и кривизну между остановками автобуса
        
    std::set<std::string_view> GetBusesOfStop(const Stop* stop)const;//получаем автобусы для остановки
    
    void SetStopsDistance(const Stop* stop_lhs, const Stop* stop_rhs, int distance); //устанавливаем дистанцию между 2 остановками
    int GetStopsDistance(const Stop* stop_lhs, const Stop* stop_rhs)const; //получаем дистанцию между 2 остановками

    std::unordered_map<std::string_view, Stop*> GetStopsMap()const; //получаем словарь остановок
    std::unordered_map<std::string_view, Bus*> GetBusesMap()const; //получаем словарь автобусов

    std::vector<const Bus*> GetBusesVec() const; //получаем вектор всех автобусов 
    std::vector<const Stop*> GetStopsVec() const; //получаем вектор всех остановок

    private:
    
    struct StopsDistanceHasher { //хэшер для словаря дистанций
        size_t operator()(const std::pair <const Stop*, const Stop*> stops) const {
            size_t stop_first_hash = std::hash<const void*>{}(stops.first);
            size_t stop_second_hash = std::hash<const void*>{}(stops.second);
            return stop_first_hash + stop_second_hash; 
        }
    };
    
    void FillStopnameToBus(const Bus* bus); //заполняем stop_to_busname_
    
    std::deque<Stop> all_stops_; //все остановки
    std::unordered_map<std::string_view, Stop*, std::hash<std::string_view> > stopname_to_stop_; //(имя остановки - остановка*)
    
    std::deque<Bus> all_buses_; //все автобусы
    std::unordered_map<std::string_view, Bus*, std::hash<std::string_view> > busname_to_bus_; //(имя автобуса - автобус*)

    std::unordered_map<const Stop*, std::set<std::string_view>> stop_to_busname_; //(остановка* - список автобусов для остановки)
    
    std::unordered_map<std::pair<const Stop*, const Stop*>, int, StopsDistanceHasher> stops_distances_;//дистанции между 2 остановок
};
}
