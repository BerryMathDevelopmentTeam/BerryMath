#include <iostream>
#include <curl/curl.h>
#include <curl/easy.h>
#include <BerryMath.h>
#include "library.h"

size_t process_data(void *buffer, size_t size, size_t nmemb, void *userp) {
    string& str = *((string*)userp);
    str.append((char*)buffer, size * nmemb);
    return size * nmemb;
}

Object* get(BM::Scope* scope, vector<Object*> unknowns) {
    string url(((String*)scope->get("url")->value())->value());

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
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)(&content));
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, process_data);

    curl_easy_perform(handle);

    curl_easy_cleanup(handle);

    curl_global_cleanup();

    return new BM::String(content);
}
Object* post(BM::Scope* scope, vector<Object*> unknowns) {
    string url(((String*)scope->get("url")->value())->value());

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
    curl_easy_setopt(handle, CURLOPT_POSTFIELDS, ((String*)scope->get("data")->value())->value().c_str());
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)(&content));
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, process_data);

    curl_easy_perform(handle);

    curl_easy_cleanup(handle);

    curl_global_cleanup();

    return new BM::String(content);
}

Object* initModule() {
    auto exports = new Object;

    auto getP = new BM::NativeFunction("get", get);
    auto postP = new BM::NativeFunction("post", post);

    getP->addDesc("url");
    postP->addDesc("url");
    postP->addDesc("data");

    postP->defaultValue("data", new String(""));

    exports->set("get", getP);
    exports->set("post", postP);

    return exports;
}