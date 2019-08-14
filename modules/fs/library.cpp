#include <string>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
#include "library.h"

Object *FileCtor(BM::Scope *scope, vector<Object *> unknowns) {
    auto self = new Object;

    auto path = (String *) scope->get("path")->value();
    self->set("path", path);

    auto file = new std::fstream();
    file->open(path->value(), std::ios_base::out | std::ios_base::app | std::ios_base::in);
    if (!file->is_open()) {
        file->open(path->value(), std::ios_base::out | std::ios_base::trunc | std::ios_base::in);
    }

    self->set("file", new NativeValue(file));
    self->set("is_open", new Number(file->is_open()));
    self->set("next", new Number(file->is_open()));

    return self;
}

Object *FileWrite(BM::Scope *scope, vector<Object *> unknowns) {
    auto self = scope->get("this")->value();

    auto fileBMO = (NativeValue *) self->get("file");
    auto valueV = scope->get("value");
    if (valueV) {
        auto value = (String *) valueV->value();
        if (fileBMO && self->get("file") && value) {
            auto file = (std::fstream *) ((NativeValue *) self->get("file"))->value();
            (*file) << value->value();
        }
    }

    return new BM::Undefined;
}

Object *FileRead(BM::Scope *scope, vector<Object *> unknowns) {
    auto self = scope->get("this")->value();
    auto content = new String;

    auto fileBMO = (NativeValue *) self->get("file");
    if (fileBMO && self->get("file")) {
        auto file = (std::fstream *) ((NativeValue *) self->get("file"))->value();
        string line;
        while (getline(*file, line)) {
            content->value() += line + "\n";
        }
    }

    return content;
}

Object *FileReadline(BM::Scope *scope, vector<Object *> unknowns) {
    auto self = scope->get("this")->value();
    auto content = new String;

    auto fileBMO = (NativeValue *) self->get("file");
    if (fileBMO && self->get("file"))
        if (getline(*((std::fstream *) ((NativeValue *) self->get("file"))->value()), content->value())) self->set("next", new Number(1));
        else self->set("next", new Number(0));
    return content;
}
Object *FileReadlines(BM::Scope *scope, vector<Object *> unknowns) {
    auto self = scope->get("this")->value();
    auto lines = new Object;
    lines->set("ctor", new Function("ctor", "let this = {};return this;"));
    auto pushFun = new Function("push", "this[this.__len] = n;this.__len++;");
    pushFun->addDesc("n");
    lines->set("push", pushFun);
    lines->set("__len", new Number(0));
    lines->set("__SYSTEM_TYPE__", new String("Array"));

    auto fileBMO = (NativeValue *) self->get("file");
    auto file = ((std::fstream *) ((NativeValue *) self->get("file"))->value());
    auto virtualIp = new BM::Interpreter("", "fs");
    virtualIp->set("this", lines);
    pushFun->setParent(virtualIp);
    if (fileBMO && self->get("file")) {
        string line;
        while (getline(*file, line)) {
            vector<Object*> args;
            map<string, Object*> hash;
            args.push_back(new String(line));
            pushFun->run(args, hash);
        }
    }
    return lines;
}

Object *FileReset(BM::Scope *scope, vector<Object *> unknowns) {
    auto self = scope->get("this")->value();

    auto fileBMO = (NativeValue *) self->get("file");
    if (fileBMO && self->get("file")) {
        auto file = (std::fstream *) ((NativeValue *) self->get("file"))->value();
        (*file).seekg(std::ios_base::beg);
    }

    return new BM::Undefined;
}
Object *FileSeek(BM::Scope *scope, vector<Object *> unknowns) {
    auto self = scope->get("this")->value();

    auto fileBMO = (NativeValue *) self->get("file");
    auto offsetBMV = scope->get("offset");
    auto whenceBMV = scope->get("whence");
    if (!offsetBMV || !whenceBMV) return new BM::Undefined;
    long long offset = ((Number*)offsetBMV->value())->value();
    short whence = ((Number*)whenceBMV->value())->value();
    if (fileBMO && self->get("file")) {
        auto file = (std::fstream *) ((NativeValue *) self->get("file"))->value();
        switch (whence) {
            case 0:
            {
                (*file).seekg(offset, std::ios_base::beg);
                break;
            }
            case 1:
            {
                (*file).seekg(offset, std::ios_base::cur);
                break;
            }
            case 2:
            {
                (*file).seekg(offset, std::ios_base::end);
                break;
            }
        }
    }

    return new BM::Undefined;
}

