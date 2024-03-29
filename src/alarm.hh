// Generated by dzn code from /home/karol/dev_projects/VerumESP32/DezyneAlarm/src/alarm.dzn
// Dezyne --- Dezyne command line tools
// Copyright © 2024Karol Kobiela <karol.kobiela@verum.com>
//
// This file is part of Dezyne.
//
// Dezyne is free software: you can redistribute it and/or modify it
// under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// Dezyne is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public
// License along with Dezyne.  If not, see <http://www.gnu.org/licenses/>.
//
// Commentary:
//
// This model represents a correct (relying on the state of its ports)
// working alarm that, upon a sensor change while being armed, sounds
// the siren after a grace period.
//
// Code:
#include <dzn/runtime.hh>
namespace dzn
{
  struct locator;
  struct runtime;
}
#include <iostream>
#include <vector>
#include <map>
#include "alarm-interfaces.hh"
#ifndef ALARMCONTROLLER_HH
#define ALARMCONTROLLER_HH
#include "alarm-interfaces.hh"
struct AlarmController: public dzn::component
{
  dzn::meta dzn_meta;
  dzn::runtime& dzn_runtime;
  dzn::locator const& dzn_locator;
  bool* dzn_reply_bool;
  std::function<void ()>* dzn_out_console;
  ::IConsole console;
  ::ISiren siren;
  ::ISensor sensor;
  ::ITimer timer;
  ::IPin auth;
  AlarmController (dzn::locator const& locator);
  private:
  void console_arm (int pin);
  void sensor_MovementDetected ();
  void timer_timeout ();
};
#endif // ALARMCONTROLLER_HH
// version 2.18.0
