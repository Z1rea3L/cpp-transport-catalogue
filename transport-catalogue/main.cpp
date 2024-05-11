#include <fstream>
#include <iostream>
#include "json_reader.h"
#include "request_handler.h"

int main() {
    json_reader::JsonReader reader(std::cin);
    transport_catalogue::TransportCatalogue catalogue;
    request_handler::RequestHandler handler(catalogue);
    
    handler.FillCatalogue(reader.GetDataBase());
    handler.SetRenderSettings(reader.GetRenderSettings());
    handler.ProcessStatRequests(reader.GetRequestBase(),std::cout);

}
