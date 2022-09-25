//
// Created by Jiajun on 9/3/2022.
//
#include "util.h"

namespace util {
    ptime getLocalTimeInSecond() {
        return second_clock::local_time();
    }

    ptime getLocalTimeInMicroSecond() {
        return microsec_clock::local_time();
    }

    ptime getUTCTimeInSecond() {
        return second_clock::universal_time();
    }

    ptime getUTCTimeInMicroSecond() {
        return microsec_clock::universal_time();
    }

    std::string getStringTime(const ptime &t) {
        return to_simple_string(t);
    }

    long getLongTime(const ptime &t) {
        ptime epoch = time_from_string("2022-09-01 00:00:00.000");
        auto diff = t - epoch;
        return diff.total_microseconds();
    }

    std::string getUUID() {
        boost::uuids::random_generator gen;
        boost::uuids::uuid id = gen();
        return boost::uuids::to_string(id);
    }
}
