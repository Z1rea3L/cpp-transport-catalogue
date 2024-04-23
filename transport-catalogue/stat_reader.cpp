#include "stat_reader.h"

void ParseAndPrintStat(const transport_catalogue::TransportCatalogue& transport_catalogue, std::string_view request, std::ostream& output) {
    
    std::string_view parsed_request = request.substr(0, request.find(' '));
    std::string_view request_data = request.substr(request.find(' ')+1,request.size());

    if(parsed_request == "Bus"){
        const auto bus = transport_catalogue.FindBus(request_data);
        if(bus==nullptr){
            output<<request<<": not found"<<std::endl;
        }else{
            std::pair<int,double> dist_and_curvature = transport_catalogue.GetBusDistAndCurvature(bus);
            output<<request<<": "<<bus->stops.size()<<" stops on route, "
                << bus->unique_stops.size()<<" unique stops, " 
                << dist_and_curvature.first<<" route length, "
                << dist_and_curvature.second<<" curvature"
                << std::endl;
        }
    }
    
    if(parsed_request == "Stop"){
        const auto stop = transport_catalogue.FindStop(request_data);
        if(stop==nullptr){
            output<<request<<": not found"<<std::endl;
            
        }else if(transport_catalogue.GetBusesOfStop(stop).size()==0){
            output<<request<<": no buses"<<std::endl;
            
        }else{
            output<<request<<": buses ";
            for(const std::string_view bus: transport_catalogue.GetBusesOfStop(stop)){
                output<<bus<<" ";
            }
            output<<std::endl;
        }
    }
}
