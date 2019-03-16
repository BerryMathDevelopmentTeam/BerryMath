/*****************************************************************************
*  BerryMath Interpreter                                                     *
*  Copyright (C) 2019 BerryMathDevelopmentTeam  zhengyh2018@gmail.com        *
*                                                                            *
*  抽象语法树生成.                                                             *
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
*  @brief    抽象语法树生成                                                    *
*  Details.                                                                  *
*                                                                            *
*  @author   yhzheng                                                         *
*  @email    zhengyh2018@gmail.com                                           *
*  @version  0.0.1 (beta)                                                    *
*  @date     2019.2.19.                                                      *
*  @license  GNU General Public License (GPL)                                *
*                                                                            *
*****************************************************************************/

#ifndef BERRYMATH_AST_H
#define BERRYMATH_AST_H

#include <iostream>
#include <cstring>
#include <vector>
using std::vector;
using std::string;
#ifdef DEBUG
using std::cout;
using std::cin;
using std::endl;
#endif

namespace BerryMath {
    class AST {
    public:
        enum ASTTypes {
            ROOT, VALUE, OPERATOR
        };
        class ASTNode {
        public:
            ASTNode() : t(ROOT), str("") { }
            ASTNode(ASTTypes t1, string s1) : t(t1), str(s1) { }
            void push(ASTTypes t1, string s1) {
                children.push_back(new ASTNode(t1, s1));
            }
            void push(ASTNode* n) {
                children.push_back(n);
            }
            ASTNode* at(long index) {
                if (index < 0) {
                    index = children.size() + index;
                }
                return children[index];
            }
            string value() {
                return str;
            }
            ASTTypes type() {
                return t;
            }
            ~ASTNode() {
                for (long i = 0 ; i < children.size() ; i++) {
                    delete children[i];
                }
            }
            void each(void (*f)(ASTNode*)) {
                f(this);
                for (long i = 0 ; i < children.size() ; i++) {
                    children[i]->each(f);
                }
            }
            friend class AST;
        private:
            vector<ASTNode*> children;
            string str;
            ASTTypes t;
        };
    public:
        AST() : root(nullptr), code("") { }
        AST(string c) : root(nullptr), code(c) { }
        void parse();
        ASTNode* value() {
            return root;
        }
        ~AST() {
            if (root) delete root;
        }
    private:
        ASTNode* root;
        string code;
        enum type {
            VALUE_EXPRESSION, OPERATOR_EXPRESSION, NONE_EXPRESSION
        };
        struct part {
            type t = NONE_EXPRESSION;
            string v;
        };
        short priority(string);
        part splitPart(string code, int& i) {
            part res;
            res.v = "";
            res.t = NONE_EXPRESSION;
            for (; i < code.length() && code[i] != ';'; i++) {
                if (code[i] == '.') {
                    if (res.t != VALUE_EXPRESSION) {// 是.运算
                        res.t = OPERATOR_EXPRESSION;
                        return res;
                    }
                    res.v += code[i];
                } else if (!(code[i] >= '0' && code[i] <= '9')) {
                    if (res.t == VALUE_EXPRESSION) {
                        i--;
                        return res;
                    }
                    res.t = OPERATOR_EXPRESSION;
                    res.v += code[i];
                } else {
                    if (res.t == OPERATOR_EXPRESSION) {
                        i--;
                        return res;
                    }
                    res.v += code[i];
                }
            }
            return res;
        }
    };
    bool isNumber(string);
}


#endif //BERRYMATH_AST_H
