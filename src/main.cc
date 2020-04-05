#include "Driver.hpp"
#include <sstream>

int main() {
        std::string input_str;
        std::cin >> input_str;

        std::stringstream sstr(input_str);

        yy::Driver driver(&sstr);
        driver.parse();

        if (driver.SyntaxErrNum() != 0 || driver.TypeErrNum() != 0) {
            if (driver.SyntaxErrNum() != 0)
                std::cout << "SYNTAX ERROR" << std::endl;

            if (driver.TypeErrNum() != 0)
                std::cout << "TYPE ERROR" << std::endl;
        } else {
            driver.Transform();
            driver.Dump(std::cout);
        }

        return 0;

}