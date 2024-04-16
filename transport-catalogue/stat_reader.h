#pragma once

#include <iosfwd>
#include <iostream>
#include <string_view>

#include "transport_catalogue.h"

void ParseAndPrintStat(const transport_catalogue::TransportCatalogue& tansport_catalogue, std::string_view request,
                       std::ostream& output);
