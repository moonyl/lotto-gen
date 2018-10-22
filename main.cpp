#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
#include <sstream>
#include <json/json.h>
#include <leveldb/db.h>
#include <thread>
#include <chrono>

using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::ostringstream;

string makeLottoUrl(int i) {
    static string baseUrl("http://www.nlotto.co.kr/common.do?method=getLottoNumber&drwNo=");

    std::ostringstream numUrl;
    numUrl << baseUrl << i;

    return numUrl.str();
}

string getHttpResponse(const string& url)   {
    std::ostringstream os;

    try {
        os << curlpp::options::Url(url);
    } catch ( curlpp::LogicError & e ) 	{
        std::cout << e.what() << std::endl;
    } catch ( curlpp::RuntimeError & e )  {
        std::cout << e.what() << std::endl;
    }

    return os.str();
}

Json::Value toJsonValue(const string& strJson) {
    Json::Value lottoJson;
    std::istringstream strStream(strJson);
    strStream >> lottoJson;
    return lottoJson;
}

leveldb::DB* openLottoDb()
{
    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = true;

    leveldb::Status status = leveldb::DB::Open(options, "./lottodb", &db);

    if (false == status.ok())
    {
        cerr << "Unable to open/create test database './lottodb'" << endl;
        cerr << status.ToString() << endl;
        return nullptr;
    }
    return db;
}

void writeLottoData(leveldb::DB* db, int index, const Json::Value &lottoJson)
{
    leveldb::WriteOptions writeOptions;

    ostringstream keyStream;

    keyStream << index;

    ostringstream valueStream;
    valueStream << lottoJson;

    db->Put(writeOptions, keyStream.str(), valueStream.str());
}

#include <set>
std::set<uint> toSixNums(const Json::Value& json) {
    std::set<uint> result;
    result.insert(json["drwtNo1"].asUInt());
    result.insert(json["drwtNo2"].asUInt());
    result.insert(json["drwtNo3"].asUInt());
    result.insert(json["drwtNo4"].asUInt());
    result.insert(json["drwtNo5"].asUInt());
    result.insert(json["drwtNo6"].asUInt());
    return result;
}

void handleBlacklist(const std::set<std::set<uint>> &setOfSixNumSet) {
    // TODO : store db
}

void printAllData(leveldb::DB* db)
{
    // Iterate over each item in the database and print them
    leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());

    int total = 0;
    std::set<std::set<uint>> setOfSixNumSet;
    for (it->SeekToFirst(); it->Valid(); it->Next())
    {
        //cout << it->key().ToString() << " : " << it->value().ToString() << endl;
        auto value = it->value().ToString();
        //cout << typeid(value).name() << endl;
        //cout << typeid(value).name() << endl;
        auto json = toJsonValue(value);
        //uint no1 = json["drwtNo1"].asUInt();
        auto numbers = toSixNums(json);
        setOfSixNumSet.insert(numbers);
        ++total;
    }

    //cout << "total count : " << total << std::endl;
    cout << "total count : " << setOfSixNumSet.size() << std::endl;

    if (false == it->status().ok())
    {
        cerr << "An error was found during the scan" << endl;
        cerr << it->status().ToString() << endl;
    }

    delete it;

    if (total == setOfSixNumSet.size()) {
        handleBlackList(setOfSixNumSet);
    }
}

int main()
   {
    // RAII cleanup

    curlpp::Cleanup myCleanup;

    // Send request and get a result.
    // Here I use a shortcut to get it in a string stream ...



    leveldb::DB* db = openLottoDb();

    bool shouldServerUpdate = false;
    if (shouldServerUpdate == true) {
        for (int i = 1; i <= 828; i++) {
            string asAskedInQuestion = getHttpResponse(makeLottoUrl(i));
            if (asAskedInQuestion.empty()) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                --i;
                continue;
            }

            Json::Value lottoJson = toJsonValue(asAskedInQuestion);

    //        std::cout << lottoJson << std::endl;
    //        std::cout << "check element" << std::endl;
    //        std::cout << lottoJson["drwtNo1"].asUInt() << std::endl;

            writeLottoData(db, i, lottoJson);
        }
    }

    printAllData(db);

    // Close the database
    delete db;

    return 0;
}
