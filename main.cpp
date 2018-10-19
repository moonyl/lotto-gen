#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
#include <sstream>
#include <json/json.h>
#include <leveldb/db.h>

using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::ostringstream;

int main()
{
    // RAII cleanup

    curlpp::Cleanup myCleanup;

    // Send request and get a result.
    // Here I use a shortcut to get it in a string stream ...

    std::ostringstream os;

    //os << curlpp::options::Url(string("http://www.nlotto.co.kr/common.do?method=getLottoNumber&drwNo=3"));
    os << curlpp::options::Url("http://www.nlotto.co.kr/common.do?method=getLottoNumber&drwNo=3");
    string asAskedInQuestion = os.str();
    std::cout << asAskedInQuestion << std::endl;

    Json::Value lottoJson;
    std::istringstream strStream(asAskedInQuestion);
    strStream >> lottoJson;

    std::cout << lottoJson << std::endl;

    std::cout << "check element" << std::endl;
    std::cout << lottoJson["drwtNo1"] << std::endl;
    std::cout << lottoJson["drwtNo1"].asUInt() << std::endl;

    //auto no1 = root["drwtNo1"];
    //std::cout << typeid(no1).name() << std::endl;

    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = true;

    leveldb::Status status = leveldb::DB::Open(options, "./testdb", &db);

    if (false == status.ok())
    {
        cerr << "Unable to open/create test database './testdb'" << endl;
        cerr << status.ToString() << endl;
        return -1;
    }

    // Add 256 values to the database
    leveldb::WriteOptions writeOptions;
    //for (unsigned int i = 0; i < 256; ++i)
    //{
        ostringstream keyStream;
        //keyStream << "Key" << i;
        keyStream << 0;

        ostringstream valueStream;
        valueStream << lottoJson;

        db->Put(writeOptions, keyStream.str(), valueStream.str());
    //}

    // Iterate over each item in the database and print them
    leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());

    for (it->SeekToFirst(); it->Valid(); it->Next())
    {
        cout << it->key().ToString() << " : " << it->value().ToString() << endl;
    }

    if (false == it->status().ok())
    {
        cerr << "An error was found during the scan" << endl;
        cerr << it->status().ToString() << endl;
    }

    delete it;

    // Close the database
    delete db;

    return 0;
}
