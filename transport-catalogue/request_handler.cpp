#include "request_handler.h"


namespace request_handler {
    
RequestHandler::RequestHandler(transport_catalogue::TransportCatalogue& transport_catalogue)
    :transport_catalogue_(transport_catalogue){

}
    
void RequestHandler::SetRenderSettings(map_renderer::RendererSettings settings){
    render_.SetRenderSettings(settings);
}

void RequestHandler::ProcessStatRequests(const std::deque<domain::RequestToStat>& stat_requests, std::ostream& output){
    json::Array result;
    result.reserve(stat_requests.size());
    for(const auto& request : stat_requests){
        
        if(request.type == domain::RequestType::find_bus){
           result.push_back(RequestHandler::ParseBusInfo(request).GetRoot());
        }
        
        if(request.type == domain::RequestType::find_stop){
            result.push_back(RequestHandler::ParseStopInfo(request).GetRoot()); 
        }

        if(request.type == domain::RequestType::render_map){
            result.push_back(RenderMap(request).GetRoot());
        }
    }
    json::Print(json::Document{result}, output);
}   
    
json::Document RequestHandler::ParseBusInfo(const domain::RequestToStat& request)const{
    const auto bus = transport_catalogue_.FindBus(request.name);
    if (bus == nullptr) {
            return json::Document(
                json::Builder()
                .StartDict()
                .Key("request_id").Value(request.id)
                .Key("error_message").Value("not found")
                .EndDict()
                .Build());
        }
        else {
            std::pair<int,double> dist_and_curvature = transport_catalogue_.GetBusDistAndCurvature(bus);
            return json::Document(
                json::Builder()
                .StartDict()
                .Key("request_id").Value(request.id)
                .Key("curvature").Value(dist_and_curvature.second)
                .Key("route_length").Value(dist_and_curvature.first)
                .Key("stop_count").Value(static_cast<int>(bus->stops.size()))
                .Key("unique_stop_count").Value(static_cast<int>(bus->unique_stops.size()))
                .EndDict()
                .Build());
        }
}
    
json::Document RequestHandler::ParseStopInfo(const domain::RequestToStat& request)const{
    const auto stop = transport_catalogue_.FindStop(request.name);
    
    if (stop == nullptr) {
            return json::Document(
                json::Builder()
                .StartDict()
                .Key("request_id").Value(request.id)
                .Key("error_message").Value("not found")
                .EndDict()
                .Build());
        }
        else {
            json::Array buses;
            for (const auto& bus_name : transport_catalogue_.GetBusesOfStop(stop)) {
                buses.push_back(json::Node{ std::string(bus_name) });
            }
            return json::Document(
                json::Builder()
                .StartDict()
                .Key("buses").Value(buses)
                .Key("request_id").Value(request.id)
                .EndDict()
                .Build());
        }
}

void RequestHandler::FillCatalogue(const std::deque<domain::RequestToAdd>& base_requests_){
    for(const auto& elem : base_requests_){
        if(elem.type==domain::RequestType::add_stop){
            transport_catalogue_.AddStop(elem.name, elem.coordinates);
        }else{
            continue;
        }
    }

    for(const auto& elem : base_requests_){
        if(elem.type==domain::RequestType::add_stop){
            for(const auto& [stopname, distance] : elem.distances){
                transport_catalogue_.SetStopsDistance(transport_catalogue_.FindStop(elem.name),
                                                      transport_catalogue_.FindStop(stopname),distance);
            }
        }else{
            continue;
        }
    }

    for(const auto& elem : base_requests_){
        if(elem.type==domain::RequestType::add_bus){
            transport_catalogue_.AddBus(elem.name, ParseBusRoute(elem.stops, elem.is_circular), elem.is_circular);
        }else{
            continue;
        }
    }
}

std::vector<std::string_view> RequestHandler::ParseBusRoute(const std::vector<std::string>& stops, bool is_roundtrip) const{
    std::vector<std::string_view>result;
    result.reserve(stops.size()*2);
    result.insert(result.begin(),stops.begin(),stops.end());

    if(!is_roundtrip){
        result.insert(result.end(),stops.rbegin()+1,stops.rend());
    }

    return result;
}

json::Document RequestHandler::RenderMap(const domain::RequestToStat& request){
    std::vector<domain::Bus*> vec;

    for(const auto& [name, bus] : transport_catalogue_.GetBusesMap()){
        vec.push_back(bus);
    }
    std::sort(vec.begin(),vec.end(),[](auto lhs, auto rhs){
        return lhs->name < rhs->name;
    });
    
    std::ostringstream strm;
    render_.Render(strm, vec);
    
    return json::Document(
            json::Builder()
            .StartDict()
            .Key("map").Value(strm.str())
            .Key("request_id").Value(request.id)
            .EndDict()
            .Build());
}

}//namespace request_handler