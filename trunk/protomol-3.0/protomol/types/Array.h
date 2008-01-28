/*  -*- c++ -*-  */
#ifndef ARRAY_H
#define ARRAY_H

// Wrapper header file for N-dim. Array class
//
// If your compiler does not support partial template
// specialization, define NO_PARTIAL_TEMPLATE_SPECIALIZATION

#ifdef NO_PARTIAL_TEMPLATE_SPECIALIZATION
#include <protomol/types/ArrayNoPartialSpecialization.h>
#else
#include <protomol/types/ArrayFastest.h>
#endif

#endif

