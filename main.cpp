#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <fstream>
#include <limits>
#include <ios>

#include <nlohmann/json.hpp>
#include <fmt/core.h>

#include "include/space_station.hpp"
#include "include/menu.hpp"
#include "include/celestial_body.hpp"
using SpaceStation = KSP_SM::SpaceStationBuilder::SpaceStation;
using SpaceStationBuilder = KSP_SM::SpaceStationBuilder;
using namespace KSP_SM;
using nlohmann::json;
using std::vector;
typedef std::unique_ptr<SpaceStation> station_uniq_ptr;


int main(int argc, char ** argv);

unsigned short readStationsFromFile(const string& filename, vector<station_uniq_ptr>& stations);
station_uniq_ptr createStation();
void listAllStations(const vector<station_uniq_ptr>& stations);
void writeStationsToFile(const string& filename, json stations_json);

constexpr string STATIONS_FILENAME = "stations.json";

int main(int argc, char **argv)
{

    vector<station_uniq_ptr> stations;
    bool exitProgram = false;
    std::string buffer;
    std::string menuText = Menu::getMenuText();

    

    while (!exitProgram)
    {
        std::cout << std::endl;
        std::cout << menuText;
        std::cout << "Enter Your Selection: ";
        std::cin >> buffer;
        if (buffer.compare("1") == 0)
        {
            // Attempt to read stations from file. Result is the number of stations read from json file.
            auto number_of_stations = readStationsFromFile(STATIONS_FILENAME, stations);
            if(!number_of_stations) // Show an error if no stations are found / file not found.
            {
                std::cerr << "Aborting." << std::endl;
                continue;
            }
            // Successful load, print number of stations loaded and go back to the main menu.
            std::cout << fmt::format("Read in {} stations from file {}.", stations.size(), STATIONS_FILENAME) << std::endl;
            continue;
        }
        if (buffer.compare("2") == 0)
        {
            json stations_json = stations;
            writeStationsToFile(STATIONS_FILENAME, stations_json);
            
            std::cout << "Wrote stations list to file." << std::endl << std::endl;
            continue;
        }
        if (buffer.compare("3") == 0)
        {
            auto newStation = SpaceStationBuilder::createStationFromConsoleInput();
            stations.push_back(std::move(newStation));
            continue;
        }
        if (buffer.compare("4") == 0)
        {
            listAllStations(stations);
            continue;
        }
        if (buffer.compare("q") == 0)
        {
            exitProgram = true;
            continue;
        }

        std::cerr << "Error reading selection." << std::endl;
    }

    return 0;
}

unsigned short readStationsFromFile(const string& filename, vector<station_uniq_ptr>& stations)
{
    std::ifstream in_file(filename);
    if(!in_file)
    {
        std::cerr << fmt::format("Error: {} not found.", filename) << std::endl;
        return 0;
    }
    json j;
    in_file >> j;
    in_file.close(); // close file when done!
    
    stations.clear(); // clear stations vector prior to loading stations from json
    stations = j.get<vector<station_uniq_ptr>>(); // convert json to vector of unique_ptrs to json
    return stations.size();
}

void listAllStations(const vector<station_uniq_ptr>& stations)
{
    for(auto i = 0; i < stations.size(); ++i)
    {
        std::cout << fmt::format("{}) {}", i, stations.at(i)->ToString()) << std::endl;
    }
}

void writeStationsToFile(const string& filename, json stations_json)
{
    std::ofstream out_file(STATIONS_FILENAME);
    out_file << stations_json.dump(4) << std::endl;
    out_file.close(); // close file when done!
}