//
//  main.cpp
//  proxy_server_s
//
//  Created by Titova Sophia on 26.05.17.
//  Copyright © 2017 Titova Sophia. All rights reserved.
//

#include "proxy.h"
#include <signal.h>

const unsigned short PORT = 7777;

int main() {
    proxy_server proxy(PORT);
    proxy.run();
    return 0;
}

