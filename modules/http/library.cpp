#include <BerryMath.h>
#include <HTTPRequest.hpp>
#include "library.h"

Object* request(BM::Scope* scope, vector<Object*> unknowns) {
    string& type = ((String*)scope->get("type"))->value();
    http::Request request(((String*)scope->get("url"))->value());
    std::transform(type.begin(), type.end(), type.begin(), toupper);
    http::Response response;
    if (type == "POST") {
        response = request.send(type, ((String*)scope->get("data"))->value(), {
                "Content-Type: application/x-www-form-urlencoded"
        });
    } else {
        response = request.send(type, ((String*)scope->get("data"))->value());
    }
    return new String(string(response.body.begin(), response.body.end()));
}

Object* initModule() {
    auto exports = new Object;

    auto requestP = new BM::NativeFunction("request", request);

    requestP->addDesc("url");
    requestP->addDesc("type");
    requestP->addDesc("data");
    requestP->defaultValue("type", new String("GET"));
    requestP->defaultValue("data", new String(""));

    exports->set("request", requestP);

    return exports;
}