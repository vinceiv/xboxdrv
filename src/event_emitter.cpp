/*
**  Xbox360 USB Gamepad Userspace Driver
**  Copyright (C) 2011 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "event_emitter.hpp"

#include "controller_message.hpp"
#include "helper.hpp"
#include "uinput.hpp"
#include "uinput_options.hpp"

EventEmitter::EventEmitter(UInput& uinput, int slot, bool extra_devices, const UInputOptions& opts) :
  m_uinput(uinput),
  m_btn_map(),
  m_axis_map(),
  axis_state(),
  m_button_state(),
  m_last_button_state()
{
  std::fill(axis_state.begin(), axis_state.end(), 0);
  m_axis_map.init(uinput, slot, extra_devices);
  
  m_axis_map.init(opts.get_axis_map(), uinput, slot, extra_devices);
  m_btn_map.init(opts.get_btn_map(), uinput, slot, extra_devices);
}

void
EventEmitter::init(const ControllerMessageDescriptor& desc)
{
  std::cout << "EventEmitter::init(const ControllerMessageDescriptor& desc)" << std::endl;
  m_btn_map.init(desc);
  m_axis_map.init(desc);
}

void
EventEmitter::send(const ControllerMessage& msg)
{
  m_last_button_state = m_button_state;
  m_button_state = msg.get_key_state();

  m_btn_map.send(m_uinput, m_button_state);
  m_axis_map.send(m_uinput, m_button_state, msg.get_abs_state());

  m_uinput.sync();
}

void
EventEmitter::update(int msec_delta)
{
  m_btn_map.update(m_uinput, msec_delta);
  m_axis_map.update(m_uinput, msec_delta);

  m_uinput.sync();
}

void
EventEmitter::reset_all_outputs()
{
  m_last_button_state.reset();
  m_button_state.reset();

  m_axis_map.send_clear(m_uinput);
  m_btn_map.send_clear(m_uinput);

  m_uinput.sync();
}

/* EOF */