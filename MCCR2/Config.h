#pragma once
#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <glm/glm.hpp>
using namespace std;
using namespace glm;


#define TEST


#ifndef TEST
#define printf(s,...) do{}while(0)
#endif;



#endif CONFIG_H