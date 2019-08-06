#include <iostream>
#include <curl/curl.h>
#include <curl/easy.h>
#include <BerryMath.h>
#include "library.h"

Object* get(BM::Scope* scope, vector<Object*> unknowns) {
    string url(((String*)scope->get("url"))->value());

    auto curlcode = curl_global_init(CURL_GLOBAL_ALL);
    if (curlcode != CURLE_OK) {
        std::cerr << "Init libcurl failed." << std::endl;
        return new BM::String("");
    }

    auto handle = curl_easy_init();

    if (!handle) {
        std::cerr << "Init libcurl handle failed." << std::endl;
        curl_global_cleanup();
        return new BM::String("");
    }

    string content;

    curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(handle, CURLOPT_NOBODY, 1);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&content);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, [&](void *buffer, size_t size, size_t nmemb, void *userp) {
        auto d = (char*)buffer;
        string& str = *((string*)userp);
        str.append((char*)buffer, size * nmemb);
        return size * nmemb;
    });

    curl_easy_perform(handle);

    curl_easy_cleanup(handle);

    std::cout << content << std::endl;

    return new BM::String(content);
}

Object* initModule() {
    auto exports = new Object;

    auto getP = new BM::NativeFunction("get", get);

    getP->addDesc("url");

    exports->set("get", getP);

    return exports;
}