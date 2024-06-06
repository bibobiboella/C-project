#include "Helper.h"
#include "Matrix.h"
map<string, map<string,string>> AnnouncementInfo;
string earliestDate = "", latestDate = "";
string TF(bool a)
{
    if(a) return "True";
    return "False";
}
map <string, string> parser(string s)
{
    istringstream ss(s);
    string value;
    map<string, string> info;
    getline(ss,value,',');
    info["Date"] = value;
    getline(ss,value,',');
    info["Open"] = value;
    getline(ss,value,',');
    info["High"] = value;
    getline(ss,value,',');
    info["Low"] = value;
    getline(ss,value,',');
    info["Close"] = value;
    getline(ss,value,',');
    info["Adjusted_close"] = value;
    getline(ss,value,',');
    info["Volume"] = value;
    return info;
}

string toUpperCase(const string& input) {
    string output = input;  // 复制输入字符串到输出
    for (char& c : output) {  // 使用引用来直接修改字符串中的字符
        c = std::toupper(c);  // 将每个字符转换为大写
    }
    return output;
}

void populateSymbolVector(const char* FileName, vector<string>& symbols)
{
    ifstream fin;
    fin.open(FileName, ios::in);
    string line, name, symbol;
    while (!fin.eof())
    {
        getline(fin, line);
        stringstream sin(line);
        getline(sin, symbol, ',');
        getline(sin, name);
        symbols.push_back(symbol);
    }
}
void populateSymbolVector(map<double, vector<string>> suprise, vector<string>& symbols)
{
    for(const auto& p: suprise )
    {
        for(const auto& s: p.second)
        {
            symbols.push_back(s);
        }
    }
}
void populateSuprise(const char* FileName,
                     map<string, map<string,string>> &AnnouncementInfo,
                     map<double, vector<string>> &SupriseMap)
{
    ifstream fin;
    fin.open(FileName, ios::in);
    if(!fin.is_open()){
        cout<< "Fail to Open the file."<<endl;
        return;
    }
    string line, name, symbol, value;
    getline(fin, line);
    while (!fin.eof())
    {
        getline(fin, line);
        stringstream sin(line);
        getline(sin, symbol, ',');
        getline(sin, value, ',');
        AnnouncementInfo[symbol]["Date"] = convertDateFormat(value);
        getline(sin, value, ',');
        AnnouncementInfo[symbol]["Period_End"] = value;
        getline(sin, value, ',');
        AnnouncementInfo[symbol]["Estimate"] = value;
        getline(sin, value, ',');
        AnnouncementInfo[symbol]["Reported"] =value;
        getline(sin, value, ',');
        AnnouncementInfo[symbol]["Suprise"] =value;
        getline(sin, value, ',');
        AnnouncementInfo[symbol]["Suprise_Percentage"] =value;
        SupriseMap[stod(value)].push_back(symbol);
    }
}

string findStringInVectors(const std::string& str, const std::vector<std::string>& vec1,
                        const std::vector<std::string>& vec2, const std::vector<std::string>& vec3) {
    // 在第一个向量中查找字符串
    for (size_t i = 0; i < vec1.size(); ++i) {
        if (vec1[i] == str) {
            return "Beat"; // 找到了，返回第一个向量的索引0
        }
    }

    // 在第二个向量中查找字符串
    for (size_t i = 0; i < vec2.size(); ++i) {
        if (vec2[i] == str) {
            return "Meet"; // 找到了，返回第二个向量的索引1
        }
    }

    // 在第三个向量中查找字符串
    for (size_t i = 0; i < vec3.size(); ++i) {
        if (vec3[i] == str) {
            return "Miss"; // 找到了，返回第三个向量的索引2
        }
    }

    return "None"; // 如果在所有向量中都没有找到，返回-1
}



void to3Group(int &length, const vector<string> &symbol,vector<string>&Miss,
              vector<string> &Meet, vector<string> &Beat)
{
    int group_size = length / 3;
    int i = 0;
    for (const auto& s:symbol)
    {
        if(i < group_size) Miss.push_back(s);
        else if (i >= group_size *2) Beat.push_back(s);
        else Meet.push_back(s);
        i++;
    }
}


void* myrealloc(void* ptr, size_t size)
{
    if (ptr)
        return realloc(ptr, size);
    else
        return malloc(size);
}

int write_data2(void* ptr, size_t size, size_t nmemb, void* data)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct* mem = (struct MemoryStruct*)data;
    mem->memory = (char*)myrealloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory) {
        memcpy(&(mem->memory[mem->size]), ptr, realsize);
        mem->size += realsize;
        mem->memory[mem->size] = 0;
    }
    return realsize;
}


std::string convertDateFormat(const std::string& inputDate) {
    struct tm tm = {};
        std::istringstream ss(inputDate);

        // Parsing the input date format
        if (strptime(inputDate.c_str(), "%d-%b-%y", &tm) == nullptr) {
            return "Invalid date";
        }

        // Correcting the year to handle yy format properly
        if (tm.tm_year < 100) {
            tm.tm_year += 100;
        }

        // Formatting the date into ISO 8601 format
        std::ostringstream output;
        output << std::put_time(&tm, "%Y-%m-%d");

        return output.str();
}


// 用于将字符串日期转换为chrono::system_clock::time_point
chrono::system_clock::time_point parse_date(const std::string& date_str) {
    tm tm = {};
    istringstream ss(date_str);
    ss >> get_time(&tm, "%Y-%m-%d");
    return chrono::system_clock::from_time_t(std::mktime(&tm));
}

