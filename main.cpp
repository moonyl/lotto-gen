#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
#include <sstream>
#include <json/json.h>
#include <leveldb/db.h>

using std::string;

int main()
{
    // RAII cleanup

    curlpp::Cleanup myCleanup;

    // Send request and get a result.
    // Here I use a shortcut to get it in a string stream ...

    std::ostringstream os;
    os << curlpp::options::Url(std::string("http://www.nlotto.co.kr/common.do?method=getLottoNumber&drwNo=3"));

    string asAskedInQuestion = os.str();
    std::cout << asAskedInQuestion << std::endl;

    Json::Value root;
    std::istringstream strStream(asAskedInQuestion);
    strStream >> root;

    std::cout << root << std::endl;

    std::cout << "check element" << std::endl;
    std::cout << root["drwtNo1"] << std::endl;
    std::cout << root["drwtNo1"].asUInt() << std::endl;

    //auto no1 = root["drwtNo1"];
    //std::cout << typeid(no1).name() << std::endl;


    return 0;
}
