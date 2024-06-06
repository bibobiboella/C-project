#pragma once
#include<string>
#include<vector>
#include<map>
#include<iostream>
#include<cmath>
using namespace std;

class Stock
{
private:
    int N = 90; //之后需要修改
    string Name;
    string AnnounceDate, Period_End;
    bool warning = false;
    double Estimate, Reported, Suprise, Suprise_Percentage;
    vector<double> DailyReturn, IWVReturn, AbnormalReturn, CumulativeReturn;
    vector<map<string,string>> DailyTrades;
public:
    Stock(string name, vector<map<string, string>> S);
    Stock(string name, vector<map<string, string>> S, map<string,string> A, int n);
    Stock(){}
    vector<map<string,string>> GetData() {return DailyTrades;}
    void CalculateDailyReturn();
    void CalculateCumulative();
    void CalculateIWVReturn(vector<map<string,string>>);
    void CalculateAbnormalReturn();
    bool GetWarning(){return warning;}
    string GetName(){return Name;}
    string GetAnnounceDate(){return AnnounceDate;}
    string GetPeriod_End(){return Period_End;}
    double GetEstimate(){return Estimate;}
    double GetReported(){return Reported;}
    double GetSuprise(){return Suprise;}
    double GetSuprisePercent(){return Suprise_Percentage;}
    vector<double> GetDailyReturn(){return DailyReturn;}
    vector<double> GetCumulativeReturn(){return CumulativeReturn;}
    vector<double> GetIWVReturn(){return IWVReturn;}
    vector<double> GetAbnormalReturn(){return AbnormalReturn;}
    
    
};
