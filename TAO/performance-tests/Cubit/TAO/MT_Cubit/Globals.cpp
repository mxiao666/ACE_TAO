// $Id$

#include "Globals.h"

Globals::Globals (void)
  : ior_file (0),
    base_port (0),
    num_of_objs (2),
    thread_per_rate (0),
    use_multiple_priority (0),
    ready_ (0),
    ready_cnd_ (ready_mtx_),
    barrier_ (0)
{
  if (ACE_OS::hostname (hostname, BUFSIZ) != 0)
    ACE_DEBUG ((LM_DEBUG,
                "%p\n",
                "gethostname"));
}

int
Globals::parse_args (int argc, char *argv[])
{
  ACE_Get_Opt opts (argc, argv, "sh:p:t:f:rm");
  int c;

  ACE_DEBUG ((LM_DEBUG,
              "%s",
              hostname));

  while ((c = opts ()) != -1)
    {
      switch (c)
      {
      case 'm':
        use_multiple_priority = 1;
        break;
      case 'r':
        thread_per_rate = 1;
        break;
      case 'f':
        ACE_NEW_RETURN (ior_file,
                        char[BUFSIZ],
                        -1);
        ACE_OS::strcpy (ior_file,
                        opts.optarg);
        break;
      case 'h':
        ACE_OS::strcpy (hostname,
                        opts.optarg);
        break;
      case 'p':
        base_port = ACE_OS::atoi (opts.optarg);
        break;
      case 't':
        num_of_objs = ACE_OS::atoi (opts.optarg);
        break;
      case '?':
      default:
        ACE_ERROR_RETURN ((LM_ERROR,
                           "usage:  %s"
                           " \t[-p <port>]\n"
                           " \t[-h <my_hostname>]\n"
                           " \t[-t <num_objects>]\n"
                           " \t[-f <ior_file>]\n"
                           " \t[-r Use thread per rate]\n"
                           "\n", argv [0]),
                          -1);
      }
    }

  if (thread_per_rate == 1)
    num_of_objs = THREAD_PER_RATE_OBJS;

  // Indicates successful parsing of the command-line.
  return 0;
}

int
Globals::sched_fifo_init (void)
{
  int result;
#if defined (ACE_HAS_THREADS)
  // Enable FIFO scheduling, e.g., RT scheduling class on Solaris.
  result = SCHED_PARAMS_FIFO;
  if (result != 0)
    {
      if (ACE_OS::last_error () == EPERM)
        ACE_DEBUG ((LM_MAX,
                    "preempt: user is not superuser, "
                    "so remain in time-sharing class\n"));
      else
        ACE_ERROR_RETURN ((LM_ERROR,
                           "%n: ACE_OS::sched_params failed\n%a"),
                          -1);
    }
#else
  ACE_ERROR_RETURN ((LM_ERROR,
                     "Test will not run.  This platform doesn't seem to have threads.\n"),
                    -1);
#endif /* ACE_HAS_THREADS */
  return 0;
}

MT_Priority::MT_Priority (void)
  : num_priorities_ (0),
    grain_ (0)
{
}

ACE_Sched_Priority
MT_Priority::get_high_priority (void)
{
  ACE_Sched_Priority high_priority;

#if defined (VXWORKS)
  high_priority = ACE_THR_PRI_FIFO_DEF;
#elif defined (ACE_WIN32)
  high_priority =
    ACE_Sched_Params::priority_max (ACE_SCHED_FIFO,
                                    ACE_SCOPE_THREAD);
#else
  high_priority = ACE_THR_PRI_FIFO_DEF + PRIORITY_INCR;
#endif /* VXWORKS */
  return high_priority;
}

ACE_Sched_Priority
MT_Priority::get_low_priority (u_int num_low_priority,
                               ACE_Sched_Priority prev_priority,
                               u_int use_multiple_priority)
{
  ACE_Sched_Priority low_priority = ACE_THR_PRI_FIFO_DEF;

  // Drop the priority.
  if (use_multiple_priority)
    {
      this->num_priorities_ = 0;

      for (ACE_Sched_Priority_Iterator priority_iterator
             (ACE_SCHED_FIFO, ACE_SCOPE_THREAD);
           priority_iterator.more ();
           priority_iterator.next ())
        this->num_priorities_++;

      // 1 priority is exclusive for the high priority client.
      this->num_priorities_--;

      // Drop the priority, so that the priority of clients will
      // increase with increasing client number.
      for (u_int j = 0;
           j < num_low_priority;
           j++)
        {
          low_priority =
            ACE_Sched_Params::previous_priority (ACE_SCHED_FIFO,
                                                 prev_priority,
                                                 ACE_SCOPE_THREAD);
          prev_priority = low_priority;
        }
      // Granularity of the assignment of the priorities.  Some OSs
      // have fewer levels of priorities than we have threads in our
      // test, so with this mechanism we assign priorities to groups
      // of threads when there are more threads than priorities.
      this->grain_ = num_low_priority / this->num_priorities_;

      if (this->grain_ <= 0)
        this->grain_ = 1;
    }
  else
    low_priority =
      ACE_Sched_Params::previous_priority (ACE_SCHED_FIFO,
                                           prev_priority,
                                           ACE_SCOPE_THREAD);
  return low_priority;
}

u_int
MT_Priority::number_of_priorities (void)
{
  return this->num_priorities_;
}

u_int
MT_Priority::grain (void)
{
  return this->grain_;
}


#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
template class ACE_Condition<ACE_SYNCH_MUTEX>;
#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
# pragma instantiate ACE_Condition<ACE_SYNCH_MUTEX>
#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */
