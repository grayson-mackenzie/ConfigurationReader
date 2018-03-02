// Copyright 2018 ikhatri@umass.edu
// College of Information and Computer Sciences,
// University of Massachusetts Amherst
//
// This software is free: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License Version 3,
// as published by the Free Software Foundation.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// Version 3 in the file COPYING that came with this distribution.
// If not, see <http://www.gnu.org/licenses/>.
// ========================================================================
#include "config_reader.h"

ConfigReader::ConfigReader(vector<string> f) { files = f; }

// CFG_VECTOR2F(test, "tree.testVec");

void ConfigReader::LuaRead(string filename) {
  // Create the LuaScript object
  LuaScript script(filename);
  // Loop through the unordered map
  unordered_map<string, unique_ptr<ConfigInterface>>::iterator itr;
  for (itr = config.begin(); itr != config.end(); itr++) {
    // Create a temporary pointer because you can't static_cast a unique_ptr
    ConfigInterface* t = itr->second.get();
    // Switch statement that serves as a runtime typecheck
    // See the config_interface.h file for documentation on the ConfigType enum
    // &
    // the GetType() function
    switch (itr->second->GetType()) {
      case (1):  // int
      {
        ConfigInt* temp = static_cast<ConfigInt*>(t);
        temp->SetVal(&script);
        cout << temp->GetKey() << " (int) was set to " << temp->GetVal()
             << endl;
        break;
      }
      case (2):  // uint
      {
        ConfigUint* temp = static_cast<ConfigUint*>(t);
        temp->SetVal(&script);
        cout << temp->GetKey() << " (uint) was set to " << temp->GetVal()
             << endl;
        break;
      }
      case (3):  // double
      {
        ConfigDouble* temp = static_cast<ConfigDouble*>(t);
        temp->SetVal(&script);
        cout << temp->GetKey() << " (double) was set to " << temp->GetVal()
             << endl;
        break;
      }
      case (4):  // float
      {
        ConfigFloat* temp = static_cast<ConfigFloat*>(t);
        temp->SetVal(&script);
        cout << temp->GetKey() << " (float) was set to " << temp->GetVal()
             << endl;
        break;
      }
      case (5):  // string
      {
        ConfigString* temp = static_cast<ConfigString*>(t);
        temp->SetVal(&script);
        cout << temp->GetKey() << " (string) was set to " << temp->GetVal()
             << endl;
        break;
      }
      case (6):  // vector2f
      {
        ConfigVector2f* temp = static_cast<ConfigVector2f*>(t);
        temp->SetVal(&script);
        cout << temp->GetKey() << " (Vector2f) was set to " << temp->GetVal()
             << endl;
        break;
      }
      case (0):  // null type: the type value used when a ConfigInterface is
                 // constructed -> should never actually be used
        cout << "This should never happen" << endl;
        break;
    }
  }
}

const int& ConfigReader::InitInt(string key) {
  config[key] = unique_ptr<ConfigInterface>(new ConfigInt(key));
  ConfigInterface* t = config.find(key)->second.get();
  ConfigInt* temp = static_cast<ConfigInt*>(t);
  return temp->GetVal();
}

const unsigned int& ConfigReader::InitUnsignedInt(string key) {
  config[key] = unique_ptr<ConfigInterface>(new ConfigUint(key));
  ConfigInterface* t = config.find(key)->second.get();
  ConfigUint* temp = static_cast<ConfigUint*>(t);
  return temp->GetVal();
}

const double& ConfigReader::InitDouble(string key) {
  config[key] = unique_ptr<ConfigInterface>(new ConfigDouble(key));
  ConfigInterface* t = config.find(key)->second.get();
  ConfigDouble* temp = static_cast<ConfigDouble*>(t);
  return temp->GetVal();
}

const float& ConfigReader::InitFloat(string key) {
  config[key] = unique_ptr<ConfigInterface>(new ConfigFloat(key));
  ConfigInterface* t = config.find(key)->second.get();
  ConfigFloat* temp = static_cast<ConfigFloat*>(t);
  return temp->GetVal();
}

const string& ConfigReader::InitString(string key) {
  config[key] = unique_ptr<ConfigInterface>(new ConfigString(key));
  ConfigInterface* t = config.find(key)->second.get();
  ConfigString* temp = static_cast<ConfigString*>(t);
  return temp->GetVal();
}

const Eigen::Vector2f& ConfigReader::InitVector2f(string key) {
  config[key] = unique_ptr<ConfigInterface>(new ConfigVector2f(key));
  ConfigInterface* t = config.find(key)->second.get();
  ConfigVector2f* temp = static_cast<ConfigVector2f*>(t);
  return temp->GetVal();
}

void ConfigReader::helptext() {
  cout << "Please pass in zero or more lua files as an "
          "argument to the program."
       << endl;
  cout << "If you do not pass in a file, the program will use the "
          "default filename which is currently set to: "
       << DEFAULT_FILENAME << endl;
  cout << "Usage: ./ConfigReader filename.lua" << endl;
}

void ConfigReader::init() {
  int length, wd, fd, i = 0;
  char buffer[BUF_LEN];

  // Initialize inotify
  fd = inotify_init();
  if (fd < 0) {
    cout << "ERROR: Couldn't initialize inotify" << endl;
  }

  // Add all the files to be watched
  for (string f : files) {
    wd = inotify_add_watch(fd, f.c_str(), IN_MODIFY);
    if (wd == -1)
      cout << "ERROR: Couldn't add watch to the file: " << f << endl;
  }

  // Loop forever
  while (1) {
    i = 0;
    length = read(fd, buffer, BUF_LEN);
    if (length < 0) cout << "ERROR: Inotify read failed" << endl;

    while (i < length) {
      struct inotify_event* event = (struct inotify_event*)&buffer[i];
      if (event->len) {
        if (event->mask & IN_MODIFY) {  // If the event was a modify event
          if (!(event->mask &
                IN_ISDIR)) {  // And the modified thing was a NOT a directory
            LuaRead(event->name);
          }
        }
      }
    }
  }
  // Clean up
  inotify_rm_watch(fd, wd);
  close(fd);
}