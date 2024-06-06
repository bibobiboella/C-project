#include "Stocks.h"
#include "Matrix.h"
//for IWV
Stock::Stock(string name, vector<map<string, string>> S):Name(name), DailyTrades(S)
{
//    AnnounceDate = AnnouncementInfo["Date"];
//    Period_End  = AnnouncementInfo["Period_End"];
//    Estimate = stod( AnnouncementInfo["Estimate"]);
//    Reported = stod(AnnouncementInfo["Reporeted"]);
//    Suprise  = stod(AnnouncementInfo["Suprise"]);
//    Suprise_Percentage = stod(AnnouncementInfo["Suprise_Percentage"]);
    
}
//vector<map<string, string>> == current stock/Daily trade
//A == anoucement info
Stock::Stock(string name, vector<map<string, string>> S, map<string,string> A, int n): Name(name), N(n)
{
    AnnounceDate = A["Date"];
    Period_End  = A["Period_End"];
    Estimate =  stod(A["Estimate"]);
    Reported = stod(A["Reported"]);
    Suprise  = stod(A["Suprise"]);
    Suprise_Percentage = stod(A["Suprise_Percentage"]);
    
    int index = 0;
    int begin, end;
    //same as i = 0, = 30 to run less
    //at least 60 days data, if not eanough 30 then must less than 60
    for(int i = 30; i < S.size(); i++ )
    {
//        cout<< S[i]["Date"]<<"  "<< AnnounceDate<<"  "<< (S[i]["Date"] == AnnounceDate)<<endl;
        if(S[i]["Date"] == AnnounceDate) {index = i; break;}
    }
    //i == annoucement day
    //CAAR needs info for previous date, so we do -1
    begin = index - N -1;
    end = index + N;
    //check is enough data
    if(begin < 0 or end >= S.size())
    {
        cerr << "Warning: Not Enough Data for "<<Name <<"."<< endl;
        warning = true;
        if(begin < 0) begin =0;
        if(end>= S.size()) end = S.size()-1;
    }
    //DailyTrade is the same type as S[i]
    for(int i = begin; i<= end; i++) DailyTrades.push_back(S[i]);
//    if (end>=245 or begin<=10)cout<<Name <<"  " <<begin<<"  "<<en d<<"  "<< DailyTrades.size()<<endl;
    
}

void Stock::CalculateDailyReturn()
{
    double rit;
    for(auto it = (DailyTrades.begin()+1); it != DailyTrades.end(); it++)
    {
        rit = log(stod((*it)["Adjusted_close"])/stod((*(it-1))["Adjusted_close"]));
        DailyReturn.push_back(rit);
    }
}

void Stock::CalculateCumulative()
{
    double cu = 0.0;
    for(const auto& i: DailyReturn)
    {
        cu+=i;
        CumulativeReturn.push_back(cu);
    }
    
}

void Stock::CalculateIWVReturn(vector<map<string, string>> IWV)
{
    auto it1 = IWV.begin();
    const auto it2 = ++DailyTrades.begin();
    double rmt;
    while((*it1)["Date"] != (*it2)["Date"]) it1++;
    int d = DailyReturn.size();
    for(int i =0; i < d; i++)
    {
        rmt = log(stod((*it1)["Adjusted_close"])/stod((*(it1-1))["Adjusted_close"]));
        IWVReturn.push_back(rmt);
        it1++;
    }

}

void Stock::CalculateAbnormalReturn()
{
    AbnormalReturn = DailyReturn - IWVReturn;
}


