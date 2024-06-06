#include <cstring>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <locale>
#include <iomanip>
#include <fstream>
#include <map>
#include <chrono>
#include<thread>

#include "curl/curl.h"
#include "Helper.h"
#include "Stocks.h"
#include "Matrix.h"
#include "gnuplot.h"
//include "Bootstrap.h"

using namespace std;


int main(void)
{
    int M = 30; //number of random Sample
    int N = 90 ; // number of Days
    vector<vector<vector<double>>> INFO(3);

    
    vector<double> avg_AARMiss, avg_AARMeet, avg_AARBeat;
    vector<double> avg_CAARMiss, avg_CAARMeet, avg_CAARBeat;
    vector<double> STDAAR, STDCAAR;
    vector<string> Group = {"Beat", "Meet", "Miss"};
    
    vector<string> symbolList;
    //double: surprice% string: ticker name
    map<double, vector<string>> SupriseMap; //handle same price issue
    vector<string> SMiss, SMeet, SBeat; //symbol List groups
    map<string, Stock> Miss, Meet, Beat; //store class
    const char* FileName = "Russell3000EarningsAnnouncements.csv";
    //update surprise map: <double surprise%, vactor of ticker names that is the same surprise %>
    populateSuprise(FileName, AnnouncementInfo, SupriseMap); //in helper, info in russell3000
    for(auto it = AnnouncementInfo.begin(); it!= AnnouncementInfo.end(); it++)
    {
        //find date
        if(earliestDate == "" || latestDate == ""){earliestDate = it->second["Date"]; latestDate = it->second["Date"];}
        if(is_date1_before_date2(it->second["Date"], earliestDate)){earliestDate = it->second["Date"];}
        else if (is_date1_before_date2(latestDate, it->second["Date"])){latestDate = it->second["Date"];}
    }
    //earliest: annoucement+140, latest: +135
    //makesure we can get enough data
    latestDate = addDays(latestDate, 135); earliestDate = addDays(earliestDate, -140);
    //make all symbols(ticker names) in SurpriseMap to symbolist, in descending surprise% order
    populateSymbolVector(SupriseMap, symbolList);
    symbolList.push_back("IWV");
    int length = symbolList.size();
    //cout<<length<<endl; //for testing
    //divide into 3 groups
    to3Group(length, symbolList, SMiss, SMeet, SBeat);
    //in helper, delete if no enough data, make a copy
    vector<string> SMiss1 = SMiss, SMeet1 = SMeet, SBeat1 = SBeat;
    int choice;
    bool running = true;
    
    while (running) {
        cout << "1 - Enter N to retrieve 2N+1 days of historical price data for all stocks." << endl;
        cout << "2 - Pull information for one stock from one group." << endl;
        cout << "3 - Show AAR, AAR-STD, CAAR and CAAR-STD for one group." << endl;
        cout << "4 - Plot Avg CAAR for 3 groups." << endl;
        cout << "5 - Plot STD CAAR for 3 groups." << endl;
        cout << "6 - Plot Avg AAR for 3 groups." << endl;
        cout << "7 - Plot STD AAR for 3 groups." << endl;
        cout << "8 - Identify stocks with bad prices." << endl;
        cout << "9 - Exit." << endl;
        cout << "Enter your choice and press return: ";
        cin >> choice;
        
        switch (choice) {
            case 1:{
                //record current start time for calculation&retrieve data
                auto start = chrono::high_resolution_clock::now();
                cout<<"Enter N (N must be between 60 and 90): ";
                cin>>N;
                //initialize INFO, 3x3 vector
                for(auto& middle: INFO)
                {
                    middle.resize(4);
                    for(auto& inner: middle)
                    {
                        inner.resize(2*N+1,0.0);
                    }
                }
                if(N > 90 or N < 60) {cout<< "N out of Range"<<endl; break;}
                avg_AARMiss.resize(2*N+1,0.0); avg_AARMeet.resize(2*N+1,0.0); avg_AARBeat.resize(2*N+1,0.0);
                avg_CAARMiss.resize(2*N+1,0.0); avg_CAARMeet.resize(2*N+1,0.0); avg_CAARBeat.resize(2*N+1,0.0);
                STDAAR.resize(2*N+1,0.0), STDCAAR.resize(2*N+1,0.0);
                cout<<"Fetching Daily Price Data for Stocks.. (Please be patient)"<<endl;
                CURL* handle;
                CURLcode status;
                // set up the program environment that libcurl needs
                curl_global_init(CURL_GLOBAL_ALL);
                // curl_easy_init() returns a CURL easy handle
                handle = curl_easy_init();
                
                //Multithread
                int num_threads = 3;//thread::hardware_concurrency();
                //vector<thread> threads;
                
                
                // if everything's all right with the easy handle
                if (handle)
                {
                    //multithread = 3
                    thread thread1(task, handle, status, ref(SMiss), ref(Miss), N);
                    thread thread2(task, handle, status, ref(SMeet), ref(Meet), N);
                    thread thread3(task, handle, status, ref(SBeat), ref(Beat), N);
                    //
                    thread1.join();
                    thread2.join();
                    thread3.join();
                }
                else
                {
                    cout << "Curl init failed!" << endl;
                    return -1;
                }
                
                // cleanup what is created by curl_easy_init
                curl_easy_cleanup(handle);
                
                // release resources acquired by curl_global_init()
                curl_global_cleanup();
                auto stop = std::chrono::high_resolution_clock::now();
                
                // 计算运行时间
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
                
                // 输出运行时间
                cout << "Time taken to fetch and populate all the stock data: "
                << chrono::duration_cast<chrono::seconds>(duration).count() << " seconds " << endl<<endl;
                cout << "Proceed Stock Abnormal Return and AAR and CAAR calculation..."<<endl;
                //starting time of calculation
                start = chrono::high_resolution_clock::now();
                Stock IWV = Beat["IWV"];
                //no erase incase we will use it later
//                Beat.erase("IWV");
                //string: beat meet miss
                map<string, vector<vector<double>>> CAAR, AAR;
                CAAR["Beat"];CAAR["Miss"];CAAR["Meet"];
                AAR["Beat"];AAR["Miss"];AAR["Meet"];
                //bootstrapping
                for(int k=0; k< 40; k++)
                {
//                    cout<<k<<endl<<endl;
                    map<string, Stock> RMiss, RMeet, RBeat; //store random samples here
                    vector<double> AARMiss(2*N+1,0.0), AARMeet(2*N+1,0.0), AARBeat(2*N+1,0.0);
                    vector<double> CAARMiss, CAARMeet, CAARBeat;
                    //M = 30 default
                    //vector of names that contains all the random tickers selected
                    vector<string> randomSMiss = selectRandomElements(SMiss, M),randomSMeet = selectRandomElements(SMeet, M),randomSBeat = selectRandomElements(SBeat, M);
                    //RMiss is empty every iteration. not important
                    CalculateAAR(AARMiss, randomSMiss, RMiss, Miss, IWV.GetData());
                    CalculateCAAR(CAARMiss, AARMiss);
                    AAR["Miss"].push_back(AARMiss);
                    CAAR["Miss"].push_back(CAARMiss);
                    
                    CalculateAAR(AARMeet, randomSMeet, RMeet, Meet, IWV.GetData());
                    CalculateCAAR(CAARMeet, AARMeet);
                    AAR["Meet"].push_back(AARMeet);
                    CAAR["Meet"].push_back(CAARMeet);
                    
                    CalculateAAR(AARBeat, randomSBeat, RBeat, Beat, IWV.GetData());
                    CalculateCAAR(CAARBeat, AARBeat);
                    AAR["Beat"].push_back(AARBeat);
                    CAAR["Beat"].push_back(CAARBeat);
                }
                //Calculating Steps

                
                CalculateAvg_AAR(INFO[0][0], AAR, "Beat");
                CalculateSTD(INFO[0][1], INFO[0][0], AAR, "Beat");//AAR STD
                CalculateAvg_CAAR(INFO[0][2], CAAR, "Beat");
                CalculateSTD(INFO[0][3], INFO[0][2], CAAR, "Beat");//CAAR STD
                
                CalculateAvg_AAR(INFO[1][0], AAR, "Meet");
                CalculateSTD(INFO[1][1], INFO[1][0], AAR, "Meet");//AAR STD
                CalculateAvg_CAAR(INFO[1][2], CAAR, "Meet");
                CalculateSTD(INFO[1][3], INFO[1][2], CAAR, "Meet");//CAAR STD
                
                CalculateAvg_AAR(INFO[2][0], AAR, "Miss");
                CalculateSTD(INFO[2][1], INFO[2][0], AAR, "Miss");//AAR STD
                CalculateAvg_CAAR(INFO[2][2], CAAR, "Miss");
                CalculateSTD(INFO[2][3], INFO[2][2], CAAR, "Miss");//CAAR STD
                
//                CalculateAvg_AAR(avg_AARBeat, AAR, "Beat");
//                CalculateSTD(STDAAR, avg_AARBeat, AAR, "Beat");
//                CalculateAvg_CAAR(avg_CAARBeat, CAAR, "Beat");
//                CalculateSTD(STDCAAR, avg_CAARBeat, CAAR, "Beat");
//                
//                CalculateAvg_AAR(avg_AARMiss, AAR, "Miss");
//                CalculateAvg_CAAR(avg_CAARMiss, CAAR, "Miss");
//                CalculateSTD(STDAAR, avg_AARMiss, AAR, "Miss");
//                CalculateSTD(STDCAAR, avg_CAARMiss, CAAR, "Miss");
                
                stop = chrono::high_resolution_clock::now();
                duration = chrono::duration_cast<chrono::seconds>(stop - start);
                cout<< "Time taken to for all calculation: "
                << chrono::duration_cast<chrono::seconds>(duration).count()<<" seconds"<<endl;
                break;
            }
                
            case 2:{
                string symbol;
                cout<< "Enter Symbol for Detailed Information: ";
                cin >> symbol;
                //use copy since some of the symbols are erased due to warning
                string group = findStringInVectors(symbol, SBeat1, SMeet1, SMiss1);
                while(group == "None")
                {
                    cout<<"Invalid Symbol, Please Re-Enter Again"<<endl;
                    cout<< "Enter Symbol for Detailed Information: ";
                    cin >> symbol;
                    group = findStringInVectors(symbol, SBeat, SMeet, SMiss);
                }
                Stock S;
                if (group == "Beat") S = Beat[symbol];
                else if (group == "Meet") S = Meet[symbol];
                else S = Miss[symbol];
                cout<<"Ticker: "<< S.GetName()<<endl;
                cout<<"Earning Announcement Date: "<<S.GetAnnounceDate()<<endl;
                cout<<"Earning Period Ending: "<< S.GetPeriod_End()<<endl;
                cout<<"Earning Estimate: "<< S.GetEstimate()<<endl;
                cout<< "Reported Earning Per Share: "<< S.GetReported()<<endl;
                cout<<"Earning Suprise: "<<S.GetSuprise()<<endl;
                cout<<"Earning Suprise Percentage: "<< S.GetSuprisePercent()<<endl;
                cout<<"Group: "<< group<<endl;
                cout<< "No Enough Daily Price Data: "<< TF(S.GetWarning())<<endl<<endl;
                
                //get daily price, stored in Stock DailyTrade
                vector<map<string,string>> Data = S.GetData();
                
                cout<<"Stock Daily Price Dates: "<<endl;
                int count = -1;
                for (auto& i: Data)
                {
                    //读了2N+2(to calculate CAAR), 但是只需要输出2N+1所以跳过第一个
                    if (count == -1) count++;
                    //右对齐宽度15
                    cout<<right<<setw(15)<<i["Date"];
                    //count ==9 to switch line
                    //???do we cout the first data
                    if (count == 9 ){cout << endl; count = 0; }
                    else count++;
                }
                cout<<endl<<endl;
                //从第一个输出
                cout<<"Stock Daily Price: "<<endl;
                count = -1;
                for (auto& i: Data)
                {
                    if (count == -1) count++;
                    cout<<fixed<<setw(12)<<setprecision(3)<<stod(i["Adjusted_close"]);
                    if (count == 10 ){cout << endl; count = 0; }
                    else count++;
                }
                cout<<endl<<endl;
                
                cout<<"Stock  Daily Return: "<<endl;
                count = 0;
                //从第二个开始输出因为我们需要多一个来计算
                S.CalculateDailyReturn();
                for (auto& i: S.GetDailyReturn())
                {
                    if (count == -1) count++;
                    cout<<fixed<<setw(10)<<setprecision(3)<<i;
                    if (count == 10 ){cout << endl; count = 0; }
                    else count++;
                }
                cout<<endl<<endl;
                
                cout<<"Stock Cumulative Daily Return: "<<endl;
                count = 0;
                S.CalculateCumulative();
                for (auto& i: S.GetCumulativeReturn())
                {
                    if (count == -1) count++;
                    cout<<fixed<<setw(10)<<setprecision(3)<<i;
                    if (count == 10 ){cout << endl; count = 0; }
                    else count++;
                }
                cout<<endl<<endl;
                
                cout<<"Stock Abnormal Daily Return: "<<endl;
                count = 0;
                S.CalculateIWVReturn(Beat["IWV"].GetData());
                S.CalculateAbnormalReturn();
                for (auto& i: S.GetAbnormalReturn())
                {
                    if (count == -1) count++;
                    cout<<fixed<<setw(10)<<setprecision(3)<<i;
                    if (count == 10 ){cout << endl; count = 0; }
                    else count++;
                }
                cout<<endl<<endl;
                
                
                
                
                    
            
                break;
                
            }
            case 3:{
                string groupname;
                cout<< "Enter Group (Beat, Meet Miss) for Detailed Information: ";
                cin>> groupname;
                while (groupname != "Beat" && groupname != "Meet" && groupname != "Miss")
                {
                    cout<< "Invalid Group Name, Please Enter it Again"<<endl;
                    cout<< "Enter Group (Beat, Meet Miss) for Detailed Information: ";
                    cin>> groupname;
                }
                int id;
                if(groupname == "Beat") id = 0;
                else if (groupname == "Meet") id = 1;
                else id = 2;
                cout<< "Avg AARs for Group "<<toUpperCase(groupname)<<endl;
                int count = 0;
                for(auto& i : INFO[id][0])
                {
                    cout<<fixed<<setw(11)<<setprecision(4)<<i;
                    if (count == 10)
                    {
                        cout<<endl;
                        count = 0;
                    }
                    else count++;
                }
                cout<<endl<<endl;
                
                cout<< "AARs STD for Group "<<toUpperCase(groupname)<<endl;
                count = 0;
                for(auto& i : INFO[id][1])
                {
                    cout<<fixed<<setw(11)<<setprecision(4)<<i;
                    if (count == 10)
                    {
                        cout<<endl;
                        count = 0;
                    }
                    else count++;
                }
                cout<<endl<<endl;
                
                cout<< "Avg CAARs for Group "<<toUpperCase(groupname)<<endl;
                count = 0;
                for(auto& i : INFO[id][2])
                {
                    cout<<fixed<<setw(11)<<setprecision(4)<<i;
                    if (count == 10)
                    {
                        cout<<endl;
                        count = 0;
                    }
                    else count++;
                }
                cout<<endl<<endl;
                
                cout<< "CAARs STD for Group "<<toUpperCase(groupname)<<endl;
                count = 0;
                for(auto& i : INFO[id][3])
                {
                    cout<<fixed<<setw(11)<<setprecision(4)<<i;
                    if (count == 10)
                    {
                        cout<<endl;
                        count = 0;
                    }
                    else count++;
                }
                cout<<endl<<endl;
                
                    
                break;
            }
            case 4:{
                // 调用相应函数
                
                
                if(N >= 60 && N<=100)
                {
                //顺序Beat Meet Miss
                //CAAR AVG
                plot Plotter(INFO[0][2], INFO[1][2], INFO[2][2], "AVG CAAR");
                Plotter.plotResult();           
                }   

                else
                {
                    cout << "Data is not retrived yet, Retrieve data from Option 1." << endl;
                }
                
                //break;
            }
            case 5:{
                if(N >= 60 && N<=100)
                {
                // 调用相应函数
                //CAAR STD
                plot Plotter(INFO[0][3], INFO[1][3], INFO[2][3], "STD CAAR");
                Plotter.plotResult();
                }
                else
                {
                    cout << "Data is not retrived yet, Retrieve data from Option 1." << endl;
                }
                //break;
            }
            case 6:{
                // 调用相应函数
                //AAR AVG
                if(N >= 60 && N<=100)
                {
                plot Plotter(INFO[0][0], INFO[1][0], INFO[2][0], "AVG AAR");
                Plotter.plotResult();
                }
                else
                {
                    cout << "Data is not retrived yet, Retrieve data from Option 1." << endl;
                }
                //break;
            }
            case 7:{
                // 调用相应函数
                //AAR STD
                if(N >= 60 && N<=100)
                {
                plot Plotter(INFO[0][1], INFO[1][1], INFO[2][1], "STD AAR");
                Plotter.plotResult();
                }
                else
                {
                    cout << "Data is not retrived yet, Retrieve data from Option 1." << endl;
                }
                //break;
            }
            case 8:
                // 调用相应函数
                break;
            case 9:
                running = false;
                cout << "Exiting program." << endl;
                break;
            default:
                cout << "Invalid choice. Please enter a number between 1 and 9." << endl;
                
        }
    }
    return 0;
}
    
    
    
    
    
    
    
    
    
    
    
    //        for(auto& i: AARBeat)
    //        {
    //            cout << i<<"  ";
    //        }
    //        cout<<endl<<endl;
    //
    //        for(auto& i: CAARBeat)
    //        {
    //            cout<< i<<"  ";
    //        }
    //        cout<<endl<<endl;
    
    //        avg_AARMiss += AARMiss;
    //        avg_AARMeet += AARMeet;
    //        avg_AARBeat += AARBeat;
    //
    //        avg_CAARMiss += CAARMiss;
    //        avg_CAARMeet += CAARMeet;
    //        avg_CAARBeat += CAARBeat;
    
    //        for(auto& i: avg_AARBeat)
    //        {
    //            cout<<fixed<<setprecision(4)<< i*100<<"  ";
    //        }
    //        cout<<endl<<endl;
    //
    //        for(auto& i: avg_CAARBeat)
    //        {
    //            cout<<fixed<<setprecision(4)<< i*100<<"  ";
    //        }
    //        cout<<endl<<endl;



