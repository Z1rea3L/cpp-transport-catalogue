#include <fstream>
#include <iostream>
#include "json_reader.h"
#include "request_handler.h"

int main() {
    json_reader::JsonReader reader(std::cin); //считываем данные json из потока
    transport_catalogue::TransportCatalogue catalogue; //создаем транспортный каталог
    request_handler::RequestHandler handler(catalogue); //создаем обработчик данных для каталога
    
    //вся обработка разделена на отдельные этапы для возможности перевызова при изменении запросов/настроек
    
    handler.FillCatalogue(reader.GetDataBase()); //заполняем транспортный каталог
    handler.SetRenderSettings(reader.GetRenderSettings()); //устанавливаем настройки рендера
    handler.SetRouterSettings(reader.GetRoutingSettings()); //устнавливаем настройки пути
    handler.MakeRouter();//создаем базу путей по транспортному каталогу который привязан к обработчику
    handler.ProcessStatRequests(reader.GetRequestBase(),std::cout); //обрабатываем запросы вывода

}
