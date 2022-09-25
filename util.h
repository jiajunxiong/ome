//
// Created by Jiajun on 9/3/2022.
//

#pragma once
#ifndef OME_UTIL_H
#define OME_UTIL_H

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::uuids;
using namespace boost::posix_time;

namespace util {
    ptime getLocalTimeInSecond();

    ptime getLocalTimeInMicroSecond();

    ptime getUTCTimeInSecond();

    ptime getUTCTimeInMicroSecond();

    std::string getStringTime(const ptime &);

    long getLongTime(const ptime &);

    std::string getUUID();

}

#endif //OME_UTIL_H