// 比较两个日期字符串的函数
bool is_date1_before_date2(const std::string& date1, const std::string& date2) {
    auto tp1 = parse_date(date1);
    auto tp2 = parse_date(date2);
    return tp1 < tp2;
}

string addDays(const std::string& dateString, int days) {
    tm date = {};
    istringstream ss(dateString);
    ss >> std::get_time(&date, "%Y-%m-%d");

    date.tm_mday += days;
    mktime(&date);

    ostringstream oss;
    oss << put_time(&date, "%Y-%m-%d");
    return oss.str();
}

void CalculateAAR(vector<double>&AAR, const vector<string>&randomS,
                  map<string, Stock>&sampleGroup,  map<string,Stock>& Group,
                  vector<map<string,string>> IWV)
{
    for(const auto & i: randomS)
    {
        sampleGroup[i] = Group[i];
        sampleGroup[i].CalculateDailyReturn();
        sampleGroup[i].CalculateIWVReturn(IWV);
        sampleGroup[i].CalculateAbnormalReturn();
        AAR = AAR + sampleGroup[i].GetAbnormalReturn();
    }
    AAR = 100* (1.0/randomS.size())* AAR;
}

void CalculateCAAR(vector<double> &CAAR, vector<double>&AAR)
{
    double sum = 0.0;
    for(const auto& i: AAR)
    {
        sum+=i;
        CAAR.push_back(sum);
    }
}


void CalculateAvg_CAAR(vector<double> &avgCAAR,map<string, vector<vector<double>>>&CAAR, string group)
{
    for(auto& i: CAAR[group])
    {
        avgCAAR += i;
    }
    avgCAAR = (1.0/40.0) * avgCAAR;
}

void CalculateAvg_AAR(vector<double> &avgAAR,map<string, vector<vector<double>>>&AAR, string group)
{
    for(auto& i: AAR[group])
    {
        avgAAR += i;
    }
    avgAAR = (1.0/40.0) * avgAAR;
}

void CalculateSTD(vector<double> &STD, const vector<double>&Avg ,map<string, vector<vector<double>>> &Data, string group)
{
    for(auto& i: Data[group])
    {
        STD = STD + (i-Avg) * (i-Avg);
    }
    STD = sqrt((1.0 / 40.0) * STD);
}
//read data
int task(CURL* handle, CURLcode status, vector<string>&symbolList, map<string, Stock>&Group, int n)
{
    string url_common = "https://eodhistoricaldata.com/api/eod/";
    string start_date = earliestDate;
    string end_date = latestDate;
    string api_token = "66031650addfd3.25062713";  // You must replace this API token with yours
//    CURLcode status2;
    
    handle = curl_easy_init();
//    status = status2;
    //decide if it++ later
    for (auto it =  symbolList.begin(); it!=symbolList.end();)
    {
        
        struct MemoryStruct data;
        data.memory = NULL;
        data.size = 0;
        
        vector<map<string, string>> currentStock; //用来作构造函数参数
        
        string symbol = *it;
//        if(i!="GS") continue;
//        cout<< (i=="CAT")<<endl;
        string url_request = url_common + symbol + ".US?" + "from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d";
        curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());
        
        //adding a user agent
        curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
        curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
        curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
        
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&data);
        
        // retrieve data
        status = curl_easy_perform(handle);
        
        if (status != CURLE_OK)
        {
            cout << "curl_easy_perform() failed: " << curl_easy_strerror(status) << endl;
            break;
            return -1;
        }
        
        stringstream sData;
        sData.str(data.memory);
        string line;
        // cout << symbol << endl;

        
        //            cout << left<<setw(10)<<"Symbol"<< right<<setw(10)<< "Date"<< left<<setw(10)<< left<<setw(10)<<"   Open"<<left<<setw(10)<<"   Close" << right<<setw(12)<<"   DailyRtn"<< right<<setw(10)<<"   CumDailyRtns"<<endl;
        // cout<<data.memory;
        while (getline(sData, line)) {
            size_t found = line.find('-');
            if (found != std::string::npos)
            {
//
//                double Daily = (stod(parser(line)["Close"])- stod(parser(line)["Open"]))/stod(parser(line)["Open"]);
//                cumdaily = cumdaily + Daily;
//                cout << left<<setw(10)<<symbol<< right<<setw(10)<< parser(line)["Date"] <<"   "
//                << left<<setw(10) << fixed << setprecision(2) << stod(parser(line)["Open"])
//                <<left<<setw(5) << fixed << setprecision(2) << stod(parser(line)["Close"])
//                <<right<<setw(14)<<fixed << setprecision(6) <<Daily
//                <<"   "<<right<<setw(10)<<fixed << setprecision(6) <<cumdaily<< endl;
                //parser: map to open, close, adj... and corresponding value
                //current stock is iterator now
                currentStock.push_back(parser(line));
            }
        }
        //avoid bootstrap IWV later, erase IWV(since a benchmark) but make it to point to next
        if (*it=="IWV") {Group[*it] = Stock(*it, currentStock); it = symbolList.erase(it);}
        else
        {
            //it == stock name
            //curent stock = close/open/adj info
            Group[*it] = Stock(*it, currentStock,AnnouncementInfo[*it], n);
            //if we get warning, erase and point to next
            if(Group[*it].GetWarning())
            {
                it = symbolList.erase(it);
            }
            else ++it;
        }
        free(data.memory);
        data.size = 0;
    }
    return 0;
}
