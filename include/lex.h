/*****************************************************************************
*  BerryMath Interpreter                                                     *
*  Copyright (C) 2019 BerryMathDevelopmentTeam  zhengyh2018@gmail.com        *
*                                                                            *
*  词法分析器接口.                                                             *
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
*  @brief    词法分析器接口                                                    *
*  Details.                                                                  *
*                                                                            *
*  @author   yhzheng                                                         *
*  @email    zhengyh2018@gmail.com                                           *
*  @version  0.0.1 (beta)                                                    *
*  @date     2019.2.19.                                                      *
*  @license  GNU General Public License (GPL)                                *
*                                                                            *
*****************************************************************************/

#ifndef BERRYMATH_LEX_H
#define BERRYMATH_LEX_H

#include <iostream>

namespace BerryMath {
    enum Token {
        NONE_TOKEN = -1,
        BAD_TOKEN,
        UNKNOWN_TOKEN,
        END_TOKEN,
        VARIABLE_TOKEN, FUNCTION_TOKEN,
        SET_TOKEN, BRACKETS_LEFT_TOKEN, BRACKETS_RIGHT_TOKEN,
        MIDDLE_BRACKETS_LEFT_TOKEN, MIDDLE_BRACKETS_RIGHT_TOKEN,
        BIG_BRACKETS_LEFT_TOKEN, BIG_BRACKETS_RIGHT_TOKEN,
        DOT_TOKEN, NUMBER_TOKEN,
        BIGGER_TOKEN, SMALLER_TOKEN, EQUAL_TOKEN, BIGGER_EQUAL_TOKEN, SMALLER_EQUAL_TOKEN, NOT_EQUAL_TOKEN,
        LOGICAL_AND_TOKEN, LOGICAL_OR_TOKEN, LOGICAL_NOT_TOKEN,
        MATH_AND_TOKEN, MATH_OR_TOKEN, MATH_XOR_TOKEN, MATH_NOT_TOKEN,
        ADD_TOKEN, SUB_TOKEN, MUL_TOKEN, DIV_TOKEN, MOD_TOKEN,
        SHIFT_LEFT_TOKEN, SHIFT_RIGHT_TOKEN,
        ADD_TO_TOKEN, SUB_TO_TOKEN, MUL_TO_TOKEN, DIV_TO_TOKEN, MOD_TO_TOKEN,
        MINUS_TOKEN,
        SELF_ADD_TOKEN, SELF_SUB_TOKEN,
        CREATE_VARIABLE_TOKEN, CREATE_FUNCTION_TOKEN,
        IF_TOKEN, ELIF_TOKEN, ELSE_TOKEN,
        WHILE_TOKEN, DO_TOKEN, FOR_TOKEN,
        INIT_TOKEN, NOTE_TOKEN
    };
    class lex {
    public:
        friend class AST;
        struct lexToken {
            Token token;
            std::string str;
        };
        lex() : program(""), token(INIT_TOKEN), str(""), parseIndex(0) { }
        lex(std::string p) : program(p), token(INIT_TOKEN), str(""), parseIndex(0) { }
        void restart() {
            token = INIT_TOKEN;
            parseIndex = 0;
            end = false;
        }
        lexToken get();
        void finish() {
            token = INIT_TOKEN;
            end = false;
        }
        void whileToEnd();
        lexToken value() {
            lexToken tmp;
            tmp.token = token;
            tmp.str = str;
            return tmp;
        }
    private:
        std::string program;
        Token token;
        std::string str;
        int parseIndex;
        bool end = false;
    };
}

#endif //BERRYMATH_LEX_H