//    cout<< "AVG_ARR:"<<endl;
//    for(auto& i: avg_AARBeat)
//    {
//        cout<<fixed<<setprecision(4)<< i<<"  ";
//    }
//    cout<<endl<<endl;
//
//    cout<< "STD_ARR:"<<endl;
//
//    for(auto& i: STDAAR)
//    {
//        cout<<fixed<<setprecision(4)<< i<<"  ";
//    }
//    cout<<endl<<endl;
//
//    cout<< "AVG_CARR:"<<endl;
//    for(auto& i: avg_CAARBeat)
//    {
//        cout<<fixed<<setprecision(4)<< i<<"  ";
//    }
//    cout<<endl<<endl;
//
//    cout<< "STD_CARR:"<<endl;
//
//    for(auto& i: STDCAAR)
//    {
//        cout<<fixed<<setprecision(4)<< i<<"  ";
//    }
//    cout<<endl<<endl;









//    avg_AARMiss = (1/40) * avg_AARMiss;
//    avg_AARMeet = (1/40) * avg_AARMeet;
//    avg_AARBeat = (1/40) * avg_AARBeat;
//
//    avg_CAARMiss = (1/40) * avg_CAARMiss;
//    avg_CAARMeet = (1/40) * avg_CAARMeet;
//    avg_CAARBeat = (1/40) * avg_CAARBeat;
//    cout<<CAAR["Beat"].size()<<endl;
//    cout<<CAAR["Beat"][20].size()<<endl;

