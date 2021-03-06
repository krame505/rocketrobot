/**
 * \author Lucas Kramer
 * \editor Himawan Go
 * \file   Environment.cpp
 * \brief  Provides environment information to objects.  
 *
 */

#include "PhysicalObject.h"
#include "Environment.h"
#include "configuration.h"

#include <iostream>
#include <math.h>
using namespace std;

Environment *Environment::currentEnv;

Environment::Environment(int width, int height) :
  id(0), numObjects(0),
  width(width), height(height) {
  objectsMutex = new mutex();
}

Environment::~Environment() {
  for (PhysicalObject *o : *this) {
    delete o;
  }
  delete objectsMutex;
}

Environment *Environment::newEnv(int width, int height) {
  currentEnv = new Environment(width, height);
  return currentEnv;
}

Environment *Environment::getEnv() {
  if (currentEnv == NULL) {
    currentEnv = new Environment();
  }
  return currentEnv;
}

void Environment::setEnv(Environment *newEnv) {
  currentEnv = newEnv;
}

int Environment::addObject(PhysicalObject *object) {
  objectsMutex->lock();
  if (id < (int)objects.size())
    objects[id] = object;
  else
    objects.push_back(object);
  id++;
  numObjects++;
  objectsMutex->unlock();
  return id - 1;
}

void Environment::removeObject(int id) {
  objectsMutex->lock();
  if (id < 0) {
    objectsMutex->unlock();
    throw new invalid_argument("Invalid id");
  }
  if (objects[id] != NULL && id < (int)objects.size()) {
    objects[id] = NULL;
    numObjects--;
  }
  objectsMutex->unlock();
}

void Environment::clear() {
  for (PhysicalObject *o : *this) {
    delete o;
  }
  id = 0;
}

unsigned Environment::getNumObjects() const {
  return numObjects;
}

PhysicalObject* Environment::getObject(int id) const {
  if (id >= 0 && id < (int)objects.size())
    return objects[id];
  else
    return NULL;
}

Environment::iterator Environment::begin() const {
  // Need to do this to initialize the iterator to the first object
  // Otherwise it can think it isn't done starting out, and run into
  // an error trying to dereference
  Environment::iterator result(this, -1);
  result++;
  return result;
}

Environment::iterator Environment::end() const {
  return Environment::iterator(this, objects.size());
}

// Collision stuff

/**
 * Checks if two objects specified by their Locations and radii
 * \return a boolean value that is true when objects are overlapping
 */
bool isTouching(Location l1, int r1, Location l2, int r2) {
  float x_diff = l1.x - l2.x;
  float y_diff = l1.y - l2.y;
  float min_distance = r1 + r2;
  float distance = sqrt(x_diff * x_diff + y_diff * y_diff);
  
  // min_distance should be <= distance
  return ((distance - min_distance) < GET_FLOAT("TOUCHING_DELTA")); 
}

bool Environment::isTouchingWall(Location l, int r) const {
  return
    (l.x - r <= 0) ||
    (l.y - r <= 0) ||
    (l.x + r >= width) ||
    (l.y + r >= height);
}


bool Environment::isOnScreen(Location l) const {
  return
    (l.x >= 0) ||
    (l.y >= 0) ||
    (l.x < width) ||
    (l.y < height);
}

bool Environment::isTouchingWall(int id) const {
  return isTouchingWall(getObject(id)->getLocation(), getObject(id)->getRadius());
}

bool Environment::isOnScreen(int id) const {
  return isOnScreen(getObject(id)->getLocation());
}

bool Environment::isTouchingObject(Location l, int r, int id) const {
  bool result = false;
  for (PhysicalObject *o : *this) {
    if (o->getId() != id)
      result |= isTouching(l, r, o->getLocation(), o->getRadius());
  }
  return result;
}

bool Environment::isTouchingHitableObject(Location l, int r, int id) const {
  bool result = false;
  for (PhysicalObject *o : *this) {
    if (o->getId() != id && o->isHitable)
      result |= isTouching(l, r, o->getLocation(), o->getRadius());
  }
  return result;
}

bool Environment::isTouchingObject(Location l, int r) const {
  return isTouchingObject(l, r, -1);
}

bool Environment::isTouchingObject(int id) const {
  return isTouchingObject(getObject(id)->getLocation(), getObject(id)->getRadius(), id);
}

bool Environment::isTouchingHitableObject(Location l, int r) const {
  return isTouchingHitableObject(l, r, -1);
}

bool Environment::isTouchingHitableObject(int id) const {
  return isTouchingHitableObject(getObject(id)->getLocation(), getObject(id)->getRadius(), id);
}

bool Environment::isColliding(Location l, int r) const {
  return isTouchingWall(l, r) || isTouchingObject(l, r);
}

bool Environment::isColliding(int id) const {
  return isTouchingWall(id) || isTouchingObject(id);
}

bool Environment::isCollidingWithHitable(Location l, int r) const {
  return isTouchingHitableObject(l, r);
}

bool Environment::isCollidingWithHitable(int id) const {
  return isTouchingHitableObject(id);
}

int Environment::getCollisionId(Location l, int r, int id) const {
  for (PhysicalObject *o : *this) {
    if (o->getId() != id &&
        isTouching(l, r, o->getLocation(), o->getRadius())) {
      objectsMutex->unlock();
      return o->getId();
    }
  }
  return -1;
}

int Environment::getHitableCollisionId(Location l, int r, int id) const {
  for (PhysicalObject *o : *this) {
    if (o->getId() != id &&
        isTouching(l, r, o->getLocation(), o->getRadius()) &&
        o->isHitable) {
      objectsMutex->unlock();
      return o->getId();
    }
  }
  return -1;
}

int Environment::getCollisionId(Location l, int r) const {
  return getCollisionId(l, r, -1);
}

int Environment::getHitableCollisionId(Location l, int r) const {
  return getHitableCollisionId(l, r, -1);
}

int Environment::getCollisionId(int id) const {
  return getCollisionId(getObject(id)->getLocation(), getObject(id)->getRadius(), id);
}

int Environment::getHitableCollisionId(int id) const {
  return getHitableCollisionId(getObject(id)->getLocation(), getObject(id)->getRadius(), id);
}

// Environment::iterator
Environment::iterator::iterator(const Environment *const env) :
  env(env),
  index(0) {}

Environment::iterator::iterator(const Environment *const env, int index) :
  env(env),
  index(index) {}

Environment::iterator::iterator(const Environment::iterator& other) :
  env(other.env),
  index(other.index) {}

Environment::iterator::~iterator() {}

void Environment::iterator::operator++() {
  env->objectsMutex->lock();
  index++;
  while (index < env->objects.size() && env->objects[index] == NULL)
    index++;
   env->objectsMutex->unlock();
}

void Environment::iterator::operator++(int) {
  env->objectsMutex->lock();
  index++;
  while (index < env->objects.size() && env->objects[index] == NULL)
    index++;
  env->objectsMutex->unlock();
}

PhysicalObject* Environment::iterator::operator*() {
  env->objectsMutex->lock();
  if (index >= env->objects.size()) {
    throw new runtime_error("Cannot dereference iterator: No more elements");
  }
  env->objectsMutex->unlock();
  return env->objects[index];
}

bool Environment::iterator::operator>(const Environment::iterator& other) {
  return index > other.index;
}

bool Environment::iterator::operator<(const Environment::iterator& other) {
  return index < other.index;
}

bool Environment::iterator::operator==(const Environment::iterator& other) {
  return index == other.index;
}

bool Environment::iterator::operator!=(const Environment::iterator& other) {
  return index != other.index;
}
