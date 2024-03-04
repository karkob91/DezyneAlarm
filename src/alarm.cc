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
#include "alarm.hh"
#include <dzn/locator.hh>
#include <dzn/runtime.hh>
#include <iterator>
#define STRINGIZING(x) #x
#define STR(x) STRINGIZING (x)
#define LOCATION __FILE__ ":" STR (__LINE__)
alarm_controller::alarm_controller (dzn::locator const& locator)
: dzn_meta ({"alarm_controller","alarm_controller",0,  {},  {},  {[this] ()
        {
          console.dzn_check_bindings ();
        }, [this] ()
        {
          siren.dzn_check_bindings ();
        }, [this] ()
        {
          sensor.dzn_check_bindings ();
        }, [this] ()
        {
          timer.dzn_check_bindings ();
        }, [this] ()
        {
          auth.dzn_check_bindings ();
        }}})
, dzn_runtime (locator.get<dzn::runtime> ())
, dzn_locator (locator)
, console ({  {"console",&console,this,&dzn_meta},  {"console",0,0,0}},this)
, siren ({  {"siren",0,0,0},  {"siren",&siren,this,&dzn_meta}},this)
, sensor ({  {"sensor",0,0,0},  {"sensor",&sensor,this,&dzn_meta}},this)
, timer ({  {"timer",0,0,0},  {"timer",&timer,this,&dzn_meta}},this)
, auth ({  {"auth",0,0,0},  {"auth",&auth,this,&dzn_meta}},this)
{
  this->dzn_meta.require =   {&siren.dzn_meta,&sensor.dzn_meta,&timer.dzn_meta,&auth.dzn_meta};
  this->dzn_runtime.performs_flush (this) = true;
  this->console.in.arm = [this] (int pin)
    {
      this->dzn_out_console = &this->console.in.arm.dzn_out_binding;
      this->dzn_reply_bool = &this->console.in.arm.reply;
      this->console_arm (pin);
      return this->console.in.arm.reply;
    };
  this->sensor.out.MovementDetected = [this] ()
    {
      this->sensor_MovementDetected ();
    };
  this->timer.out.timeout = [this] ()
    {
      this->timer_timeout ();
    };
}
void
alarm_controller::console_arm (int pin)
{
  if (console.state == ::iconsole::State::Disarmed)
    {
      bool valid = this->auth.in.valid (pin);
      if (valid)
        {
          this->timer.in.set (5000);
          this->console.out.arming ();
        }
      *this->dzn_reply_bool = valid;
      if ((*this->dzn_out_console)) (*this->dzn_out_console) ();
      (*this->dzn_out_console) = nullptr;
    }
  else if (console.state == ::iconsole::State::Arming)
    {
      bool valid = this->auth.in.valid (pin);
      if (valid)
        {
          this->timer.in.cancel ();
          this->console.out.disarmed ();
        }
      *this->dzn_reply_bool = valid;
      if ((*this->dzn_out_console)) (*this->dzn_out_console) ();
      (*this->dzn_out_console) = nullptr;
    }
  else if (console.state == ::iconsole::State::Armed)
    {
      bool valid = this->auth.in.valid (pin);
      if (valid)
        {
          this->console.out.disarmed ();
        }
      *this->dzn_reply_bool = valid;
      if ((*this->dzn_out_console)) (*this->dzn_out_console) ();
      (*this->dzn_out_console) = nullptr;
    }
  else if (console.state == ::iconsole::State::Triggered)
    {
      bool valid = this->auth.in.valid (pin);
      if (valid)
        {
          this->timer.in.cancel ();
          if (siren.enabled)
            {
              this->siren.in.disable ();
            }
          this->console.out.disarmed ();
        }
      *this->dzn_reply_bool = valid;
      if ((*this->dzn_out_console)) (*this->dzn_out_console) ();
      (*this->dzn_out_console) = nullptr;
    }
  else if (!(console.state == ::iconsole::State::Triggered) && (!(console.state == ::iconsole::State::Armed) && (!(console.state == ::iconsole::State::Arming) && !(console.state == ::iconsole::State::Disarmed)))) this->dzn_locator.get<dzn::illegal_handler> ().handle (LOCATION);
  else this->dzn_locator.get<dzn::illegal_handler> ().handle (LOCATION);
}
void
alarm_controller::sensor_MovementDetected ()
{
  if (console.state == ::iconsole::State::Disarmed)   {}
  else if (console.state == ::iconsole::State::Arming)   {}
  else if (console.state == ::iconsole::State::Armed)
    {
      this->console.out.detected ();
      this->timer.in.set (5000);
    }
  else if (console.state == ::iconsole::State::Triggered)   {}
  else if (!(console.state == ::iconsole::State::Triggered) && (!(console.state == ::iconsole::State::Armed) && (!(console.state == ::iconsole::State::Arming) && !(console.state == ::iconsole::State::Disarmed)))) this->dzn_locator.get<dzn::illegal_handler> ().handle (LOCATION);
  else this->dzn_locator.get<dzn::illegal_handler> ().handle (LOCATION);
}
void
alarm_controller::timer_timeout ()
{
  if (console.state == ::iconsole::State::Arming)
    {
      this->console.out.armed ();
    }
  else if (console.state == ::iconsole::State::Triggered)
    {
      this->siren.in.enable ();
    }
  else if (!(console.state == ::iconsole::State::Triggered) && !(console.state == ::iconsole::State::Arming)) this->dzn_locator.get<dzn::illegal_handler> ().handle (LOCATION);
  else this->dzn_locator.get<dzn::illegal_handler> ().handle (LOCATION);
}
// version 2.18.0