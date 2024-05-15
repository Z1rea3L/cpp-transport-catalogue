#include "json_reader.h"

namespace json_reader {

    JsonReader::JsonReader(std::istream& input){
        ReadJson(input);
    }
    
    void JsonReader::ReadJson(std::istream& input){
        json::Document input_ = json::Load(input);
        if(!base_requests_.empty())base_requests_.clear();//если снова вызываем чтение - чистим базу запросов
        if(!stat_requests_.empty())stat_requests_.clear();
        
        if(input_.GetRoot().AsDict().contains("base_requests")){
            ParseDataRequests(input_.GetRoot().AsDict().at("base_requests"));
        }
                
        if(input_.GetRoot().AsDict().contains("stat_requests")){
            ParseStatRequests(input_.GetRoot().AsDict().at("stat_requests"));
        }
                
        if(input_.GetRoot().AsDict().contains("render_settings")){
            ParseRenderSettings(input_.GetRoot().AsDict().at("render_settings").AsDict());
        }
        
    }
    
    const std::deque<domain::RequestToAdd> JsonReader::GetDataBase()const{
        return base_requests_;
    }
    
    const std::deque<domain::RequestToStat> JsonReader::GetRequestBase()const{
        return stat_requests_;
    }
    
    map_renderer::RendererSettings JsonReader::GetRenderSettings()const{
        return render_data_;
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
    
    void JsonReader::ParseDataRequests(const json::Node& arr_base){

        for(const auto& elem :arr_base.AsArray()){
            if(elem.AsDict().at("type").AsString()=="Stop"){
                ParseStopRequest(elem.AsDict());
            }
            if(elem.AsDict().at("type").AsString()=="Bus"){
                ParseBusRequest(elem.AsDict());
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

        for(const auto& [name, dist] : stop_request.at("road_distances").AsDict()){
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

            if(elem.AsDict().at("type").AsString()=="Bus"){
                req.type = domain::RequestType::find_bus;
                req.name = elem.AsDict().at("name").AsString();
                req.id = elem.AsDict().at("id").AsInt();
            }

            if(elem.AsDict().at("type").AsString()=="Stop"){
                req.type = domain::RequestType::find_stop;
                req.name = elem.AsDict().at("name").AsString();
                req.id = elem.AsDict().at("id").AsInt();
            }

            if(elem.AsDict().at("type").AsString()=="Map"){
                req.type = domain::RequestType::render_map;
                req.name = "map";
                req.id = elem.AsDict().at("id").AsInt();
            }

            stat_requests_.push_back(std::move(req));
        }
    }

}//json_reader namespace;
