#include "Utils.h"
#include <iostream>
#include <string>
#include "utilities_js.hpp"

using namespace std;

int main(void)
{
    string rpcAddr = "";
    string rpcUserpass = "";
    string response;
    string request = "{\"jsonrpc\":\"1.0\",\"id\":\"1\",\"method\":\"getblocktemplate\",\"params\":[]}";
    bool res = blockchainNodeRpcCall(rpcAddr.c_str(), rpcUserpass.c_str(), request.c_str(), response);

    if (!res){
        cout << "res false" << endl;
    }else{
        cout << "res true" << endl;
    }

    cout << "response:" << response << endl;

    JsonNode r;
    if (!JsonNode::parse(response.c_str(), response.c_str() + response.length(), r)) {
      cout << "decode gbt failure: " << response << endl;
      return 0;
    }

    if (r["result"].type() != Utilities::JS::type::Obj ||
            r["result"]["previousblockhash"].type() != Utilities::JS::type::Str ||
            r["result"]["height"].type()            != Utilities::JS::type::Int ||
            r["result"]["coinbasevalue"].type()     != Utilities::JS::type::Int ||
            r["result"]["bits"].type()              != Utilities::JS::type::Str ||
            r["result"]["mintime"].type()           != Utilities::JS::type::Int ||
            r["result"]["curtime"].type()           != Utilities::JS::type::Int ||
            r["result"]["version"].type()           != Utilities::JS::type::Int) {
        cout << "gbt check fields failure" << endl;
        return 0;
    }

    return 1;
}
