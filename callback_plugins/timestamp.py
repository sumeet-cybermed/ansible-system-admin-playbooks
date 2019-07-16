# (C) 2012-2013, Michael DeHaan, <michael.dehaan@gmail.com>

# This file is part of Ansible
#
# Ansible is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Ansible is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Ansible.  If not, see <http://www.gnu.org/licenses/>.

import time
#from ansible.callbacks import display

# define start time
t0 = tn = time.time()

def secondsToStr(t):

    # http://bytes.com/topic/python/answers/635958-handy-short-cut-formatting-elapsed-time-floating-point-seconds
    rediv = lambda ll,b : list(divmod(ll[0],b)) + ll[1:]
    return "%d:%02d:%02d.%03d" % tuple(reduce(rediv,[[t*1000,], 1000,60,60]))

def filled(msg, fchar="*"):

    if len(msg) == 0:
        width = 73
    else:
        msg = "%s " % msg
        width = 73 - len(msg)
    if width < 3:
        width = 3 
    filler = fchar * width
    return "%s%s%s " % ("    ", msg, filler)

def timestamp():

    global tn
    #time_current = time.strftime('%A %d %B %Y  %H:%M:%S %z')
    time_current = time.strftime('%d/%m/%y %H:%M:%S  (zone: %z)')
    time_elapsed = secondsToStr(time.time() - tn)
    time_total_elapsed = secondsToStr(time.time() - t0)
    print ( filled( '%s (%s)%s%s' % (time_current, time_elapsed, ' ' * 7, time_total_elapsed )))
    tn = time.time()



class CallbackModule(object):

    """
    this is an example ansible callback file that does nothing.  You can drop
    other classes in the same directory to define your own handlers.  Methods
    you do not use can be omitted.

    example uses include: logging, emailing, storing info, etc
    """

    def playbook_on_task_start(self, name, is_conditional):
        timestamp()
        pass

    def playbook_on_setup(self):
        timestamp()
        pass

    def playbook_on_play_start(self, pattern):
        timestamp()
        print (filled("", fchar="="))
        pass

    def playbook_on_stats(self, stats):
        timestamp()
        print (filled("", fchar="="))
        pass

