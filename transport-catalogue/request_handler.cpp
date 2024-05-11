#include "request_handler.h"


namespace request_handler {
    
RequestHandler::RequestHandler(transport_catalogue::TransportCatalogue& transport_catalogue)
    :transport_catalogue_(transport_catalogue){

}
    
void RequestHandler::SetRenderSettings(map_renderer::RendererSettings settings){
    render_.SetRenderSettings(settings);
}

void RequestHandler::ProcessStatRequests(const std::deque<domain::RequestToStat>& stat_requests,std::ostream& output){
    std::vector<json::Node> result;
    for(const auto& request : stat_requests){
        
        if(request.type == domain::RequestType::find_bus){
           result.push_back(RequestHandler::ParseBusInfo(request));
        }
        
        if(request.type == domain::RequestType::find_stop){
            result.push_back(RequestHandler::ParseStopInfo(request)); 
        }

        if(request.type == domain::RequestType::render_map){
            result.push_back(RenderMap(request));
        }
    }
    json::Node node_result(std::move(result));
    json::Document doc(node_result);
    json::Print(doc,output);
}   
    
json::Node RequestHandler::ParseBusInfo(const domain::RequestToStat& request)const{
    const auto bus = transport_catalogue_.FindBus(request.name);
    std::map<std::string, json::Node> bus_info;
    bus_info.insert({"request_id", json::Node(request.id)});
    
    if(bus==nullptr){
        bus_info.insert({"error_message", json::Node(std::string("not found"))});
        
    }else{
         std::pair<int,double> dist_and_curvature = transport_catalogue_.GetBusDistAndCurvature(bus);
         bus_info.insert({"curvature", json::Node(dist_and_curvature.second)});
         bus_info.insert({"route_length", json::Node(dist_and_curvature.first)});
         bus_info.insert({"stop_count", json::Node(static_cast<int>(bus->stops.size()))});
         bus_info.insert({"unique_stop_count", json::Node(static_cast<int>(bus->unique_stops.size()))});
    }
    return json::Node(bus_info);
}
    
json::Node RequestHandler::ParseStopInfo(const domain::RequestToStat& request)const{
    const auto stop = transport_catalogue_.FindStop(request.name);
    std::map<std::string, json::Node> stop_info;
    stop_info.insert({"request_id", json::Node(request.id)});
    
    if(stop==nullptr){
        stop_info.insert({"error_message", json::Node(std::string("not found"))});
        
    }else{
        std::vector<json::Node> buses_of_stop;
        for(const auto& bus_name : transport_catalogue_.GetBusesOfStop(stop)){
            std::string temp_name{bus_name.data(),bus_name.size()};
            buses_of_stop.push_back(json::Node(temp_name));
        }
        stop_info.insert({"buses",json::Node(buses_of_stop)});
    }
    return json::Node(stop_info);
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

json::Node RequestHandler::RenderMap(const domain::RequestToStat& request){

    std::vector<domain::Bus*> vec;

    for(const auto& [name, bus] : transport_catalogue_.GetBusesMap()){
        vec.push_back(bus);
    }
    std::sort(vec.begin(),vec.end(),[](auto lhs, auto rhs){
        return lhs->name < rhs->name;
    });
    
    std::ostringstream strm;
    render_.Render(strm, vec);

    std::map<std::string, json::Node> result;
    result.insert({"map" , json::Node(strm.str())});
    result.insert({"request_id" , json::Node(request.id)});

    return json::Node(result);
}

}//namespace request_handler
