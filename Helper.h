#pragma once
#include <cstring>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <locale>
#include <algorithm>
#include <random>
#include <iomanip>
#include <fstream>
#include <map>
#include "curl/curl.h"
#include "Stocks.h"
#include <chrono>


using namespace std;
//extern vector<string> symbolList;
extern string earliestDate, latestDate;
extern map<string, map<string,string>> AnnouncementInfo;
string TF(bool a);
map <string, string> parser(string s);
string toUpperCase(const string& input);
void populateSymbolVector(const char* FileName, vector<string>& symbols);
void populateSymbolVector(map<double, vector<string>> suprise, vector<string>& symbols);

//Make the Russell3000Earning Announcement into to Maps
void populateSuprise(const char* FileName,
                     map<string, map<string,string>> &AnnouncementInfo,
                     map<double, vector<string>> &SupriseMap);
string findStringInVectors(const std::string& str, const std::vector<std::string>& vec1,
                           const std::vector<std::string>& vec2, const std::vector<std::string>& vec3);
void to3Group(int &length, const vector<string> &symbol,vector<string>&Miss,
              vector<string> &Meet, vector<string> &Beat); //Dividing the Symbol to Beat, Meet, Miss
void* myrealloc(void* ptr, size_t size);
int write_data2(void* ptr, size_t size, size_t nmemb, void* data);

struct MemoryStruct {
    char* memory;
    size_t size;
};
string convertDateFormat(const std::string& dateStr);

// 用于将字符串日期转换为chrono::system_clock::time_point
chrono::system_clock::time_point parse_date(const std::string& date_str);
bool is_date1_before_date2(const std::string& date1, const std::string& date2);
string addDays(const std::string& dateString, int days);

//For boostraping Method
template<typename T>
vector<T> selectRandomElements(const std::vector<T>& data, size_t n) {
    if (n > data.size()) {
        std::cerr << "Requested more elements than are available in the vector." << std::endl;
        return {};
    }

    vector<T> shuffledData = data;
    // 创建随机数生成器
    random_device rd;
    mt19937 g(rd());
    shuffle(shuffledData.begin(), shuffledData.end(), g);
    shuffledData.resize(n);
    return shuffledData;
}

void CalculateAAR(vector<double>&AAR, const vector<string>&randomS,
                  map<string, Stock>&sampleGroup,  map<string,Stock>& Group,
                  vector<map<string,string>> IWV);

void CalculateCAAR(vector<double> &CAAR, vector<double>&AAR);

void CalculateAvg_CAAR(vector<double> &avgCAAR,map<string, vector<vector<double>>>&CAAR, string group);
void CalculateAvg_AAR (vector<double> &avgAAR,map<string, vector<vector<double>>>&AAR, string group);
void CalculateSTD(vector<double> &STD, const vector<double>&Avg ,map<string, vector<vector<double>>> &Data, string group);


int task(CURL* handle, CURLcode status, vector<string>&symbolList, map<string, Stock>&Group, int n); // for multithread use
