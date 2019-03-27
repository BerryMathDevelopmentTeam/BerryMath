/*****************************************************************************
*  BerryMath Interpreter                                                     *
*  Copyright (C) 2019 BerryMathDevelopmentTeam  zhengyh2018@gmail.com        *
*                                                                            *
*  分析存储BerryMath数据类型标识符.                                             *
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
*  @brief    分析存储BerryMath数据类型标识符                                    *
*  Details.                                                                  *
*                                                                            *
*  @author   yhzheng                                                         *
*  @email    zhengyh2018@gmail.com                                           *
*  @version  0.0.1 (beta)                                                    *
*  @date     2019.2.19.                                                      *
*  @license  GNU General Public License (GPL)                                *
*                                                                            *
*****************************************************************************/

#ifndef BERRYMATH_BTYPE_H
#define BERRYMATH_BTYPE_H

#include <iostream>
#include <json.h>
using std::string;

namespace BerryMath {
    enum TYPE {
        NUMBER, STRING, OBJECT, FUNCTION, NATIVE_FUNCTION, UNDEFINED, // BerryMath值
        TOKEN_NAME// BerryMath标记值(不提供给语言，仅仅是native接口值, 比如存储变量名、函数名等)
    };
    TYPE type(string);
    bool isSymbol(char);
    bool isTrue(string);
    bool isTokenName(string);
}

#endif //BERRYMATH_BTYPE_H
