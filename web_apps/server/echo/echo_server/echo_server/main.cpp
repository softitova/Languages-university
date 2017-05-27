//  main.cpp
//  echo_server
//
//  Created by Titova Sophia on 12.03.17.
//  Govnokod removed by Mekhrubon Turaev
//  Copyright © 2017 Titova Sophia. All rights reserved.
//

#include "echo.h"

int main()
{
    std::set<int> s;
    s.insert(8004);
    echo_server echo_s(s);
    
    echo_s.handle_signal(SIGINT, [&echo_s](struct kevent& kev) {
            std::cout << "\nSIGINT\n";
            echo_s.soft_stop();
    });
    
    echo_s.handle_signal(SIGTSTP, [&echo_s](struct kevent& kev) {
        std::cout << "\nSIGTSTP\n";
        echo_s.soft_stop();
    });
    
    echo_s.start();
    return 0;
}
