#include "input_reader.h"

#include <algorithm>
#include <cassert>
#include <iterator>

CommandDescription::operator bool() const {
    return !command.empty();
}

bool CommandDescription::operator!() const {
    return !CommandDescription::operator bool();
}

/**
 * Парсит строку вида "10.123,  -30.1837" и возвращает пару координат (широта, долгота)
*/

geo::Coordinates ParseCoordinates(std::string_view str) {
    static const double nan = std::nan("");

    const auto not_space = str.find_first_not_of(' ');
    const auto comma = str.find(',');

    if (comma == str.npos) {
        return {nan, nan};
    }

    const auto not_space2 = str.find_first_not_of(' ', comma + 1);

    const double latitude = std::stod(std::string(str.substr(not_space, comma - not_space)));
    const double longitude = std::stod(std::string(str.substr(not_space2)));

    return {.lat = latitude, .lng = longitude};
}

/**
 * Удаляет пробелы в начале и конце строки
 */
std::string_view Trim(std::string_view string) {
    const auto start = string.find_first_not_of(' ');
    if (start == string.npos) {
        return {};
    }
    return string.substr(start, string.find_last_not_of(' ') + 1 - start);
}

/**
 * Разбивает строку string на n строк, с помощью указанного символа-разделителя delim
 */
std::vector<std::string_view> Split(std::string_view string, char delim) {
    std::vector<std::string_view> result;

    size_t pos = 0;
    while ((pos = string.find_first_not_of(' ', pos)) < string.length()) {
        auto delim_pos = string.find(delim, pos);
        if (delim_pos == string.npos) {
            delim_pos = string.size();
        }
        if (auto substr = Trim(string.substr(pos, delim_pos - pos)); !substr.empty()) {
            result.push_back(substr);
        }
        pos = delim_pos + 1;
    }

    return result;
}

/**
 * Парсит маршрут.
 * Для кольцевого маршрута (A>B>C>A) возвращает массив названий остановок [A,B,C,A]
 * Для некольцевого маршрута (A-B-C-D) возвращает массив названий остановок [A,B,C,D,C,B,A]
 */
std::vector<std::string_view> ParseRoute(std::string_view route) {
    if (route.find('>') != route.npos) {
        return Split(route, '>');
    }

    auto stops = Split(route, '-');
    std::vector<std::string_view> results(stops.begin(), stops.end());
    results.insert(results.end(), std::next(stops.rbegin()), stops.rend());

    return results;
}

CommandDescription ParseCommandDescription(std::string_view line) {
    auto colon_pos = line.find(':');
    if (colon_pos == line.npos) {
        return {};
    }

    auto space_pos = line.find(' ');
    if (space_pos >= colon_pos) {
        return {};
    }

    auto not_space = line.find_first_not_of(' ', space_pos);
    if (not_space >= colon_pos) {
        return {};
    }

    return {std::string(line.substr(0, space_pos)),
            std::string(line.substr(not_space, colon_pos - not_space)),
            std::string(line.substr(colon_pos + 1))};
}

void InputReader::ParseLine(std::string_view line) {
    auto command_description = ParseCommandDescription(line);
    if (command_description) {
        commands_.push_back(std::move(command_description));
    }
}

std::deque<std::pair<int, std::string>>ParseStopDescription(std::string description){
    std::vector<std::string_view> splited_description = Split(description, ',');
    if(splited_description.size()<=2){// первые 2 элемента это координаты, если больше ничего нет возврат пустоты
        return {};
    }
    std::deque<std::pair<int, std::string>> result;  //дек (расстояние - остановка)

    const size_t vec_size = splited_description.size();

    for(size_t i = 1; i<vec_size; ++i){ //первые 2 элемента это координаты, начинаем с третьего элемента
        std::string temp{splited_description[i].data(),splited_description[i].size()};
        int distance = std::stoi(temp.substr(0, temp.find('m')));
        std::string stop_name = temp.substr(temp.find('o')+2);
        result.push_back({distance, stop_name});
    }

    return result;
}  
    
void InputReader::ApplyCommands([[maybe_unused]]transport_catalogue::TransportCatalogue& catalogue) const {
  std::unordered_map <std::string_view, std::deque<std::pair<int, std::string>>> stop_to_stop_distances; //остановка - дек(расстояние-остановка)
  for(const auto& request : commands_) {
        if(request.command == "Stop") {
            catalogue.AddStop(request.id, ParseCoordinates(request.description));
            stop_to_stop_distances[request.id]=ParseStopDescription(request.description);
        }else{
            continue;
        }
    }
   for(const auto& request : commands_) {
        if(request.command == "Bus") {
            catalogue.AddBus(request.id, ParseRoute(request.description));
        }else{
            continue;
        }
    }

    for(const auto& [stop_first, deque_of_stops]:stop_to_stop_distances){ // заполняем расстояния между остановками
        for(const auto& [distance, stop_second]:deque_of_stops){
            catalogue.SetStopsDistance(catalogue.FindStop(stop_first),catalogue.FindStop(stop_second),distance);
        }
    }
}