//    for(auto& i : CAAR["Beat"][20])
//    {
//        cout<<i<<"  ";
//    }
//    cout<<endl<<endl;
//
//
//    for(auto& i: avg_AARBeat)
//    {
//        cout<<fixed<<setprecision(4)<< i*100<<"  ";
//    }
//    cout<<endl<<endl;
//
//    for(auto& i: avg_CAARBeat)
//    {
//        cout<<fixed<<setprecision(4)<< i*100<<"  ";
//    }
//    cout<<endl<<endl;
//



//    for(auto& i : RMiss){
//        cout<< i.first<<" ";
//    }
//    cout<<endl<<endl;
//
//    for(auto& i : RMeet){
//        cout<< i.first<<" ";
//    }
//    cout<<endl<<endl;
//
//    for(auto& i : RBeat){
//        cout<< i.first<<" ";
//    }
//    cout<<endl<<endl;
//
//    for(auto& i: AARMiss)
//    {
//        cout<<i<<"  ";
//    }
//    cout<<endl<<endl;
//    for(auto& i: AARMeet)
//    {
//        cout<<i<<"  ";
//    }
//    cout<<endl<<endl;
//    for(auto& i: AARBeat)
//    {
//        cout<<i<<"  ";
//    }
//    cout<<endl<<endl;
//
//
//    for(auto& i: CAARMiss)
//    {
//        cout<<i<<"  ";
//    }
//    cout<<endl<<endl;
//    for(auto& i: CAARMeet)
//    {
//        cout<<i<<"  ";
//    }
//    cout<<endl<<endl;
//
//    for(auto& i: CAARBeat)
//    {
//        cout<<i<<"  ";
//    }
//    cout<<endl<<endl;
//
//    //














//    cout<< Miss.size()<<endl;
//    cout<<Meet.size()<<endl;
//    cout<<Beat.size()<<endl;

//    string ticker = "MYPS";
//    Beat[ticker].CalculateDailyReturn();
//    Beat[ticker].CalculateIWVReturn(IWV.GetData());
//    Beat[ticker].CalculateAbnormalReturn();
//    cout<< Beat[ticker].GetIWVReturn().size()<<"  "<< Beat[ticker].GetDailyReturn().size()<<endl;
//    for(int i = 0; i < Beat[ticker].GetDailyReturn().size();i++)
//    {
//        cout<<Beat[ticker].GetDailyReturn()[i]<<"  "<<Beat[ticker].GetIWVReturn()[i]<<"  "<<Beat[ticker].GetAbnormalReturn()[i]<<endl;
//    }
//



