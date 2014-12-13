#ifndef OSCHECK_H
#define OSCHECK_H

#if !(defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__))
#define NIX
#endif

#endif // OSCHECK_H
