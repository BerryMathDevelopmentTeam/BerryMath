/*****************************************************************************
*  BerryMath Interpreter                                                     *
*  Copyright (C) 2019 BerryMathDevelopmentTeam  zhengyh2018@gmail.com        *
*                                                                            *
*  脚本运行.                                                                  *
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
*  @brief    运行BerryMath脚本                                                *
*  Details.                                                                  *
*                                                                            *
*  @author   yhzheng                                                         *
*  @email    zhengyh2018@gmail.com                                           *
*  @version  0.0.1 (beta)                                                    *
*  @date     2019.2.19.                                                      *
*  @license  GNU General Public License (GPL)                                *
*                                                                            *
*****************************************************************************/

#ifndef BERRYMATH_SCRIPT_H
#define BERRYMATH_SCRIPT_H

#include <iostream>
#include "memory.h"
using std::string;

namespace BerryMath {
    class script {
    public:
        script(script* r_ = nullptr) : code(""), selfAst(false), r(r_) { }
        script(string s, script* r_ = nullptr) : code(s), selfAst(false), r(r_) { }
        script(AST* a, script* r_ = nullptr) : ast(a), selfAst(true), r(r_) { }
        value* run(long line = 0);
        ~script() {
//            if (ast) delete ast;
        }
        void parse(value*&, AST::ASTNode*, long line);
        void Throw(long, string, string, string);
    private:
        string code;
        AST* ast;
        bool selfAst;// 直接存ast
        block* scope;
        script* r;
    };
}

#endif //BERRYMATH_SCRIPT_H
