/*****************************************************************************
*  BerryMath Interpreter                                                     *
*  Copyright (C) 2019 BerryMathDevelopmentTeam  zhengyh2018@gmail.com        *
*                                                                            *
*  这个文件为BerryMath语言本身做内存管理的(比如声明变量).                          *
*                                                                            *
*  This program is free software; you can redistribute it and/or modify      *
*  it under the terms of the GNU General Public License version 3 as         *
*  published by the Free Software Foundation.                                *
*                                                                            *
*  You should have received a copy of the GNU General Public License         *
*  along with OST. If not, see <http://www.gnu.org/licenses/>.               *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
*                                                                            *
*  @file     BerryMath.h                                                     *
*  @brief    内存管理                                                         *
*  Details.                                                                  *
*                                                                            *
*  @author   yhzheng                                                         *
*  @email    zhengyh2018@gmail.com                                           *
*  @version  0.0.1 (beta)                                                    *
*  @date     2019.2.19.                                                      *
*  @license  GNU General Public License (GPL)                                *
*                                                                            *
*****************************************************************************/

#ifndef BERRYMATH_MEMORY_H
#define BERRYMATH_MEMORY_H

#include <iostream>
#include <cstring>
#include <map>
#include "btype.h"
using std::string;

namespace BerryMath {
    class value;
    typedef std::map<std::string, value*> hash;
    enum flag {// 存储寻找、设置变量的方式
        DEFAULT_FLAG, GLOBAL_FLAG
    };
    /**
    * @brief BerryMath数值、字符串等值的c++存储实现
    */
    class value {
    public:
        value();
        value(TYPE, std::string);
        value(std::string);
        value(value&);
        hash& properties() {
            return prototypes;
        }
        value* property(std::string name) {
            hash::iterator iter = prototypes.find(name);
            if (iter == prototypes.end()) {
                return new value();
            }
            return iter->second;
        }
        void insert(std::string, value*);
        std::string valueOf() {
            return data;
        }
        TYPE typeOf() {
            return type;
        }
        void valueOf(std::string);
        ~value();
        friend class variable;
    private:
        TYPE type;
        std::string data;
        hash prototypes;
        int use;
    };
    class variable {
    public:
        variable(string n) : name(n), v(new value(UNDEFINED, "undefined")) { }
        variable(string n, string s) : name(n), v(new value(s)) { }
        variable(string, variable&);
        variable(string n, value* s) : name(n), v(s) {
            v->use++;
        }
        value& valueOf() {
            return *v;
        }
        void valueOf(value* val) {
            if ((--v->use) == 0) {
                delete v;
                v = val;
                v->use++;
            }
        }
        string nameOf() {
            return name;
        }
        ~variable() {
            if (--v->use < 1) delete v;
        }
    private:
        string name;
        value* v;
    };
    typedef std::map<std::string, variable*> table;
    class block {
    public:
        block() : parent(nullptr) {}
        block(block* p) : parent(nullptr) {}
        variable* of(string n, flag f = DEFAULT_FLAG) {
            if (f == DEFAULT_FLAG) {// 普通模式
                auto iter = variables.find(n);
                if (iter == variables.end()) {
                    if (parent) {
                        return parent->of(n);
                    } else {
                        return nullptr;
                    }
                } else {
                    return iter->second;
                }
            }
            if (parent) {// global模式表示要至少在上一层找变量
                return parent->of(n);
            } else {
                return nullptr;
            }
        }
        void set(string n, value* v, flag f = DEFAULT_FLAG) {
            auto var = of(n, f);
            if (var) {
                var->valueOf(v);
            } else {
                insert(new variable(n, v));
            }
        }
        void insert(variable* var) {
            variables.insert(std::pair<string, variable*>(var->nameOf(), var));
        }
        void each(void (*f)(variable*));
        ~block() {
            for (auto i = variables.begin() ; i != variables.end() ; i++) {
                delete i->second;
            }
        }
        friend class script;
    private:
        block* parent;
        table variables;
    };
    enum parseStates {
        FAILED, SUCCESS
    };
    parseStates jsonToPrototypes(const std::string&, hash&);
}

#endif //BERRYMATH_MEMORY_H
