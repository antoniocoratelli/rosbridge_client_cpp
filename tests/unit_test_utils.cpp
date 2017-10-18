/*
 * Copyright (C) 2017 Antonio Coratelli
 *
 
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <gtest/gtest.h>
#include <rosbridge_client_cpp/utils.h>

using namespace rosbridge_client_cpp;

TEST(unit_test_utils, to_json)
{
    keys_values_t source = { {"key_1", "value-vw2L45Ei"}, {"key_2", "value-bqxALKiz"} };
    picojson::object target;
    target["key_1"] = picojson::value("value-vw2L45Ei");
    target["key_2"] = picojson::value("value-bqxALKiz");
    EXPECT_EQ(to_json(source), target);
}

TEST(unit_test_utils, to_string)
{
    keys_values_t source = { {"key_1", "value-vw2L45Ei"}, {"key_2", "value-bqxALKiz"} };
    std::string target = "{\"key_1\":\"value-vw2L45Ei\",\"key_2\":\"value-bqxALKiz\"}";
    picojson::object json = to_json(source);
    EXPECT_EQ(rosbridge_client_cpp::to_string(json), target);
    EXPECT_EQ(std::to_string(json), target);
}
