/**
 * \author Lucas Kramer
 * \file   SimpleRobot.cpp
 * \brief  The representation of robot within the simulation
 */

/* //Can be uncommented for debugging
   #include <iostream>
   using std::cout;
   using std::endl;
   using std::string;
*/

#include <GL/glu.h>
#include <GL/glut.h>
#include <stdexcept>

#include "Environment.h"
#include "Robot.h"
#include "SimpleRobot.h"
#include "configuration.h"

SimpleRobot::SimpleRobot(int radius,
                         Color color,
                         Color lineColor,
                         int targetId,
                         Environment *env) :
  Robot(SIMPLE, radius, color, lineColor, targetId, env) {}

SimpleRobot::SimpleRobot(int radius,
                         Location loc,
                         Color color,
                         Color lineColor,
                         int targetId,
                         Environment *env) :
  Robot(SIMPLE, radius, loc, color, lineColor, targetId, env) {}

SimpleRobot::~SimpleRobot() {}

float SimpleRobot::getLeftSpeed(float leftLightSensorVal, float rightLightSensorVal,
                                float leftRobotSensorVal, float rightRobotSensorVal,
                                float leftObstacleSensorVal, float rightObstacleSensorVal,
                                float leftTargetSensorVal, float rightTargetSensorVal) {
  return rightTargetSensorVal * GET_FLOAT("SPEED_SCALE_FACTOR") + GET_INT("ROBOT_INITIAL_SPEED");
}

float SimpleRobot::getRightSpeed(float leftLightSensorVal, float rightLightSensorVal,
                                 float leftRobotSensorVal, float rightRobotSensorVal,
                                 float leftObstacleSensorVal, float rightObstacleSensorVal,
                                 float leftTargetSensorVal, float rightTargetSensorVal) {
  return leftTargetSensorVal * GET_FLOAT("SPEED_SCALE_FACTOR") + GET_INT("ROBOT_INITIAL_SPEED");
}
