/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2013 - 2016 Symless Ltd.
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#include "test/shared/TestEventQueue.h"

#include "base/TMethodEventJob.h"

#include <stdexcept>

void TestEventQueue::raiseQuitEvent()
{
  addEvent(Event(EventTypes::Quit));
}

void TestEventQueue::initQuitTimeout(double timeout)
{
  assert(m_pQuitTimeoutTimer == nullptr);
  m_pQuitTimeoutTimer = newOneShotTimer(timeout, nullptr);
  adoptHandler(
      EventTypes::Timer, m_pQuitTimeoutTimer,
      new TMethodEventJob<TestEventQueue>(this, &TestEventQueue::handleQuitTimeout)
  );
}

void TestEventQueue::cleanupQuitTimeout()
{
  removeHandler(EventTypes::Timer, m_pQuitTimeoutTimer);
  deleteTimer(m_pQuitTimeoutTimer);
  m_pQuitTimeoutTimer = nullptr;
}

void TestEventQueue::handleQuitTimeout(const Event &, void *vclient)
{
  throw std::runtime_error("test event queue timeout");
}
