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
    };
}


#endif //BERRYMATH_AST_H