Object *FileRemove(BM::Scope *scope, vector<Object *> unknowns) {
    auto self = scope->get("this")->value();

    auto pathBMO = (String*) self->get("path");
    if (pathBMO) {
        auto fileBMO = (NativeValue *) self->get("file");
        remove(pathBMO->value().c_str());
        ((std::fstream *) fileBMO->value())->close();
    } else return new BM::Number(0);

    return new BM::Number(1);
}

Object *FileClose(BM::Scope *scope, vector<Object *> unknowns) {
    auto self = scope->get("this")->value();

    auto fileBMO = (NativeValue *) self->get("file");
    if (fileBMO) {
        ((std::fstream *) fileBMO->value())->close();
    }

    return new BM::Undefined;
}

Object *mkdir(BM::Scope *scope, vector<Object *> unknowns) {
    auto pathBMV = (BM::Variable*) scope->get("path");

    if (pathBMV) {
        auto pathBMO = (String*) pathBMV->value();
        ::mkdir(pathBMO->value().c_str(), 0755);
    }

    return new BM::Undefined;
}
Object *copy(BM::Scope *scope, vector<Object *> unknowns) {
    auto pathaBMV = (BM::Variable*) scope->get("patha");
    auto pathbBMV = (BM::Variable*) scope->get("pathb");

    if (pathaBMV && pathbBMV) {
        auto pathaBMO = (String*) pathaBMV->value();
        auto pathbBMO = (String*) pathbBMV->value();
#ifdef I_OS_WIN32
        system(("xcopy " + pathaBMO->value() + " " + pathbBMO->value()).c_str());
#else
        system(("cp -r " + pathaBMO->value() + " " + pathbBMO->value()).c_str());
#endif
    }

    return new BM::Undefined;
}
Object *move(BM::Scope *scope, vector<Object *> unknowns) {
    auto pathaBMV = (BM::Variable*) scope->get("patha");
    auto pathbBMV = (BM::Variable*) scope->get("pathb");

    if (pathaBMV && pathbBMV) {
        auto pathaBMO = (String*) pathaBMV->value();
        auto pathbBMO = (String*) pathbBMV->value();
#ifdef I_OS_WIN32
        system(("move " + pathaBMO->value() + " " + pathbBMO->value()).c_str());
#else
        system(("mv " + pathaBMO->value() + " " + pathbBMO->value()).c_str());
#endif
    }

    return new BM::Undefined;
}
Object *remove(BM::Scope *scope, vector<Object *> unknowns) {
    auto pathBMV = (BM::Variable*) scope->get("path");

    if (pathBMV) {
        auto pathBMO = (String*) pathBMV->value();
#ifdef I_OS_WIN32
        system(("del " + pathBMO->value()).c_str());
#else
        system(("rm -r " + pathBMO->value()).c_str());
#endif
    }

    return new BM::Undefined;
}

Object *initModule() {
    auto exports = new Object;

    auto FileClass = new Object;
    auto prototype = new Object;

    FileClass->set("prototype", prototype);

    auto FileCtorP = new NativeFunction("ctor", FileCtor);
    FileCtorP->addDesc("path");
    prototype->set("ctor", FileCtorP);

    auto FileWriteP = new NativeFunction("write", FileWrite);
    FileWriteP->addDesc("value");
    prototype->set("write", FileWriteP);

    auto FileSeekP = new NativeFunction("seek", FileSeek);
    FileSeekP->addDesc("offset");
    FileSeekP->addDesc("whence");
    prototype->set("seek", FileSeekP);

    auto FileReadP = new NativeFunction("read", FileRead);
    prototype->set("read", FileReadP);

    auto FileCloseP = new NativeFunction("close", FileClose);
    prototype->set("close", FileCloseP);

    auto FileRemoveP = new NativeFunction("remove", FileRemove);
    prototype->set("remove", FileRemoveP);

    auto FileReadlineP = new NativeFunction("readline", FileReadline);
    prototype->set("readline", FileReadlineP);

    auto FileReadlinesP = new NativeFunction("readlines", FileReadlines);
    prototype->set("readlines", FileReadlinesP);

    auto FileResetP = new NativeFunction("reset", FileReset);
    prototype->set("reset", FileResetP);

    exports->set("File", FileClass);

    auto mkdirP = new NativeFunction("mkdir", mkdir);
    mkdirP->addDesc("path");
    exports->set("mkdir", mkdirP);

    auto removeP = new NativeFunction("remove", remove);
    removeP->addDesc("path");
    exports->set("remove", removeP);

    auto copyP = new NativeFunction("copy", copy);
    copyP->addDesc("patha");
    copyP->addDesc("pathb");
    exports->set("copy", copyP);

    auto moveP = new NativeFunction("move", move);
    moveP->addDesc("patha");
    moveP->addDesc("pathb");
    exports->set("move", moveP);

    return exports;
}