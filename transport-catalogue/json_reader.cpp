#include "json_reader.h"
#include "request_handler.h"

namespace json_reader {

    JsonReader::JsonReader(std::istream& input, std::ostream& output)
            : input_(json::Load(input))
            , output_(output)
    {
        if(input_.GetRoot().AsMap().contains("base_requests")){
            MakeBase(input_.GetRoot().AsMap().at("base_requests"));
        }
                
        if(input_.GetRoot().AsMap().contains("stat_requests")){
            ParseStatRequests(input_.GetRoot().AsMap().at("stat_requests"));
        }
                
        if(input_.GetRoot().AsMap().contains("render_settings")){
            ParseRenderSettings(input_.GetRoot().AsMap().at("render_settings").AsMap());
        }
        
        request_handler::RequestHandler handler_(base_requests_, stat_requests_, render_data_);
    }
    
    svg::Color ParsingColor(const json::Node& node){
        if(node.IsArray()){
            auto array = node.AsArray();
            if(array.size() == 4){
                return svg::Color(svg::Rgba(array[0].AsInt()
                                            , array[1].AsInt()
                                            , array[2].AsInt()
                                            , array[3].AsDouble()));
            }
            return svg::Color(svg::Rgb(array[0].AsInt()
                                       , array[1].AsInt()
                                       , array[2].AsInt()));
        }else{
            return svg::Color(node.AsString());
        }
    }
    
    void JsonReader::ParseRenderSettings(const std::map<std::string,json::Node>& settings){
        
        render_data_.width_ = settings.at("width").AsDouble();
        render_data_.height_ = settings.at("height").AsDouble();
        render_data_.padding_ = settings.at("padding").AsDouble();
        render_data_.line_width_ = settings.at("line_width").AsDouble();
        render_data_.stop_radius_ = settings.at("stop_radius").AsDouble();
        render_data_.bus_label_font_size_ = settings.at("bus_label_font_size").AsDouble();
        render_data_.stop_label_font_size_ = settings.at("stop_label_font_size").AsDouble();
        render_data_.underlayer_width_ = settings.at("underlayer_width").AsDouble();
        
        auto temp = settings.at("bus_label_offset").AsArray();
        render_data_.bus_label_offset_ = svg::Point(temp[0].AsDouble(),temp[1].AsDouble());
        
        temp = settings.at("stop_label_offset").AsArray();
        render_data_.stop_label_offset_ = svg::Point(temp[0].AsDouble(),temp[1].AsDouble());
        
        render_data_.underlayer_color_ = ParsingColor(settings.at("underlayer_color"));

        for(const auto& elem : settings.at("color_palette").AsArray()){
            if(elem.IsString()){
                render_data_.color_palette_.push_back(elem.AsString());
            }else{
                auto vec = elem.AsArray();
                render_data_.color_palette_.push_back(ParsingColor(vec));
            }
        }
        
    }
    
    void JsonReader::MakeBase(const json::Node& arr_base){

        for(const auto& elem :arr_base.AsArray()){
            if(elem.AsMap().at("type").AsString()=="Stop"){
                ParseStopRequest(elem.AsMap());
            }
            if(elem.AsMap().at("type").AsString()=="Bus"){
                ParseBusRequest(elem.AsMap());
            }
        } 
    }

    void JsonReader::ParseStopRequest(const std::map<std::string,json::Node>& stop_request){
        domain::RequestToAdd req;
        std::map<std::string,int64_t> temp;

        req.type = domain::RequestType::add_stop;
        req.name = stop_request.at("name").AsString();
        req.coordinates={stop_request.at("latitude").AsDouble(),
                            stop_request.at("longitude").AsDouble()};

        for(const auto& [name, dist] : stop_request.at("road_distances").AsMap()){
            temp.insert({name , dist.AsInt()});
        }

        req.distances = std::move(temp);
        base_requests_.push_back(std::move(req));
    }

    void JsonReader::ParseBusRequest(const std::map<std::string,json::Node>& bus_request){
        domain::RequestToAdd req;
        std::vector<std::string> temp;

        req.type = domain::RequestType::add_bus;
        req.name = bus_request.at("name").AsString();
        req.is_circular = bus_request.at("is_roundtrip").AsBool();

        for(const auto& elem : bus_request.at("stops").AsArray()){
            temp.push_back(elem.AsString());
        }

        req.stops = std::move(temp);
        base_requests_.push_back(std::move(req));
    }

    void JsonReader::ParseStatRequests(const json::Node& requests){
        for(const auto& elem : requests.AsArray()){
            domain::RequestToStat req;

            if(elem.AsMap().at("type").AsString()=="Bus"){
                req.type = domain::RequestType::find_bus;
                req.name = elem.AsMap().at("name").AsString();
                req.id = elem.AsMap().at("id").AsInt();
            }

            if(elem.AsMap().at("type").AsString()=="Stop"){
                req.type = domain::RequestType::find_stop;
                req.name = elem.AsMap().at("name").AsString();
                req.id = elem.AsMap().at("id").AsInt();
            }

            if(elem.AsMap().at("type").AsString()=="Map"){
                req.type = domain::RequestType::render_map;
                req.name = "map";
                req.id = elem.AsMap().at("id").AsInt();
            }

            stat_requests_.push_back(std::move(req));
        }
    }

}//json_reader namespace;
